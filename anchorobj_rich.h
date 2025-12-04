#ifndef ANCHOROBJ_RICH_H
#define ANCHOROBJ_RICH_H

#include "anchorobj.h"
#include "free_rich.h"

// 设计上，该对象仅仅对Free_Rich进行了一层封装。
// 显然，有些设计并不是很好，存在不少冗余。
class Anchorobj_Rich : public AnchorObj
{
public:
    Anchorobj_Rich();
    virtual void dealLayout() override; //处理布局
    virtual AnchorObj* dropRight(float dropWidth) override; //尝试截断并丢弃右边
    virtual AnchorObj* dropLeft(float dropWidth) override;  //尝试截断并丢弃左边
    virtual QQuickItem* generateQmlItem() override {
        free_rich->qmlItem = free_rich->generateQmlItem();
        return free_rich->qmlItem;
    }
    virtual void updateDataToQmlItem(QQuickItem* item) override {
        item->setProperty("anchorVersion",true); //FRich qml要记得实现这个功能来判断是否是anchor版本~
        free_rich->updateDataToQmlItem(item);
        item->setProperty("data_id",id); //重定向id
    };
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override {
        return free_rich->dealCommandFromQmlItem(command,arg);
    };
    virtual std::optional<int> getData(int dataName) override {
        return free_rich->getData(dataName);
    }
    virtual QVariant qmlGetData(int dataName) override {
        return free_rich->qmlGetData(dataName);
    }

    virtual void discard_qmlItem() override {
        free_rich->discard_qmlItem();
        qmlItem = 0;
    }

    virtual const Obj_Global_Info& objInfo() const noexcept override {
        static Obj_Global_Info gi = {.dealLayoutable = false,.allowSelect = true,.allowPartSelect = false,
                                     .anchorInfo = {.isSelfWidth = true,.isRealHeight = true,
                                                    .impact_hscale = false}};
        return gi;
    }

    Free_Rich* operator->() noexcept {
        return free_rich;
    }
    const Free_Rich* operator->() const noexcept {
        return free_rich;
    }

    Free_Rich* free_rich;

//private:
//    Free_Rich* free_rich;
};

#endif // ANCHOROBJ_RICH_H
