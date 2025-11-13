#ifndef FREE_TABLEUNIT_H
#define FREE_TABLEUNIT_H

#include "free_rich.h"

class TableInfo;

class Free_TableUnit : public Free_Rich
{
public:
    Free_TableUnit();
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override;
    virtual QQuickItem* generateQmlItem() override {
        return uiPool_FUnit::fetchItem();
    }
    virtual void discard_qmlItem() override {
        uiPool_FUnit::returnItem(qmlItem);
        qmlItem = 0;
    }
    virtual QVariant qmlGetData(int dataName) override;
public:
    TableInfo *tableinfo = 0;
private:
    static constexpr char __UINAME__[] = "FUnit";
public:
    typedef UIItemPool<__UINAME__,20> uiPool_FUnit; //ui控件池
};

#endif // FREE_TABLEUNIT_H
