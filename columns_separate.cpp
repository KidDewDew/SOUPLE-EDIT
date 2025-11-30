#include "columns_separate.h"
#include "wordpage_vline.h"
#include "souplemanager.h"

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
