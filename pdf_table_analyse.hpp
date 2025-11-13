#ifndef PDF_TABLE_ANALYSE_H
#define PDF_TABLE_ANALYSE_H

#include "pdf2souple.h"
#include "blockinner_horline.h"
#include <span>
#include <ranges>
#include "souplemanager.h"
#include <concepts>
#include "TableFactory.h"

#define LINE_INTERSECT_OFFSET 5 //多大间距认为线条相交
#define LINE_SAME_OFFSET_cm 0.06625 //两条线坐标相同的误差
#define TABLEPART_WIDTH_SAME_OFFSET_cm 0.1
#define COLUMN_LINE_SAME_OFFSET_cm 0.1

//本文件提供对pdf解析表格的函数

enum {NotEndPage = 0,IsEndPage = 1};


// //使用模板仅仅是为了让外部类方便设置函数友元
// // ~~~ c++模板可以把类型信息的获取延迟到实现时。
// template<typename T1,typename T2>
// void analyseTable(T1,T2);

// // template<typename T1,typename T2>
// // void tryMergeTable(T1,T2);

// void layoutTables();


template<TT_Str tt>
void Pdf2Souple::impl_analyseTable(
    PDFPage* page,
    RandomAccessCont<std::shared_ptr<PDFOBJ>> auto& objs)
{
    /** 第一步，解析objs中的表格 */

    // 注意，要求获取的线条的x1 y1 x2 y2必须考虑到线宽，不能忽略线宽！
    struct Line_Obj_Record {
        bool hasBelong = false;
        int obj_index;
        float x1,y1,x2,y2;
        std::shared_ptr<Pdf2Souple::PDFOBJ_PATH> path_obj;
        float cx() const noexcept { return (x1+x2)/2; }
        float cy() const noexcept { return (y1+y2)/2; }
        float width() const noexcept { return x2 - x1; }
        float height() const noexcept { return y2 - y1; }
        //检查两个线条是否有相交
        bool intersect(Line_Obj_Record const& l2) const noexcept {
            auto offset = Helper::cm2pixel(LINE_SAME_OFFSET_cm);
            if(x2 + offset < l2.x1 || l2.x2 + offset < x1
                || y2 + offset < l2.y1 || l2.y2 + offset < y1) return false;
            return true;
        }
    };

    QList<Line_Obj_Record> line_objs;

    float line_same_offset = Helper::cm2pixel(LINE_SAME_OFFSET_cm);

    // 找出所有`线条路径`类型的PDFOBJ，同时记录索引。
    for(int i = 0; i < /*page->all_objs*/objs.size(); ++i) {
        if(auto path_obj = std::dynamic_pointer_cast<Pdf2Souple::PDFOBJ_PATH>(/*page->all_objs*/objs[i]);
            path_obj) {
            //float x1,y1,x2,y2;
            //if( ! path_obj->toSolidRect(x1,y1,x2,y2)) continue;
            std::vector<Pdf2Souple::PDFOBJ_PATH::Line> glines;
            if(! path_obj->toLinesIfRect(glines)) continue;
            for(auto[j,line] : glines | views::enumerate) {
                Line_Obj_Record rec;
                rec.obj_index = (j == 0 ? i : -1);
                rec.path_obj = path_obj;
                rec.x1 = line.x1;
                rec.x2 = line.x2;
                rec.y1 = line.y1;
                rec.y2 = line.y2;
                if(rec.x2 - rec.x1 < rec.y2 - rec.y1) {
                    // 线宽小于4pt
                    if(rec.x2 - rec.x1 <= Helper::point2pixel(4))
                        line_objs.push_back(std::move(rec));
                } else {
                    // 线高小于4pt
                    if(rec.y2 - rec.y1 <= Helper::point2pixel(4))
                        line_objs.push_back(std::move(rec));
                }
            }
        }
    }

    qDebug() << "lineObjs.count = " << line_objs.count();

    for(int i = 0; i < line_objs.size(); ++i) {
        auto& line = line_objs[i];
        if(line.hasBelong) continue;
        // line是一条线(实心矩形),找出与它邻接的所有线(实心矩形)
        QList<Line_Obj_Record*> h_borders,v_borders;
        if(line.y2 - line.y1 > line.x2 - line.x1) v_borders.push_back(&line);
        else h_borders.push_back(&line);
        line.hasBelong = true;

        while(true) {
            bool hasNew = false;
            for(auto& line2 : line_objs) {
                if(line2.hasBelong) continue;

                for(auto border : views::join(std::array{h_borders,v_borders})) {
                    //if(QRectF(line2.x1,line2.y1,line2.x2-line2.x1,line2.y2-line2.y1)
                    //        .intersects(QRectF(border->x1,border->y1,border->x2-border->x1,border->y2-border->y1)))
                    if(line2.intersect(*border))
                    {
                        line2.hasBelong = true;
                        if(line2.y2 - line2.y1 > line2.x2 - line2.x1) {
                            v_borders.push_back(&line2);
                        }
                        else {
                            h_borders.push_back(&line2);
                        }
                        hasNew = true;
                        break;
                    }
                }
            }
            if( !hasNew ) break;
        }
        //到此，找到了组成边框的所有路径对象
        //qDebug() << "找到边框：";
        //for(auto r : borders) r->path_obj->print();
        /** 判断边框是否围成矩形 */

        if(h_borders.size() < 2 || v_borders.size() < 2) continue; //不是表格

        std::ranges::sort(h_borders,[](auto l1,auto l2){ return l1->y1 < l2->y1; });
        std::ranges::sort(v_borders,[](auto l1,auto l2){ return l1->x1 < l2->x1; });

        float width = v_borders.back()->cx() - v_borders.front()->cx();
        float height = h_borders.back()->cy() - h_borders.front()->cy();
        float width2 = v_borders.back()->x2 - v_borders.front()->x1;
        float height2 = h_borders.back()->y2 - h_borders.front()->y1;
        float table_left = v_borders.front()->cx();
        float table_top = h_borders.front()->cy();

        qDebug() << "检测表格，页码：" << page->page_index;
        //qDebug() << v_borders.front()->height() - height2 <<
        //    v_borders.back()->height() - height2 << h_borders.front()->width() - width2
        //         <<h_borders.back()->width() - width2;

        auto tablepart = std::make_shared<Pdf2Souple::PDFOBJ_TablePart>();

        std::vector<size_t> border_pdfobj_id_list;

        for(auto border : std::ranges::views::join(std::array{h_borders,v_borders})) {
            if(border->obj_index >= 0) { //记录边框的pdfobj，便于合并后删除
                border_pdfobj_id_list.push_back(border->obj_index);
            }
        }


        /** 合并看上去连续的边线(比如latex导出的表格喜欢把一条完整的线割断) */
        // 一定要在删除pdfobj后再合并！

        for(int i = 0; i < h_borders.size(); ) {
            auto h0 = h_borders[i];
            int j = i+1;
            while(j < h_borders.size()) {
                auto h1 = h_borders[j];
                if(abs(h0->cy() - h1->cy()) > line_same_offset) break;
                ++j;
            }
            // h_borders [i,j)即同一行的hlines了
            // 给它们排个序：按x1坐标升序
            std::sort(&h_borders[i],&h_borders[j-1]+1,_Pred(e1->x1 < e2->x1));
            qDebug() << "同一行的h_border:";
            for(int c = i; c < j; ++c) {
                qDebug() << h_borders[c]->x1 << h_borders[c]->x2 << h_borders[c]->y1 <<
                    h_borders[c]->y2;
            }
            // 合并
            int k = 1;
            for(int c = i+1; c < j; ++c) {
                auto h_left = h_borders[c-1];
                auto h_cur = h_borders[c];
                if(abs(h_left->x2 - h_cur->x1) < line_same_offset) {
                    //需要合并
                    h_borders[c-1] = nullptr;
                    h_cur->x1 = h_left->x1; //合并到右边的h_border
                    qDebug() << "合并h_border";
                } else {
                    ++k;
                }
            }
            // 现在,[j-k,j-1]即合并后的obj
            i = j;
        }

        //删除无效h_border
        Helper::removeAll(h_borders,(Line_Obj_Record*)nullptr);

        for(int i = 0; i < v_borders.size(); ) {
            auto v0 = v_borders[i];
            int j = i+1;
            while(j < v_borders.size()) {
                auto v1 = v_borders[j];
                if(abs(v0->cx() - v1->cx()) > line_same_offset) break;
                ++j;
            }
            // v_borders [i,j)即同一列的vlines了
            // 给它们排个序：按y1坐标升序
            std::sort(&v_borders[i],&v_borders[j-1]+1,_Pred(e1->y1 < e2->y1));
            qDebug() << "同一列的v_border:";
            for(int c = i; c < j; ++c) {
                qDebug() << v_borders[c]->x1 << v_borders[c]->x2 << v_borders[c]->y1 <<
                    v_borders[c]->y2;

            }
            // 合并
            int k = 1;
            for(int c = i+1; c < j; ++c) {
                auto v_left = v_borders[c-1];
                auto v_cur = v_borders[c];
                if(abs(v_left->y2 - v_cur->y1) < line_same_offset) {
                    //需要合并
                    v_borders[c-1] = nullptr;
                    v_cur->y1 = v_left->y1; //合并到右边的v_border
                    qDebug() << "合并v_border";
                } else {
                    ++k;
                }
            }
            // 现在,[j-k,j-1]即合并后的obj
            i = j;
        }

        //删除无效v_border
        Helper::removeAll(v_borders,(Line_Obj_Record*)nullptr);


        if(h_borders.size() < 2 || v_borders.size() < 2) continue; //不是表格

        //单个矩形不认为是表格（即便它可能是被分栏、分页截断的表头）
        if(h_borders.size() == 2 && v_borders.size() == 2) continue;

        if(abs(v_borders.front()->height() - height2) > LINE_INTERSECT_OFFSET
            || abs(v_borders.back()->height() - height2) > LINE_INTERSECT_OFFSET
            || abs(h_borders.front()->width() - width2) > LINE_INTERSECT_OFFSET
            || abs(h_borders.back()->width() - width2) > LINE_INTERSECT_OFFSET) {
            qDebug() << "不是矩形=>" << "不是表格";
            qDebug() << v_borders.front()->height() - height2 << v_borders.back()->height() - height2
                     << h_borders.front()->width() - width2 << h_borders.back()->width() - width2;
            continue; //不是表格
        }

        // 找到所有处在这个表格里的pdf对象，记录并删除。
        std::list<std::shared_ptr<Pdf2Souple::PDFOBJ>> objs_inTable;

        //先删边框，再找obj_inTable
        for(size_t obj_i : border_pdfobj_id_list) {
            /*page->all_objs*/objs[obj_i].reset(); //删除边框对应的pdfobj
        }

        for(auto& obj : /*page->all_objs*/objs) {
            if(! obj) continue;
            if(obj->rect.left() > table_left && obj->rect.right() < table_left + width
                && obj->rect.top() > table_top && obj->rect.bottom() < table_top + height) {
                objs_inTable.push_back(obj);
                obj.reset(); //置0
            }
        }

        /** 创建pdf解析时的tableline，此tableline包含了准确的contentTop x y width height等坐标信息 */

        //float left_x = h_borders[0]->cx();
        float last_y = h_borders[0]->cy() + page->page_top_margin;
        float border_y = -1e9;
        // 【Deleted】 注：h_borders的第一个hborder必定完整。即不用考虑顶线有多条。
        //////  注：第一个hborder也不一定完整，因为顶线也可能割裂！2025_9_8

        TableLine *last_tableline = 0;
        for(auto hborder : h_borders ){ //| std::views::drop(1)) { //c++20 drop(1):删除第一个元素。惰性view

            //qDebug() << "---hborder: " << hborder->cy() << hborder->x1 << hborder->x2;

            //跳过顶边线
            if(abs(hborder->cy() - h_borders[0]->cy() < line_same_offset)) continue;
            //跳过同一行的水平线
            if(abs(hborder->cy() + page->page_top_margin - border_y) < line_same_offset) continue;

            border_y = hborder->cy() + page->page_top_margin; //表格水平线 y 坐标
            TableLine *tableline = new TableLine;
            //只创建对象，不注册。
            /** Create TableLine */
            tableline->x = table_left;
            tableline->y = border_y;
            tableline->width = width;
            tableline->height = tableline->y - last_y;
            tablepart->tablelines.push_back(tableline);
            last_y = tableline->y;
            if(last_tableline) {
                last_tableline->anchor_nextHLine.push_back(tableline);
                tableline->hline = last_tableline;
                last_tableline->logic_nextHLine = tableline;
                tableline->logic_lastHLine = last_tableline;
            }
            last_tableline = tableline;
        }

        /*page->all_objs*/objs.push_back(tablepart); //添加对象~部分表格

        /** 检查该表格是否与上一页的表格是接续的*/

        /** 创建表格*/
        //auto pdf_table = std::make_shared<Pdf2Souple::PDFTable>();
        //page->tables.push_back(pdf_table);

        // 添加tableline(行)
        //pdf_table->tablelines.append_range(tablepart->tablelines);

        tablepart->rect.setRect(table_left,table_top,width2,height2);
        page->table_parts.push_back(tablepart);

        tablepart->row_units.resize(tablepart->tablelines.size());

        qDebug() << "解析部分表格: 行数" << tablepart->tablelines.size();

        // 添加各行单元格
        int hi = 1;
        for(auto [row_i,tableline] : tablepart->tablelines | std::views::enumerate) {
            auto line_top = tableline->y - tableline->height;
            auto& line_bottom = tableline->y; //对局部变量使用引用来“取别名”。
            std::vector<Free_TableUnit*> units;
            float left_x = table_left;
            int hendi = hi;  // [hi,hendi) ~ 位于tableline行的h_border索引区间
            while(hendi < h_borders.size() && abs(tableline->y - page->page_top_margin - h_borders[hendi]->cy())
                                                   < Helper::cm2pixel(LINE_SAME_OFFSET_cm)
                   ) {
                hendi += 1;
            }

            // 找出属于该行的PDFOBJ
            std::list<std::shared_ptr<Pdf2Souple::PDFOBJ>> objs_inLine;
            for(auto it = objs_inTable.begin(); it != objs_inTable.end(); ) {
                auto obj = *it;
                float obj_cy = obj->rect.center().y() + page->page_top_margin;
                if(obj_cy <= line_bottom) {
                    //由于tableline从上往下遍历，只需且只能判断line_bottom来判断该单元格是否可能属于本行。
                    objs_inLine.push_back(obj);
                    it = objs_inTable.erase(it); //删除obj in objs_inTable
                } else {
                    ++it;
                }
            }

            for(auto vborder : v_borders | std::views::drop(1)) {
                // 从左往右，找出所有覆盖本行的垂直表格线，即可划分出单元格。
                if(vborder->y1 + page->page_top_margin >= line_bottom -  tableline->height/2
                    || vborder->y2 + page->page_top_margin <= line_top + tableline->height/2)
                    continue; //未覆盖

                //qDebug() << "垂直切割：" << vborder->y1 + page->page_top_margin
                //         << vborder->y2 + page->page_top_margin;

                //单元格：left = left_x; right = vborder->cx();
                //此时，不需要确定单元格的高度。只需确定单元格的left、right及归属行。
                bool hasBottomBorder = false; //下边有没有边框呢？没有的话，跳过~此单元格归属于下面的某行。
                for(auto hborder : std::span(h_borders).subspan(hi,hendi-hi)) { //span: 一种轻型容器
                    if(hborder->x1 <= left_x && hborder->x2 >= vborder->cx())
                    {
                        hasBottomBorder = true;
                        break; }
                }

                if(hasBottomBorder) { //该单元格为本行所有
                    auto unit = new Free_TableUnit;
                    float unit_top;
                    SoupleManager::registerObj(unit);
                    unit->x = left_x;
                    unit->width = vborder->cx() - left_x;
                    std::vector<std::shared_ptr<Pdf2Souple::PDFOBJ>> objs_inUnit;

                    /** 找到属于本单元格的PDFOBJ，建立块内布局。 */
                    for(auto it = objs_inLine.begin(); it != objs_inLine.end(); ) {
                        auto obj = *it;
                        if(obj->rect.left() >= left_x && obj->rect.right() <= vborder->cx()) {
                            objs_inUnit.push_back(obj);
                            it = objs_inLine.erase(it);
                        } else {
                            ++it;
                        }
                    }

                    /** 需要确定该单元格的top */
                    /// ~往前面的hborder找到第一个覆盖本单元格的

                    for(auto hborder2 : h_borders | std::views::take(hi) | std::views::reverse)
                    {
                        if(hborder2->x1 <= left_x + unit->width*0.3 && //注意留出变动值
                            hborder2->x2 >= vborder->x1 - unit->width*0.3) {
                            unit_top = hborder2->cy() + page->page_top_margin;
                            break;
                        }
                    }

                    float unit_height = line_bottom - unit_top;

                    unit->height = unit_height;
                    unit->y = unit_top;

                    // qDebug() << " - - 确定单元格: rect(" << unit->x << unit->y << unit->width << unit->height;

                    QList<Pdf2Souple::HBlock> hblocks;
                    Pdf2Souple::createHBlocks(objs_inUnit,page->page_width,hblocks,false);
                    unit->firstLine = 0;
                    Pdf2Souple::createRich(page,hblocks,unit,&unit->vAlignMode,&unit->contentHeight,
                                           &unit->firstLine,QRectF{unit->x,unit_top,unit->width,unit_height});
                    tablepart->row_units[row_i].push_back(unit); //记录单元格
                }
                left_x = vborder->cx();
            }

            // 我们要把objs_inLine里面剩下的obj(~由于单元格合并导致)还给objs_inTable
            for(auto& obj : objs_inLine)
                objs_inTable.push_back(obj);
            //pdf_table->row_units.push_back(std::move(units));
            hi = hendi;
        }

        //如果"inside=true"，需要立即创建表格实例。
        if constexpr(tt.getBoolArg("inside",false) == true) {

        }

    }


    // 删除涉及的对象
    int _n2 = 0;
    for(auto ptr : /*page->all_objs*/objs)
        if(ptr) /*page->all_objs*/objs[_n2++] = ptr;
    /*page->all_objs*/objs.resize(_n2);
}

/**
 * @brief analyseTable 解析页面的表格，删除表格涉及的对象，
 *        把PDFOBJ_TablePart加入到page->all_objs中。
 * @param page 当前页
 * @param old_page 上一页
 * @template TArg可以忽略。
 */
template<class TArg>
void Pdf2Souple::analyseTable(Pdf2Souple::PDFPage* page, Pdf2Souple::PDFPage* old_page)
{
    impl_analyseTable<TStr("inside=false")>(page,page->all_objs);
    return;
}

template<TT_Str tt /*= TStr("enable_table_row_num=false")*/>
TableInfo* Pdf2Souple::createTableFromTableParts
    (Iterable<Pdf2Souple::PDFOBJ_TablePart*> auto& tablepart_list,
        int table_row_num)
{

    TableInfo *ti = new TableInfo;
    std::vector<float> column_x_set;

    // 是否需要从tp_list中计算表格行数
    if constexpr(tt.getBoolArg("enable_table_row_num",false) == false) {
        table_row_num = 0;
        for(Pdf2Souple::PDFOBJ_TablePart* tp : tablepart_list) {
            table_row_num += (int)tp->tablelines.size();
        }
    }

    for(auto tp : tablepart_list) {
        for(auto& row_units : tp->row_units) {
            for(auto unit : row_units) {
                //注意，column_x_set插入的是各垂直线到表格左边框的距离。因为表格可能跨栏。
                column_x_set.push_back(unit->x - tp->rect.left());
                column_x_set.push_back(unit->x+unit->width - tp->rect.left());
            }
        }
    }

    ranges::sort(column_x_set);

    int num_column_x = 0;
    for(int i = 0; i < column_x_set.size(); ) {
        float x_sum = 0, lval = column_x_set[i];
        int nl = 0;
        for( ; i < column_x_set.size(); ++i) {
            if(column_x_set[i] - lval > Helper::cm2pixel(COLUMN_LINE_SAME_OFFSET_cm)) {
                break;
            }
            x_sum += column_x_set[i];
            ++nl;
        }
        column_x_set[num_column_x++] = x_sum / nl;
    }

    column_x_set.resize(num_column_x);

    //////(2)设置tableinfo
    float left_vline_x = *column_x_set.begin();
    for(float line_x : column_x_set | views::drop(1)) {
        ti->colWidths.push_back(line_x - left_vline_x); //列宽
        left_vline_x = line_x;
    }

    // grid_filled: 记录表格各个单元格是否被“占据”
    unsigned char *grid_filled = (unsigned char*)operator new(ti->colCount() * table_row_num);
    memset(grid_filled,0x0,ti->colCount() * table_row_num);

    //tablepart_list[0]->tablelines
    ti->firstLine = tablepart_list.front()->tablelines.front();
    ti->endLine = tablepart_list.back()->tablelines.back();
    int rowId = 0;
    PDFOBJ_TablePart* last_tablepart = 0;
    for(auto tablepart : tablepart_list) {
        if(last_tablepart) { //逻辑连接
            tablepart->tablelines.front()->setLogicLastHLine(last_tablepart->tablelines.back());
        }
        for(auto[part_rowId,tableline] : tablepart->tablelines | views::enumerate) {
            tableline->table_info = ti;
            tableline->row = rowId;
            ti->rowHeights.push_back(tableline->height);
            ti->units.push_back(vector<TableInfo::UnitInfo>(ti->colCount()));
            auto &row = ti->units.back();
            int col_at = 0;
            for(auto unit : tablepart->row_units[part_rowId]) {
                float unit_right = unit->getRightX() - tablepart->rect.left();
                float unit_left = unit->x - tablepart->rect.left();
                //auto line_right_iter = column_x_set.lower_bound(unit_right - Helper::cm2pixel(COLUMN_LINE_SAME_OFFSET_cm));
                //if(line_right_iter == column_x_set.end()) --line_right_iter;
                //auto line_left_iter = column_x_set.lower_bound(unit_left - Helper::cm2pixel(COLUMN_LINE_SAME_OFFSET_cm));
                //if(line_left_iter == column_x_set.end()) --line_left_iter;
                // auto iter_next = line_iter; /// 考虑极窄的单元格？
                // ++iter_next;
                // while(iter_next != column_x_set.end() && )
                //表格的列数有限，直接遍历找到单元格跨的所有列
                int col1,col2;
                float pile_width = 0;
                float _offset = Helper::cm2pixel(COLUMN_LINE_SAME_OFFSET_cm);
                for(int i = 0; i < ti->colCount(); ++i) {
                    if(pile_width + _offset >= unit_left
                        || (qAbs(pile_width - unit_left) <= qAbs(pile_width + ti->colWidths[i] - unit_left))) {
                        col1 = i;
                        break;
                    }
                    pile_width += ti->colWidths[i];
                }

                if(col1 < col_at) {
                    col1 = col_at;
                    //qDebug()
                }

                pile_width = 0;
                for(int i = 0; i < ti->colCount(); ++i) {
                    pile_width += ti->colWidths[i];
                    if(pile_width + _offset >= unit_right
                        || (i+1 < ti->colCount() &&
                            qAbs(pile_width - unit_right) <= qAbs(pile_width + ti->colWidths[i+1] - unit_right))) {
                        col2 = i;
                        break;
                    }
                }
                /// ---> 单元格覆盖列[col1,col2]
                while(col_at < col2) { //把前面的单元格置null，它们不属于本行
                    row[col_at].u = 0;
                    ++col_at;
                }
                col_at = col2+1;
                row[col2].start_col = col1;
                row[col2].end_col = col2;
                row[col2].end_row = rowId;
                // 确定单元格跨行数
                // 单元格不会跨越tablepart
                int start_row = rowId;
                for(int j = part_rowId; j > 0; --j) {
                    //if(ti->units[start_row - 1][col2].u != 0) {
                    if(grid_filled[(start_row - 1)*ti->colCount()+col2] > 0) {
                        break;
                    }
                    start_row -= 1;
                }
                row[col2].start_row = start_row;
                row[col2].u = unit;
                unit->tableinfo = ti;
                // 记录
                for(int i : views::iota(start_row,rowId+1))
                    for(int j : views::iota(col1,col2+1))
                        grid_filled[i*ti->colCount()+j] = 1;
            }

            rowId += 1;
        } //tableline FOR

        last_tablepart = tablepart;

    } //tablepart FOR

    ///赋予
    TableFactory::calcControlInfo(ti); //计算控制行信息

    ///表格对齐
    ti->alignMode = Helper::AlignHCenter;

    //表格宽度
    ti->width = accumulate(ti->colWidths.begin(),ti->colWidths.end(),0);

    operator delete(grid_filled,ti->colCount() * ti->rowCount()); //free

    return ti;
}


/**
 * @brief tryMergeTable 在页面解析完毕后，尝试合并本页连续的表格、合并本页与上一页连续的表格。
 * @param pages: 包含所有PDFPage的可迭代容器。(c++20 concepts)
 */
void Pdf2Souple::mergeAndCreateTables(Iterable<std::shared_ptr<Pdf2Souple::PDFPage>> auto& pages)
{
    typedef std::shared_ptr<Pdf2Souple::PDFPage> PageType;
    std::set<Pdf2Souple::PDFOBJ_TablePart*> tableparts_hasDeal; //已经处理过的表格部分
    std::unordered_map<TableLine*,Pdf2Souple::PDFOBJ_TablePart*> firstline_to_tablepart;
    qDebug() << "mergeAndCreateTables BEGIN";
    for(PageType& page : pages) {
        for(auto& tp : page->table_parts) {
            firstline_to_tablepart[tp->tablelines.front()] = tp.get(); //记录第一行对应的表格部分
        }
    }
    for(PageType& page : pages) {
        qDebug() << "第" << page->page_index+1 << "页";
        for(auto& tp : page->table_parts) {
            if(tableparts_hasDeal.contains(tp.get())) continue;
            ///开始创建表格
            //TableInfo *ti = new TableInfo;
            std::vector<Pdf2Souple::PDFOBJ_TablePart*> tablepart_list;
            //找到所有和该部分接续的部分表格
            TableLine* tline = tp->tablelines.front(), *last_tline = 0;
            unsigned int table_row_num = 0;
            while(tline) {
                qDebug() << "tline: " << tline->name;
                auto it = firstline_to_tablepart.find(tline);
                if(it != firstline_to_tablepart.end()) {
                    if(last_tline) {
                        if(last_tline->leftLine->isWordPageLine() != tline->leftLine->isWordPageLine())
                            break;
                        if(qAbs(last_tline->width - tline->width) > Helper::cm2pixel(TABLEPART_WIDTH_SAME_OFFSET_cm))
                            break;
                        last_tline->page = SoupleManager::getPage(*last_tline);
                        tline->page = SoupleManager::getPage(*tline);
                        if(tline->leftLine->isWordPageLine()) {
                            if(tline->page == last_tline->page
                                && tline->leftLine == last_tline->leftLine) { //同栏
                                if(tline->topMargin > Helper::cm2pixel(LINE_SAME_OFFSET_cm))
                                    break; //两个表格部分距离太远
                            }
                            //if(tline->l)
                            } else {
                            if(tline->page == last_tline->page) { //同页
                                if(tline->topMargin > Helper::cm2pixel(LINE_SAME_OFFSET_cm))
                                    break; //两个表格部分距离太远
                            }
                        }
                    }
                    tablepart_list.push_back(it->second);
                }
                ++ table_row_num;
                if(tline->anchor_nextHLine.empty()) break;
                last_tline = tline;
                tline = tline->anchor_nextHLine.front()->as<TableLine*>();
            }

            qDebug() << "合并表格：num of part: " << tablepart_list.size();
            for(auto tp : tablepart_list) {
                tableparts_hasDeal.insert(tp);
            }
            // if(page->page_type == Helper::Word_Page) { //表格源自Word类型页面
            //     //int col_index = page->souple_page->getLeftLineColumnIndex(tp->tablelines[0]->leftLine);

            // } else if(page->page_type == Helper::NoFormat_Page) { //无格式页面

            // }

            /** 创建TableInfo */
            //(1)确定所有列
            // std::set<float,
            // decltype([](float a,float b){
            //              if(qAbs(a-b) < Helper::cm2pixel(COLUMN_LINE_SAME_OFFSET_cm))
            //                  return false;
            //              return a < b;
            //              //return qAbs(a-b) < Helper::cm2pixel(COLUMN_LINE_SAME_OFFSET_cm);
            //          })> column_x_set;

            TableInfo *ti = createTableFromTableParts
                            <TStr("enable_table_row_num=true")>(tablepart_list,table_row_num);

            // std::vector<float> column_x_set;
            // for(auto tp : tablepart_list) {
            //     for(auto& row_units : tp->row_units) {
            //         for(auto unit : row_units) {
            //             //注意，column_x_set插入的是各垂直线到表格左边框的距离。因为表格可能跨栏。
            //             column_x_set.push_back(unit->x - tp->rect.left());
            //             column_x_set.push_back(unit->x+unit->width - tp->rect.left());
            //         }
            //     }
            // }

            // ranges::sort(column_x_set);

            // int num_column_x = 0;
            // for(int i = 0; i < column_x_set.size(); ) {
            //     float x_sum = 0, lval = column_x_set[i];
            //     int nl = 0;
            //     for( ; i < column_x_set.size(); ++i) {
            //         if(column_x_set[i] - lval > Helper::cm2pixel(COLUMN_LINE_SAME_OFFSET_cm)) {
            //             break;
            //         }
            //         x_sum += column_x_set[i];
            //         ++nl;
            //     }
            //     column_x_set[num_column_x++] = x_sum / nl;
            // }

            // column_x_set.resize(num_column_x);

            // //////(2)设置tableinfo
            // float left_vline_x = *column_x_set.begin();
            // for(float line_x : column_x_set | views::drop(1)) {
            //     ti->colWidths.push_back(line_x - left_vline_x); //列宽
            //     left_vline_x = line_x;
            // }

            // // grid_filled: 记录表格各个单元格是否被“占据”
            // unsigned char *grid_filled = (unsigned char*)operator new(ti->colCount() * table_row_num);
            // memset(grid_filled,0x0,ti->colCount() * table_row_num);

            // //tablepart_list[0]->tablelines
            // ti->firstLine = tablepart_list.front()->tablelines.front();
            // ti->endLine = tablepart_list.back()->tablelines.back();
            // int rowId = 0;
            // PDFOBJ_TablePart* last_tablepart = 0;
            // for(auto tablepart : tablepart_list) {
            //     if(last_tablepart) { //逻辑连接
            //         tablepart->tablelines.front()->setLogicLastHLine(last_tablepart->tablelines.back());
            //     }
            //     for(auto[part_rowId,tableline] : tablepart->tablelines | views::enumerate) {
            //         tableline->table_info = ti;
            //         tableline->row = rowId;
            //         ti->rowHeights.push_back(tableline->height);
            //         ti->units.push_back(vector<TableInfo::UnitInfo>(ti->colCount()));
            //         auto &row = ti->units.back();
            //         int col_at = 0;
            //         for(auto unit : tablepart->row_units[part_rowId]) {
            //             float unit_right = unit->getRightX() - tablepart->rect.left();
            //             float unit_left = unit->x - tablepart->rect.left();
            //             //auto line_right_iter = column_x_set.lower_bound(unit_right - Helper::cm2pixel(COLUMN_LINE_SAME_OFFSET_cm));
            //             //if(line_right_iter == column_x_set.end()) --line_right_iter;
            //             //auto line_left_iter = column_x_set.lower_bound(unit_left - Helper::cm2pixel(COLUMN_LINE_SAME_OFFSET_cm));
            //             //if(line_left_iter == column_x_set.end()) --line_left_iter;
            //             // auto iter_next = line_iter; /// 考虑极窄的单元格？
            //             // ++iter_next;
            //             // while(iter_next != column_x_set.end() && )
            //             //表格的列数有限，直接遍历找到单元格跨的所有列
            //             int col1,col2;
            //             float pile_width = 0;
            //             float _offset = Helper::cm2pixel(COLUMN_LINE_SAME_OFFSET_cm);
            //             for(int i = 0; i < ti->colCount(); ++i) {
            //                 if(pile_width + _offset >= unit_left
            //                     || (qAbs(pile_width - unit_left) <= qAbs(pile_width + ti->colWidths[i] - unit_left))) {
            //                     col1 = i;
            //                     break;
            //                 }
            //                 pile_width += ti->colWidths[i];
            //             }

            //             if(col1 < col_at) {
            //                 col1 = col_at;
            //                 //qDebug()
            //             }

            //             pile_width = 0;
            //             for(int i = 0; i < ti->colCount(); ++i) {
            //                 pile_width += ti->colWidths[i];
            //                 if(pile_width + _offset >= unit_right
            //                     || (i+1 < ti->colCount() &&
            //                         qAbs(pile_width - unit_right) <= qAbs(pile_width + ti->colWidths[i+1] - unit_right))) {
            //                     col2 = i;
            //                     break;
            //                 }
            //             }
            //             /// ---> 单元格覆盖列[col1,col2]
            //             while(col_at < col2) { //把前面的单元格置null，它们不属于本行
            //                 row[col_at].u = 0;
            //                 ++col_at;
            //             }
            //             col_at = col2+1;
            //             row[col2].start_col = col1;
            //             row[col2].end_col = col2;
            //             row[col2].end_row = rowId;
            //             // 确定单元格跨行数
            //             // 单元格不会跨越tablepart
            //             int start_row = rowId;
            //             for(int j = part_rowId; j > 0; --j) {
            //                 //if(ti->units[start_row - 1][col2].u != 0) {
            //                 if(grid_filled[(start_row - 1)*ti->colCount()+col2] > 0) {
            //                     break;
            //                 }
            //                 start_row -= 1;
            //             }
            //             row[col2].start_row = start_row;
            //             row[col2].u = unit;
            //             unit->tableinfo = ti;
            //             // 记录
            //             for(int i : views::iota(start_row,rowId+1))
            //                 for(int j : views::iota(col1,col2+1))
            //                     grid_filled[i*ti->colCount()+j] = 1;
            //         }

            //         rowId += 1;
            //     } //tableline FOR

            //     last_tablepart = tablepart;

            // } //tablepart FOR

            // ///赋予
            // TableFactory::calcControlInfo(ti); //计算控制行信息

            // ///表格对齐
            // ti->alignMode = Helper::AlignHCenter;

            // //表格宽度
            // ti->width = accumulate(ti->colWidths.begin(),ti->colWidths.end(),0);

            // operator delete(grid_filled,ti->colCount() * ti->rowCount()); //free
        } //page.tp FOR
    }//page FOR
} //func END

/**
 * @brief layoutTables 在pdf所有页面解析完后调用一次本函数。本函数为所有合并好的表格创建布局。
 */
// void layoutTables()
// {

// }



#endif // PDF_TABLE_ANALYSE_H
