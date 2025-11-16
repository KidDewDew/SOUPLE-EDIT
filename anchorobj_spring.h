#ifndef ANCHOROBJ_SPRING_H
#define ANCHOROBJ_SPRING_H

#include "anchorobj.h"
#include "uiitempool.h"

/**
 * @brief The AnchorObj_Spring class
 *     "弹簧": three-types: 1.空白 2.字符 3.线条
 *     顾名思义，该对象会尽力把它两边的对象撑满本行。
 *     行为上，该对象设置width=hline.width - 左边.solid_width - 右边.solid_width
 *     solid_width 是什么呢？参考 ObjInfo.
 *     它会积极地尝试把下一行地内容收缩回来: nextline->leftobj->dropLeft(this->width)
 */
class AnchorObj_Spring: public AnchorObj
{
public:
    AnchorObj_Spring()
    : showType(0),radius(0.8),lineWidth(1),
      spacing(3.0){

    }
    virtual void dealLayout() noexcept override;
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override;
    virtual void discard_qmlItem() override {
        uiPool_Spring::returnItem(qmlItem);
        qmlItem = 0;
    }

    virtual AnchorObj* dropRight(float dropWidth) override; //尝试截断并丢弃右边
    virtual AnchorObj* dropLeft(float dropWidth) override;  //尝试截断并丢弃左边

    virtual QQuickItem* generateQmlItem() override {
        return uiPool_Spring::fetchItem();
    }
    virtual void updateDataToQmlItem(QQuickItem* item) override {
        AnchorObj::updateDataToQmlItem(item);
        item->setProperty("radius",radius);
        item->setProperty("lineWidth",lineWidth);
        item->setProperty("showType",showType);
        item->setProperty("spacing",spacing);
    };
    virtual QVariant qmlGetData(int dataName) override {
        switch(dataName) {
        case Helper::RADIUS:
            return radius; break;
        case Helper::LINE_WIDTH:
            return lineWidth; break;
        default: return AnchorObj::qmlGetData(dataName);
        }
    }
public:
    // showType 显示类型 -1:空白   0:点  1:虚线  2:实线
    char showType;
    float radius,lineWidth; //点的半径 or 线宽
    float spacing;
    //QFont font; //使用的字体(对于showType 0)
private:
    static constexpr char __UINAME__[] = "Spring";
public:
    typedef UIItemPool<__UINAME__,15> uiPool_Spring; //ui控件池
};

#endif // ANCHOROBJ_SPRING_H
