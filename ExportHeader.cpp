#include "ExportHeader.h"
#include "obj.h"
#include "wordpage_vline.h"
#include "souplemanager.h"
#include <ranges>

using namespace std;
using namespace ranges;

void Page::dealLayout()
{
    for(auto& column : columns) {
        column.rightLine->y = column.leftLine->y = getTopLineY();
        column.rightLine->height = column.leftLine->height = getBottomLineY() - getTopLineY();
    }
}


int Page::get_x_of_column_id(float x) const noexcept
{
    for(auto[i,column] : columns | views::enumerate) {
        if(column.leftLine->x > x) {
            return std::max(0,(int)i-1);
        }
    }
    return columns.size() - 1;
}

std::optional<PageColumn> Page::getNextColumn(Obj* leftLine) const noexcept
{
    for(auto[i,column]: columns | views::enumerate) {
        if(column.leftLine == leftLine) {
            if(i+1 < columns.size()) return columns[i+1];
            return {};
        }
    }
    return {};
}

std::optional<PageColumn> Page::getPrevColumn(Obj* leftLine) const noexcept
{
    for(auto[i,column] : columns | views::enumerate) {
        if(column.leftLine == leftLine) {
            if(i > 0) return columns[i-1];
            return {};
        }
    }
    return {};
}

void Page::initColumnWidthAndSpacing() { //n(w+spacing) = width - pl -pr + spacing
    float columnSpacing = Helper::cm2pixel(Default_Column_Spacing_cm);      //栏距
    float columnWidth =
        (this->width - page_left_margin - page_right_margin + columnSpacing) / columns.size() - columnSpacing;
    float leftX = page_left_margin;
    for(auto& column : columns) {
        column.leftLine->x = leftX;
        column.rightLine->x = leftX + columnWidth;
        column.leftLine->y = column.rightLine->y = getTopLineY();
        column.leftLine->height = column.rightLine->height = height - topMargin - bottomMargin;
        leftX += columnWidth + columnSpacing;
    }
}

void Page::setWordPageColumnNum(int num) {
    if( num == columns.size() ) return;
    if(num < columns.size()) { // 减少栏数

    } else if(num > columns.size()) { // 增加栏数
        while(columns.size() < num) {
            PageColumn column;
            column.leftLine = new WordPage_VLine;
            column.rightLine = new WordPage_VLine;

            // 设置page属性 [任意分栏add]
            column.leftLine->be<WordPage_VLine*>()->page = this;
            column.rightLine->be<WordPage_VLine*>()->page = this;

            column.leftLine->be<WordPage_VLine*>()->page_index
                = column.rightLine->be<WordPage_VLine*>()->page_index
                = this->index;

            column.leftLine->be<WordPage_VLine*>()->column_id
                = column.rightLine->be<WordPage_VLine*>()->column_id
                = columns.size()-1;

            SoupleManager::registerObj(column.leftLine);
            SoupleManager::registerObj(column.rightLine);
            columns.push_back(column);
        }

        // 设置next_column属性 [任意分栏add]
        for(int i = 0; i < columns.size()-1; ++i) {
            columns[i].leftLine->be<WordPage_VLine*>()->next_column
                = columns[i+1];
        }
    }
    initColumnWidthAndSpacing();
}
