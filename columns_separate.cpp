#include "columns_separate.h"
#include "wordpage_vline.h"
#include "souplemanager.h"

QQuickItem* ColumnsSeparate::generateQmlItem() {
    if(SoupleManager::showHelpLine == false) return (QQuickItem*)Helper::QmlItemStatus::CreatedButHidden;
    return uiPool_sep_line::fetchItem();
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
    // S3: 维护自己的qml属性(如果可见)
    if(Helper::isQmlItemValid(qmlItem))
    {
        x = columns.front().leftLine->x;
        width = columns.back().rightLine->x - x;
        qmlItem->setWidth(width);
        qmlItem->setPosition({x,y});
    }
}

void ColumnsSeparate::changeColumnsNum(int num)
{
    if(!isTop) {
        throw std::runtime_error("ColumnsSeparate::changeColumnsNum can be called only by top sep-line.");
    }

    if(num > columns.size())
    {
        while(columns.size() < num) {
            PageColumn column;
            AnchorObj_VLine* leftLine = new AnchorObj_VLine;
            AnchorObj_VLine* rightLine = new AnchorObj_VLine;
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
        }

        // 设置next_column属性 [任意分栏add]
        for(int i = 0; i < columns.size()-1; ++i) {
            columns[i].leftLine->be<WordPage_VLine*>()->next_column
                = columns[i+1];
        }

        // 循环指向第一栏。
        columns.back().leftLine->be<WordPage_VLine*>()->next_column
            = columns.front();

    }
}
