#ifndef COLUMNS_SEPARATE_H
#define COLUMNS_SEPARATE_H

#include "anchorobj_vline.h"

class AnchorObj_HLine;

// 分栏区间分隔线
class ColumnsSeparate : public Obj
{
    friend class SoupleManager;
public:
    QQuickItem* generateQmlItem() {
        return 0; // no qml item.
    }

    // 获取所有栏的const引用
    const std::vector<PageColumn>& getColumns() {
        return columns;
    }

    // 修改分栏数量
    void changeColumnsNum(int new_num);

    Page* getPage() noexcept {
        return page;
    }

    bool isTopSeqLine() noexcept {
        return isTop;
    }

    ColumnsSeparate *getPal() noexcept {
        return pal;
    }

    // 通知本对象：你被往下挪动了
    void notifyMoveDown() noexcept;

private:
    bool isTop;
    AnchorObj_HLine* hline_up = 0;  //它的上邻居hline
    AnchorObj_HLine* hline_down = 0; //它的下邻居hline
    ColumnsSeparate *pal;
    Page* page = 0;
    std::vector<PageColumn> columns; //栏
};

#endif // COLUMNS_SEPARATE_H
