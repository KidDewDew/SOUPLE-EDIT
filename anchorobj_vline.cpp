#include "anchorobj_vline.h"
#include "anchorobj_hline.h"
#include "helper.h"

AnchorObj_VLine::AnchorObj_VLine() {
    while(hash_vline.contains("V"+QString::number(s_vline_count))) {
        ++s_vline_count;
    }
    name = "V"+QString::number(s_vline_count); //自动命名
    hash_vline[name] = this; //记录hline
    height = 100;
    z = Helper::Layer_Z::Top; //绝对置顶
}

QQuickItem* AnchorObj_VLine::generateQmlItem() //创建用于ui的qml元素
{
    if(Helper::showHelpLine == false)
        return (QQuickItem*)Helper::QmlItemStatus::CreatedButHidden;
    return uiPool_VLine::fetchItem();
    //return Helper::invokeQmlFunction<QQuickItem*>("generateObj","VLine");
}

void AnchorObj_VLine::updateDataToQmlItem(QQuickItem* item)    //更新数据到qml元素
{
    if(! item) return;
    item->setX(x - item->width()/2);
    item->setY(y);
    item->setHeight(height);
    item->setProperty("data_id",id);
    item->setObjectName(name);
}

int AnchorObj_VLine::dealCommandFromQmlItem(int command,const QVariant& arg)  //处理来自对应qml item的指令
{
    if(! qmlItem) return -1;
    if(command == Helper::X_UP) {
        x = arg.toFloat() + qmlItem->width()/2;
        qmlItem->setProperty("x",x - qmlItem->width()/2);
    } else if(command == Helper::NAME_UP) {
        if(name == arg) return 0;
        auto it = hash_vline.find(arg.toString());
        if(it == hash_vline.end()) {
            hash_vline.remove(name);
            hash_vline[name = arg.toString()] = this;
        } else {
            return Helper::Error_Repeat;
        }
    }
    return 0;
}

void AnchorObj_VLine::notifyHLineYChanged(AnchorObj_HLine* hline)
{
    bool isLeft = hline->getLeftLine() == this; //判断本对象在hline左边还是右边
    auto nl = hline->hline->as<AnchorObj_HLine*>();
    if(!nl || (isLeft?nl->getLeftLine():nl->getRightLine()) != this) { //顶端
        y = hline->y;
        if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setY(y);
    }
    auto& nls = hline->getAnchorNextLine();
    nl = nls.size() == 0 ? 0 : nls.front();
    if(!nl || (isLeft?nl->getLeftLine():nl->getRightLine()) != this) { //底端
        height = hline->y - y;
        if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setHeight(height);
    }
}
