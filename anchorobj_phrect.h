#ifndef ANCHOROBJ_PHRECT_H
#define ANCHOROBJ_PHRECT_H

#include "anchorobj.h"
#include "uiitempool.h"
#include "horline_base.h"

//占位矩形
class AnchorObj_PHRect : public AnchorObj
{
public:
    AnchorObj_PHRect();
    virtual QQuickItem* generateQmlItem() override;
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override;
    virtual AnchorObj* dropRight(float dropWidth) override; //尝试截断并丢弃右边
    virtual AnchorObj* dropLeft(float dropWidth) override;  //尝试截断并丢弃左边
    virtual bool tryMergeRight() override; //尝试合并右边
    virtual void dealLayout() override;

    virtual QVariant qmlGetData(int dataName) override {
        if(dataName == Helper::MAX_DRAG_X) {
            if(!hline) return 10000;
            return hline->getRightX();
        }
        return AnchorObj::qmlGetData(dataName);
    }

    // virtual const AnchorObj_Global_Info& global_info() const override {
    //     static AnchorObj_Global_Info _global_info = {.isSelfWidth = true,.isRealHeight=false};
    //     return _global_info;
    // }
    virtual const Obj_Global_Info& objInfo() const noexcept override {
        static Obj_Global_Info gi = {.dealLayoutable = false,.allowSelect = true,.allowPartSelect = false,
                                     .isTransparent = true,
                                     .anchorInfo = {.isSelfWidth = true,.isRealHeight = false}};
        return gi;
    }

    virtual float showHScale(float hscale,float addx,bool justQueryAddWidth) override;

    virtual void discard_qmlItem() override {
        uiPool_PHRect::returnItem(qmlItem);
        qmlItem = 0;
    }

    template<typename Serial>
    void serialize(Serial& serial) {
        serial / SOUPLE_PP(id)
            / SOUPLE_PP(width);
    }

private:
    //记录分段：合并一次就会增加一段
    std::vector<float> section;
private:
    static constexpr char __UINAME__[] = "PH_Rect";
public:
    typedef UIItemPool<__UINAME__,150> uiPool_PHRect; //ui控件池
};

SOUPLE_REGISTER_CLASS(AnchorObj_PHRect,DATE_ID(202511211508))

#endif // ANCHOROBJ_PHRECT_H
