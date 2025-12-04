#include "tableline.h"
#include "souplemanager.h"
#include "TableFactory.h"
#include <QPainterPath>



// 获取表格行数：根据存储的行数据数量确定
int TableInfo::rowCount() const {
    return static_cast<int>(units.size());
}

// 获取表格列数：取第一行的列数（假设所有行结构一致）
int TableInfo::colCount() const {
    if (units.empty()) return 0;
    return static_cast<int>(units[0].size());
}

// 获取指定单元格的文本：从 Free_TableUnit 中提取文本
QString TableInfo::getCellText(int row, int col) const {
    // 越界检查
    if (row < 0 || row >= rowCount()) return "";
    if (col < 0 || col >= colCount()) return "";

    // 获取单元格单元（Free_TableUnit）
    const UnitInfo& unitInfo = units[row][col];
    if (!unitInfo.u) return ""; // 单元为空时返回空字符串

    return unitInfo.u->getText();
}

TableLine::TableLine() {
    while(hash_hline.contains("T"+QString::number(s_tline_count))) {
        ++s_tline_count;
    }
    name = "T"+QString::number(s_tline_count);
    hash_hline[name] = this; //记录tline
    width = 200;
    x = 50;
    z = Helper::Layer_Z::Top; //绝对置顶
    contentBottom = 0;
    topMargin = 0;
}

void TableLine::updateDataToQmlItem(QQuickItem* item) {
    AnchorObj_HLine::updateDataToQmlItem(item);
}

int TableLine::dealCommandFromQmlItem(int command,const QVariant& arg) {
    switch(command) {
    case Helper::Y_UP: //更新行高
        y = arg.toFloat();
        table_info->rowHeights[row] = y - hline->as<HorLine_Base*>()->getContentBottom();
        break;
    }
    return 0;
}


//注意布局时，一定要仅依赖table_info的数据，不能错误地依赖TableUnit等的数据。
void TableLine::dealLayout()
{
    //qDebug() << "TableLine::dealLayout()" << "y: " << y;
    if(!table_info) return;
    //return;
    float old_y = y;

    //qDebug() << name << "TopMargin=" <<topMargin <<
    //    "ll:" << hline->__dstr();

    if(leftLine && rightLine) {
        x = leftLine->x;
        width = rightLine->x - leftLine->x;
        if( Helper::isQmlItemValid(qmlItem) ) {
            qmlItem->setX(x);
            qmlItem->setWidth(width);
        }
    }

    //if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setVisible(true);

    // 1. 处理换页(跨栏[暂略])

    //HorLine_Base *lastLine = hline->as<HorLine_Base*>();
    // if( lastLine ) {
    //     if(lastLine->page == page) { //这里可能需要判断 page 是否 != 0
    //         y = lastLine->y + table_info->rowHeights[row];
    //     } else {
    //         y = page->getTopLineY() + table_info->rowHeights[row];
    //     }
    // }


    int control_start_row = table_info->control_rows[row];
    // if(TableFactory::checkThenFindControlRow(table_info,row,&control_start_row)) {
    //     //本行是控制行
    // }

    height = table_info->rowHeights[row];

    if(control_start_row != -1) { // 本行是控制行。使用额外记录代替实时计算。
        float sumHeight = 0; // 合并行高
        AnchorObj *control_start_line = this;
        for(int i = control_start_row; i < row; ++i) {
            sumHeight += table_info->rowHeights[i];
            control_start_line = control_start_line->hline;
        }
        sumHeight += table_info->rowHeights[row];
        contentTop = -sumHeight;

        AnchorObj_HLine *lastLine = control_start_line->hline->as<AnchorObj_HLine*>();

        topMargin = control_start_line->as<TableLine*>()->row == 0 ? table_info->table_top_margin : 0;

        // HorLine_Base *lastLine = control_start_line->hline->as<HorLine_Base*>();
        // //qDebug() << name << "lastLine:" << lastLine->as<AnchorObj_HLine*>()->getName();
        // if(lastLine) {
        //     if(! page) page = lastLine->page;
        //     else if(lastLine->page && page && page->index < lastLine->page->index) {
        //         page = lastLine->page;
        //     }
        //     if(page == lastLine->page) {
        //         //qDebug() << name << "lbb:" << lastLine->getContentBottom();
        //         y = lastLine->getContentBottom() + sumHeight +
        //             (control_start_line->as<TableLine*>()->row == 0 ? topMargin : 0);
        //     }
        // }

        //qDebug() << "tableline.lr:" << leftLine->__dstr() << rightLine->__dstr();

        AnchorObj_HLine::dealAnchor(lastLine);
        AnchorObj_HLine::dealSpan(lastLine,y+contentTop,y);
        AnchorObj_HLine::check_after_dealSpan();

        //qDebug() << "tableline.lr2:" << leftLine->__dstr() << rightLine->__dstr();
        // if(page) {
        //     bool bGotoNextPage = false;
        //     if(y > page->getBottomLineY()) {
        //         //下页转移
        //         if(! page->next_page) {
        //             //自动生成下一页
        //             SoupleManager::addPage(page->width,page->height,page->topMargin,page->bottomMargin);
        //         }
        //         page = page->next_page;
        //         y = page->getTopLineY() + sumHeight;
        //         bGotoNextPage = true;
        //     } else if(y - sumHeight < page->getTopLineY()) {
        //         //对齐标线
        //         y = page->getTopLineY() + sumHeight;
        //     }

        //     if( ! bGotoNextPage && lastLine && lastLine->page != page && lastLine->page ) {
        //         if(lastLine->page->getBottomLineY() - lastLine->getContentBottom()
        //                 + (row == 0 ? topMargin : 0)
        //             >= sumHeight) {
        //             //上页转移
        //             page = lastLine->page;
        //             y = lastLine->y + sumHeight + (row == 0 ? topMargin : 0);
        //         }
        //     }
        // }

        //布局被本行控制的行
        TableLine *line = this;
        for(int i = row; i > control_start_row; --i) {
            line = line->hline->as<TableLine*>();
            line->page = page;
            line->y = line->logic_nextHLine->y - table_info->rowHeights[i];
            //qDebug() << "被控制行" << line->name << "y: " << line->y;
            line->x = line->x;
            line->width = width;
            line->leftLine = leftLine; line->rightLine = rightLine;
            if(Helper::isQmlItemValid(line->qmlItem))
                line->qmlItem->setY(line->y - line->qmlItem->height()/2);
        }

        if(abs(y - old_y) > 1e-2) {
            // 如何通知verLine让它们调整高度呢？[待解决] ......
            for(auto nl : anchor_nextHLine)
                SoupleManager::requestUpdateHLine(nl);
        }
        if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setY(y - qmlItem->height()/2);
    }

    if(! page || abs(y-old_y) > 1e-2) {
        page = SoupleManager::getPage(*this);
    }


    // 2. 布局本行单元格
    int cols = table_info->colCount();
    auto& units = table_info->units[row]; //获取本行的单元格数据
    if( 0 == cols ) return;
    float begin_x;
    switch( table_info->alignMode ) {
    case Helper::AlignLeft:
        begin_x = x;
        break;
    case Helper::AlignHCenter:
        begin_x = x + 0.5 * ( width - table_info->width );
        break;
    case Helper::AlignRight:
        begin_x = x + width - table_info->width;
        break;
    }
    //float pile_width = 0;
    for(int i = 0; i < cols; ++i) {
        auto& u = units[i];
        if(! u.u) { begin_x += table_info->colWidths[i]; continue;}
        float uw = 0, uh = 0;
        for(int j = u.start_col; j <= u.end_col; ++j)
            uw += table_info->colWidths[j];
        for(int j = u.start_row; j <= u.end_row; ++j)
            uh += table_info->rowHeights[j];
        begin_x += table_info->colWidths[i];
        u.u->updateX(begin_x - uw);
        u.u->updateY(y - uh);
        u.u->updateWidth(uw);
        u.u->updateHeight(uh);
        if(Helper::isQmlItemValid(u.u->qmlItem)) {
            u.u->qmlItem->setProperty("drawRightLine",i+1==cols);
            u.u->qmlItem->setProperty("drawBottomLine",row+1==table_info->rowCount());
        }
    }

    if(Helper::isQmlItemValid(qmlItem) && (row == 0 || row == table_info->rowCount() - 1)) {
        qmlItem->setProperty("isFirstLine",row == 0);
        qmlItem->setProperty("isLastLine",row == table_info->rowCount() - 1);
        if(row == 0) {
            qmlItem->setProperty("rightButtonY",(table_info->endLine->y-y)/2);
        }
        qmlItem->setProperty("tableActive",table_info->tableActive);
    }

}

void TableLine::qt_paint(QPainter& painter,Page* page)
{
    QPainterPath path;
    QPen pen(QColor("black"),Helper::point2pixel(0.6));
    float begin_x;
    switch( table_info->alignMode ) {
    case Helper::AlignLeft:
        begin_x = x;
        break;
    case Helper::AlignHCenter:
        begin_x = x + 0.5 * ( width - table_info->width );
        break;
    case Helper::AlignRight:
        begin_x = x + width - table_info->width;
        break;
    }
    auto ll = (HorLine_Base*)hline;
    if(row == 0 || (ll && ll->getPCPos() < this->getPCPos()))
    { //第1行需要绘制顶边或者处于栏顶s
        path.moveTo(begin_x,y-page->top_y-table_info->rowHeights[row]);
        path.lineTo(begin_x+table_info->width,y-page->top_y-table_info->rowHeights[row]);
    }
    path.moveTo(begin_x,y-page->top_y);
    path.lineTo(begin_x,y-page->top_y-table_info->rowHeights[row]);
    painter.strokePath(path,pen);
}
