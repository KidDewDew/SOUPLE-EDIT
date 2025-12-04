#ifndef COLUMNS_SEPARATE_H
#define COLUMNS_SEPARATE_H

#include "anchorobj_hline.h"

// 分栏区间分隔线
class ColumnsSeparate : public AnchorObj_HLine
{
public:
    QQuickItem* generateQmlItem() {
        return 0; // no qml item.
    }
};

#endif // COLUMNS_SEPARATE_H
