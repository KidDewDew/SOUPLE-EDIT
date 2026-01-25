#ifndef ANCHORNULL_H
#define ANCHORNULL_H

#include "anchorobj.h"

//啥也没有的anchorobj，长度为 1
class AnchorNull : public AnchorObj
{
public:
    virtual QQuickItem* generateQmlItem() override {
        return 0;
    }
    template<typename Serial>
    void serialize(Serial& serial) {
        serial / SOUPLE_PP(id);
    }

    virtual bool isStake() const noexcept override {
        return true;
    }

    virtual AnchorObj* dropRight(float dropWidth) override
    {
        if(dropWidth < 1e-2) return 0; //避免抖动
        removeSelf(false);
        return this;
    }
    virtual AnchorObj* dropLeft(float dropWidth) override
    {
        if(dropWidth < 1e-2) return 0; //避免抖动
        removeSelf(false);
        return this;
    }  //尝试截断并丢弃左边
};

SOUPLE_REGISTER_CLASS(AnchorNull,DATE_ID(202601260428))

#endif // ANCHORNULL_H
