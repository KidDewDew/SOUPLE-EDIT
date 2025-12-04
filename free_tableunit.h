#ifndef FREE_TABLEUNIT_H
#define FREE_TABLEUNIT_H

#include "free_rich.h"
#include <QString>

class TableInfo;

class Free_TableUnit : public Free_Rich
{
public:
    Free_TableUnit();
    virtual int dealCommandFromQmlItem(int command, const QVariant& arg) override;
    virtual QQuickItem* generateQmlItem() override {
        return uiPool_FUnit::fetchItem();
    }
    virtual void discard_qmlItem() override {
        uiPool_FUnit::returnItem(qmlItem);
        qmlItem = nullptr;
    }
    virtual QVariant qmlGetData(int dataName) override;
    virtual void qt_paint(QPainter& painter, Page* page) override;

    // 新增：获取单元格文本内容
    QString getText() const;

    // 设置单元格所在行列（用于定位文本）
    void setCellPosition(int row, int col) {
        m_row = row;
        m_col = col;
    }

public:
    TableInfo* tableinfo = nullptr;

private:
    static constexpr char __UINAME__[] = "FUnit";
    int m_row = -1;   // 单元格行索引（从0开始）
    int m_col = -1;   // 单元格列索引（从0开始）

public:
    typedef UIItemPool<__UINAME__, 20> uiPool_FUnit; // ui控件池
};

#endif // FREE_TABLEUNIT_H
