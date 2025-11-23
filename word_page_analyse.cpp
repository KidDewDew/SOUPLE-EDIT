#include "pdf2souple.h"
#include "wordpage_vline.h"
#include "free_path.h"
#include "obj_start_sign.h"

using namespace std;

#define Column_Line_Max_Width_cm 0.3
#define Column_Line_Min_Height_Scale 0.5
#define Column_Min_Width_cm 0.1

auto _sort_func_wordspec = [](shared_ptr<Pdf2Souple::PDFOBJ> a,shared_ptr<Pdf2Souple::PDFOBJ> b)->bool{
    bool a_up_b = a->rect.bottom()-Y_LINEOBJ_MAX_MARGIN < b->rect.top();
    bool b_up_a = b->rect.bottom()-Y_LINEOBJ_MAX_MARGIN < a->rect.top();
    if(!(a_up_b ^ b_up_a)) { //1 1 or 0 0
        //到这儿，我们认为a、b它们在同一行
        // if(a->rect.left()+X_COVER_OFFSET < b->rect.right() && a->rect.right() > b->rect.left()+X_COVER_OFFSET) {
        //     //qDebug() << "重叠发生";
        //     return a->render_id < b->render_id; //如果同一行出现重叠，先绘制的优先(即被覆盖的优先)
        // }
        return a->rect.left() < b->rect.left(); //严格弱序
    }
    if(a_up_b) return true; //a在b上面，选择a // 1 0
    if(b_up_a) return false; //b在a上面，选择b // 0 1
    return false; // --- 不可能达到此处 ---
};

/**
 * @brief Pdf2Souple::analyseWordPage 尝试解析页面为word类型页面。
 *        调用本函数前，保证 1)解析完页眉页脚 2)解析完表格Part 。
 * @return 是否成功解析成word类型页面
 */
bool Pdf2Souple::analyseWordPage(std::shared_ptr<PDFPage> page,
                                 std::shared_ptr<PDFPage> prev_page)
{

    SoupleManager::enableRegister = false; //禁用对象注册
    //std::vector<Obj*> created_objs;

    // SCOPE_EXIT_DO 于Helper.h定义的宏 确保括号内代码在作用域离开时执行。
    // SCOPE_EXIT_DO(
    //     SoupleManager::enableRegister=true; //恢复对象注册
    //     SoupleManager::wait_register_obj_list.clear();
    // );

    /** 分栏 */
    std::ranges::sort(page->all_objs,[](auto& a,auto& b){
        return a->rect.left() < b->rect.left();
    });
    std::vector<float> content_covers;
    float rightest = -100;
    for(auto& obj : page->all_objs) {
        if(! obj->visible) continue;
        { auto path = dynamic_pointer_cast<PDFOBJ_PATH>(obj);
        if(path) {
            float x1,y1,x2,y2;
            [[maybe_unused]] unsigned char numLines;
            if(path->toSolidRect(x1,y1,x2,y2,&numLines)) {
                if(x2 - x1 < Helper::cm2pixel(Column_Line_Max_Width_cm)
                    && y2 - y1 > page->page_height * Column_Line_Min_Height_Scale) {
                    break; // 这也许是一条分栏分隔线。
                }
            }
        } }
        if(obj->rect.left() > rightest + Helper::cm2pixel(Column_Min_Width_cm)) {
            //新栏
            if(rightest >= -0.1) content_covers.push_back(rightest);
            content_covers.push_back(obj->rect.left());
        } //else {
            //content_covers.back() = std::min(content_covers)
        //}
        rightest = max(rightest,(float)obj->rect.right());
    }
    content_covers.push_back(rightest);

    qDebug() << "页面" << page->page_index << "分栏：";
    for(float x : content_covers) {
        qDebug() << x;
    }

    for(int i = 0; i < content_covers.size(); i += 2) {
        float left = content_covers[i], right = content_covers[i+1];
        PageColumn column;
        column.leftLine = new WordPage_VLine;
        column.rightLine = new WordPage_VLine;
        column.leftLine->x = left;
        column.rightLine->x = right;
        column.leftLine->be<WordPage_VLine*>()->page_index = page->page_index;
        column.rightLine->be<WordPage_VLine*>()->page_index = page->page_index;
        column.leftLine->be<WordPage_VLine*>()->column_id = i >> 1;
        column.rightLine->be<WordPage_VLine*>()->column_id = i >> 1;
        column.leftLine->y = column.rightLine->y = page->souple_page->getTopLineY();
        column.leftLine->height = column.rightLine->height =
            page->souple_page->height - page->souple_page->topMargin - page->souple_page->bottomMargin;
        page->souple_page->columns.push_back(column);
    }

    auto &columns = page->souple_page->columns;

    // 处于每一栏的对象
    std::vector<std::vector<std::shared_ptr<PDFOBJ>>> c_objs(columns.size());

    for(auto& obj : page->all_objs) {
        if(! obj->visible) continue;
        for(auto[i,column] : columns | views::enumerate) {
            if(obj->rect.left()+1e-2 >= column.leftLine->x
                && obj->rect.right() <= column.rightLine->x+1e-2) {
                c_objs[i].push_back(obj);
                Break(true);
            }
        }
        if(BreakVal == false) { //该obj不在栏中，是分隔线
            qDebug() << "分隔线: ";
            qDebug() << obj->rect;
            obj->print();
            auto path = dynamic_pointer_cast<PDFOBJ_PATH>(obj);
            if(path) {
                Free_Path *souple_path = new Free_Path;
                souple_path->path_actions = path->list_path_actions;
                souple_path->x = obj->rect.left();
                souple_path->y = obj->rect.top();
                souple_path->width = obj->rect.width();
                souple_path->height = obj->rect.height();
                SoupleManager::wait_register_obj_list.push_back(souple_path);
            }
        }
    }


    // analyse frames,areas,richs...
    std::vector<std::shared_ptr<PDFOBJ_Rich_or_Area>> rich_or_areas;
    std::vector<std::shared_ptr<PDFOBJ_FramePart>> frameparts;
    for(int i = 0; i < columns.size(); ++i) {
        // 解析rich、area、framepart
        analyse_framepart_or_rich_or_area(page.get(),c_objs[i],rich_or_areas,frameparts);
    }



    /** 按行布局 */

    AnchorObj_HLine* last_column_hline = 0;
    AnchorObj_HLine* first_top_hline = 0;

    for(auto[i,column] : columns | views::enumerate) {
        QList<HBlock> hblocks;
        ranges::sort(c_objs[i],_sort_func_wordspec); //行排序
        if(! Pdf2Souple::createHBlocks_specForWord(c_objs[i],hblocks))
        { //创建行块(不考虑水平断开)
            goto TURNBACK; //如果失败直接撤回
        }

        float ori_left = column.leftLine->x;
        float ori_right = column.rightLine->x;
        //创建Souple对象
        AnchorObj_HLine* last_hline {0}, *top_hline{0};
        for(auto[j,bk] : hblocks | views::enumerate) {
            // if(j > 0 && checkRectVCross(bk.rect,hblocks[j-1].rect)) {
            //     /// 这不是WORD布局 ---> 有回旋的余地吗？之后补充，尽量去适配更多的情况。
            //     // 适配1：左一

            //     goto TURNBACK; //撤回
            // }
            if(auto table = dynamic_pointer_cast<PDFOBJ_TablePart>(bk.objs[0]); table) {
                // 表格 PDFOBJ_TablePart
                for(auto[i,tableline] : table->tablelines | views::enumerate) {
                    tableline->leftLine = column.leftLine->be<AnchorObj_VLine*>();
                    tableline->rightLine = column.rightLine->be<AnchorObj_VLine*>();
                    tableline->page = page->souple_page;
                    SoupleManager::wait_register_obj_list.push_back(tableline);
                }
                auto first_tline = table->tablelines.front();
                //qDebug() << "word-analyse: 表格线" << first_tline->name;
                if(last_hline) {
                    first_tline->setAnchorLastHLine(last_hline);
                }
                if(first_top_hline == 0ull) first_top_hline = first_tline;
                if(j == 0) top_hline = first_tline;
                last_hline = table->tablelines.back();
                //qDebug() << last_hline->name << "y:" << last_hline->y;
            } else {
                // 非表格
                //qDebug() << "word-analyse: 普通线";
                AnchorObj_HLine* hline = new AnchorObj_HLine;
                SoupleManager::wait_register_obj_list.push_back(hline);
                Pdf2Souple::layoutHLine(page,hline,last_hline,bk,
                                        column.leftLine->be<AnchorObj_VLine*>(),
                                        column.rightLine->be<AnchorObj_VLine*>());
                //hline->page = SoupleManager::getPage(*hline);
                if(first_top_hline == 0ull) first_top_hline = hline;
                if(j == 0) top_hline = hline;
            }
        }

        auto hline = last_hline;

        while(hline) {
            //检查使用[居中占位]还是普通占位
            if(typeid(*hline) != typeid(AnchorObj_HLine)) { //其他类型的标线不插入
                hline = hline->logic_lastHLine ? hline->logic_lastHLine : hline->hline->as<AnchorObj_HLine*>();
                continue;
            }
            tryInsertPH_onHLineLeft(hline,ori_left,ori_right);
            hline = hline->logic_lastHLine;
        }

        hline = last_hline;

        while(hline && hline != top_hline) {
            //检查<上一行>是否需要插入换行符
            if(typeid(*hline) != typeid(AnchorObj_HLine)) { //其他类型的标线不插入
                hline = hline->logic_lastHLine ? hline->logic_lastHLine : hline->hline->as<AnchorObj_HLine*>();
                continue;
            }
            tryLastHlineAppendBreak(hline,ori_right);
            tryInsertPHLeft(hline); //尝试插入PH_Left
            hline = hline->logic_lastHLine;
        }

        Pdf2Souple::dealHLines_2(top_hline); //deal2

        //连接上一栏最后一条hline
        if(last_column_hline && top_hline) {
            if(typeid(*last_column_hline) == typeid(AnchorObj_HLine)) {
                top_hline->setAnchorLastHLine(last_column_hline);
                top_hline->setLogicLastHLine(last_column_hline);
            } else {
                top_hline->setAnchorLastHLine(last_column_hline);
            }
            top_hline->topMargin =
                std::max(page->souple_page->getBottomLineY() - last_column_hline->getContentBottom() + 0.01f,
                         last_column_hline->topMargin);
        }

        last_column_hline = last_hline;
    }

    goto RETURN_OK;
TURNBACK: //撤回流程
    for(auto& col : columns) {
        delete col.leftLine;
        delete col.rightLine;
    }
    columns.clear();
    ranges::destroy(SoupleManager::wait_register_obj_list); //注：ranges::destroy ~ delete ...
    SoupleManager::wait_register_obj_list.clear();
    SoupleManager::enableRegister=true;
    return false; //由 SCOPE_EXIT_DO 来开启 enableRegister 以及清除队列。
RETURN_OK:
    /** 成功布局为WORD页面 */
    page->souple_page->page_type = Helper::Word_Page;

    // 处理对上一页的接续
    if(prev_page) {
        if(prev_page->the_last_hline_to_continue) {
            first_top_hline->setLogicLastHLine(prev_page->the_last_hline_to_continue);
            first_top_hline->setAnchorLastHLine(prev_page->the_last_hline_to_continue);
            Pdf2Souple::setSpanPageHLineTopMargin(first_top_hline,
                                                  prev_page->the_last_hline_to_continue,
                                                  *prev_page.get());
            tryLastHlineAppendBreak(first_top_hline,
                                    prev_page->the_last_hline_to_continue->getRightX());
            tryInsertPHLeft(first_top_hline); //尝试插入PH_Left
        } else {
            //...
        }
    }

    page->the_last_hline_to_continue = last_column_hline; //设置页面最后一条hline

    SoupleManager::enableRegister=true;
    for(auto obj : SoupleManager::wait_register_obj_list)
        SoupleManager::registerObj(obj);

    // 对于第1页，需要设置内容起点
    if(page->page_index == 0) {
        Obj_Start_Sign *ss = new Obj_Start_Sign;
        SoupleManager::registerObj(ss);
        ss->attach_hline = first_top_hline;
        SoupleManager::start_sign = ss;
        qDebug() << "create Obj_Start_Sign.";
    }

    for(auto& column : page->souple_page->columns) {
        SoupleManager::registerObj(column.leftLine);
        SoupleManager::registerObj(column.rightLine);
    }

    SoupleManager::wait_register_obj_list.clear();
    return true;
}
