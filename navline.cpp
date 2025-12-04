#include "navline.h"
#include "helper.h"
#include "horline_base.h"

void NavItem::dealLayout() noexcept
{
    if(hline) {
        x = hline->x;
        y = hline->y;
    }
    if(Helper::isQmlItemValid(qmlItem)) {
        qmlItem->setPosition({x-qmlItem->width()+width,
                              y-qmlItem->height()*0.5});
    }
    AnchorObj_PHLeft::doPHLeft();
}

int NavItem::dealCommandFromQmlItem(int command,const QVariant& arg)
{
    switch(command) {

    case Helper::CLICKED:
        //跳转到...
        break;
    default:
        return AnchorObj_PHLeft::dealCommandFromQmlItem(command,arg);
    }
    return 0;
}
