#include "anchorobj_phrect.h"
#include "helper.h"

AnchorObj_PHRect::AnchorObj_PHRect() {
    height = 16;
    width = 40;
}

QQuickItem* AnchorObj_PHRect::generateQmlItem()
{
    return uiPool_PHRect::fetchItem();
    //return Helper::invokeQmlFunction<QQuickItem*>("generateObj","PH_Rect");
}

float AnchorObj_PHRect::showHScale(float hscale,float addx)
{
    if(Helper::isQmlItemValid(qmlItem)) {
        qmlItem->setX(x+addx);
        float aw = width * (hscale - 1);
        qmlItem->setProperty("sw",aw);
        return aw;
    }
    return 0;
}

void AnchorObj_PHRect::dealLayout() {
    // if(leftObj) {
    //     height = leftObj->height;
    //     y = leftObj->y;
    //     x = leftObj->x + leftObj->width;
    //     if( Helper::isQmlItemValid(qmlItem) ){
    //         if(x != qmlItem->x()) qmlItem->setX(x);
    //         if(y != qmlItem->y()) qmlItem->setY(y);
    //     }
    // } else AnchorObj::dealLayout();
    if(hline) {
        y = hline->y - 10;
        if(leftObj) x = leftObj->x + leftObj->width;
        if( Helper::isQmlItemValid(qmlItem) ){
            if(x != qmlItem->x()) qmlItem->setX(x);
            if(y != qmlItem->y()) qmlItem->setY(y);
        }
    }
}

AnchorObj* AnchorObj_PHRect::dropRight(float dropWidth)
{
    if(dropWidth+1e-3 < width) return 0;
    removeSelf(false);
    return this;
}

AnchorObj* AnchorObj_PHRect::dropLeft(float dropWidth)
{
    if(dropWidth+1e-3 < width) return 0;
    removeSelf(false);
    return this;
}

bool AnchorObj_PHRect::tryMergeRight()
{
    if(rightObj && typeid(*rightObj) == typeid(AnchorObj_PHRect)) {
        width += rightObj->width;
        rightObj->removeSelf(true);
        if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setWidth(width);
        return true;
    }
    return false;
}

int AnchorObj_PHRect::dealCommandFromQmlItem(int command,const QVariant& arg)
{
    if(command == Helper::WIDTH_UP) {
        width = arg.toFloat();
        if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setWidth(width);
    }
    return 0;
}
