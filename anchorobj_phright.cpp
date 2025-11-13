#include "anchorobj_phright.h"
#include "anchorobj_hline.h"
#include "helper.h"

AnchorObj_PHRight::AnchorObj_PHRight() {
    height = 16;
}

QQuickItem* AnchorObj_PHRight::generateQmlItem(){
    return uiPool_PHRight::fetchItem();
    //return Helper::invokeQmlFunction<QQuickItem*>("generateObj","PH_Right");
}

void AnchorObj_PHRight::dealLayout()
{
    if(leftObj) {
        height = leftObj->height; //继承左边obj的高度
        if(Helper::isQmlItemValid(qmlItem) && height != qmlItem->height()) qmlItem->setHeight(height);
    }
    AnchorObj::dealLayout(); //处理基本的相对布局
    if( ! hline) return;
    //填充右边空间
    width = hline->getRightX() - x;
    if(width <= 1e-4) {
        width = 10.0f; //使溢出
    }
    //if(rightObj) {
    //    qDebug() << "PHRight has rightObj";
    //    qDebug() << hline->getRightX() << rightObj->x << rightObj->width;
    //}
    if(Helper::isQmlItemValid(qmlItem) && width != qmlItem->width()) qmlItem->setWidth(width);
}

AnchorObj* AnchorObj_PHRight::dropRight(float dropWidth)
{
    //if(dropWidth < width && x < hline->getRightX()) return nullptr;
    //if(leftObj && leftObj->getRightX() >=)
    if(dropWidth < 1e-2) return 0;
    removeSelf(false);
    //qDebug() << "AnchorObj_PHRight::dropRight(" << dropWidth;
    return this;
}

AnchorObj* AnchorObj_PHRight::dropLeft(float dropWidth)
{
    removeSelf(false);
    return this;
}
