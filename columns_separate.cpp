#include "columns_separate.h"
#include "wordpage_vline.h"
#include "souplemanager.h"

QQuickItem* ColumnsSeparate::generateQmlItem() {
    if(SoupleManager::showHelpLine == false) return (QQuickItem*)Helper::QmlItemStatus::CreatedButHidden;
    return uiPool_sep_line::fetchItem();
}

QString ColumnsSeparate::__dstr() const noexcept
{
    return QString("SepLine id=%1 isTop=%2 hline_up=\"%3\" hline_down=\"%4\""
                   "y=%5").arg(id)
    .arg(isTop)
        .arg(hline_up ? hline_up->__dstr() : "null")
        .arg(hline_down ? hline_down->__dstr() : "null")
        .arg(y);
}

void ColumnsSeparate::notifyMoveDown() noexcept
{
    if(y > page->getBottomLineY()) {
        //得转移到下一页
        if(! page->next_page && ! SoupleManager::addInheritPage())
            return;
        page = page->next_page;
        y = page->getTopLineY();

        // 更新相关属性
    }
    SoupleManager::requestUpdateHLine(hline_down);
}

void ColumnsSeparate::dealLayout()
{
    //处理布局，这个函数被调用的频率较低。

    if(! page) {
        page = SoupleManager::getPage(y);
    }

    // S1: 维护上邻线，确保它下面没有接续的HLine喽。
    if(hline_up) {
        auto nextLine = hline_up->getNextLine();
        while(nextLine) {
            if( ! nextLine->canBe<AnchorObj_HLine>()) {
                throw LLException("ColumnsSeparate::dealLayout(): !nextLine->canBe<AnchorObj_HLine>()");
            }
            hline_up = nextLine->be<AnchorObj_HLine*>();
            nextLine = hline_up->getNextLine();
        }
    }
    // S2: 维护下邻线
    if(hline_down) {
        auto prevLine = hline_down->getPrevLine();
        while(prevLine) {
            if( ! prevLine->canBe<AnchorObj_HLine>()) {
                throw LLException("ColumnsSeparate::dealLayout(): !prevLine->canBe<AnchorObj_HLine>()");
            }
            hline_up = prevLine->be<AnchorObj_HLine*>();
            prevLine = hline_up->getNextLine();
        }
    }

    // S3: 维护一下布局
    if(hline_up) {
        float y2 = hline_up->getContentBottom();
        if(y2 > y) {
            y = y2;
            notifyMoveDown();
        }
        else if(isTop){
            y = y2;
        }
    }


    // plus: 维护自己的qml属性(如果可见)
    if(Helper::isQmlItemValid(qmlItem))
    {
        if(isTop) {
            x = columns.front().leftLine->x;
            width = columns.back().rightLine->x - x;
        } else {
            x = pal->x;
            width = pal->width;
        }
        qmlItem->setSize({width,1});
        qmlItem->setPosition({x,y});
    }
}

void ColumnsSeparate::changeColumnsNum(int num)
{
    qDebug() << "ColumnsSeparate::changeColumnsNum(" << num;

    if(!isTop) {
        throw std::runtime_error("ColumnsSeparate::changeColumnsNum can be called only by top sep-line.");
    }

    if(num > columns.size())
    {
        while(columns.size() < num) {
            PageColumn column;
            WordPage_VLine* leftLine = new WordPage_VLine;
            WordPage_VLine* rightLine = new WordPage_VLine;

            column.leftLine = leftLine;
            column.rightLine = rightLine;

            leftLine->top_columns_separate = this;
            leftLine->bottom_columns_separate = this;
            // 设置page属性 [任意分栏add]
            // column.leftLine->be<WordPage_VLine*>()->page = page;
            // column.rightLine->be<WordPage_VLine*>()->page = page;

            SoupleManager::registerObj(column.leftLine);
            SoupleManager::registerObj(column.rightLine);
            columns.push_back(column);
            qDebug() << "create Column LeftLine = " << column.leftLine->__dstr();
            qDebug() << "create Column RightLine = " << column.rightLine->__dstr();
        }

        // 设置next_column属性 [任意分栏add]
        for(int i = 0; i < columns.size()-1; ++i) {
            columns[i].leftLine->be<AnchorObj_VLine*>()->column_id = i;
            columns[i].leftLine->be<AnchorObj_VLine*>()->next_column
                = columns[i+1];
        }

        // 循环指向第一栏。
        columns.back().leftLine->be<AnchorObj_VLine*>()->next_column
            = columns.front();

        // 标记末栏属性。
        columns.back().leftLine->be<AnchorObj_VLine*>()->is_last_column_leftline
            = true;
        columns.back().leftLine->be<AnchorObj_VLine*>()->column_id = columns.size() - 1;

    }
}
