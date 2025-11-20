#ifndef ANCHOROBJ_GLUE_H
#define ANCHOROBJ_GLUE_H

#include "anchorobj.h"
#include "uiitempool.h"

/**
 * @brief AnchorObj_Glue于PH_Rect类似，但它会“粘”在水平线的最左或最右端，不会移动。
 */
class AnchorObj_Glue : public AnchorObj
{
    friend class SoupleManager;
    friend class Pdf2Souple;
    friend class HorLine_Base;
public:
    AnchorObj_Glue();
    virtual QQuickItem* generateQmlItem() override;
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override;
    virtual AnchorObj* dropRight(float dropWidth) override; //尝试截断并丢弃右边
    virtual AnchorObj* dropLeft(float dropWidth) override;  //尝试截断并丢弃左边
    virtual bool tryMergeRight() override; //尝试合并右边
    virtual void dealLayout() override;
    virtual void updateDataToQmlItem(QQuickItem* item) override;

    virtual QVariant qmlGetData(int dataName) override {
        if(dataName == Helper::MAX_DRAG_X) {
            if(!hline) return 10000;
            return hline->getRightX();
        }
        return AnchorObj::qmlGetData(dataName);
    }
    virtual const Obj_Global_Info& objInfo() const noexcept override {
        static Obj_Global_Info gi = {.dealLayoutable = false,.allowSelect = true,.allowPartSelect = false,
                                     .anchorInfo = {.isSelfWidth = true,.isRealHeight = false,.impact_hscale = false}};
        return gi;
    }
    virtual void discard_qmlItem() override {
        uiPool_PHGlue::returnItem(qmlItem);
        qmlItem = 0;
    }
public:
protected:
    bool glue_left;  //true:左占位 false:右占位
private:
    static constexpr char __UINAME__[] = "PH_Glue";
public:
    typedef UIItemPool<__UINAME__,20> uiPool_PHGlue; //ui控件池
};

SOUPLE_REGISTER_CLASS(AnchorObj_Glue,DATE_ID(202511202137))

#endif // ANCHOROBJ_GLUE_H
