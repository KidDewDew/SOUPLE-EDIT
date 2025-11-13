#ifndef ANCHOROBJ_PHRIGHT_H
#define ANCHOROBJ_PHRIGHT_H

#include "anchorobj.h"
#include "uiitempool.h"

//右占位(换行) 数据端对象
class AnchorObj_PHRight : public AnchorObj
{
public:
    AnchorObj_PHRight();
    virtual QQuickItem* generateQmlItem() override;
    virtual void dealLayout() override;
    virtual AnchorObj* dropRight(float dropWidth) override; //尝试截断并丢弃右边
    virtual AnchorObj* dropLeft(float dropWidth) override;  //尝试截断并丢弃左边

    //Special: 换行认为可以切分，因为它可长可短
    virtual bool canSplit() const noexcept override {
        return true;
    }
    // virtual const AnchorObj_Global_Info& global_info() const override {
    //     static AnchorObj_Global_Info _global_info = {.isSelfWidth = false,.isRealHeight=false};
    //     return _global_info;
    // }

    virtual const Obj_Global_Info& objInfo() const noexcept override {
        static Obj_Global_Info gi = {.dealLayoutable = false,.allowSelect = true,.allowPartSelect = false,
                                     .anchorInfo = {.isSelfWidth = false,
                                                    .isRealHeight = false,
                                                    .impact_hscale = false,
                                                    .kill_hscale = true }};
        return gi;
    }

    virtual void discard_qmlItem() override {
        uiPool_PHRight::returnItem(qmlItem);
        qmlItem = 0;
    }
private:
    static constexpr char __UINAME__[] = "PH_Right";
public:
    typedef UIItemPool<__UINAME__,60> uiPool_PHRight; //ui控件池
};

#endif // ANCHOROBJ_PHRIGHT_H
