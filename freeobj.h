#ifndef FREEOBJ_H
#define FREEOBJ_H

#include "obj.h"
#include "anchorobj_hline.h"

class Simple_FreeObj : public Obj {};

//自由对象；具有两种布局方式：1.绝对坐标与尺寸 2.由标线来确定边框
//注意: 1、2两种方式可以结合使用，比如左边缘由V1决定，但宽度是一个绝对量。
class FreeObj : public Obj
{
public:
    FreeObj();
    virtual void dealLayout() override;
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override;
    virtual void updateDataToQmlItem(QQuickItem* item) override {
        Obj::updateDataToQmlItem(item);
        item->setProperty("topLine",top_hline ? top_hline->getName() : "");
        item->setProperty("bottomLine",bottom_hline ? bottom_hline->getName() : "");
        item->setProperty("leftLine",left_vline ? left_vline->getName() : "");
        item->setProperty("rightLine",right_vline ? right_vline->getName() : "");
        if(top_hline) item->setProperty("topMargin",top_margin);
        if(bottom_hline) item->setProperty("bottomMargin",bottom_margin);
        if(left_vline) item->setProperty("leftMargin",left_margin);
        if(right_vline) item->setProperty("rightMargin",right_margin);
    };
    virtual QVariant qmlGetData(int dataName) override
    {
        switch(dataName) {
        case Helper::TOPLINE: return top_hline ? top_hline->getName() : "";
        case Helper::BOTTOMLINE: return bottom_hline ? bottom_hline->getName() : "";
        case Helper::LEFTLINE: return left_vline ? left_vline->getName() : "";
        case Helper::RIGHTLINE: return right_vline ? right_vline->getName() : "";
        case Helper::TOP_MARGIN: return top_margin;
        case Helper::BOTTOM_MARGIN: return bottom_margin;
        case Helper::LEFT_MARGIN: return left_margin;
        case Helper::RIGHT_MARGIN: return right_margin;
        }
        return Obj::qmlGetData(dataName);
    }
    virtual const Obj_Global_Info& objInfo() const noexcept override {
        static Obj_Global_Info gi = {.dealLayoutable = true};
        return gi;
    }
protected:
    float top_margin{0},bottom_margin{0},left_margin{0},right_margin{0};
    AnchorObj_HLine *top_hline{0},*bottom_hline{0}; //上下标线 =0表示未锚定
    AnchorObj_VLine *left_vline{0},*right_vline{0}; //左右标线 =0表示未锚定
};

#endif // FREEOBJ_H
