#ifndef PDF2SOUPLE_TEMPLATE_IMPL_HPP
#define PDF2SOUPLE_TEMPLATE_IMPL_HPP

#include "pdf2souple.h"
#include "souplemanager.h"
#include "frame_ofhlines.h"
#include <span>

#define Debug_RectAnalyse false

///为了实现模板函数定义和声明的分离，可以像这样采用头文件互相包含(记得避免循环包含)。

using namespace std;

template<TT_Str tt>
bool Pdf2Souple::createRich(const PDFPage* page,Iterable<HBlock> auto& blocks,Obj* parent,
                            unsigned char* vAlignMode,float* contentHeight,
                            HorLine_Base** firstLine,std::optional<QRectF> fixed_rect)
{
    float content_left_x = 1e8,content_right_x = -1e8,
          content_top_y = 1e8,content_bottom_y = -1e8;
    for(auto[j,bk] : blocks | views::enumerate) {
        if constexpr(tt.getBoolArg("allowVCross",false) == false) {
            if(j > 0 && checkRectVCross(bk.rect,blocks[j-1].rect)) {
                ///不是行布局的文档
                return false;
            }
        }
        content_left_x = qMin(content_left_x,bk.rect.left());
        content_right_x = qMax(content_right_x,bk.rect.right());
        content_top_y = qMin(content_top_y,bk.rect.top());
        content_bottom_y = qMax(content_bottom_y,bk.rect.bottom());
    }

    HorLine_Base *lastHLine = 0;
    float rich_width,rich_left,rich_top;
    if(fixed_rect) {
        rich_width = fixed_rect->width();
        rich_left = fixed_rect->left();
        rich_top = fixed_rect->top();
    } else {
        rich_width = content_right_x - content_left_x;
        rich_left = content_left_x;
        // 既然没有给出，就要主动赋予parent坐标尺寸
        parent->x = rich_left;
        parent->y = content_top_y;
        parent->width = rich_width;
        parent->height = content_bottom_y - content_top_y;
        *vAlignMode = Helper::AlignVCenter; //默认垂直居中
        qDebug() << "createRich:: set parent.area: "
                 << parent->x << parent->y << parent->width << parent->height;
    }

    bool fixed = fixed_rect.has_value();
    std::vector<BlockInner_HorLine *> hlines;

    for(auto[i,bk] : blocks | views::enumerate) {
        if(bk.objs.empty()) continue;
        if(std::shared_ptr<PDFOBJ_TablePart> table =
            dynamic_pointer_cast<PDFOBJ_TablePart>(bk.objs[0]); table) {
            //对表格part进行特判
            //获取rich的vline.
            qDebug() << "createRich: tablepart";
            std::any any_vline = parent->getAnyData("vline");
            assert(any_vline.has_value());
            auto[leftLine,rightLine] =
                std::any_cast<pair<AnchorObj_VLine*,AnchorObj_VLine*>>(any_vline);
            assert(leftLine && rightLine);
            qDebug() << leftLine->__dstr() << rightLine->__dstr();

            // [注意] 必须在connectHLine_up之前赋予leftLine和rightLine
            for(auto tableline : table->tablelines) {
                qDebug() << "endow:" << tableline->__dstr();
                tableline->leftLine = leftLine;
                tableline->rightLine = rightLine;
                tableline->page = page->souple_page;
                SoupleManager::registerObj(tableline);
            }

            if(i == 0) {
                auto tl = table->tablelines.at(0);
                *firstLine = table->tablelines.at(0);
                if(fixed_rect) {
                    tl->topMargin = tl->getContentTop() - rich_top;
                } else tl->topMargin = 0;
            } else if(lastHLine) {
                table->tablelines.at(0)->connectHLine_up(lastHLine);
            }

            lastHLine = table->tablelines.back();
        } else {
            BlockInner_HorLine *hline = new BlockInner_HorLine;
            SoupleManager::registerObj(hline);
            hline->parent = parent;
            hline->z = parent->z + 1;
            generateHorLine(page,hline,bk,rich_left,rich_width);
            if(i == 0) {
                *firstLine = hline;
                if(fixed_rect) {
                    hline->topMargin = hline->getContentTop() - rich_top;
                } else hline->topMargin = 0;
            } else
            if(lastHLine) {
                //lastHLine->nextLine = hline;
                ///lastHLine->setNextLine(hline);
                lastHLine->connectHLine_down(hline);
                hline->hline = lastHLine;
            }
            lastHLine = hline;
            hlines.push_back(hline);
        }
    }

    //换行 居中占位 Glue...

    for(int i = hlines.size() - 1; i >= 0; --i) {
        tryInsertPH_onHLineLeft(hlines[i]);
    }

    for(int i = hlines.size() - 1; i > 0; --i) {
        tryLastHlineAppendBreak(hlines[i],hlines[i-1]);
        tryInsertPHLeft(hlines[i]); //尝试插入PH_Left
    }

    return true;
}

template<TT_Str tt>
bool Pdf2Souple::PDFOBJ_PATH::toLinesIfRect(CanPushback<Line> auto& lines)
{
    float x1,y1,x2,y2;
    uint8_t numLines;

#if Debug_RectAnalyse == true
    qDebug() << "PDFOBJ_PATH::toLinesIfRect(";
#endif

    if(toSolidRect<TStr("extendLineWidth=true,forceFill=false")>(x1,y1,x2,y2,&numLines)) {
        if(numLines == 1)
            lines.push_back(Line{x1,y1,x2,y2});
        else if(numLines == 4) {
            lines.push_back(Line{x1,y1,x2,y1});
            lines.push_back(Line{x1,y2,x2,y2});
            lines.push_back(Line{x1,y1,x1,y2});
            lines.push_back(Line{x2,y1,x2,y2});
        }
#if Debug_RectAnalyse == true
        qDebug() << "常规方法解析成功。numLines: " << numLines;
        for(auto& line : lines) {
            qDebug() << line.x1 << line.y1 << line.x2 << line.y2;
        }
#endif
        return true;
    }

    /// 使用近似矩形判定方法

    constexpr float MIN_SCALE = 0.5;
    constexpr float Straight_Offset_cm = 0.06;
    constexpr float Center_Offset_cm = 0.16;

    /** 注意：pdfium库得出的Path的Bound-rect通常不准确，不能依赖rect来做矩形解析！
     ***/
    x1 = 1e9;
    y1 = 1e9;
    x2 = -1e9;
    y2 = -1e9;

    std::vector<float> vlines,hlines;
    float last_x = rect.left(),last_y = rect.top();

#if Debug_RectAnalyse == true
    qDebug() << "Rect: " << this->rect;
#endif

    for(size_t i = 0; i < list_path_actions.size(); ) {
        auto& a = list_path_actions[i];

        x1 = std::min(x1,a.x);
        x2 = std::max(x2,a.x);
        y1 = std::min(y1,a.y);
        y2 = std::max(y2,a.y);

        switch(a.type) {
        case Path_Action::MoveTo:
            last_x = a.x;
            last_y = a.y;
            ++i;
            break;
        case Path_Action::LineTo:
#if Debug_RectAnalyse == true
            //qDebug() << "LineTo: " << a.x << a.y << "Last: " << last_x << last_y;
#endif
            if(abs(a.x-last_x) < abs(a.y-last_y)) {
                if(abs(a.x-last_x) < Helper::cm2pixel(Straight_Offset_cm)
                    && abs(a.y-last_y) > rect.height() * MIN_SCALE
                    && abs(a.y+last_y-rect.center().y()*2) < Helper::cm2pixel(Center_Offset_cm)) {
                    //特征垂直线
                    vlines.push_back(a.x);
                }
            } else {
                if(abs(a.y-last_y) < Helper::cm2pixel(Straight_Offset_cm)
                    && abs(a.x-last_x) > rect.width() * MIN_SCALE
                    && abs(a.x+last_x-rect.center().x()*2) < Helper::cm2pixel(Center_Offset_cm)) {
                    //特征水平线
                    hlines.push_back(a.y);
                }
            }
            last_x = a.x;
            last_y = a.y;
            ++i;
            break;
        case Path_Action::BezierTo: {
            if(i+2 >= list_path_actions.size()) break;
            auto& a2 = list_path_actions[i+2];
            last_x = a2.x;
            last_y = a2.y;
            i += 3;
            break;
        }
        default:
            ++i;
        }
    }

    if(hlines.empty() || vlines.empty() ||
        hlines.size() > 4 || hlines.size()%2 == 1 || vlines.size() > 4
        || vlines.size()%2 == 1 || hlines.size() != vlines.size()) {
#if Debug_RectAnalyse == true
        qDebug() << "Failed: 边数不符合: hline.num: " << hlines.size()
                                    << "vline.num: " << vlines.size();
#endif
        return false;
    }

    ranges::sort(hlines);
    ranges::sort(vlines);

    if(abs(hlines[0] - y1 /*rect.top()*/) > 2
        || abs(hlines.back() - y2 /*rect.bottom()*/) > 2
        || abs(vlines[0] - x1 /*rect.left()*/) > 2
        || abs(vlines.back() - x2 /*rect.right()*/) > 2
    ) {
#if Debug_RectAnalyse == true
        qDebug() << "Failed: 外边框不是矩形";
#endif
        return false;
    }

    float ttlw = this->stroke ? lineWidth : 0;

    if(hlines.size() == 2) { //单边框
        if(this->fill) {
            lines.push_back(Line{/*(float)rect.left()-ttlw*0.5f*/x1-ttlw,
                                 /*(float)rect.top()-ttlw*0.5f*/y1-ttlw,
                                 /*(float)rect.right()+ttlw*0.5f*/x2+ttlw,
                                 /*(float)rect.bottom()+ttlw*0.5f*/y2+ttlw});
        } else if(this->stroke) {
            lines.push_back(Line{/*(float)rect.left()-ttlw*0.5f*/x1-ttlw,
                                 /*(float)rect.top()-ttlw*0.5f*/y1-ttlw,
                                 /*(float)rect.left()+ttlw*0.5f*/x1+ttlw,
                                 /*(float)rect.bottom()+ttlw*0.5f*/y2+ttlw}); //左
            lines.push_back(Line{/*(float)rect.right()-ttlw*0.5f*/x2-ttlw,
                                 /*(float)rect.top()-ttlw*0.5f*/y1-ttlw,
                                 /*(float)rect.right()+ttlw*0.5f*/x2+ttlw,
                                 /*(float)rect.bottom()+ttlw*0.5f*/y2+ttlw}); //右
            lines.push_back(Line{/*(float)rect.left()-ttlw*0.5f*/x1-ttlw,
                                 /*(float)rect.top()-ttlw*0.5f*/y1-ttlw,
                                 /*(float)rect.right()+ttlw*0.5f*/x2+ttlw,
                                 /*(float)rect.top()+ttlw*0.5f*/y1+ttlw}); //上
            lines.push_back(Line{/*(float)rect.left()-ttlw*0.5f*/x1-ttlw,
                                 /*(float)rect.bottom()-ttlw*0.5f*/y2-ttlw,
                                 /*(float)rect.left()+ttlw*0.5f*/x1+ttlw,
                                 /*(float)rect.bottom()+ttlw*0.5f*/y2+ttlw}); //下
        } else {
#if Debug_RectAnalyse == true
            qDebug() << "Failed: 单边框-no fill or stroke";
#endif
            return false;
        }
    } else { // ~~ if(hlines.size() == 4)
        if(this->fill == false) {
            lines.push_back(Line{/*(float)rect.left()-ttlw*0.5f*/x1-ttlw,
                                 /*(float)rect.top()-ttlw*0.5f*/y1-ttlw,
                                 /*(float)rect.left()+ttlw*0.5f*/x1+ttlw,
                                 /*(float)rect.bottom()+ttlw*0.5f*/y2+ttlw}); //左
            lines.push_back(Line{/*(float)rect.right()-ttlw*0.5f*/x2-ttlw,
                                 /*(float)rect.top()-ttlw*0.5f*/y1-ttlw,
                                 /*(float)rect.right()+ttlw*0.5f*/x2+ttlw,
                                 /*(float)rect.bottom()+ttlw*0.5f*/y2+ttlw}); //右
            lines.push_back(Line{/*(float)rect.left()-ttlw*0.5f*/x1-ttlw,
                                 /*(float)rect.top()-ttlw*0.5f*/y1-ttlw,
                                 /*(float)rect.right()+ttlw*0.5f*/x2+ttlw,
                                 /*(float)rect.top()+ttlw*0.5f*/y1+ttlw}); //上
            lines.push_back(Line{/*(float)rect.left()-ttlw*0.5f*/x1-ttlw,
                                 /*(float)rect.bottom()-ttlw*0.5f*/y2-ttlw,
                                 /*(float)rect.left()+ttlw*0.5f*/x1+ttlw,
                                 /*(float)rect.bottom()+ttlw*0.5f*/y2+ttlw}); //下
        } else {
            lines.push_back(Line{ vlines[0]-ttlw/**0.5f*/,
                                 hlines[0]-ttlw/**0.5f*/,
                                 vlines[1]+ttlw/**0.5f*/,
                                 hlines[3]+ttlw/**0.5f*/}); //左
            lines.push_back(Line{vlines[2]-ttlw/**0.5f*/,
                                 hlines[0]-ttlw/**0.5f*/,
                                 vlines[3]+ttlw/**0.5f*/,
                                 hlines[3]+ttlw/**0.5f*/}); //右
            lines.push_back(Line{vlines[0]-ttlw/**0.5f*/,
                                 hlines[0]-ttlw/**0.5f*/,
                                 vlines[3]+ttlw/**0.5f*/,
                                 hlines[1]+ttlw/**0.5f*/}); //上
            lines.push_back(Line{vlines[0]-ttlw/**0.5f*/,
                                 hlines[2]-ttlw/**0.5f*/,
                                 vlines[3]+ttlw/**0.5f*/,
                                 hlines[3]+ttlw/**0.5f*/}); //下
        }
    }
#if Debug_RectAnalyse == true
    qDebug() << "近似方法解析成功：numLines: " << lines.size();
    for(auto& line : lines) {
        qDebug() << line.x1 << line.y1 << line.x2 << line.y2;
    }
#endif
    return true;
}

template<TT_Str tt>
void Pdf2Souple::createHBlocks(const Iterable<std::shared_ptr<PDFOBJ>> auto& all_objs,
                               float page_width,CanPushback<HBlock> auto& blocks,
                               bool enable_horizontal_break)
{
    //遍历所有obj，对往后找到所有同一HBlock的obj
    for(int i = 0; i < all_objs.size();) {
        HBlock block; //默认ex_left = 0
        auto obj1 = all_objs[i];
        block.objs.push_back(obj1);
        float y1 = obj1->rect.top(), y2 = obj1->rect.bottom();
        float top = y1, bottom = y2, left = obj1->rect.left(), right = obj1->rect.right();

        if(i > 0) {
            auto obj0 = all_objs[i-1];
            if(checkRectVCross(obj0->rect,obj1->rect)) { //垂直相交
                block.ex_left = obj0->rect.right(); //如果obj的左边有obj，则设置ex_left为左边obj的right
            }
        }
        block.ex_right = page_width; //初始设定

        for(++i; i < all_objs.size(); ++i) {
            auto obj2 = all_objs[i];
            //qDebug() << "obj2.rect = " << obj2->rect;

            if(obj2->rect.bottom() < y2 && i+1 < all_objs.size())
            { //怀疑obj1右边是多行结构
                bool isMultiLineRight = false;
                for(int j = i + 1; j < all_objs.size(); ++j) {
                    auto& obj3 = all_objs[j];
                    if(obj3->rect.top()+Y_LINEOBJ_MAX_MARGIN <= y2 //同一大行
                        && (obj3->rect.left()+1 < right //水平重叠
                            || obj3->rect.top()-Y_LINEOBJ_MAX_MARGIN >= obj2->rect.bottom()//或垂直多行
                            ))
                    {
                        block.ex_right = obj2->rect.left();
                        isMultiLineRight = true;
                        break;
                    }
                }
                if(isMultiLineRight) {  //认定右边为多行结构,那么直接停止读取
                    if constexpr(tt.getBoolArg("multi2rich") == true) {
                        //要求把多行结构转换为富文本框
                        std::vector<std::shared_ptr<PDFOBJ>> rich_objs;
                        rich_objs.push_back(obj2);
                        for(int j = i + 1; j < all_objs.size(); ++j) {
                            auto& obj3 = all_objs[j];
                            if(obj3->rect.top()+Y_LINEOBJ_MAX_MARGIN <= y2 //同一大行
                                && (obj3->rect.left()+1 < right //水平重叠
                                    || obj3->rect.top()-Y_LINEOBJ_MAX_MARGIN >= obj2->rect.bottom()//或垂直多行
                                    ))
                            {
                                rich_objs.push_back(obj3);
                            }
                        }

                        std::vector<HBlock> rich_hblocks;
                        //createHBlocks(rich)

                    } else {
                        break;
                    }
                }
            }

            if(obj2->rect.top()+Y_LINEOBJ_MAX_MARGIN >= y2 || obj2->rect.bottom()-Y_LINEOBJ_MAX_MARGIN <= y1) {
                //优先考虑垂直断开
                //注：有可能两行元素有一点点的y重合，需要加一个小量来避免
                block.ex_right = page_width; //拓展右边界为页右边缘
                break;
            }

            if(enable_horizontal_break) {
                if(obj2->rect.left() - right > 90
                    || obj2->selfHBlock() || obj1->selfHBlock()) {
                    //水平断开 or obj2独占一个HBlock or obj1独占一个HBlock
                    //qDebug() << "水平中断: 换行";
                    block.ex_right = obj2->rect.left(); //拓展右边界为右边block的左边界
                    break;
                }
            }
            right = obj2->rect.right();


            top = std::min(top,(float)obj2->rect.top());
            bottom = std::max(bottom,(float)obj2->rect.bottom());
            block.objs.push_back(obj2);

            obj1 = obj2;
        }
        block.rect = {left,top,right-left,bottom-top};


        qDebug() << "HorBlock{" << block.rect << ", children-num = " << block.objs.size();
        for(auto &obj : block.objs) {
            obj->print();
        }
        blocks.push_back(std::move(block));
    }
}

// word布局版本的createHBlocks,力求成功
template<TT_Str tt>
bool Pdf2Souple::createHBlocks_specForWord(const RandomAccessCont<std::shared_ptr<PDFOBJ>> auto& raw_objs,
                                      CanPushback<HBlock> auto& blocks)
{
    return impl_createHBlocks_specForWord(raw_objs,blocks,Y_LINEOBJ_MAX_MARGIN,X_LINEOBJ_MAX_MARGIN);
}


template<TT_Str tt>
bool Pdf2Souple::impl_createHBlocks_specForWord(const RandomAccessCont<std::shared_ptr<PDFOBJ>> auto& raw_objs,
                                           CanPushback<HBlock> auto& blocks,
                                           float lineYOffset,float lineXOffset)
{
    //按大行划分；
    //大行再按列划分；每一列判断多行结构
    //对于多行结构创建富文本框。(frameless)

    qDebug() << "impl_createHBlocks_specForWord:" << lineXOffset<<lineYOffset;

    std::vector<std::shared_ptr<PDFOBJ>> objs{raw_objs.begin(),raw_objs.end()};

    if(objs.empty()) return true;

    ranges::sort(objs,_Pred(e1->rect.center().y() < e2->rect.center().y())); //按y坐标排序

    for(auto& obj : objs) {
        //qDebug() << "左" << obj->rect.left() << "右" << obj->rect.right();
        obj->print();
    }

    int last_i = 0;
    float bottomest = objs[0]->rect.bottom();

    bool hasProgress = false; //是否有成效

    for(int i = 1; i <= objs.size(); ++i) {
        // 采用大分行算法

        if(i == objs.size() || objs[i]->rect.top()+lineYOffset >= bottomest) {

            HBlock bk; //该大行的HBlock

            //新水平分割线，这里的算法和页面分栏检测算法完全相同：只需要线性时间就可以做切分。
            // objs[last_i,i-1]即这一大行包含的对象。下面进行列切分。
            /** 列切分 */

            std::sort(&objs[last_i],&objs[i-1]+1,_Pred(e1->rect.left() < e2->rect.left()));
            float rightest = objs[last_i]->rect.right();

            float left = objs[last_i]->rect.left(), right = objs[last_i]->rect.right();
            float top = objs[last_i]->rect.top(),bottom = objs[last_i]->rect.bottom();

            for(int k = last_i+1; k <= i-1; ++k) {
                top = std::min(top,(float)objs[k]->rect.top());
                bottom = std::max(bottom,(float)objs[k]->rect.bottom());
                left = std::min(left,(float)objs[k]->rect.left());
                right = std::max(right,(float)objs[k]->rect.right());
            }

            int last_j = last_i;
            for(int j = last_i+1; j <= i; ++j) {
                //qDebug() << "j_> " <<j << "rightest: " << rightest;
                //if(j < i) qDebug() << "left" << objs[j]->rect.left();
                if(j == i || objs[j]->rect.left()+lineXOffset >= rightest) {
                    //objs[last_j,j-1]构成一列

                    //qDebug() << "j: " <<j << "rightest: " << rightest;
                    //if(j < i) qDebug() << "left: " << objs[j]->rect.left();

                    /** 列：objs[last_j,j-1] */
                    if(last_j == j-1) { //单行结构
                        bk.objs.push_back(objs[last_j]);
                        qDebug() << "单行--->";
                        objs[last_j]->print();
                    } else {
                        // 检查是否是多行结构

                        std::vector<HBlock> rich_bks;
                        if(last_j == 0 && j-1 == objs.size()-1) {
                            // 陷入循环，放宽条件
                            // 决定放宽行间距还是列间距：哪个更小取哪个
                            // 计算最小可能需要的行距、列距是多少
                            float need_lineYOffset,
                                  need_lineXOffset;

                            //采用二分法查找lineYOffset以区分至少一行 O(nlog)

                            std::vector<QRectF> rl;
                            for(int k = last_j; k < j; ++k) {
                                rl.push_back(objs[k]->rect);
                            }

                            // 按y坐标排序
                            ranges::sort(rl,[](QRectF& r1,QRectF& r2){
                                return r1.center().y() < r2.center().y();
                            });
                            float L=lineYOffset,R=1e5;
                            while(abs(L-R)>1e-2) {
                                float M = (L+R)*0.5f;
                                //行切分算法
                                float bottomest = rl[0].bottom();
                                bool hasCut = false; //是否有切分
                                for(int i = 1; i < rl.size(); ++i) {
                                    if(rl[i].top()+M >= bottomest) {
                                        hasCut = true;
                                        break;
                                    }
                                    bottomest = std::max<float>(bottomest,rl[i].bottom());
                                }
                                if(hasCut) {
                                    R = M;
                                } else {
                                    L = M;
                                }
                            }
                            need_lineYOffset = (L+R)*0.5f+1e-6f;

                            //采用二分法查找lineXOffset以区分至少一列 O(nlog)
                            // 按x坐标排序
                            ranges::sort(rl,[](QRectF& r1,QRectF& r2){
                                return r1.left() < r2.left();
                            });
                            L=lineXOffset,R=1e5;
                            while(abs(L-R)>1e-2) {
                                float M = (L+R)*0.5f;
                                //行切分算法
                                float rightest = rl[0].right();
                                bool hasCut = false; //是否有切分
                                for(int i = 1; i < rl.size(); ++i) {
                                    if(rl[i].left()+M >= rightest) {
                                        hasCut = true;
                                        break;
                                    }
                                    rightest = std::max<float>(rightest,rl[i].right());
                                }
                                if(hasCut) {
                                    R = M;
                                } else {
                                    L = M;
                                }
                            }
                            need_lineXOffset = (L+R)*0.5+1e-7;

                            float xoff,yoff;
                            if(need_lineXOffset < need_lineYOffset) {
                                xoff = need_lineXOffset;
                                yoff = lineYOffset;
                            } else {
                                xoff = lineXOffset;
                                yoff = need_lineYOffset;
                            }
                            impl_createHBlocks_specForWord(std::span{&objs[last_j],(size_t)j-last_j},
                                                           rich_bks,
                                                           yoff,
                                                           xoff);
                        } else {
                            impl_createHBlocks_specForWord(std::span{&objs[last_j],(size_t)j-last_j},
                                                           rich_bks,
                                                           lineYOffset,
                                                           lineXOffset);
                        }
                        if(rich_bks.size() == 1) { //认为是单行结构
                            for(auto obj : rich_bks[0].objs)
                                bk.objs.push_back(obj);
                        } else {
                            std::shared_ptr<PDFOBJ_RICH> pdfobj_rich = std::make_shared<PDFOBJ_RICH>();
                            pdfobj_rich->rich_obj = new Anchorobj_Rich;
                            auto rich = pdfobj_rich->rich_obj;
                            SoupleManager::registerObj(pdfobj_rich->rich_obj);
                            float contentHeight_unused;
                            // allowVCross: 允许行垂直相交
                            createRich<TStr("allowVCross = true")>
                                (       Shared::current_page.lock().get(),
                                        rich_bks,
                                        rich->free_rich,
                                        &rich->free_rich->vAlignMode,
                                        &contentHeight_unused,
                                        &(rich->free_rich->firstLine),
                                        {}
                                );
                            rich->x = rich->free_rich->x;
                            rich->y = rich->free_rich->y;
                            rich->width = rich->free_rich->width;
                            rich->height = rich->free_rich->height;
                            pdfobj_rich->rect = {rich->x,rich->y,
                                                 rich->width,rich->height};
                            qDebug() << "创建富文本框：" << pdfobj_rich->rect;
                            rich->free_rich->width = rich->width;
                            rich->free_rich->height = rich->height;
                            rich->free_rich->setSign(Free_Rich::Has_Border,false); //无边框
                            bk.objs.push_back(pdfobj_rich);
                        }
                    }
                    last_j = j;
                }
                if(j < i)
                    rightest = std::max(rightest,(float)objs[j]->rect.right());
            } //END 列切分

            bk.rect = {left,top,
                       right-left,
                       bottom-top};

            qDebug() << "创建block: " << bk.rect;

            last_i = i;
            /** 列切分END */

            blocks.push_back(std::move(bk));

        } //END 行切分
        if(i < objs.size())
            bottomest = std::max(bottomest,(float)objs[i]->rect.bottom());
    }
    return true;
}

// 解析framepart rich area
// 该函数是主动解析内容框。
// 解析之前，确保该页面已经解析过“表格”了
// 此外，该函数不会进行矩形合并，请确保已经进行了恰当的矩形合并。
// tip: 该函数的实现包含较多的冗余设计，修改时注意不要只修改一半。
void Pdf2Souple::analyse_framepart_or_rich_or_area(
    PDFPage* page,
    RandomAccessCont<std::shared_ptr<PDFOBJ>> auto& to_analyse_objs,
    //const RandomAccessCont<float> auto& column_lines,
    RandomAccessCont<std::shared_ptr<PDFOBJ_Rich_or_Area>> auto& rich_or_areas,
    RandomAccessCont<std::shared_ptr<PDFOBJ_FramePart>> auto& frame_parts)
{

    qDebug() << "Pdf2Souple::analyse_framepart_or_rich_or_area BEGIN";

    // --- Step1 找出可能作为背景、或背景边框的obj，并排序
    std::vector<std::pair<int,std::shared_ptr<PDFOBJ>>> maybe_bg_list;
    for(auto[i,obj] : to_analyse_objs | views::enumerate) {
        if(dynamic_pointer_cast<PDFOBJ_PATH>(obj) ||
            dynamic_pointer_cast<PDFOBJ_IMAGE>(obj)) {
            maybe_bg_list.push_back({i,obj});
        }
    }

    //保证外圈的先遍历到；即使边框和内容大小相同，也能保证先遍历到边框。
    ranges::sort(maybe_bg_list,[](std::pair<int,std::shared_ptr<PDFOBJ>>& _a,
                                   std::pair<int,std::shared_ptr<PDFOBJ>>& _b){
        auto& a = _a.second, &b = _b.second;
        return a->rect.top() < b->rect.top() ||
               (a->rect.top() == b->rect.top() && (a->rect.left() < b->rect.left() ||
                            (a->rect.left() == b->rect.left() && a->render_id > b->render_id)));
    });

    // --- Step2 找出所有背景块，__Bg

    // 临时存储的背景块
    struct __Bg {
        bool visible = true;
        char fillMode; //背景填充类型
        QColor fillColor; //填充颜色
        QString fillSrc; //填充图片路径
        float lineWidth;
        float radius; //圆角半径
        QColor borderColor;
        QRectF rect;
        vector<int> border_ids;
        int fill_id;
        // 如您所想，出于各种原因，边框到这儿都被割裂为线条啦
        // 但这里仍然会去考虑完整边框对象的，增加一些冗余。
        vector<std::shared_ptr<PDFOBJ_PATH>> __border_obj;
        std::shared_ptr<PDFOBJ> __fill_obj;
    };

    std::vector<__Bg> __bg_list;

    for(auto[i,bg] : maybe_bg_list) {
        // 这里的visible==false是指：这个对象已经被使用了，已经成为了__Bg的一部分。
        QRectF bg_rect; //该背景的区间
        if(bg->visible == false) continue;

        qDebug() << "at " << bg->rect;

        if(std::max(bg->rect.width(),bg->rect.height())
            <= Helper::point2pixel(MAX_LINE_WIDTH_pt)) continue;

        // --- Step2.1 确定__border和__fill

        int __fill_obj_pos;
        vector<int> __border_obj_pos;
        float __lineWidth = 0.0;
        vector<std::shared_ptr<PDFOBJ_PATH>> __border_objs = {};
        std::shared_ptr<PDFOBJ> __fill_obj = {};
        if(auto path_bg = dynamic_pointer_cast<PDFOBJ_PATH>(bg);
            path_bg) { // 路径类型的背景
            //检查是否是背景框
            std::vector<PDFOBJ_PATH::Line> rect_lines;
            bool canBeRect = path_bg->toLinesIfRect(rect_lines); //矩形识别
            if(canBeRect) {
                if(rect_lines.size() == 4) { // this is border-rect: no-fill
                    // seq of rect_lines(4): 左 右 上 下
                    __border_objs.push_back(path_bg);
                    __border_obj_pos.push_back(i);
                    __lineWidth = rect_lines[0].x2 - rect_lines[0].x1; //描边宽度
                    bg_rect = bg->rect;
                    //尝试找fill-bg内容，可能是PATH或Image
                    for(int j = i+1; j < maybe_bg_list.size(); ++j) {
                        auto& bg2 = maybe_bg_list[j].second;
                        if(bg2->rect.left() <= rect_lines[0].x2+1
                            && bg2->rect.right() >= rect_lines[1].x1-1
                            && bg2->rect.top() <= rect_lines[2].y2+1
                            && bg2->rect.bottom() >= rect_lines[3].y1-1
                            && bg2->rect.right() <= rect_lines[1].x2
                            && bg2->rect.bottom() <= rect_lines[3].y2) {
                            //可以认为它是fill-bg
                            if(auto path_fill_bg = dynamic_pointer_cast<PDFOBJ_PATH>(bg2)
                                ; path_fill_bg) { //fill-bg是path
                                std::vector<PDFOBJ_PATH::Line> rect_lines_2;
                                if(path_fill_bg->toLinesIfRect(rect_lines_2)
                                    && rect_lines_2.size() == 1) { //实心矩形
                                    __fill_obj = bg2;
                                    __fill_obj_pos = maybe_bg_list[j].first;
                                }
                            }
                            else { //fill-bg是image
                                __fill_obj = bg2;
                                __fill_obj_pos = maybe_bg_list[j].first;
                            }
                        }
                    }
                } else if(rect_lines.size() == 1){ // this is fill-bg or single-border
                    if(path_bg->rect.width() < Helper::point2pixel(MAX_LINE_WIDTH_pt)
                        || path_bg->rect.height() < Helper::point2pixel(MAX_LINE_WIDTH_pt)) {
                        //认为这是一个被切开的边框线，按道理由fill部分来处理它
                        // 但是，有的边框它没有fill内容啊，因此这里至少得检查一下有没有fill
                        // 那还不如顺便在这里把Bg提出来，fill对border的解析作为冗余设计
                        qDebug() << "## find a Border-Line";
                        auto neighbors =
                            find_neighbors_of_path<TStr("onlyLine=true")>
                            (
                                maybe_bg_list|views::transform([](auto& p){return p.second;}),
                                -1,path_bg
                            );
                        // 邻居+自身数量<=4~无法组成边框，更别提fill-part.
                        if(neighbors.size() <= 4) continue;
                        // 遍历所有邻居，找出left\right\top\bottom\fill part
                        float top=1e9,
                            left=1e9,
                            right=-1e9,
                            bottom=-1e9;
                        pair<int,std::shared_ptr<PDFOBJ_PATH>>
                            topBorder,bottomBorder,leftBorder,rightBorder,
                            fillPart;
                        float maxLineWidth = 0.0f;
                        for(auto[k,neighbor]:neighbors) {
                            if(neighbor->rect.top() < top
                                && neighbor->rect.width() > Helper::point2pixel(MAX_LINE_WIDTH_pt)
                                && neighbor->rect.height() < Helper::point2pixel(MAX_LINE_WIDTH_pt)) {
                                top = neighbor->rect.top();
                                topBorder = std::pair{(k>=0?maybe_bg_list[k].first:i),neighbor};
                                maxLineWidth = std::max<double>(maxLineWidth,
                                                        neighbor->rect.height());
                            }
                            if(neighbor->rect.bottom() > bottom
                                && neighbor->rect.width() > Helper::point2pixel(MAX_LINE_WIDTH_pt)
                                && neighbor->rect.height() < Helper::point2pixel(MAX_LINE_WIDTH_pt))
                            {
                                bottom = neighbor->rect.bottom();
                                bottomBorder = std::pair{(k>=0?maybe_bg_list[k].first:i),neighbor};
                                maxLineWidth = std::max<double>(maxLineWidth,
                                                        neighbor->rect.height());
                            }
                            if(neighbor->rect.left() < left
                                && neighbor->rect.height() > Helper::point2pixel(MAX_LINE_WIDTH_pt)
                                && neighbor->rect.width() < Helper::point2pixel(MAX_LINE_WIDTH_pt)) {
                                left = neighbor->rect.left();
                                leftBorder = std::pair{(k>=0?maybe_bg_list[k].first:i),neighbor};
                                maxLineWidth = std::max<double>(maxLineWidth,
                                                        neighbor->rect.width());
                            }
                            if(neighbor->rect.right() > right
                                && neighbor->rect.height() > Helper::point2pixel(MAX_LINE_WIDTH_pt)
                                && neighbor->rect.width() < Helper::point2pixel(MAX_LINE_WIDTH_pt))
                            {
                                right = neighbor->rect.right();
                                rightBorder = std::pair{(k>=0?maybe_bg_list[k].first:i),neighbor};
                                maxLineWidth = std::max<double>(maxLineWidth,
                                                        neighbor->rect.width());
                            }
                        }
                        // 注意，这些邻居中可能会有内部路径对象，需要过滤掉
                        if NOT(leftBorder.second && rightBorder.second
                                && topBorder.second && bottomBorder.second) {
                            continue; //无法 构成 完整边框
                        }

                        float innerw = right-left-2*maxLineWidth,
                            innerh = bottom-top-2*maxLineWidth;

                        vector<pair<int,shared_ptr<PDFOBJ_PATH>>> candidate_for_fillpart;

                        // 来找找fill-part
                        for(auto[k,neighbor] : neighbors) {
                            if(neighbor == leftBorder.second
                                || neighbor == rightBorder.second
                                || neighbor == topBorder.second
                                || neighbor == bottomBorder.second) continue;
                            if(std::abs(neighbor->rect.width()-innerw) <= 2.0f
                                && std::abs(neighbor->rect.height()-innerh) <= 2.0f) {
                                //fillPart = ;
                                candidate_for_fillpart.push_back(
                                    std::pair{(k>=0?maybe_bg_list[k].first:i),neighbor}
                                );
                                //break;
                            } else {
                                qDebug() << "**: "
                                         << neighbor->rect.width()
                                         << right-left << neighbor->rect.height()
                                         << bottom - top;
                            }
                        }

                        if( ! candidate_for_fillpart.empty()) {
                            fillPart = candidate_for_fillpart[0];
                            for(auto& p : candidate_for_fillpart) {
                                if(p.second->rect.width() > fillPart.second->rect.width()) {
                                    fillPart = p;
                                }
                            }
                        }

                        // 整理
                        __border_objs.assign({leftBorder.second,rightBorder.second,
                                              topBorder.second,bottomBorder.second});
                        __border_obj_pos.assign({leftBorder.first,rightBorder.first,
                                                 topBorder.first,bottomBorder.first});

                        if(fillPart.second) {
                            __fill_obj = fillPart.second;
                            __fill_obj_pos = fillPart.first;
                            qDebug() << "fillPart=" << fillPart.first;
                        } else {
                            qDebug() << "No FillPart Found";
                        }

                        __lineWidth = topBorder.second->rect.height(); //设置线宽
                        bg_rect = {left,top,right-left,bottom-top};

                    } else {
                        // 否则，认为这就是fill_bg.
                        // 显然，对于这个fill_obj,得尝试找它的边框。
                        // 作为冗余设计，这里不仅考虑单边线，还会考虑完整边框。
                        qDebug() << "## find a Fill-Obj";
                        __fill_obj = bg;
                        __fill_obj_pos = i;
                        bg_rect = bg->rect;
                        //这里的查找需要从头进行遍历
                        for(auto[j,bg2] : maybe_bg_list) {
                            if( ! bg2->visible) continue;
                            if(bg2.get() == bg.get()) continue;
                            auto path_bg2 = dynamic_pointer_cast<PDFOBJ_PATH>(bg2);
                            if(! path_bg2) continue;
                            float mlw = Helper::point2pixel(MAX_LINE_WIDTH_pt);
                            [[maybe_unused]] vector<PDFOBJ_PATH::Line> lines;
                            if(bg2->rect.width() < mlw)
                            {
                                // this is vborder
                                if(abs(bg2->rect.height()-bg->rect.height()) < mlw
                                    && abs(bg2->rect.center().y()-bg->rect.center().y()) < mlw
                                    &&
                                    (bg2->rect.left() <= bg->rect.left()+1.5f
                                        || bg2->rect.right() >= bg->rect.right()-1.5f)
                                    && path_bg2->toLinesIfRect(lines)) {
                                    // 确认是左or右 border
                                    __border_objs.push_back(path_bg2);
                                    __border_obj_pos.push_back(j);
                                }
                            } else if(bg2->rect.height() < mlw) {
                                // this is hborder
                                if(abs(bg2->rect.width()-bg->rect.width()) < mlw
                                    && abs(bg2->rect.center().x()-bg->rect.center().x()) < mlw
                                    && (bg2->rect.top() <= bg->rect.top()+1.5f
                                        || bg2->rect.bottom() >+ bg->rect.bottom()-1.5f)
                                    && path_bg2->toLinesIfRect(lines)) {
                                    // 确认是上or下 border
                                    __border_objs.push_back(path_bg2);
                                    __border_obj_pos.push_back(j);
                                }
                            } else {
                                // 怀疑是完整边框
                                // to-do
                            }
                        }
                        if(__border_objs.size() > 0) {
                            auto& b = __border_objs[0];
                            __lineWidth = std::min(b->rect.width(),b->rect.height());
                        }
                    }
                }
            } //canBeRect
        }//path_bg
        else {
            //这是图片bg
            __fill_obj = bg;
            __fill_obj_pos = i;
            bg_rect = bg->rect;
        }

        ///Here: __fill_obj是可能的填充对象，__border_obj是可能的描边对象

        if(!__fill_obj && __border_objs.empty()) continue;

        //if(! __border_obj)
        // --- Step2.2 确定这个bg上是否有实际内容存在，但不需要记录
        //std::vector<pair<int,std::shared_ptr<PDFOBJ>>> inside_objs;
        bool hasContent = false;
        for(auto[k,obj] : to_analyse_objs | views::enumerate) {
            if(!obj->visible || obj == __fill_obj)
                continue;
            for(auto& __border_obj : __border_objs) {
                if(obj == __border_obj) continue;
            }
            if(bg->rect.left()-2.0f <= obj->rect.left()
                && bg->rect.top()-2.0f <= obj->rect.top()
                && bg->rect.right()+2.0f >= obj->rect.right()
                && bg->rect.bottom()+2.0f >= obj->rect.bottom()) {
                hasContent = true;
            }
        }

        if(hasContent) { //真正确定它是内容框的背景，记录__Bg
            __Bg &__bg = (__bg_list.push_back({}),__bg_list.back()); //note:逗号表达式
            __bg.__fill_obj = __fill_obj;
            __bg.__border_obj = __border_objs;
            __bg.fill_id = __fill_obj_pos;
            __bg.border_ids = __border_obj_pos;
            //__bg.rect = bg->rect;
            __bg.rect = bg_rect;

            //如果断言failed，证明前面有地方忘记设置bg_rect了
            assert(bg_rect.width()>1e-4&&bg_rect.height()>1e-4);

            if(__fill_obj) {
                __fill_obj->visible = false;
                if(auto path_fill_bg = dynamic_pointer_cast<PDFOBJ_PATH>(__fill_obj);
                    path_fill_bg) { //纯色填充
                    __bg.fillMode = Helper::FillMode::Color_Fill;
                    __bg.fillColor = path_fill_bg->fillColor;
                } else { //图片填充
                    auto img = static_pointer_cast<PDFOBJ_IMAGE>(__fill_obj);
                    __bg.fillMode = Helper::FillMode::Image_Fill;
                    __bg.fillSrc = img->source;
                }
            } else {
                __bg.fillMode = Helper::FillMode::No_Fill;
                __bg.fillColor = "transparent"; //透明
            }
            if(!__border_objs.empty()) {
                for(auto _obj : __border_objs)
                { _obj->visible = false; }
                __bg.lineWidth = __lineWidth;
                // 判断使用strokeColor还是fillColor作为边框色
                //if(__border_obj->fill && __border_obj->fillColor.alphaF()>0.1)
                //    __bg.borderColor = __border_obj->fillColor;
                //else if(__border_obj->stroke) __bg.borderColor = __border_obj->strokeColor;
                //else __bg.borderColor = __border_obj->fillColor;;
                __bg.borderColor = __border_objs[0]->getLookColor();
            }
        }

    }

    for(__Bg& bg : __bg_list) {
        qDebug() << "- analyse __Bg:" << bg.fillColor << bg.fillSrc
                 << bg.lineWidth << bg.borderColor;
        qDebug() << bg.rect;
        qDebug() << "- - - borders.num=" << bg.__border_obj.size();
        for(int i : bg.border_ids) {
            qDebug() << "border-id+=" << i;
        }
    }

    // --- Step3 检测交叉的path、image，把它们解析为area.
    // to-do 如果表格检测足够好，理论上area出现的概率很小


    // --- Step4 检测Rich和FramePart，解析Rich，记录FramePart
    //std::vector<int> to_remove_objs;
    for(__Bg& bg : __bg_list) {
        if(! bg.visible) continue;
        // 如果一个bg左边和右边都没有任何obj，则认为该bg是FramePart
        // 正是因此，该函数不需要知道页面的分栏的情况。
        // 否则认为是Rich。
        // 注意，这里不能忽略visible=false的对象: 因为它们可能正是一个被解析过的FramePart
        std::vector<std::shared_ptr<PDFOBJ>> inside_objs;
        bool isFrame = true;
        for(auto& obj : to_analyse_objs) {
            if(!obj) continue;
            if(obj == bg.__fill_obj) continue;
            for(auto& _border : bg.__border_obj) {
                if(obj == _border) goto L1;
            }

            if(obj->rect.bottom() <= bg.rect.top()
                || obj->rect.top() >= bg.rect.bottom())
                continue;

            if(obj->rect.left()+3 < bg.rect.left()
                || obj->rect.right()-3 > bg.rect.right()) {
                isFrame = false;
            } else if(obj->visible) {
                inside_objs.push_back(obj);
            }
        L1: continue;
        }
        qDebug() << bg.border_ids.size() << bg.fill_id;
        //if(bg.__border_obj) to_analyse_objs[bg.border_id] = {};
        //for(int bi : bg.border_ids) to_analyse_objs[bi] = {};
        //if(bg.__fill_obj) to_analyse_objs[bg.fill_id] = {};
        if(isFrame) { //认为是FramePart
            qDebug() << "解析=>FramePart.";
            std::shared_ptr<PDFOBJ_FramePart> fp = std::make_shared<PDFOBJ_FramePart>();
            //fp要记录因为它而隐藏了哪些对象
            //fp->invisible_items.assign(bg.__border_obj);
            for(auto& obj : bg.__border_obj) {
                fp->invisible_items.push_back(static_pointer_cast<PDFOBJ>(obj));
            }
            if(bg.__fill_obj) fp->invisible_items.push_back(bg.__fill_obj);
            fp->of_page = page;
            // 设置rect
            fp->rect = bg.rect;
            //fp->rect.moveTop(fp->rect.top()+page->)
            // 设置各种属性
            fp->fillColor = bg.fillColor;
            fp->borderColor = bg.borderColor;
            fp->lineWidth = bg.lineWidth;
            fp->fillSrc = bg.fillSrc;
            fp->fillMode = bg.fillMode;
            fp->radius = bg.radius;
            frame_parts.push_back(fp);
        } else { //认为是 Rich
            qDebug() << "解析=>Rich";
            //
            // to_analyse_objs.push_back(createRich())
        }
    }

    Helper::removeAllIf(to_analyse_objs,[](auto& obj) {
        return !obj || !obj->visible;
    }); // 删除涉及到的对象
    qDebug() << "Pdf2Souple::analyse_framepart_or_rich_or_area END$";
}

template<TT_Str TArg>
std::vector<std::pair<int,std::shared_ptr<Pdf2Souple::PDFOBJ_PATH>>>
Pdf2Souple::find_neighbors_of_path(const Iterable<const std::shared_ptr<PDFOBJ>> auto& all_objs,
                       int path_i,std::shared_ptr<PDFOBJ_PATH> path0)
{
    // 最坏情况O(n^3) 即所有路径一字排开连接。然而由于[路径合并预处理]的存在，这种情况几乎不会出现。
    // 通常时间复杂度为O(kn)，k一般小于5^2=25。而n也一般<30。因此没必要优化。(并查集...)
    qDebug() << "Pdf2Souple::find_neighbors_of_path BEGIN";
    vector<std::pair<int,std::shared_ptr<Pdf2Souple::PDFOBJ_PATH>>> neighbors;
    std::set<Pdf2Souple::PDFOBJ_PATH*> _set;
    neighbors.push_back({path_i,path0});
    _set.insert(path0.get());
    const float offset = Helper::cm2pixel(LINE_SAME_OFFSET_cm);
    while(true) {
        bool hasNew = false;
        for(auto[i,obj] : all_objs | std::views::enumerate)
        {
            if( ! obj->visible) continue;
            auto path = dynamic_pointer_cast<PDFOBJ_PATH>(obj);
            if(!path) continue;
            if constexpr(TArg.getBoolArg("onlyLine")) {
                vector<PDFOBJ_PATH::Line> lines;
                if(!path->toLinesIfRect(lines)) continue;
                if(lines.size() > 1) continue;
            }
            if(_set.count(path.get())) continue;
            for(auto&[j,path1] : neighbors) {
                if(path->rect.right()+offset < path1->rect.left()
                    ||path->rect.bottom()+offset < path1->rect.top()
                    ||path->rect.left()-offset > path1->rect.right()
                    ||path->rect.top()-offset > path1->rect.bottom()) continue;
                neighbors.push_back({i,path});
                _set.insert(path.get());
                hasNew = true;
                break;
            }
        }
        if(!hasNew) break;
    }
    qDebug() << "Pdf2Souple::find_neighbors_of_path END neighbors.num=" << neighbors.size();
    return neighbors;
}


bool Pdf2Souple::findFrameTopAndBottomLine(RandomAccessCont<HorLine_Base*> auto& sorted_hlines,
                               Pdf2Souple::PDFOBJ_FramePart* framepart,float page_y)
{
    // 1.1 找到第一条位于framepart顶线的下方的hline
    auto it_top = ranges::lower_bound(sorted_hlines,framepart->rect.top()+page_y,
                                      std::ranges::less{},
                                      [](HorLine_Base* hline){ return hline->y; });
    if(it_top == sorted_hlines.end()) return false; //failed
    // 1.2 往下继续找，找出被framepart包含的最宽的hline;
    // 为什么？考虑嵌套rich框，嵌套。。。会导致一个区域会有多种宽度的水平标线
    HorLine_Base* hline_ofMaxWidth = 0;
    auto it = it_top;
    qDebug() << "findFrameTopAndBottomLine( framepart.rect=" << framepart->rect;
    while(it != sorted_hlines.end()
           && (*it)->y >= framepart->rect.top()+page_y
           && (*it)->y <= framepart->rect.bottom()+page_y) {
        //qDebug() << "at " << (*it)->__dstr() << (*it)->x << (*it)->y << (*it)->width;
        HorLine_Base* hline = *it;
        float leftX_sub = hline->x + hline->getLeftTransparentWidth();
        float rightX_sub = hline->x + hline->width - hline->getRightTransparentWidth();
        //qDebug() << "l,r:sub=" << leftX_sub << rightX_sub;
        if(leftX_sub+2.0f >= framepart->rect.left()
            || rightX_sub-2.0f <= framepart->rect.right())
        {
            if(!hline_ofMaxWidth || hline->width > hline_ofMaxWidth->width) {
                hline_ofMaxWidth = hline;
            }
        }
        ++ it;
    }

    // 没有任何inside的hline
    if(! hline_ofMaxWidth) {
        qDebug() << "Failed: not found hline_ofMaxWidth";
        return false;
    }

    // 1.3 现在找到了最宽的inside-hline，再根据它得到top/bottom hline
    HorLine_Base *topHLine, *bottomHLine;
    topHLine = bottomHLine = hline_ofMaxWidth;
    while(true) {
        auto prevLine = topHLine->getPrevLine();
        if(! prevLine) break;
        if(prevLine->y < framepart->rect.top()) break;
        float leftX_sub = prevLine->x + prevLine->getLeftTransparentWidth();
        float rightX_sub = prevLine->x + prevLine->width - prevLine->getRightTransparentWidth();
        if(leftX_sub+2.0f < framepart->rect.left()
            || rightX_sub-2.0f > framepart->rect.right()) break;
        topHLine = prevLine;
    }
    while(true) {
        auto nextLine = bottomHLine->getNextLine();
        if(! nextLine) break;
        if(nextLine->y > framepart->rect.bottom()) break;
        float leftX_sub = nextLine->x + nextLine->getLeftTransparentWidth();
        float rightX_sub = nextLine->x + nextLine->width - nextLine->getRightTransparentWidth();
        if(leftX_sub+2.0f < framepart->rect.left()
            || rightX_sub-2.0f > framepart->rect.right()) break;
        bottomHLine = nextLine;
    }

    std::tie(framepart->topLine,
             framepart->bottomLine) = {topHLine,bottomHLine};
    return true;
}

// 合并跨栏跨页的frames
// 显然，这里的frames是最外层的frame.因为内层的frame直接就创建为Frame了
void Pdf2Souple::mergeAndCreateFrames(Iterable<std::shared_ptr<Pdf2Souple::PDFPage>> auto& pages)
{
    qDebug() << "## Pdf2Souple::mergeAndCreateFrames BEGIN";
    std::set<PDFOBJ_FramePart*> hasDeal; //已经被处理过的FramePart

    // 找出所有水平线,并按y坐标排序
    vector<HorLine_Base*> all_hlines;
    for(auto obj : SoupleManager::all_objs) {
        auto hline = obj->as<HorLine_Base*>();
        if(hline) all_hlines.push_back(hline);
    }

    if(all_hlines.empty()) return;

    ranges::sort(all_hlines,[](auto& a,auto& b){return a->y < b->y;});

    // 给每个FramePart找出它的top和bottom水平线
    for(auto& page : pages) {
        for(auto& framepart : page->frame_parts) {
            bool found = findFrameTopAndBottomLine(all_hlines,framepart.get(),page->page_top_margin);
            if(!found) {
                qDebug() << "Not Found top&bottom lines for a framepart in page" << page->page_index;
            } else {
                qDebug() << "topLine=" << framepart->topLine->__dstr();
                qDebug() << "bottomLine=" << framepart->bottomLine->__dstr();
            }
        }
    }

    struct Pre_Frame {
        HorLine_Base *startLine, *endLine;
        PDFOBJ_FramePart* head; //第一个framepart
        PDFOBJ_FramePart* tail;
    };

    std::map<HorLine_Base*,Pre_Frame> endline2framepart;

    // 对每个framepart进行遍历和create
    for(std::shared_ptr<PDFOBJ_FramePart>& framepart : pages|MEMBER(frame_parts)|views::join) {
        if(!framepart->topLine || !framepart->bottomLine) continue;
        HorLine_Base* prevLine = framepart->topLine->getPrevLine();
        if(prevLine && endline2framepart.count(prevLine)) {
            Pre_Frame prevFrame = endline2framepart[prevLine];
            //检查一下，是否要和这个Frame合并哦
            if(prevFrame.head->fillMode == framepart->fillMode
                && (prevFrame.head->fillMode != Helper::Color_Fill
                    || prevFrame.head->fillColor == framepart->fillColor
                        && prevFrame.head->borderColor == framepart->borderColor
                        && abs(prevFrame.head->lineWidth - framepart->lineWidth) < 0.2f)
                && (prevFrame.head->fillMode != Helper::Image_Fill
                    || prevFrame.head->fillSrc == framepart->fillSrc)
                && (abs(prevFrame.head->radius - framepart->radius) < 0.2f)
                )
            {
                //得合并
                prevFrame.tail = framepart.get();
                prevFrame.endLine = framepart->bottomLine;
                endline2framepart.erase(prevLine); //记得erase一下
                endline2framepart[prevFrame.endLine] = prevFrame; //重新记录它
                qDebug() << "frame merged.";
                continue;
            }
        }
        // 至此，表明：未合并。
        Pre_Frame preframe; //创建Pre_Frame
        preframe.startLine = framepart->topLine;
        preframe.endLine = framepart->bottomLine;
        preframe.head = preframe.tail = framepart.get();
        endline2framepart[preframe.endLine] = preframe;
    }

    // 从Pre_Frame创建出Frame_ofHLines...p.s.这里为什么不用auto，因为clang推断不出这里的类型啊，后面就给不出代码提示。。
    for(Pre_Frame& pre_frame : endline2framepart | views::values)
    {
        qDebug() << "create Frame_ofHLines";
        auto instance = Frame_ofHLines_Instance::createFrame(
            pre_frame.startLine,pre_frame.endLine);
        instance->background_type = pre_frame.head->fillMode;
        instance->background_color = pre_frame.head->fillColor;
        instance->background_src = pre_frame.head->fillSrc;
        instance->lineWidth = pre_frame.head->lineWidth;
        instance->radius = pre_frame.head->radius;
        instance->border_color = pre_frame.head->borderColor;
        instance->leftPadding = pre_frame.startLine->x - pre_frame.head->rect.left();
        instance->rightPadding = pre_frame.head->rect.right() - pre_frame.startLine->getRightX();
        instance->topPadding = pre_frame.startLine->getContentTop()
                    - (pre_frame.head->rect.top()+pre_frame.head->of_page->page_top_margin);
        instance->bottomPadding =
                    (pre_frame.tail->rect.bottom()+pre_frame.tail->of_page->page_top_margin)
                    -pre_frame.endLine->getContentBottom();
    }

    qDebug() << "## Pdf2Souple::mergeAndCreateFrames END";
}

template<TT_Str tt>
bool Pdf2Souple::PDFOBJ_PATH::toSolidRect(float& x1,float& y1,float& x2, float& y2,
                                          uint8_t* numLines) const
{
    switch(list_path_actions.size()) {
    case 1: {
        if(list_path_actions[0].type != Path_Action::LineTo) return false;
        *numLines = 1;
        break;
    }
    case 2: {
        if(list_path_actions[0].type != Path_Action::MoveTo
            || list_path_actions[1].type != Path_Action::LineTo) return false;
        if(abs(list_path_actions[0].x - list_path_actions[0].x) > 1.0f
            && abs(list_path_actions[0].y - list_path_actions[0].y) > 1.0f) {
            return false;
        }
        *numLines = 1;
        break;
    }
    case 3: {
        if constexpr (tt.getBoolArg("forceFill",true) == true) {
            if(fill == false) return false;
        }
        if(list_path_actions[0].type != Path_Action::LineTo
            || list_path_actions[1].type != Path_Action::LineTo
            || list_path_actions[2].type != Path_Action::LineTo) return false;
        std::set<int> x_set, y_set;
        x_set.insert(0); y_set.insert(0);
        for(auto& a : list_path_actions) {
            x_set.insert(round(a.x));
            y_set.insert(round(a.y));
        }
        if(x_set.size() <= 2 && y_set.size() <= 2) {
            *numLines = 4;
            break;
        }
        else return false;
    }
    case 4: {
        if constexpr (tt.getBoolArg("forceFill",true) == true) {
            if(fill == false) return false;
        }
        if(list_path_actions[0].type != Path_Action::MoveTo
            || list_path_actions[1].type != Path_Action::LineTo
            || list_path_actions[2].type != Path_Action::LineTo
            || list_path_actions[3].type != Path_Action::LineTo) return false;
        std::set<int> x_set, y_set;
        for(auto& a : list_path_actions) {
            x_set.insert(round(a.x/2));
            y_set.insert(round(a.y/2));
        }
        if(x_set.size() <= 2 && y_set.size() <= 2) { //矩形判断
            *numLines = 4;
            break;
        }
        else return false;
    }
    case 5: {
        if constexpr (tt.getBoolArg("forceFill",true) == true) {
            if(fill == false) return false;
        }
        if(list_path_actions[0].type != Path_Action::MoveTo
            || list_path_actions[1].type != Path_Action::LineTo
            || list_path_actions[2].type != Path_Action::LineTo
            || list_path_actions[3].type != Path_Action::LineTo
            || list_path_actions[4].type != Path_Action::LineTo) return false;
        std::set<int> x_set, y_set;
        for(auto& a : list_path_actions) {
            x_set.insert(round(a.x/2));
            y_set.insert(round(a.y/2));
        }
        if(x_set.size() <= 2 && y_set.size() <= 2) { //矩形判断
            *numLines = 4;
            break;
        }
        else return false;
    }
    default:
        return false;
    }

    x1 = 1e9;
    x2 = -1e9;
    y1 = 1e9;
    y2 = -1e9;

    for(auto& a : list_path_actions) {
        x1 = std::min(x1,a.x);
        x2 = std::max(x2,a.x);
        y1 = std::min(y1,a.y);
        y2 = std::max(y2,a.y);
    }

    if(*numLines == 4) {
        *numLines = 1;
    }
    if constexpr(tt.getBoolArg("extendLineWidth",true)) {
        //x1 = rect.left(), y1 = rect.top(), x2 = rect.right(), y2 = rect.bottom();
        x1 -= lineWidth;
        x2 += lineWidth;
        y1 -= lineWidth;
        y2 += lineWidth;
    } else {
        //x1 = rect.left()+lineWidth*0.5, y1 = rect.top()+lineWidth*0.5,
        //    x2 = rect.right()-lineWidth*0.5, y2 = rect.bottom()-lineWidth*0.5;

    }
    qDebug() << "toSolidRect Completed:ok,numlines=" << (numLines?*numLines:-1);
    return true;
}

#endif // PDF2SOUPLE_TEMPLATE_IMPL_HPP
