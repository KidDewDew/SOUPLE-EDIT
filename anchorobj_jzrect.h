#ifndef ANCHOROBJ_JZRECT_H
#define ANCHOROBJ_JZRECT_H

#include "anchorobj.h"
#include "uiitempool.h"

class AnchorObj_JZRect : public AnchorObj
{
public:
    AnchorObj_JZRect();

    virtual QQuickItem* generateQmlItem() override;
    virtual AnchorObj* dropRight(float dropWidth) override;
    virtual AnchorObj* dropLeft(float dropWidth) override;
    virtual void dealLayout() override;
    // virtual const AnchorObj_Global_Info& global_info() const override {
    //     static AnchorObj_Global_Info _global_info = {.isSelfWidth = false,.isRealHeight=false};
    //     return _global_info;
    // }
    virtual const Obj_Global_Info& objInfo() const noexcept override {
        static Obj_Global_Info gi = {.dealLayoutable = false,.allowSelect = true,.allowPartSelect = false,
                                     .anchorInfo = {.isSelfWidth = false,.isRealHeight = false,
                                                    .impact_hscale = false,.kill_hscale = true,}};
        return gi;
    }
    virtual void discard_qmlItem() override {
        uiPool_JZRect::returnItem(qmlItem);
        qmlItem = 0;
    }

private:
    static inline constexpr char __NAME__[] = "JZRect";;
public:
    typedef UIItemPool<__NAME__,40> uiPool_JZRect;
};

SOUPLE_REGISTER_CLASS(AnchorObj_JZRect,DATE_ID(202511202139))

#endif // ANCHOROBJ_JZRECT_H
