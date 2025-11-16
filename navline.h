#ifndef NAVLINE_H
#define NAVLINE_H

#include "anchorobj_phleft.h"
#include "safe_pointer.h"

// 导航符；可以用作目录项
class NavItem : public AnchorObj_PHLeft
{
public:
    NavItem() {
        width = height = 0;
    }
    virtual void dealLayout() noexcept override;
    virtual void discard_qmlItem() override {
        uiPool_Nav::returnItem(qmlItem);
        qmlItem = 0;
    }
    virtual QQuickItem* generateQmlItem() override {
        return uiPool_Nav::fetchItem();
    }
    virtual void updateDataToQmlItem(QQuickItem* item) override {
        AnchorObj_PHLeft::updateDataToQmlItem(item);
        item->setSize({22,18});
    };
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override;
public:
    Safe_Obj_Pointer<Obj> nav_at; //导航定位到
private:
    static constexpr char __UINAME__[] = "Nav";
public:
    typedef UIItemPool<__UINAME__,15> uiPool_Nav; //ui控件池
};


#endif // NAVLINE_H
