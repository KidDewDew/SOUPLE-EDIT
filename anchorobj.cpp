#include "anchorobj.h"
#include "anchorobj_hline.h"




void AnchorObj::dealLayout()
{
    //更新相对坐标...
    //float old_y = y, old_x = x;
    if(hline) {
        switch(vAlignMode) {
        case Helper::AlignVCenter:
            y = hline->y - height/2 + vAlignOffset;
            break;
        case Helper::AlignBottom:
            y = hline->y - height + vAlignOffset;
            break;
        } 
        //case ...
        z = hline->z;
    }
    if(leftObj) {
        x = leftObj->x + leftObj->width;
    }
    // if( Helper::isQmlItemValid(qmlItem) ){  [2025/12/2 deleted. 移除ui操作]
    //     if(x != qmlItem->x()) qmlItem->setX(x);
    //     if(y != qmlItem->y()) qmlItem->setY(y);
    //     if(width != qmlItem->width()) qmlItem->setWidth(width);
    //     if(height != qmlItem->height()) qmlItem->setHeight(height);
    //     if(z != qmlItem->z()) qmlItem->setZ(z);
    // }
}


void AnchorObj::removeSelf(bool dead)
{
    //AnchorObj移除自身
    if(leftObj) leftObj->rightObj = rightObj;
    if(rightObj) rightObj->leftObj = leftObj;
    if(hline) { //更新hline左右对象
        if(hline->leftObj == this) hline->leftObj = rightObj;
        if(hline->rightObj == this) hline->rightObj = leftObj;
    }
    leftObj = rightObj = nullptr; //记住清零
    if(dead) {
        dead_sign = true; //打死亡标记
        if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setVisible(false); //不可见
    }
}

AnchorObj *AnchorObj::getLastObj() const
{
    //HorLine_Base * hline = dynamic_cast<HorLine_Base*>(hline);
    if(leftObj || !hline) return leftObj;
    // auto lastHline = hline->as<AnchorObj_HLine*>()->getLogicLastLine();
    // if(lastHline) return lastHline->rightObj;
    return hline->getLastObj(); //[2025/7/31]修改 更好的泛化
}

void AnchorObj::insertOnLeft(AnchorObj* obj) //从左边插入
{
    obj->rightObj = this;
    obj->leftObj = leftObj;
    obj->hline = hline;    //[2025/9/1 添加]
    if(leftObj) leftObj->rightObj = obj;
    leftObj = obj;

    if(hline && hline->leftObj == this)
        hline->leftObj = obj; //更新hline的左obj
}

void AnchorObj::insertOnRight(AnchorObj* obj) //从右边插入
{
    obj->leftObj = this;
    obj->rightObj = rightObj;
    obj->hline = hline;    //[2025/9/1 添加]
    if(rightObj) rightObj->leftObj = obj;
    rightObj = obj;

    if(hline && hline->rightObj == this)
        hline->rightObj = obj; //更新hline的右obj
}

void AnchorObj::getCursorFromLeft(int RN) //从左边获取光标,RN记录递归次数，防止极小概率的栈溢出
{
    //缺省操作均为转移光标
    if( ! Helper::isQmlItemValid(qmlItem) || RN > 32) return;
    if(rightObj) rightObj->getCursorFromLeft();
}

void AnchorObj::getCursorFromRight(int RN) //从右边获取光标
{
    //缺省操作均为转移光标
    if( ! Helper::isQmlItemValid(qmlItem) || RN > 32) return;
    if(leftObj) leftObj->getCursorFromRight();
}

AnchorObj* AnchorObj::get_neighbor_before() noexcept {
    if(leftObj) return leftObj;
    if(hline) {
        auto prevLine = ((HorLine_Base*)hline)->getPrevLine();
        if(prevLine) return prevLine->rightObj;
    }
    return 0;
}

AnchorObj* AnchorObj::get_neighbor_after() noexcept {
    if(rightObj) return rightObj;
    if(hline) {
        auto nextLine = ((HorLine_Base*)hline)->getNextLine();
        if(nextLine) return nextLine->leftObj;
    }
    return 0;
}
