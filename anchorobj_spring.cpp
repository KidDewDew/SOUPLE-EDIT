#include "anchorobj_spring.h"
#include "horline_base.h"

void AnchorObj_Spring::dealLayout() noexcept
{
    AnchorObj::dealLayout();
    float solid_right_width = 0.0f;
    auto r = rightObj;
    bool hasUnsolid = false;
    while(r) {
        if(r->objInfo().anchorInfo.isSelfWidth) {
            solid_right_width += r->width;
        } else {
            hasUnsolid = true;
            r->width = 0; //让unsolid元素的宽度变成0
        }
        r = r->rightObj;
    }

    width = std::max(0.0f,hline->getRightX() - x - solid_right_width);
    //qDebug() << hline->getRightX() - x - solid_right_width;
    if(! hasUnsolid) {
        //尝试对本行进行收缩
        HorLine_Base *nextline = hline->be<HorLine_Base*>()->getNextLine();
        if(nextline->leftObj && nextline->leftObj->objInfo().anchorInfo.isSelfWidth) {
            auto shrink = nextline->leftObj->dropLeft(width);
            if(shrink) {
                hline->insertOnRight(shrink);
                //qDebug() << "shrink:" << shrink->__dstr();
                width -= shrink->width;
            }
        }
    }
}

int AnchorObj_Spring::dealCommandFromQmlItem(int command,const QVariant& arg)
{
    return 0;
}

AnchorObj* AnchorObj_Spring::dropRight(float dropWidth) //尝试截断并丢弃右边
{
    removeSelf(false);
    return this;
}

AnchorObj* AnchorObj_Spring::dropLeft(float dropWidth) //尝试截断并丢弃左边
{
    removeSelf(false);
    return this;
}
