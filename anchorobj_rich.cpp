#include "anchorobj_rich.h"

Anchorobj_Rich::Anchorobj_Rich() {
    free_rich = new Free_Rich;
}

void Anchorobj_Rich::dealLayout() //处理布局
{
    width = free_rich->width;
    height = free_rich->height;
    AnchorObj::dealLayout();
    free_rich->x = x;
    free_rich->y = y;
    free_rich->z = z;
    free_rich->dealLayout();
}

AnchorObj* Anchorobj_Rich::dropRight(float dropWidth) //尝试截断并丢弃右边
{
    removeSelf(false);
    return this;
}

AnchorObj* Anchorobj_Rich::dropLeft(float dropWidth)  //尝试截断并丢弃左边
{
    if(dropWidth >= width) {
        removeSelf(false);
        return this;
    }
    return 0;
}
