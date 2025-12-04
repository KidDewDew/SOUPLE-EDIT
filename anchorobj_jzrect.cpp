#include "anchorobj_jzrect.h"
#include "helper.h"
#include "anchorobj_hline.h"

AnchorObj_JZRect::AnchorObj_JZRect() {
    width = 16;
    height = 12;
}

QQuickItem* AnchorObj_JZRect::generateQmlItem()
{
    return uiPool_JZRect::fetchItem();
}

void AnchorObj_JZRect::dealLayout()
{
    AnchorObj::dealLayout();
    //计算同一行上右边的所有实体宽度
    auto obj = rightObj;
    float solid_width = 0;
    while(obj) {
        if(obj->objInfo().anchorInfo.isSelfWidth)
            solid_width += obj->width;
        obj = obj->rightObj;
    }
    //float right_width = hline->as<AnchorObj_HLine*>()->getRightX() - x;
    float right_width = hline->getRightX() - x; //[2025/7/31 修改] 因为hline不一定是AnchorObj_HLine
#ifdef Q_OS_ANDROID
    width = std::max(0.01,(right_width - solid_width) * 0.5);
#else
    width = std::max(0.01,(right_width - solid_width) * 0.5);
#endif
    if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setWidth(width);
}

AnchorObj* AnchorObj_JZRect::dropRight(float dropWidth)
{
    removeSelf(false);
    return this;
}

AnchorObj* AnchorObj_JZRect::dropLeft(float dropWidth)
{
    //auto hline = this->hline->as<AnchorObj_HLine*>();
    //auto lastHLine = hline->getLogicLastLine();

    //[2025/7/31 修改] 通过lastObj来间接获取上一行。因为hline不一定提供getLogicLastLine的方法。
    auto lastObj = hline->getLastObj();
    auto lastHLine = lastObj ? lastObj->hline : 0;
    if(! lastHLine) return 0;
    if(dropWidth == lastHLine->width) {
        removeSelf(false);
        return this;
    }
    return 0;
}
