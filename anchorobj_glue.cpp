#include "anchorobj_glue.h"

AnchorObj_Glue::AnchorObj_Glue() {
    height = 16;
    width = 26;
}

void AnchorObj_Glue::updateDataToQmlItem(QQuickItem* item) {
    item->setProperty("isLeft",glue_left);
    item->setHeight(height);
    item->setWidth(width);
    item->setProperty("data_id",id);
}

QQuickItem* AnchorObj_Glue::generateQmlItem()
{
    return uiPool_PHGlue::fetchItem();
}

int AnchorObj_Glue::dealCommandFromQmlItem(int command,const QVariant& arg)
{
    if(command == Helper::WIDTH_UP) {
        width = arg.toFloat();
        qDebug() << "AnchorObj_Glue::dealCommandFromQmlItem(" << arg;
        if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setWidth(width);
    }
    return AnchorObj::dealCommandFromQmlItem(command,arg);
}

AnchorObj* AnchorObj_Glue::dropRight(float dropWidth) {
    if(!glue_left) {
        if(leftObj) {
            auto drop = leftObj->dropRight(dropWidth);
            if(drop && leftObj) {
                x = leftObj->getRightX();
            }
            return drop;
        }
    } else {
        removeSelf(false);
        return this;
    }
    return 0;
}

AnchorObj* AnchorObj_Glue::dropLeft(float dropWidth) {
    if(glue_left) {
        if(rightObj) return rightObj->dropLeft(dropWidth);
    } else {
        if(dropWidth >= width) {
            removeSelf(false);
            return this;
        }
    }
    return 0;
}

bool AnchorObj_Glue::tryMergeRight() {
    return false;
}

void AnchorObj_Glue::dealLayout() {
    if(glue_left) {
        float leftPileWidth = 0;
        while(leftObj) {
            if(leftObj->as<AnchorObj_Glue*>() && leftObj->be<AnchorObj_Glue*>()->glue_left == true)
                break;
            leftPileWidth += leftObj->width;
            AnchorObj* old_leftObj = leftObj;
            leftObj->removeSelf(false);
            this->insertOnRight(old_leftObj);
        }
        x = leftObj ? leftObj->getRightX() : hline->x;
        y = hline->y - height/2;
        if(Helper::isQmlItemValid(qmlItem)) {
            qmlItem->setX(x);
            qmlItem->setY(y);
        }
    } else {
        while(rightObj) {
            if(rightObj->objInfo().anchorInfo.isSelfWidth == false
                || rightObj->as<AnchorObj_Glue*>() && rightObj->be<AnchorObj_Glue*>()->glue_left == false)
                break;
            AnchorObj* old_rightObj = rightObj;
            rightObj->removeSelf(false);
            this->insertOnLeft(old_rightObj);
            old_rightObj->dealLayout();
        }
        x = leftObj ? leftObj->getRightX() : hline->x;
        y = hline->y - height/2;
        if(Helper::isQmlItemValid(qmlItem)) {
            qmlItem->setX(x);
            qmlItem->setY(y);
        }
    }
}
