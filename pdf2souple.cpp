#include "pdf2souple.h"
#include "helper.h"
#include <QThread>
#include <algorithm>
#include <QDebug>
#include <QRectF>
#include <QFontMetrics>
#include <QFile>
#include <QBitmap>
#include <QFontDatabase>
#include "souplemanager.h"
#include "anchorobj_jzrect.h"
#include "anchorobj_path.h"
#include "anchorobj_phleft.h"
#include "pdf_table_analyse.hpp"
#include "anchorobj_glue.h"

using namespace std;

static uchar *buf_image = 0;

#define Only_Test_FirstPage false
#define Print_Path_Read true //是否输出路径的原始数据

//loadPdf: 异步加载pdf，并把souple存放在SoupleSerializer中
void Pdf2Souple::loadPdf(const QString& pdf_filename)
{
    static bool _protect_var = false;

    if(_protect_var) {
        emit Helper::helper->errorMsg("错误","请等待PDF加载完毕！");
        return;
    }

    _protect_var = true;

    emit Helper::helper->beginLoadDocument();

    QThread *thread = new QThread;
    SoupleManager::paused = true; //暂停处理，防止线程冲突，避免上锁
    QObject::connect(thread,&QThread::started,[pdf_filename,thread]{
        imp_loadPdf(pdf_filename);

        //加载完毕
        emit Helper::helper->fileLoaded(); //通知ui

        _protect_var = false; //极小概率访问冲突 [!]
        SoupleManager::paused = false;
        if(buf_image) {
            delete[] buf_image; //释放临时buffer
        }
        thread->exit(); //退出线程
    });
    QObject::connect(thread,&QThread::finished,thread,&QThread::deleteLater);

    thread->start();

}

void Pdf2Souple::imp_loadPdf(const QString& filename)
{
    //使用poppler读取pdf，并把pdf转换为souple
    //通过Helper的两个信号: addVLine和addHLine来添加obj到编辑器

    qDebug() << "imp_loadPdf(" << filename;

    auto document = FPDF_LoadDocument(filename.toUtf8(),""); //加载document

    if(document == NULL) {
        qDebug() << "无法打开文档！";
        emit Helper::helper->errorMsg("错误",
                        "无法打开"+filename+"\nPdfium错误码:"+(char)(FPDF_GetLastError() - '0')
                                      +"\n你可以尝试在文件-首选项中修改pdf路径的编码。");
        return;
    }

    int page_count = FPDF_GetPageCount(document); //获取pdf页数

    qDebug() << "page_count:" << page_count;


    float total_height = 0; //累计页面高度和

    int hLineNum = 0, vLineNum = 0; //标线数量

    double leftLine_x, rightLine_x; //的x
    QString leftLine_name = "", rightLine_name ="";
    vector<shared_ptr<Souple_Area>> list_area = {};
    vector<shared_ptr<PDFPage>> pdfpage_list;
    std::shared_ptr<PDFPage> old_page;

    Shared::pdf_page_number = page_count; //分享页数

    future_preload.waitForFinished(); //等待pdf预读取完毕

    for(int page_index = 0; page_index < page_count; ++page_index)
    {
        //if(page_index  11) continue;
        auto fpage = FPDF_LoadPage(document,page_index);
        auto text_page = FPDFText_LoadPage(fpage); //加载文本层

        // FPDFText_
        // int rnum = FPDFText_CountRects(text_page,0,-1);
        // qDebug() << "text-rext num = " << rnum;
        float page_width = Helper::point2pixel(FPDF_GetPageWidth(fpage));
        float page_height = Helper::point2pixel(FPDF_GetPageHeight(fpage));

        auto page = make_shared<PDFPage>();

        Shared::current_page = page;

        //FS_RECTF contentBox;
        //尝试使用pdfium的实验性方法获取内容框，以得到页面的上下边距
        //FPDF_GetPageBoundingBox(page,&contentBox);
        //qDebug() <<"contentBox: " << contentBox.top << contentBox.bottom;


        //通知ui
        //Helper::invokeQmlFunction<void>("appendPage",page_width,page_height);

        int obj_num = FPDFPage_CountObjects(fpage); //获取obj数量

        struct OBJ { //临时存储obj区域和页索引
            QRectF rect;
            FPDF_PAGEOBJECT page_obj;
            int page_index;
        };

        page->page_left_margin = 0;
        page->page_top_margin = total_height;
        page->page_width = page_width;
        page->page_height = page_height;
        page->page_index = page_index;

        imp_dealText(text_page,page->all_objs,page_width,page_height); //读取文本层

        for(int i = 0; i < obj_num; ++i) { //读取并处理非文本类型的pdfobj
            auto page_obj = FPDFPage_GetObject(fpage,i);
            std::shared_ptr<PDFOBJ> obj = readPdfObj(page_obj,text_page,page_width,page_height);
            if(! obj) continue;
            obj->render_id = i; //渲染顺序(越小越先绘制)
            page->all_objs.push_back(obj);
        }


        imp_analysePdfPage(page,old_page);

        total_height += page_height; //记录累计高度

        emit Helper::helper->fileLoadProgress((page_index+1.0)/page_count);

        FPDFText_ClosePage(text_page);
        FPDF_ClosePage(fpage);

        pdfpage_list.push_back(page);
        old_page = page;
#if Only_Test_FirstPage == true
        break; //只需要读取一页做测试，就define该宏为true
#endif
    }

    //合并临时表格、创建表格
    mergeAndCreateTables(pdfpage_list);

    //合并FrameParts，创建装饰框
    mergeAndCreateFrames(pdfpage_list);

    FPDF_CloseDocument(document);
}

//#define true false
/**
 * @brief _sort_func 按照同行、上、左顺序对PDFOBJ进行排序。可用于std::sort作为比较器。
 */
auto _sort_func = [](shared_ptr<Pdf2Souple::PDFOBJ> a,shared_ptr<Pdf2Souple::PDFOBJ> b)->bool{
    bool a_up_b = a->rect.bottom()-Y_LINEOBJ_MAX_MARGIN < b->rect.top();
    bool b_up_a = b->rect.bottom()-Y_LINEOBJ_MAX_MARGIN < a->rect.top();
    if(!(a_up_b ^ b_up_a)) { //1 1 or 0 0
        //到这儿，我们认为a、b它们在同一行
        if(a->rect.left()+X_COVER_OFFSET < b->rect.right() && a->rect.right() > b->rect.left()+X_COVER_OFFSET) {
            //qDebug() << "重叠发生";
            return a->render_id < b->render_id; //如果同一行出现重叠，先绘制的优先(即被覆盖的优先)
        }
        return a->rect.left() < b->rect.left(); //严格弱序
    }
    if(a_up_b) return true; //a在b上面，选择a // 1 0
    if(b_up_a) return false; //b在a上面，选择b // 0 1
    return false; // --- 不可能达到此处 ---
};



//解析一页pdf，建立标线布局,返回该页末尾的可能往下一页延伸的HLine
//all_objs: 该页所有PDFOBJ
//last_page_areas: 上一页解析返回的areas
//page_.. :页面属性、页码
void Pdf2Souple::imp_analysePdfPage(shared_ptr<PDFPage> page,shared_ptr<PDFPage> old_page)
{

    /**第一步，找出所有自由元素
     *rule-1: 重叠的元素中最上面的那个是锚定元素，其余是自由元素
     *rule-2: 一个元素判定为自由元素后，不会再变成锚定元素。锚定元素条件更苛刻。**/

    /**第二步，从所有锚定元素中合并一些近似连续的块，[合并本质连续的文字(按照字距判断,垂直误差<1.5,水平误差<2)]，形成各个水平锚定块
     *根据各个水平锚定块的左边界，按上下左右顺序遍历水平锚定块，对遍历到的锚定块往下寻找所有左边界近似相同(差值小于10)或者被目前宽度覆盖的
     *水平锚定块（保证从初始左边界到它的左边界为空白），每次记录当前块的“最大”宽度，即从原定左边界到右边第一个元素左边界或页右边界的距离，
     *若“最大*宽度+原定左边界x坐标小于之前任何一个元素的实际右边界x坐标，那么终止过程，该行无效。或者该行实际右边界大于之前所有元素的
     *最小“最大”宽度，同样终止
     **  这样扫描完后，获得了多个锚定区域，它们各自有自己的左右标线  */

    /**第三步，根据垂直方向上碰撞关系来建立HLine的锚定关系
     * 若上下一对一邻接，则同时建立锚定和逻辑连续关系
     * 若一对多，则仅建立锚定关系；若多对一，使用[多重锚定]...(目前先不锚定。。。)*/

    /*然后，每一页的第一条HLine要尝试和上一页的底部HLine建立逻辑连续关系*/

    //step 1
    //暂时略，目前处理没有重叠元素的pdf
    //end step 1


    //imp_path_doSomeMerge(page->all_objs);


    //在筛去自由元素后，
    //遍历一边all_objs，找到页上下边距，以创建页面
    Page *export_page = nullptr;


    if(page->all_objs.empty()) {
        //空页...
        export_page = SoupleManager::addPage(page->page_width,page->page_height,50,50);
        return; //返回空
    }


    //如果用户指定了页眉或页脚，我们需要优先解析页眉页脚

    //qDebug() << "PDF_Load_Advice ptr: " << &PDF_Load_Advice;
    qDebug() << "hasUniHeader: " << PDF_Load_Advice::hasUniHeader;

    /** 解析页眉页脚 */
    analyseHeader(page);

    if(page->all_objs.empty()) {
        //除去页眉页脚后是空页，我们按照页眉页脚来设置页上下边距
        //ToDo.......
        export_page = SoupleManager::addPage(page->page_width,page->page_height,50,50);
        return; //返回空
    }

    float top_min = page->all_objs[0]->rect.top(),
            bottom_max = page->all_objs[0]->rect.bottom();
    for(auto obj : page->all_objs) {
        top_min = std::min(top_min,(float)obj->rect.top());
        bottom_max = std::max(bottom_max,(float)obj->rect.bottom());
    }

    {   // 创建页面
        float topMargin,bottomMargin;
        //if(hasHeader) topMargin = std::max(header,page_height*0.15f); //如果用户指定了页眉
        topMargin = std::min(top_min-0.5f,page->page_height*0.15f);
        //if(hasFooter) bottomMargin = footer; //如果用户指定了页脚
        bottomMargin = std::min(page->page_height-bottom_max-0.5f,page->page_height*0.15f);
        export_page = SoupleManager::addPage(page->page_width,page->page_height,topMargin,bottomMargin);
    }


    page->souple_page = export_page;


    /**  [2025/11/22 added] 合并邻接矩形为一个path、切分一些path为线条 */
    imp_path_doSomeMerge(page->all_objs);

    /** 解析表格 */
    analyseTable(page.get(),old_page.get());

    /** 优先尝试解析成 Word类型页面，格式化页面 */
    if(analyseWordPage(page,old_page)) return;

    // [以下200行代码]
    //step 2
    sort(page->all_objs.begin(), page->all_objs.end(), _sort_func);


    //qDebug() << "排序后的obj: ";
    //for(auto obj : page->all_objs) {
        //obj->print();
    //}

    QList<HBlock> blocks;

    createHBlocks(page->all_objs,page->page_width,blocks); //创建水平块

    //step3

    //扫描各水平锚定块
    int area_index = 0;
    //vector<std::shared_ptr<Souple_Area>> list_area; //记录锚定区域
    for(int i = 0; i < blocks.size(); )
    {//扫描各水平锚定块
        while(i < blocks.size() && blocks[i].has_belonging) ++i; //找到第一个没被扫描过的HBlock
        if(i >= blocks.size()) break;

        //扫描第i个HBlock，目标找出其所在的锚定区域

        std::vector<int> composed_indexs; //由哪些HBlock组成了该锚定区域
        composed_indexs.push_back(i);
        int ori_i = i;
        HBlock& ori_bk = blocks[ori_i];
        float leftest = ori_bk.ex_left, rightest = ori_bk.ex_right; //最大左右边界
        float left_min = ori_bk.rect.left(), right_min = ori_bk.rect.right(); //最小左右边界
        float row_bottom = ori_bk.rect.bottom();
        float area_top_y = ori_bk.rect.top();
        float area_bottom_y = ori_bk.rect.bottom();
        // float bottom_y = ori_bk.rect.bottom();
        //bool isFull = true;
        //blocks[ori_i].belonging = area_index++;
        ori_bk.has_belonging = true;
        bool hasFillTheLine = true; //是否已经扫描过了当前行的HBlock
        int line_num = 1;
        for(i = ori_i + 1; i < blocks.size(); ++i) {
            auto &bk = blocks[i];
            auto &last_bk = blocks[composed_indexs.back()]; //上一个HBlock
            if(bk.rect.top()+1.0 >= row_bottom) { //进入下一行
                if(! hasFillTheLine) {
                    //如果当前行没有找到合适的HBlock，则终止扫描s
                    qDebug()<<"当前行没有找到合适的HBlock";
                    break;
                }
                hasFillTheLine = false; //进入下一行
                //row_bottom = bk.rect.bottom(); //禁止在这里更新row_bottom，因为下一个bk可能被skip
                //qDebug() << "换行->";
                ++line_num;
            }
            if(hasFillTheLine) continue;
            else {
                if(bk.has_belonging) { //如果已经属于另一个锚定区域了
                    if(bk.rect.right() < left_min || bk.rect.left() > right_min) {
                        qDebug() << "A####《skip》";
                        continue;
                    }
                    else {qDebug()<<"遮挡断开>>>>>>>>>";break;} //
                }
                if(bk.ex_left <= left_min && bk.ex_right >= right_min
                    && bk.rect.left() > leftest && bk.rect.right() < rightest)
                { //如果hblock水平拓展后覆盖了最小水平区域，且其原始范围在锚定区域水平拓展范围内
                    hasFillTheLine = true; //该HBlock即为该行的HBlock
                    //更新锚定区域边界
                    leftest = std::max(bk.ex_left,leftest);
                    rightest = std::min(bk.ex_right,rightest);
                    left_min = std::min((float)bk.rect.left(),left_min);
                    right_min = std::max((float)bk.rect.right(),right_min);
                    bk.has_belonging = true;
                    composed_indexs.push_back(i);
                    area_bottom_y = max(area_bottom_y, (float)bk.rect.bottom()); //更新area底部y
                    qDebug() << "添加<-";
                    bk.objs.front()->print();
                    row_bottom = bk.rect.bottom(); //这是该行的row_bottom
                    continue;
                } else { //未覆盖
                    if(bk.rect.right() > left_min && bk.rect.left() < right_min) { //若水平交错
                        //qDebug() << "**********水平交错";
                        //qDebug() << "leftest = " << leftest;
                        //qDebug() << "rightest = " << rightest;
                        //qDebug() << "left_min = " << left_min;
                        //qDebug() << "right_min = " << right_min;
                        //qDebug() << "but rect is " << bk.rect;
                        //qDebug() << "ex-left = " << bk.ex_left;
                        //qDebug() << "ex-right = " << bk.ex_right;
                        break; //证明该行没有满足条件的HBlock，终止扫描
                    } else {
                        qDebug() << "skip HBlock: ";
                        bk.objs.front()->print();
                        //hasFillTheLine = false;
                        continue; //该HBlock处于锚定区域的两侧，本质不属于锚定区域，但也不破坏锚定区域
                    }
                }
            }
        }
        //qDebug() << "锚定区域: leftest=" << leftest << ",left=" << left_min;
        //qDebug() << "right=" << right_min << ",rightest=" << rightest;
        //qDebug() << "HBlock_Num = " << composed_indexs.size();
        //qDebug() << "Line Span = " << line_num;

        // for(int k : composed_indexs) {
        //     for(auto obj : blocks[k].objs) {
        //         obj->print();
        //     }
        // }
    END_I:

        //为锚定区域创建Souple元素
        AnchorObj_VLine *vline_left = new AnchorObj_VLine, //创建左右垂直标线
                        *vline_right = new AnchorObj_VLine;
        SoupleManager::registerObj(vline_left);
        SoupleManager::registerObj(vline_right);
        vline_left->x = left_min + page->page_left_margin;
        vline_right->x = right_min + page->page_left_margin;
        AnchorObj_HLine *last_hline = 0;
        AnchorObj_HLine *top_hline = 0,*bottom_hline = 0; //记录区域的top bottom HLine
        //bool isTable = false; //是否是表格HBlock
        for(auto[id,j] : composed_indexs|std::views::enumerate) { //扫描各个HBlock(各行)
            auto& bk = blocks[j];
            if(auto table = dynamic_pointer_cast<PDFOBJ_TablePart>(bk.objs[0]); table) {

                //表格需要特殊处理
                if(last_hline) { //表格上间距
                    auto tline = table->tablelines[0];

                    //last_hline->logic_nextHLine = tline;
                    //tline->logic_lastHLine = last_hline;
                    tline->hline = last_hline;
                    last_hline->anchor_nextHLine.push_back(tline);

                    tline->topMargin =
                        tline->getContentTop() - last_hline->getContentBottom();
                    // if(table->isHeaderPart) {
                    //     //第一行是表格的第一行
                    //     float leftMargin = tline->x - vline_left->x;
                    //     float rightMargin = vline_right->x - tline->getRightX();
                    //     if(abs(leftMargin - rightMargin) <
                    //         (vline_right->x - vline_left->x) * SCALE_OFFSET_JUDGE_HCENTER) {
                    //         //居中
                    //         tline->table_info->alignMode = Helper::AlignHCenter;
                    //     } else if(leftMargin < rightMargin ){
                    //         tline->table_info->alignMode = Helper::AlignLeft;
                    //     } else {
                    //         tline->table_info->alignMode = Helper::AlignRight;
                    //     }
                    // }
                }

                if(id == 0) { //第一行
                    vline_left->y = vline_right->y = table->tablelines[0]->y;
                    top_hline = table->tablelines[0];
                }
                // no else
                if(id == composed_indexs.size() - 1) { //最后一行
                    //vline_left->height = vline_right->height = table->tablelines[0]->y - vline_left->y;
                    bottom_hline = table->tablelines.back();
                }
                ++id;

                for(auto tline : table->tablelines) {
                    tline->leftLine = vline_left;
                    tline->rightLine = vline_right;
                    tline->page = page->souple_page;
                    SoupleManager::registerObj(tline);
                }
                last_hline = table->tablelines.back(); //记录上一行为表格的末行
            } else { //非表格
                //建立水平标线
                AnchorObj_HLine *hline = new AnchorObj_HLine;
                SoupleManager::registerObj(hline);

                if(id == 0) { //第一行
                    vline_left->y = vline_right->y = hline->y;
                    top_hline = hline;
                }
                // no else
                if(id == composed_indexs.size() - 1) { //最后一行
                    vline_left->height = vline_right->height = hline->y - vline_left->y;
                    bottom_hline = hline;
                }

                layoutHLine(page,hline,last_hline,bk,vline_left,vline_right);
                ++id;
            }
        } //扫描各个HBlock(各行) END

        //到这儿，vLine_right的x坐标完全确定，可以进行占位符的插入

        auto hline = bottom_hline;

        while(hline) {
            //检查使用[居中占位]还是普通占位
            if(typeid(*hline) != typeid(AnchorObj_HLine)) { //其他类型的标线不插入
                hline = hline->logic_lastHLine ? hline->logic_lastHLine : hline->hline->as<AnchorObj_HLine*>();
                continue;
            }
            tryInsertPH_onHLineLeft(hline,left_min+page->page_left_margin,right_min+page->page_left_margin);
            hline = hline->logic_lastHLine;
        }

        // auto hline = bottom_hline;
        // while(hline) {
        //     //检查使用[居中占位]还是普通占位
        //     if(hline->leftObj) {
        //         float left_rect_width = hline->leftObj->x - vline_left->x;
        //         qDebug() << "left_rect_width: " << left_rect_width;
        //         if(left_rect_width > 1e-6) { //左边有空白
        //             float solid_width = hline->rightObj->getRightX() - hline->leftObj->x;
        //             float entire_width = right_min + page->page_left_margin - vline_left->x;
        //             qDebug() << "solid_width: " << solid_width << ", entire_width: " << entire_width;
        //             if(abs(left_rect_width*2+solid_width - entire_width) < 3.0) {
        //                 //居中处理
        //                 AnchorObj_JZRect *jzrect = new AnchorObj_JZRect;
        //                 SoupleManager::registerObj(jzrect);
        //                 hline->insertOnLeft(jzrect);
        //                 qDebug() << "插入居中";
        //                 //AnchorObj_PHRight *ph = new AnchorObj_PHRight; //换行和居中配套使用
        //                 //hline->insertOnRight()
        //             } else {
        //                 AnchorObj_PHRect *phrect = new AnchorObj_PHRect;
        //                 SoupleManager::registerObj(phrect);
        //                 phrect->width = left_rect_width;
        //                 hline->insertOnLeft(phrect);
        //                 qDebug() << "插入占位矩形";
        //             }
        //         }
        //     }
        //     hline = hline->logic_lastHLine;
        // }

        hline = bottom_hline;

        while(hline != top_hline) {
            //检查<上一行>是否需要插入换行符
            if(typeid(*hline) != typeid(AnchorObj_HLine)) { //其他类型的标线不插入
                hline = hline->logic_lastHLine ? hline->logic_lastHLine : hline->hline->as<AnchorObj_HLine*>();
                continue;
            }
            tryLastHlineAppendBreak(hline,right_min+page->page_left_margin);
            tryInsertPHLeft(hline); //尝试插入PH_Left
            hline = hline->logic_lastHLine;
        }

        shared_ptr<Souple_Area> area = make_shared<Souple_Area>(Souple_Area{
                        .left = left_min,
                        .right = right_min,
                        .ex_left = leftest,
                        .ex_right = rightest,
                        .top_y = area_top_y+page->page_top_margin,
                        .bottom_y = area_bottom_y+page->page_top_margin, //bottom_y
                        .top_line = top_hline,
                        .bottom_line = bottom_hline
                });

        //qDebug() << "添加souple_area: top_y=" << row_top << "bottom_y=" << row_bottom;

        //到此，确定了该锚定区域
        //现在需要找到该区域的上方的锚定区域的底部的HLine
        //.....如何确定双栏文字呢？暂时忽略.....

        //Souple_Area *top_area = 0;

        //找到上方的Souple_Area
        //可能有一个或多个；优先找[left,right]与本区域交错或覆盖的；
        //找不到再找[ex-left,ex-right]交错或覆盖的
        { //开始找上方的area
            //using rw = std::reference_wrapper<Souple_Area>;
            findTopArea(page->areas,area);
            if( ! area->top_area && page->page_index > 0) {
                //在本页中没找到，那么就往上一页找
                findTopArea(old_page->areas,area,true);
                if(area->top_area && area->top_line) {
                    //找到，还需要特别检查是否逻辑接续（因为跨页，可能它们两个区域应该是逻辑接续的）
                    //重新设定top_margin，因为跨页，需要减去两页的空白高度
                    auto last_page = SoupleManager::getPageByIndex(page->page_index - 1);

                    area->top_line->topMargin -=
                        (last_page->bottomMargin + export_page->topMargin);

                    // 可是事实上呢？


                    //最上面的area，需要设置页顶距
                    area->top_line->page_topMargin = max(0.0f,area->top_line->getContentTop()
                                                                - export_page->getTopLineY());

                    //如果...，则认为该行内容与上一页末行接续，topMargin直接继承...。
                    float def_margin = area->top_area->bottom_line->topMargin;
                    if(area->top_line->page_topMargin < 1.0
                        && area->top_line->topMargin < def_margin) {
                        area->top_line->topMargin = def_margin;
                    }

                    qDebug() << "跨页area.page_topMargin=" << area->top_line->page_topMargin;
                    if(typeid(*(area->top_line)) == typeid(AnchorObj_HLine)
                        && typeid(*(area->top_area->bottom_line)) == typeid(AnchorObj_HLine)) {
                        if(abs(area->top_area->left - area->left) < 10.0f
                            && abs(area->top_area->right - area->right) < 25.0f) {
                            area->top_line->setLogicLastHLine(area->top_area->bottom_line); //逻辑接续
                            //需要检查上一行是否需要插入换行符
                            tryLastHlineAppendBreak(area->top_line,area->top_area->right);
                            //需要检查是否需要插入PH_Left
                            tryInsertPHLeft(area->top_line);
                        }
                    }
                }
            }

        } //END 找上方的area

        page->areas.push_back(std::move(area)); //记录本锚定区域

        i = ori_i + 1;
    }//扫描各水平锚定块 END

    return;
}




//找出所有all_objs[index]在onWhat方向上的邻接obj
//要求all_objs已按照规定排序过
// std::vector<shared_ptr<Pdf2Souple::PDFOBJ>> Pdf2Souple::imp_findPDFOBJAlignON(int onWhat,std::vector<PDFOBJ*>& all_objs,int index,
//             uchar sign)
// {
//     std::vector<Pdf2Souple::PDFOBJ*> list;
//     PDFOBJ* obj = all_objs[index];
//     switch(onWhat) {
//     case Pdf2Souple::Bottom: {
//         for(int i = index+1; i < all_objs.size(); ++i) {
//             auto& rect2 = all_objs[i]->rect;
//             //if(rect2.)
//         }
//         break;
//     }
//     }
//     return list;
// }


//获取text的真实Bound,由tight_rect:最小边界
QRectF Pdf2Souple::getTextFullBound(QFontMetricsF fm,const QString& text,const QRectF& tight_rect)
{
    //qDebug() << "getTextFullBound(text=" << text;
    auto fm_tight_rect = fm.tightBoundingRect(text);
    auto fm_full_rect = fm.boundingRect(text);
    // qDebug() << "pdf-rect.width = " << tight_rect.width();
    // qDebug() << "pdf-rect.height = " << tight_rect.height();
    // qDebug() << "pdf-rect:   = " << tight_rect;
    // qDebug() << "fm-tight-rect.width = " << fm_tight_rect.width();
    // qDebug() << "fm-tight-rect.height = " << fm_tight_rect.height();
    // qDebug() << "fm-rect.width = " << fm_full_rect.width();
    // qDebug() << "fm-rect.height = " << fm_full_rect.height();
    // qDebug() << "fm-tight-rect: " << fm_tight_rect;
    // qDebug() << "fm-rect: " << fm_full_rect;

    //float height_bias = (fm_tight_rect.height() - tight_rect.height()) / 2;

    float top_margin = fm_tight_rect.top() + fm.ascent() - 0;
    float bottom_margin = fm.height() - fm_tight_rect.bottom() - fm.ascent();
    float left_margin = fm_tight_rect.left() ;
    //float right_margin = fm.horizontalAdvance(text) - fm_tight_rect.right();
    //fm.ascent()
    //FPDFText_
    //qDebug() << "margins: " << left_margin << "," << right_margin << "," << top_margin << "," << bottom_margin;
    return {tight_rect.left(),tight_rect.top()-top_margin,
            fm.horizontalAdvance(text),tight_rect.height()+top_margin+bottom_margin};
}


shared_ptr<Pdf2Souple::PDFOBJ> Pdf2Souple::readPdfObj(FPDF_PAGEOBJECT fpdf_pageobj,FPDF_TEXTPAGE textpage
                                           ,float page_width,float page_height)
{
    static unsigned short buf[512];
    static char buf2[128];

    float left,right,top,bottom;
    //FPDFText_GetLooseCharBox()
    FPDFPageObj_GetBounds(fpdf_pageobj,&left,&bottom,&right,&top); //获取obj的占据区域

    left = Helper::point2pixel(left);
    right = Helper::point2pixel(right);
    top = Helper::point2pixel(top);
    bottom = Helper::point2pixel(bottom);
    top = page_height - top;
    bottom = page_height - bottom;

    switch(FPDFPageObj_GetType(fpdf_pageobj)) {
    case 1: { //Text
        break; //文本由imp_dealText单独处理，这里忽略
    }
    case 3: { //Image
        auto obj_image =  make_shared<PDFOBJ_IMAGE>();
        auto fpdf_image = FPDFImageObj_GetBitmap(fpdf_pageobj);
        int width = FPDFBitmap_GetWidth(fpdf_image);
        int height = FPDFBitmap_GetHeight(fpdf_image);
        int format = FPDFBitmap_GetFormat(fpdf_image);
        void *buffer = FPDFBitmap_GetBuffer(fpdf_image);
        int stride = FPDFBitmap_GetStride(fpdf_image);
        QImage::Format qfm;
        switch(format) {
        case FPDFBitmap_BGR: qfm = QImage::Format_BGR888; break;
        case FPDFBitmap_BGRA: qfm = QImage::Format_ARGB32; break; //可能有误
        case FPDFBitmap_Gray: qfm = QImage::Format_Grayscale8; break; //可能有误
        default: qfm = QImage::Format_BGR888;
        }
        QImage image((uchar*)buffer,width,height,qfm);
        //qDebug() << "读取FPDF_BITMAP: " << width << " x " << height << " stride = " << stride;
        //qDebug() << "读取QImage: " << image;
        obj_image->source = "cache/pdf_image/"+
                            QString::number(Helper::cache_image_index++)+
                            ".png";
        image.save(obj_image->source);//存储为文件
        obj_image->rect = {left,top,right-left,bottom-top};

        FPDFBitmap_Destroy(fpdf_image); //! [2025/7/15 补充 因为fpdf_image内存由调用者来管理！]

        return obj_image;
      }
    case FPDF_PAGEOBJ_PATH: {
        int path_segment_num = FPDFPath_CountSegments(fpdf_pageobj); //路径由多少个片段组成
        auto obj_path = make_shared<PDFOBJ_PATH>();

        obj_path->rect = {left,top,right-left,bottom-top};

        // 计算紧边框
        float tight_x1 = 1e9,tight_y1 = 1e9,
            tight_x2 = -1e9,tight_y2 = -1e9;

        //qDebug() << "rect = " << obj_path->rect;

        FS_MATRIX matrix;
        FPDFPageObj_GetMatrix(fpdf_pageobj,&matrix); //获取变换矩阵

        //qDebug() << "mat: " << matrix.a << matrix.b << matrix.c << matrix.d << matrix.e << matrix.f;

        float tx = Helper::point2pixel(matrix.e),
            ty = Helper::point2pixel(matrix.f);

        unsigned int R,G,B,A;
        FPDFPageObj_GetFillColor(fpdf_pageobj,&R,&G,&B,&A);
        obj_path->fillColor.setRgb(R,G,B,A);
        FPDFPageObj_GetStrokeColor(fpdf_pageobj,&R,&G,&B,&A);
        obj_path->strokeColor.setRgb(R,G,B,A);
        FPDFPageObj_GetStrokeWidth(fpdf_pageobj,&obj_path->lineWidth);

        int fillMode;
        int isStroke;
        FPDFPath_GetDrawMode(fpdf_pageobj,&fillMode,&isStroke); //获取渲染模式
        switch(fillMode) {
        case FPDF_FILLMODE_NONE: obj_path->fill = false; break;
        case FPDF_FILLMODE_ALTERNATE: obj_path->fill = true; break;
        case FPDF_FILLMODE_WINDING: obj_path->fill = true; break;
        }

        obj_path->stroke = isStroke;

        bool like_noFill = false, like_noStroke = false;

        if(! obj_path->fill
            || obj_path->rect.width() < 0.01
            || obj_path->rect.height() < 0.01) like_noFill = true;
        else if(obj_path->fillColor.alphaF() < 0.01) like_noFill = true;
        //透明的颜色仿佛就是没有填充；那么细的形状，填充了也看不见，就好像没填充。

        if(! obj_path->stroke || obj_path->lineWidth < 0.01) like_noStroke = true;
        else if(obj_path->strokeColor.alphaF() < 0.01) like_noStroke = true;
        //透明的颜色仿佛就是没有描边，描边宽度那么小，就好像没有描边

        if(like_noFill && like_noStroke) {
            qDebug() << "不填充也不描边~完全隐藏~当作不存在";
            return nullptr; //不填充也不描边~完全隐藏~当作不存在
        }

        // 对path做一些trim
        if(like_noStroke) {
            obj_path->lineWidth = 0.0f;
            obj_path->stroke = false;
            obj_path->strokeColor = QColor::fromRgb(0,0,0,0);
        }

        if(like_noFill) {
            obj_path->fill = false;
            obj_path->fillColor = QColor::fromRgb(0,0,0,0);
        }

#if Print_Path_Read
        qDebug() << "new path: " << obj_path->rect;
        qDebug() << "lineWidth: " << obj_path->lineWidth;
#endif

        for(int i = 0; i < path_segment_num; ++i)
        {
            auto path_segment = FPDFPath_GetPathSegment(fpdf_pageobj,i);
            int type = FPDFPathSegment_GetType(path_segment);
            float sx,sy;

            switch(type) {
            case FPDF_SEGMENT_MOVETO: { //移动画笔 moveTo
                FPDFPathSegment_GetPoint(path_segment,&sx,&sy);
                sx = Helper::point2pixel(sx)*matrix.a+tx,
                    sy = page_height - Helper::point2pixel(sy)*matrix.d - ty;
#if Print_Path_Read
                qDebug() << "moveTo " << sx << "," << sy;
#endif
                obj_path->list_path_actions.push_back(Path_Action(Path_Action::MoveTo,sx,sy));
                break;
            } //0:69  1:106.5  2:151.5
            case FPDF_SEGMENT_LINETO: { //直线 lineTo
                FPDFPathSegment_GetPoint(path_segment,&sx,&sy);
                sx = Helper::point2pixel(sx)*matrix.a+tx,
                    sy = page_height - Helper::point2pixel(sy)*matrix.d - ty;
#if Print_Path_Read
                qDebug() << "lineTo " << sx << "," << sy;
#endif
                obj_path->list_path_actions.push_back(Path_Action(Path_Action::LineTo,sx,sy));
                break;
            }
            case FPDF_SEGMENT_BEZIERTO: { //贝塞尔曲线
                //贝塞尔曲线需要3个点，即这里要读取3次
#if Print_Path_Read
                qDebug() << "bezierTo";
#endif
                FPDFPathSegment_GetPoint(path_segment,&sx,&sy);
                sx = Helper::point2pixel(sx)*matrix.a+tx,
                    sy = page_height - Helper::point2pixel(sy)*matrix.d - ty;
                obj_path->list_path_actions.push_back({Path_Action::BezierTo,sx,sy});
                //qDebug() << "bezier0: " << sx << "," << sy;
                for(uint8_t j = 0; j < 2; ++j){
                    path_segment = FPDFPath_GetPathSegment(fpdf_pageobj,++i);
                    FPDFPathSegment_GetPoint(path_segment,&sx,&sy);
                    sx = Helper::point2pixel(sx)*matrix.a+tx,
                        sy = page_height - Helper::point2pixel(sy)*matrix.d - ty;
                    obj_path->list_path_actions.push_back({Path_Action::BezierTo,sx,sy});
                    //qDebug() << "bezier+: " << sx << "," << sy;
                }
                break;
            }
            } //switch END

            tight_x1 = std::min(tight_x1,sx);
            tight_y1 = std::min(tight_y1,sy);
            tight_x2 = std::max(tight_x2,sx);
            tight_y2 = std::max(tight_y2,sy);
        }

        obj_path->t_rect = QRectF{tight_x1,tight_y1,tight_x2-tight_x1,tight_y2-tight_y1};

        return obj_path;
    } //Read Path End
    } //Read Obj End

    return nullptr;

    //obj->rect = {left,top,right-left,bottom-top};
    // if(auto text_obj = dynamic_cast<PDFOBJ_TEXT*>(obj); text_obj) {
    //     //需要计算其真实bounds, full_bound
    //     QFontMetricsF fm(text_obj->toFont());
    //     obj->rect = getTextFullBound(fm,text_obj->text,text_obj->rect);
    // }
}

//dealText:处理文本层
void Pdf2Souple::imp_dealText(FPDF_TEXTPAGE textpage,std::vector<shared_ptr<PDFOBJ>> &list,float page_width,float page_height)
{
    //许多pdf文本均为单字符割裂，不如直接按char处理
    int char_count = FPDFText_CountChars(textpage);
    unsigned short buf[4];
    char fontname_buf[64], family_buf[64];
    //uint8_t *font_data = new uint8_t[1024*1024*20]; //申请5MB内存

    struct Text {
        QString text;
        QFont font;
        FS_RECTF rect;
        QColor stroke_color,fill_color;
        float strokeWidth;
        bool isFill,isStroke;
        float ascent_y; //基线y坐标
        shared_ptr<PDFOBJ> toPDFOBJ() {
            auto obj = make_shared<PDFOBJ_TEXT>();
            obj->render_id = Helper::Layer_Z::Text; //文本层置顶。
            obj->text = text;
            obj->font = font;
            QFontMetricsF fm(font);
            //obj->ascent = ascent;
            obj->ascent = fm.ascent();
            //obj->descent = fm.descent();
            obj->stroke_color = stroke_color;
            obj->fill_color = fill_color;
            obj->isFill = isFill;
            obj->isStroke = isStroke;
            obj->strokeWidth = strokeWidth;
            //校正rect
            // float fontHeight = fm.height();
            // if(rect.bottom - rect.top > fontHeight) { //矩形区域比字体高度要高
            //     float ay = rect.bottom - rect.top - fontHeight;
            //     rect.top += ay;
            //     //rect.bottom -= ay/2;
            // }
            obj->rect = {rect.left,ascent_y - fm.ascent(),rect.right - rect.left,fm.height()};
            //qDebug() << "添加TEXT: " << text;
            //qDebug() << "该TEXT的区域为: " << obj->rect;
            //qDebug() << "该TEXT的基线为: " << obj->ascent;
            return obj;
            //PID: x: 106.5 y: 399.2~427.3799 by:19.375  H: 28.1799
            //控制算法: x: 138.858 y: 400.28~426.1  by:21.1563 H: 25.82
        }
    };

    Text *curText = 0;

    for(int i = 0; i < char_count; ++i)
    {
        FS_RECTF rect;
        FS_MATRIX matrix;
        float fontsize;
        float strokeWidth;
        QFont font;
        int weight, flag;
        QString str;
        float ascent;
        double ascent_y;
        struct {
            uint32_t R,G,B,A;
        } strokeColor,fillColor;

        FPDFText_GetLooseCharBox(textpage,i,&rect); //获取“宽松”box

        rect.left = Helper::point2pixel(rect.left);
        rect.right = Helper::point2pixel(rect.right);
        rect.top = Helper::point2pixel(rect.top);
        rect.bottom = Helper::point2pixel(rect.bottom);
        rect.top = page_height - rect.top;
        rect.bottom = page_height - rect.bottom;

        FPDFText_GetText(textpage,i,1,buf);

        //FPDF

        //qDebug() << "char: " << QString::fromUtf16(buf) << " box=" << rect.left << "," << rect.right
        //         << "," << rect.top << "," << rect.bottom;
        str = QString::fromUtf16(buf);
        if(str == "\n" || str == "\r"
            || str == ' ') continue; //space \r \n是pdfium自己给出的提示字符，无意义
        //又注：事实表明，pdf文档中几乎不会出现有意义的空格。就是真的是空格，[也没关系(无影响)]。
        fontsize = FPDFText_GetFontSize(textpage,i);
        FPDFText_GetMatrix(textpage,i,&matrix);
        FPDFText_GetStrokeColor(textpage,i,&strokeColor.R,&strokeColor.G,&strokeColor.B,&strokeColor.A);
        FPDFText_GetFillColor(textpage,i,&fillColor.R,&fillColor.G,&fillColor.B,&fillColor.A);

        //FPDFFont_G

        weight = FPDFText_GetFontWeight(textpage,i);

        if(weight < 0) continue;

        //FPDFText_
        //FPDFText

        FPDFText_GetFontInfo(textpage,i,fontname_buf,64,&flag);

        auto text_obj = FPDFText_GetTextObject(textpage,i);

        FPDFPageObj_GetStrokeWidth(text_obj,&strokeWidth);

        FPDF_FONT ffont = FPDFTextObj_GetFont(text_obj);

        FPDFFont_GetFamilyName(ffont,family_buf,64);

        //FPDFFont_GetBaseFontName(ffont,fontname_buf,64);

        //敬请注意，读取时不要使用Qt按照字体解析的基线位置，一定要使用pdfium告诉我们的基线位置
        //原因可能是pdfium支持小数级别的pointSize
        //FPDFFont_GetAscent(ffont,fontsize * matrix.d,&ascent);
        //ascent = Helper::point2pixel(ascent); //注意转为像素

        double __x;
        FPDFText_GetCharOrigin(textpage,i,&__x,&ascent_y); //获取基线y坐标
        ascent_y = page_height - Helper::point2pixel(ascent_y); //坐标转换

        int fweight = FPDFFont_GetWeight(ffont);
        //FPDFPageObj_


        font.setPointSizeF(fontsize * matrix.a);


        //if(weight > 400) font.setBold(true);

        //FPDFText

        if(weight < 150) font.setWeight(QFont::Thin);
        else if(weight < 250) font.setWeight(QFont::ExtraLight);
        else if(weight < 350) font.setWeight(QFont::Light);
        else if(weight < 450) font.setWeight(QFont::Normal);
        else if(weight < 550) font.setWeight(QFont::Medium);
        else if(weight < 650) font.setWeight(QFont::DemiBold);
        else if(weight < 750) font.setWeight(QFont::Bold);
        else if(weight < 850) font.setWeight(QFont::ExtraBold);
        else font.setWeight(QFont::Black);

        QString bfn = QString::fromUtf8(fontname_buf);

        bfn = bfn.split('+').last();  //去掉子集标识

        // 计算替代字体
        if(Helper::original_font_set.contains(bfn)) {
            font.setFamily(bfn); //系统原本就有的字体族
        } else {
            QStringList family_list;
            family_list.push_back(bfn);
            if(Helper::font_substituions.contains(bfn)) {
                auto& sub = Helper::font_substituions[bfn];
                family_list.push_back(sub.substituion_family);
                if( ! QFontDatabase::hasFamily(bfn))
                { //如果这个内嵌字体未解析成功，则可以考虑字体附加属性
                    if(sub.bold) font.setBold(true);
                    if(sub.light) font.setWeight(QFont::Light);
                    if(sub.italic) font.setItalic(true);
                }
            }
            font.setFamilies(family_list); //双字体
            // qDebug() << "多字体族：" << family_list;
        }

        //qDebug() << "strokeColor = " << strokeColor.R << strokeColor.G << strokeColor.B << strokeColor.A;
        //qDebug() << "fillColor = " << fillColor.R << fillColor.G << fillColor.B << fillColor.A;

        //[ 暂时不考虑文字还能[虚线]描边 ]
        int dash_count = FPDFPageObj_GetDashCount(text_obj);
        //qDebug() << "dash_count = " << dash_count;


        bool isFill;
        bool isStroke;

        //获取渲染模式
        auto render_mode = FPDFTextObj_GetTextRenderMode(text_obj);

        // typedef enum {
        //     FPDF_TEXTRENDERMODE_UNKNOWN = -1,
        //     FPDF_TEXTRENDERMODE_FILL = 0,
        //     FPDF_TEXTRENDERMODE_STROKE = 1,
        //     FPDF_TEXTRENDERMODE_FILL_STROKE = 2,
        //     FPDF_TEXTRENDERMODE_INVISIBLE = 3,
        //     FPDF_TEXTRENDERMODE_FILL_CLIP = 4,
        //     FPDF_TEXTRENDERMODE_STROKE_CLIP = 5,
        //     FPDF_TEXTRENDERMODE_FILL_STROKE_CLIP = 6,
        //     FPDF_TEXTRENDERMODE_CLIP = 7,
        //     FPDF_TEXTRENDERMODE_LAST = FPDF_TEXTRENDERMODE_CLIP,
        // } FPDF_TEXT_RENDERMODE;

        if(render_mode == FPDF_TEXTRENDERMODE_FILL) isFill = true, isStroke = false;
        else if(render_mode == FPDF_TEXTRENDERMODE_STROKE) isFill = false, isStroke = true;
        else if(render_mode == FPDF_TEXTRENDERMODE_FILL_STROKE) isFill = isStroke = true;
        else {
            isFill = isStroke = true;
        }
        //qDebug() << "isFill: " << isFill;
        //qDebug() << "isStroke: " << isStroke;
        int angle;
        FPDFFont_GetItalicAngle(ffont,&angle);

        if(angle > 0 || matrix.c > 0) font.setItalic(true); //斜体

        //qDebug() << "char " << QString::fromUtf16(buf);

        // qDebug() << "strokeWidth = " << strokeWidth;
        // qDebug() << "font-name = " << QString::fromUtf8(fontname_buf);
        // qDebug() << "angle = " << angle;

        //qDebug() << "matrix: " << "a=" << matrix.a<< "b=" << matrix.b<< "c=" << matrix.c<< "d=" << matrix.d
        //          << "e=" << matrix.e << "f=" << matrix.f;

        if(! curText) {
            curText = new Text{.text=str,.font = font,.rect = rect,
                        .stroke_color = QColor::fromRgb(strokeColor.R,strokeColor.G,strokeColor.B,strokeColor.A),
                        .fill_color = QColor::fromRgb(fillColor.R,fillColor.G,fillColor.B,fillColor.A),
                        .strokeWidth = strokeWidth,
                        .isFill = isFill,.isStroke = isStroke,.ascent_y = (float)ascent_y};
        } else
            if(font == curText->font &&
                   strokeColor.R == curText->stroke_color.red() &&
                   strokeColor.B == curText->stroke_color.blue() &&
                   strokeColor.G == curText->stroke_color.green() &&
                   strokeColor.A == curText->stroke_color.alpha()
                   && curText->rect.right + TEXT_MERGE_OFFSET >= rect.left //接续
                   && abs(curText->rect.top - rect.top) < 0.5
                   && abs(curText->rect.bottom - rect.bottom) < 0.5
                   && isFill == curText->isFill && isStroke == curText->isStroke
                   && abs(strokeWidth - curText->strokeWidth) < 1e-3
                )
        {
            curText->rect.right = rect.right; //延长右边界
            curText->text += QString::fromUtf16(buf);
        } else { //不连续
            list.push_back(curText->toPDFOBJ());
            delete curText;
            curText = new Text{.text=str,.font = font,.rect = rect,
                    .stroke_color = QColor::fromRgb(strokeColor.R,strokeColor.G,strokeColor.B,strokeColor.A),
                            .fill_color = QColor::fromRgb(fillColor.R,fillColor.G,fillColor.B,fillColor.A),
                            .strokeWidth = strokeWidth,
                            .isFill = isFill,.isStroke = isStroke,.ascent_y = (float)ascent_y};
        }
    }
    if(curText) {
        list.push_back(curText->toPDFOBJ());
        delete curText;
    }

    //delete[] font_data;

}

//处理内嵌字体，并把字体文件提取出来
void Pdf2Souple::dealEmbedFont(FPDF_FONT ffont)
{
    char *font_data_buf = new char[1024*1024*5];
    if(FPDFFont_GetIsEmbedded(ffont)) {
        //FPDFFont_GetFontData(ffont,)
    }
    delete[] font_data_buf;
}

void Pdf2Souple::findTopArea(const std::vector<shared_ptr<Souple_Area>>& list_area,
                             shared_ptr<Souple_Area> area, bool span_page)
{
    std::vector<shared_ptr<Souple_Area>> to_select_list; //可选择的area列表
    QSet<shared_ptr<Souple_Area>> top_of_top_areas; //上面area的上面area
    for(int i = list_area.size() - 1; i >= 0; --i) {
        auto area2 = list_area[i];
        if(top_of_top_areas.contains(area2))
            continue; //顶部area的顶部area...不选择
        if((area2->bottom_y-6.0 <= area->top_y || span_page)
            && (area2->ex_left <= area->right || area2->ex_right >= area->left)) { //area2在area上方或跨页(span_page)
            to_select_list.push_back(area2);
            //qDebug() << "findTopArea添加备选: " << area2->ex_left << ", " << area2->ex_right;
            auto a = area2->top_area;
            while(a) { //记录上上面的area
                top_of_top_areas.insert(a);
                a = a->top_area;
            }
            //break;
        }
    }

    if(to_select_list.size() > 0) {
        //找到最优top_area
#ifdef Q_OS_WIN32
        ranges::sort(to_select_list,[area](auto& a1, auto& a2){
            bool i1 = a1->left <= area->right && a1->right >= area->left;
            bool i2 = a2->left <= area->right && a2->right >= area->left;
            if(i1^i2) return i1; //谁非拓展交错，谁优先
            return a1->left < a2->left;
        });
#else
        sort(to_select_list.begin(),to_select_list.end(),[area](auto& a1, auto& a2){
            bool i1 = a1->left <= area->right && a1->right >= area->left;
            bool i2 = a2->left <= area->right && a2->right >= area->left;
            if(i1^i2) return i1; //谁非拓展交错，谁优先
            return a1->left < a2->left;
        });
#endif

        area->top_area = to_select_list[0];
        area->top_line->setAnchorLastHLine(area->top_area->bottom_line); //锚定上标线
        area->top_line->topMargin = area->top_line->getContentTop() - area->top_area->bottom_line->getContentBottom();
    }
}

/**
 * @brief Pdf2Souple::analyseHeader 解析页面的页眉页脚。删除涉及的PDFOBJ，创建相应的Souple对象。
 * @param page: 正在解析的pdf页面。
 */
void Pdf2Souple::analyseHeader(std::shared_ptr<PDFPage> page)
{
    bool hasHeader = false, hasFooter = false;
    float header,footer;

    //解析页眉
    if((PDF_Load_Advice::spec_header.contains(page->page_index)
             ? PDF_Load_Advice::spec_header[page->page_index] > 0
                        : PDF_Load_Advice::hasUniHeader)) {
        hasHeader = true;
        header = PDF_Load_Advice::spec_header.contains(page->page_index) ?
                     PDF_Load_Advice::spec_header[page->page_index]
                        : PDF_Load_Advice::header_margin;

        qDebug() << "页眉=" << header;

        vector<shared_ptr<PDFOBJ>> header_objs; //位于页眉的objs
        for(int i = 0; i < page->all_objs.size(); ++i) {
            if(page->all_objs[i]->rect.bottom() - HEADER_JUDGE_OFFSET <= header) {
                header_objs.push_back(page->all_objs[i]);
                page->all_objs[i] = {}; //删除
            }
        }

        sort(header_objs.begin(), header_objs.end(), _sort_func); //排序
        QList<HBlock> blocks;
        createHBlocks(header_objs,page->page_width,blocks); //创建水平块
        //创建自由标线+锚定对象，以及自由对象（重叠对象）
        for(auto& block : blocks) {
            if(block.objs.empty()) continue;
            HorLine_Base *hline = new HorLine_Base; //创建自由标线
            hline->x = block.rect.left();
            //hline->y = block.rect.bottom() + page_top_margin;
            if(auto text = dynamic_cast<PDFOBJ_TEXT*>(block.objs.front().get()); text) {
                //如果第一个对象是文本，那么按照它的基线对齐
                hline->y = text->ascent + text->rect.top() + page->page_top_margin;
            } else { //否则按照block的底部对齐
                hline->y = block.rect.bottom() + page->page_top_margin;
            }
            SoupleManager::registerObj(hline);
            float __unused;
            for(auto obj : block.objs) {
                auto aobj = obj->toAnchorObj(hline,page->page_top_margin,__unused);
                hline->insertOnRight(aobj);
            }
        }
    }

    //解析页脚
    if(PDF_Load_Advice::spec_footer.contains(page->page_index) ? PDF_Load_Advice::spec_footer[page->page_index] > 0
                                                        : PDF_Load_Advice::hasUniFooter) {
        hasFooter = true;
        footer = PDF_Load_Advice::spec_footer.contains(page->page_index) ?
                     PDF_Load_Advice::spec_footer[page->page_index] : PDF_Load_Advice::footer_margin;
        vector<shared_ptr<PDFOBJ>> footer_objs; //位于页眉的objs
        for(int i = 0; i < page->all_objs.size(); ++i) {
            if(!page->all_objs[i]) continue; //解析页眉时破坏了一些obj
            if(page->all_objs[i]->rect.top() >= page->page_height - footer) {
                footer_objs.push_back(page->all_objs[i]);
                page->all_objs[i] = {};
            }
        }

        sort(footer_objs.begin(), footer_objs.end(), _sort_func); //排序

        QList<HBlock> blocks;
        createHBlocks(footer_objs,page->page_width,blocks); //创建水平块
        //创建自由标线+锚定对象，以及自由对象（重叠对象）
        for(auto& block : blocks) {
            if(block.objs.empty()) continue;
            HorLine_Base *hline = new HorLine_Base; //创建自由标线
            hline->x = block.rect.left();
            //hline->y = block.rect.bottom() + page_top_margin;
            if(auto text = dynamic_cast<PDFOBJ_TEXT*>(block.objs.front().get()); text) {
                //如果第一个对象是文本，那么按照它的基线对齐
                hline->y = text->ascent + text->rect.top() + page->page_top_margin;
            } else { //否则按照block的底部对齐
                hline->y = block.rect.bottom() + page->page_top_margin;
            }
            SoupleManager::registerObj(hline);
            float __unused;
            for(auto obj : block.objs) {
                auto aobj = obj->toAnchorObj(hline,page->page_top_margin,__unused);
                hline->insertOnRight(aobj);
            }
        }

    }

    //删除位于页眉页脚的obj
    int _n2 = 0;
    for(auto ptr : page->all_objs)
        if(ptr) page->all_objs[_n2++] = ptr;

    page->all_objs.resize(_n2);
}

void Pdf2Souple::tryInsertPH_onHLineLeft(HorLine_Base* hline)
{
    if(hline->canBe<TableLine>()) return;
    if(hline->leftObj) {
        float left_rect_width = hline->leftObj->x - hline->x;
        if(left_rect_width > 1e-2) { //左边有空白
            float solid_width = hline->rightObj->getRightX() - hline->leftObj->x;
            float entire_width = hline->width;
            if(solid_width*1.8 < entire_width
                    && abs(left_rect_width*2+solid_width - entire_width) < 3.0) {
                //居中处理
                AnchorObj_JZRect *jzrect = new AnchorObj_JZRect;
                SoupleManager::registerObj(jzrect);
                hline->insertOnLeft(jzrect);
            } else {
                AnchorObj_Glue *glue = new AnchorObj_Glue;
                glue->glue_left = true;
                glue->width = left_rect_width;
                hline->insertOnLeft(glue);
                SoupleManager::registerObj(glue);
            }
        }
    }
}

// 在左边插入占位符
void Pdf2Souple::tryInsertPH_onHLineLeft(AnchorObj_HLine* hline,float ori_left,float ori_right)
{
    if(hline->leftObj) {
        float left_rect_width = hline->leftObj->x - hline->leftLine->x;
        if(left_rect_width > 1e-2) { //左边有空白
            float solid_width = hline->rightObj->getRightX() - hline->leftObj->x;
            float entire_width = ori_right - hline->leftLine->x;
            //qDebug() << "居中：" << left_rect_width << solid_width << entire_width;
            if( solid_width*1.8f < entire_width &&
                abs(left_rect_width*2+solid_width - entire_width) < 3.0) {
                //居中处理
                AnchorObj_JZRect *jzrect = new AnchorObj_JZRect;
                SoupleManager::registerObj(jzrect);
                hline->insertOnLeft(jzrect);
                qDebug() << "插入居中";
                //AnchorObj_PHRight *ph = new AnchorObj_PHRight; //换行和居中配套使用
                //hline->insertOnRight()
            } else {
                // AnchorObj_PHRect *phrect = new AnchorObj_PHRect;
                // SoupleManager::registerObj(phrect);
                // phrect->width = left_rect_width;
                // hline->insertOnLeft(phrect);
                //qDebug() << "插入占位矩形";
                AnchorObj_Glue *glue = new AnchorObj_Glue;
                glue->glue_left = true;
                glue->width = left_rect_width;
                hline->insertOnLeft(glue);
                SoupleManager::registerObj(glue);
            }
        }
    }
}

/**
 * @brief Pdf2Souple::layoutHLine 为一条AnchorObj_HLine布局
 * @param hline      待布局的水平标线
 * @param last_hline 上一条水平标线
 * @param bk 布局内容
 * @param leftLine rightLine 左右标线
 */
void Pdf2Souple::layoutHLine(std::shared_ptr<PDFPage> page, AnchorObj_HLine* hline,AnchorObj_HLine*& last_hline,
                 HBlock& bk,AnchorObj_VLine* leftLine,AnchorObj_VLine* rightLine)
{
    if(last_hline) {
        last_hline->logic_nextHLine = hline;
        hline->logic_lastHLine = last_hline;
        hline->hline = last_hline;
        last_hline->anchor_nextHLine.push_back(hline);
    }
    hline->leftLine = leftLine;
    hline->rightLine = rightLine;
    //检查bk的第一个obj类型
    auto obj = bk.objs.front();
    if(auto text_obj = dynamic_pointer_cast<PDFOBJ_TEXT>(obj); text_obj) {
        //文本类型
        //默认按照基线对齐
        hline->y = page->page_top_margin + text_obj->rect.top() + text_obj->ascent; //对齐其基线
    } else {
        //否则，默认采用底部对齐
        hline->y = page->page_top_margin + obj->rect.bottom();
    }
    hline->page = page->souple_page;
    //计算contentTop和contentBottom
    hline->contentTop = bk.rect.top() + page->page_top_margin - hline->y;
    hline->contentBottom = bk.rect.bottom() + page->page_top_margin - hline->y;

    if(last_hline) {
        hline->topMargin = hline->getContentTop() - last_hline->getContentBottom();
    }

    float adjust_width = 0; //准确的AnchorObj宽度
    //float adjust_content_top = 0, adjust_content_bottom = 0; //准确的content_top和bottom
    //下面开始为各个obj创建Souple元素
    float last_right = leftLine->x;
    float left_rect_width = 0;
    //bool first = true;
    for(auto[i,obj] : bk.objs | views::enumerate) {
        float left_margin = obj->rect.left() - last_right; //到左边右边缘的距离
        if(left_margin > PHRECT_MIN_WIDTH) {
            if(i == 0) { //第一个obj，那么我们先不建立ph_rect,因为可能要插入[居中占位]
                left_rect_width = left_margin;
                adjust_width += left_margin;
            } else {
                //需要创建 水平占位符PH_Rect
                AnchorObj_PHRect *phrect = new AnchorObj_PHRect;
                //if constexpr(Register == true) {
                    SoupleManager::registerObj(phrect);
                //}
                phrect->width = left_margin;
                hline->insertOnRight(phrect);
                adjust_width += left_margin;
            }
        }
        float obj_adjust_width;
        auto anchorObj = obj->toAnchorObj(hline,page->page_top_margin,obj_adjust_width);
        hline->insertOnRight(anchorObj);
        adjust_width += obj_adjust_width;

        last_right = obj->rect.right(); //更新右边缘
    }

    hline->x = leftLine->x;

    if(PDF_Load_Args::layoutHLine_adjustWidth) {
        if(rightLine->x - leftLine->x + 1 < adjust_width) {
            //需要调整vline位置
            rightLine->x = leftLine->x + adjust_width;
        }
    }

    hline->width = rightLine->x - leftLine->x;
    last_hline = hline; //更新上一行
}

bool Pdf2Souple::tryLastHlineAppendBreak(HorLine_Base* hline,HorLine_Base* last_hline)
{
    if(!last_hline) return false;
    if(hline->canBe<TableLine>() && ! last_hline->canBe<TableLine>()) {
        AnchorObj_PHRight *phright = new AnchorObj_PHRight;
        SoupleManager::registerObj(phright);
        last_hline->insertOnRight(phright);
        return true;
    }
    if(hline->leftObj) {
        auto back_obj = last_hline->rightObj;
        bool needBreak = false;
        if(! back_obj) { //上一行为空，显然需要插入换行符
            needBreak = true;
        } else {
            if(hline->leftObj->canBe<AnchorObj_Glue>()) {
                needBreak = true;
            } else {
                float lastLine_right_margin = last_hline->getRightX() - back_obj->getRightX()
                                        - Helper::cm2pixel(PH_RIGHT_OFFSET_cm);
                auto _drop = hline->leftObj->dropLeft(lastLine_right_margin);
                if(_drop) { //收缩
                    needBreak = true; //需要换行，不然会导致收缩
                    hline->insertOnLeft(_drop); //再还回去
                    _drop->tryMergeRight();
                }else { //检查上一行存在居中占位的情况,且该行没有居中占位
                    if(typeid(*hline->leftObj) != typeid(AnchorObj_JZRect)
                        && typeid(*last_hline->leftObj) == typeid(AnchorObj_JZRect)) {
                        needBreak = true; //需要换行，不然会导致收缩
                    }
                }
            }
        }
        if(needBreak) {
            AnchorObj_PHRight *phright = new AnchorObj_PHRight;
            SoupleManager::registerObj(phright);
            last_hline->insertOnRight(phright);
            return true;
        }
    }
    return false;
}

//template <bool Register>
bool Pdf2Souple::tryLastHlineAppendBreak(AnchorObj_HLine* hline,float raw_right)
{
    auto last_hline = hline->logic_lastHLine; //逻辑上标线
    if(last_hline && last_hline->rightLine && hline->leftObj) {
        auto back_obj = last_hline->rightObj;
        bool needBreak = false;
        if(! back_obj) { //上一行为空，显然需要插入换行符
            needBreak = true;
        } else {
            if(hline->leftObj->canBe<AnchorObj_Glue>()) {
                needBreak = true;
            } else {
                float lastLine_right_margin = raw_right - back_obj->getRightX()
                                              - Helper::cm2pixel(PH_RIGHT_OFFSET_cm);
                auto _drop = hline->leftObj->dropLeft(lastLine_right_margin);
                if(_drop) { //收缩
                    needBreak = true; //需要换行，不然会导致收缩
                    hline->insertOnLeft(_drop); //再还回去
                    _drop->tryMergeRight();
                }
                else { //检查上一行存在居中占位的情况,且该行没有居中占位
                    if(typeid(*hline->leftObj) != typeid(AnchorObj_JZRect)
                        && typeid(*last_hline->leftObj) == typeid(AnchorObj_JZRect)) {
                        needBreak = true; //需要换行，不然会导致收缩
                    }
                }
            }
        }
        if(needBreak) {
            AnchorObj_PHRight *phright = new AnchorObj_PHRight;
            //if constexpr(Register) {
                SoupleManager::registerObj(phright);
            //}
            last_hline->insertOnRight(phright);
            return true;
        }
    }
    return false;
}

//template <bool Register>
// 尝试插入段落记号...
bool Pdf2Souple::tryInsertPHLeft(HorLine_Base* hline)
{
    if(hline->canBe<TableLine>()) return false;
    auto lastLine = hline->getPrevLine();
    if(lastLine == 0) return false;
    bool need = false;
    //[1]如果该行topMargin不等于上一行topMargin，则认为是新段落
    if(lastLine->getPrevLine() != 0 && hline->getTopMargin() - lastLine->getTopMargin()
        > Helper::cm2pixel(PARA_MIN_SPACING_OFFSET_cm)) {
        need = true;
    }

    //[2]如果上一行有换行
    if(lastLine->rightObj && lastLine->rightObj->canBe<AnchorObj_PHRight>()) {
        need = true;
    }

    // [3] tab变大
    if(hline->getPHLeftWidth() - lastLine->getPHLeftWidth() > 3) {
        need = true;
    }

    if(need) { //需要插入PH_Left
        auto ph_left = new AnchorObj_PHLeft;
        SoupleManager::registerObj(ph_left);
        if(hline->getPHLeftWidth() > 1e-5) {
            ph_left->width = hline->leftObj->width;
            hline->leftObj->removeSelf(true);
        }
        hline->insertOnLeft(ph_left);
    }

    return false;
}

AnchorObj* Pdf2Souple::PDFOBJ_TEXT::toAnchorObj(HorLine_Base* hline,float page_top_margin,float& adjustWidth)
{
    AnchorObj_FlowText *flowtext = new AnchorObj_FlowText;
    SoupleManager::registerObj(flowtext);
    //flowtext->y = hline->y - text_obj->ascent;
    flowtext->vAlignMode = Helper::AlignVBaseLine;
    flowtext->vAlignOffset = ascent + rect.top()
                             + page_top_margin - hline->y;

    flowtext->text = text;
    flowtext->font = toFont();
    flowtext->z = render_id; //渲染id作为z坐标
    flowtext->x = rect.left();
    flowtext->y = rect.top() + page_top_margin;
    flowtext->width = rect.width(); //为了之后的布局调整，需要设置坐标信息
    flowtext->isFill = isFill;
    flowtext->isStroke = isStroke;
    flowtext->strokeWidth = strokeWidth;
    flowtext->stroke_color = stroke_color;
    flowtext->fill_color = fill_color;
    //hline->insertOnRight(flowtext);
    QFontMetricsF fm(flowtext->font);
    adjustWidth = fm.horizontalAdvance(flowtext->text);
    return flowtext;
}

FreeObj* Pdf2Souple::PDFOBJ_TEXT::toFreeObj()
{
    return 0;
}

AnchorObj* Pdf2Souple::PDFOBJ_IMAGE::toAnchorObj(HorLine_Base* hline,float page_top_margin,float& adjustWidth)
{
    AnchorObj_Image *image = new AnchorObj_Image;
    SoupleManager::registerObj(image);
    image->z = render_id; //渲染id作为z坐标
    image->source = source;
    image->width = rect.width();
    image->height = rect.height(); //宽高<=>缩放
    image->x = rect.left(); //为了之后的布局调整，需要设置坐标信息
    image->y = rect.top() + page_top_margin;
    //图像默认底部对齐
    image->vAlignMode = Helper::AlignBottom;
    image->vAlignOffset =
        rect.top() + rect.height() + page_top_margin - hline->y;
    adjustWidth = image->width;
    //hline->insertOnRight(image);
    return image;
}

FreeObj* Pdf2Souple::PDFOBJ_IMAGE::toFreeObj()
{
    return 0;
}

AnchorObj* Pdf2Souple::PDFOBJ_PATH::toAnchorObj(HorLine_Base* hline,float page_top_margin,float& adjustWidth)
{
    AnchorObj_Path *path = new AnchorObj_Path;
    SoupleManager::registerObj(path);
    path->width = rect.width();
    path->height = rect.height();
    path->x = rect.left();
    path->z = render_id;
    path->y = rect.top() + page_top_margin;
    //形状默认中心对齐
    path->vAlignMode = Helper::AlignVCenter;
    path->vAlignOffset = rect.top() + rect.height()/2
                         + page_top_margin - hline->y;
    path->strokeStyle = strokeColor;
    path->fillStyle = fillColor;
    path->lineWidth = lineWidth;

    path->isFill = fill;
    path->isStroke = stroke;

    path->path_actions = std::move(list_path_actions);

    for(auto& a : path->path_actions) {
        a.x = a.x - rect.left();
        a.y = a.y - rect.top();
        //qDebug() << "path point at " << a.x << ", " << a.y;
    }

    //qDebug() << "添加PATH.rect = " << rect;

    adjustWidth = rect.width();
    //hline->insertOnRight(path);
    return path;
}

FreeObj* Pdf2Souple::PDFOBJ_PATH::toFreeObj()
{
    return 0;
}

QString Pdf2Souple::dealFont(const QString& fontName)
{
    // bfn = bfn.right(bfn.length() - bfn.indexOf('+') - 1);
    // if(! QFontDatabase::hasFamily(bfn)) {
    //     //需要从pdf中读取内嵌字体
    //     if(FPDFFont_GetIsEmbedded(ffont)) { //需要从pdf中读取内嵌字体
    //         uint8_t *font_data = new uint8_t[1024*1024*5]; //申请5MB内存
    //         size_t out_size;
    //         FPDFFont_GetFontData(ffont,font_data,1024*1024*5,&out_size);
    //         if( QFontDatabase::addApplicationFontFromData(QByteArray::fromRawData((char*)font_data,out_size))
    //             == -1) { delete[] font_data; goto FAIL_2; }
    //         qDebug() << "读取内嵌字体到fontdatabase: " << bfn;
    //         delete[] font_data;
    //     } else {
    //     FAIL_2:
    //         int _i = bfn.indexOf('-');
    //         if(_i != -1) {
    //             bfn = bfn.first(_i);
    //             font.setFamily(bfn);
    //         }
    //     }
    // }

    // font.setFamily(bfn);
    return "";
}

void Pdf2Souple::generateHorLine(const PDFPage* page,HorLine_Base *hline,const HBlock& bk,
                    std::optional<float> hline_x,std::optional<float> hline_width)
{
    assert(!(hline_x.has_value() ^ hline_width.has_value())
           && "Pdf2Souple::generateHorLine(): hline_x和hline_width必须同时有效或无效");
    bool fixedWX = bool(hline_x);
    auto obj = bk.objs.front();
    hline->x = fixedWX ? hline_x.value() : obj->rect.left();
    hline->width = fixedWX ? hline_width.value() : 0;
    if(auto text_obj = dynamic_pointer_cast<PDFOBJ_TEXT>(obj); text_obj) {
        //文本类型
        //默认按照基线对齐
        hline->y = page->page_top_margin + text_obj->rect.top() + text_obj->ascent; //对齐其基线
    } else {
        //否则，默认采用底部对齐
        hline->y = page->page_top_margin + obj->rect.bottom();
    }
    hline->contentTop = bk.rect.top() + page->page_top_margin - hline->y;
    hline->contentBottom = bk.rect.bottom() + page->page_top_margin - hline->y;
    float right_x = hline->x;
    for(auto[i,obj] : bk.objs | views::enumerate) {
        if(obj->rect.left() - right_x >= Helper::cm2pixel(PHRECT_MIN_WIDTH_cm)) {
            if(i > 0) {
                //插入PH_Rect
                AnchorObj_PHRect *phrect = new AnchorObj_PHRect;
                phrect->width = obj->rect.left() - right_x;
                hline->insertOnRight(phrect);
                SoupleManager::registerObj(phrect);
            }
            // else {
            //     //插入PH_Glue
            //     AnchorObj_Glue *glue = new AnchorObj_Glue;
            //     glue->glue_left = true;
            //     glue->width = obj->rect.left() - right_x;
            //     hline->insertOnRight(glue);
            //     SoupleManager::registerObj(glue);
            // }
        }
        float adjust_width;
        AnchorObj* anchorObj = obj->toAnchorObj(hline,page->page_top_margin,adjust_width);
        hline->insertOnRight(anchorObj);
        if(! fixedWX) {
            hline->width += adjust_width;
        }
        right_x = obj->rect.right();
    }
    // if(fixedWX) {
    //     AnchorObj_PHRight* phright = new AnchorObj_PHRight;
    //     SoupleManager::registerObj(phright);
    //     hline->insertOnRight(phright);
    // }
}

// 当一块HLine创建后，应调用dealHLines_2
// first_hline:新创建的HLine的第一条
// 该函数功能：
// 1.尝试添加右Glue替换PH_Right(换行)；
// 2. undefined
void Pdf2Souple::dealHLines_2(AnchorObj_HLine* first_hline)
{
    auto hline = first_hline;
    std::vector<AnchorObj_HLine*> deal_hlines;
    while(hline) {
        if(hline->canBe<TableLine>()) break;
        if(hline->rightObj->canBe<AnchorObj_PHRight>() && hline->rightObj->leftObj) { //发现换行
            float delta = Helper::cm2pixel(GLUE_RIGHT_MARGIN_OFFSET_cm);
            float margin = hline->getRightX() - hline->rightObj->leftObj->getRightX();
            if(margin > Helper::cm2pixel(GLUE_RIGHT_MAX_MARGIN_cm)) { //太宽了，不认为是右胶着
                goto NOT_INSERT; }
            deal_hlines.clear();
            deal_hlines.push_back(hline);
            int same_num = 1;
            hline = hline->logic_nextHLine;
            while(hline) {
                if(hline->canBe<TableLine>()
                    || !(hline->rightObj->canBe<AnchorObj_PHRight>() && hline->rightObj->leftObj)
                    )
                { break;   }
                float margin_1 = hline->getRightX() - hline->rightObj->leftObj->getRightX();
                if(std::abs(margin - margin_1) > delta) break;
                deal_hlines.push_back(hline);
                hline = hline->logic_nextHLine;
            }

            if(deal_hlines.size() >= GLUE_RIGHT_SAME_MIN_NUM) {
                // 确定添加
                for(auto hline : deal_hlines) {
                    auto ph = hline->rightObj;
                    ph->removeSelf(true);
                    AnchorObj_Glue* glue = new AnchorObj_Glue;
                    glue->width = hline->getRightX() - hline->rightObj->getRightX();
                    glue->glue_left = false;
                    hline->insertOnRight(glue);
                    SoupleManager::registerObj(glue);
                }
            }

            if(!hline) break;
            continue;
        }
    NOT_INSERT:
        hline = hline->logic_nextHLine;
    }
}

// imp_path_doSomeMerge
// @brief 分割一些路径，合并一些路径，以简化后续的各种识别。
// @param objList页面的对象列表
void Pdf2Souple::imp_path_doSomeMerge(std::vector<std::shared_ptr<PDFOBJ>>& objList)
{
    struct Path {
        uint32_t index;
        PDFOBJ_PATH* path_obj;
    };
    std::vector<Path> path_lines;
    qDebug() << "Pdf2Souple::imp_path_doSomeMerge BEGIN";
    for(size_t i = 0; i < objList.size(); ++i) {
        auto& pdfobj = objList[i];
        auto path = dynamic_cast<PDFOBJ_PATH*>(pdfobj.get());
        if(! path) continue;
        //float x1,y1,x2,y2;
        uint8_t numLines;
        // 这里的语法由TT_Str定义，为纯编译期计算。
        // 可以通过编译期字符串自由、直观地传递模板参数
        vector<PDFOBJ_PATH::Line> lines;
        if(path->toLinesIfRect(lines)) { //矩形
            numLines = lines.size();
            if(numLines == 4) {
                // if(path->fill == true) { //当作一条线
                //     path_lines.push_back(Path{(uint32_t)i,path});
                // } else { //当作四条线
                //[作废]注意path的rect是不考虑线宽的！
                for(auto& line : lines) {
                    shared_ptr<PDFOBJ_PATH> p = make_shared<PDFOBJ_PATH>();
                    p->rect = {line.x1,line.y1,line.x2-line.x1,line.y2-line.y1};
                    p->render_id = path->render_id; //保持同样的渲染顺序
                    p->stroke = false;
                    p->lineWidth = 0.0f;
                    p->fill = true;
                    p->strokeColor = QColor::fromRgb(0,0,0,0);
                    p->fillColor = path->fill ? path->fillColor : path->strokeColor;
                    // 切出的路径都创建为无描边的矩形即可
                    p->list_path_actions.assign({
                        Path_Action{Path_Action::MoveTo,line.x1,line.y1},
                        Path_Action{Path_Action::LineTo,line.x2,line.y1},
                        Path_Action{Path_Action::LineTo,line.x2,line.y2},
                        Path_Action{Path_Action::LineTo,line.x1,line.y2},
                        Path_Action{Path_Action::LineTo,line.x1,line.y1}
                    });
                    objList.push_back(p);
                }
                //【注意】，这新增的4条线，之后自然会被遍历到的！
                // 所以这里不用添加到path_lines
                objList[i] = {}; //删除该路径
                //}
            } else if(numLines == 1) {
                path_lines.push_back(Path{(uint32_t)i,path});
            }
        }
    }

    /** 至此，一切边框矩形已经被切成4条矩形了 */
    // 考虑合并接续的矩形为一个path，直接进行n^2遍历查找需要合并的矩形

    // 您会发现，下面使用的都是path的t_rect，即路径紧边框。而非rect。
    // 您应该注意到，t_rect是不包含lineWidth的。这一点必须牢记。

    // 按y坐标排序，优先处理垂直方向的合并
    std::ranges::sort(path_lines,[](Path& p1,Path& p2) {
        return p1.path_obj->t_rect.top() < p2.path_obj->t_rect.top();
    });
    for(int i = 0, n = path_lines.size(); i < n; ++i)
    {
        auto& p1 = path_lines[i];
        if(p1.index == -1) continue; //该path已经被合并啦
        bool has_merged{false};
        float new_bottom=p1.path_obj->t_rect.bottom()+p1.path_obj->lineWidth/**0.5f*/;
        qDebug() << "Path-VMerge-source: " << p1.path_obj->rect;
        for(int j = i+1; j < n; ++j) {
            auto& p2 = path_lines[j];
            if(p2.index == -1) continue;
            // 注意，这里比较坐标时可要考虑lineWidth!
            // 因为path_obj的rect是中心框，减去了0.5*lineWidth的
            if(abs(p1.path_obj->t_rect.center().x()-p2.path_obj->t_rect.center().x())<2.0f
                && abs(p1.path_obj->t_rect.width()-p2.path_obj->t_rect.width())<2.0f //水平对齐
                && new_bottom+2.0f
                       >= p2.path_obj->t_rect.top()-p2.path_obj->lineWidth/**0.5f*/ //垂直邻接(或交叠)
                && new_bottom < p2.path_obj->t_rect.bottom()
                && (qDebug()<<"lookLike",p1.path_obj->isLookLike_ifrect(p2.path_obj))
                ) {
                //ok 它被合并了
                has_merged = true;
                new_bottom = p2.path_obj->t_rect.bottom()+p2.path_obj->lineWidth/**0.5f*/;
                //同时从文档流中移除它
                objList[p2.index] = {};
                p2.index = -1; //标记死亡
                qDebug() << "Path-VMerge+1";
            }
        }
        if(has_merged) {
            qDebug() << "Path-VMerge:END";
            p1.path_obj->t_rect.setBottom(new_bottom-p1.path_obj->lineWidth/**0.5f*/);
            p1.path_obj->rect.setBottom(new_bottom);
            p1.path_obj->list_path_actions.clear();

            p1.path_obj->list_path_actions.assign({
                Path_Action(Path_Action::MoveTo,p1.path_obj->t_rect.left(),
                                                p1.path_obj->t_rect.top()),
                Path_Action(Path_Action::LineTo,p1.path_obj->t_rect.right(),
                                                p1.path_obj->t_rect.top()),
                Path_Action(Path_Action::LineTo,p1.path_obj->t_rect.right(),
                                                new_bottom-p1.path_obj->lineWidth),
                Path_Action(Path_Action::LineTo,p1.path_obj->t_rect.left(),
                                                new_bottom-p1.path_obj->lineWidth),
                Path_Action(Path_Action::LineTo,p1.path_obj->t_rect.left(),
                                                p1.path_obj->rect.top())
            });
            qDebug() << "VMerge合并后：" << p1.path_obj->rect;
            //p1.path_obj->list_path_actions
        }
    }
    // 移除所有失效的path
    Helper::removeAllIf(path_lines,[](Path& p){
        return p.index == -1;
    });
    // 按x坐标排序，处理水平方向的合并
    std::ranges::sort(path_lines,[](Path& p1,Path& p2) {
        return p1.path_obj->rect.left() < p2.path_obj->rect.left();
    });
    for(int i = 0, n = path_lines.size(); i < n; ++i)
    {
        auto& p1 = path_lines[i];
        if(p1.index == -1) continue; //该path已经被合并啦
        bool has_merged{false};
        float new_right=p1.path_obj->t_rect.right()+p1.path_obj->lineWidth/**0.5f*/;
        qDebug() << "Path-HMerge-source: " << p1.path_obj->rect;
        for(int j = i+1; j < n; ++j) {
            auto& p2 = path_lines[j];
            if(p2.index == -1) continue;

            //[重要offset] 0.3f: 判断垂直相交
            if(abs(p1.path_obj->t_rect.center().y()-p2.path_obj->t_rect.center().y())<0.3f
                && abs(p1.path_obj->t_rect.height()-p2.path_obj->t_rect.height())<2.0f //垂直对齐
                && new_right+2.0f
                       >= p2.path_obj->t_rect.left()-p2.path_obj->lineWidth/**0.5f*/ //水平邻接(或少量交叠)
                && new_right < p2.path_obj->t_rect.right()
                && (qDebug()<<"lookLike",p1.path_obj->isLookLike_ifrect(p2.path_obj))
                ) {
                //ok 它被合并了
                has_merged = true;
                new_right = p2.path_obj->t_rect.right()+p2.path_obj->lineWidth/**0.5f*/;
                //同时从文档流中移除它
                objList[p2.index] = {};
                p2.index = -1; //标记死亡
                qDebug() << "Path-HMerge+1";
            }
        }
        if(has_merged) {
            qDebug() << "Path-HMerge:END";
            p1.path_obj->t_rect.setRight(new_right-p1.path_obj->lineWidth/**0.5f*/);
            p1.path_obj->rect.setRight(new_right);
            p1.path_obj->list_path_actions.clear();
            p1.path_obj->list_path_actions.assign({
                Path_Action(Path_Action::MoveTo,p1.path_obj->t_rect.left(),
                                                p1.path_obj->t_rect.top()),
                Path_Action(Path_Action::LineTo,p1.path_obj->t_rect.right(),
                                                p1.path_obj->t_rect.top()),
                Path_Action(Path_Action::LineTo,p1.path_obj->t_rect.right(),
                                                p1.path_obj->t_rect.bottom()),
                Path_Action(Path_Action::LineTo,p1.path_obj->t_rect.left(),
                                                p1.path_obj->t_rect.bottom()),
                Path_Action(Path_Action::LineTo,p1.path_obj->t_rect.left(),
                                                p1.path_obj->t_rect.top())
            });
            //p1.path_obj->list_path_actions
            qDebug() << "HMerge合并后：" << p1.path_obj->rect;
        }
    }
    // 移除所有失效的path
    Helper::removeAllIf(path_lines,[](Path& p){
        return p.index == -1;
    });

    int _n2 = 0;
    for(auto ptr : objList)
        if(ptr) objList[_n2++] = ptr;
    objList.resize(_n2);
    qDebug() << "Pdf2Souple::imp_path_doSomeMerge END$";
}
