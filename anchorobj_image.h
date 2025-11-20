#ifndef ANCHOROBJ_IMAGE_H
#define ANCHOROBJ_IMAGE_H

#include "anchorobj.h"
#include "uiitempool.h"

//锚定图像类
class AnchorObj_Image : public AnchorObj
{
    friend class Pdf2Souple;
public:
    AnchorObj_Image();
    virtual QQuickItem* generateQmlItem() override;
    virtual void updateDataToQmlItem(QQuickItem*) override;
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override;
    virtual AnchorObj* dropRight(float dropWidth) override; //尝试截断并丢弃右边
    virtual AnchorObj* dropLeft(float dropWidth) override;  //尝试截断并丢弃左边
    virtual QVariant qmlGetData(int dataName) override {
        if(dataName == Helper::SOURCE) return source;
        return AnchorObj::qmlGetData(dataName);
    }
    // virtual void getCursorFromRight(int RN = 0) override;
    // virtual void getCursorFromLeft(int RN = 0) override;
    // virtual const AnchorObj_Global_Info& global_info() const override {
    //     static AnchorObj_Global_Info _global_info = {.isSelfWidth = true,.isRealHeight=true};
    //     return _global_info;
    // }
    virtual const Obj_Global_Info& objInfo() const noexcept override {
        static Obj_Global_Info gi = {.dealLayoutable = false,.allowSelect = true,.allowPartSelect = false,
                                     .anchorInfo = {.isSelfWidth = true,.isRealHeight = true}};
        return gi;
    }
    virtual void discard_qmlItem() override {
        uiPool_Image::returnItem(qmlItem);
        qmlItem = 0;
    }
    void qt_paint(QPainter& painter,Page* page) override;
private:
    QString source = ""; //缺省source
    static constexpr char __UINAME__[] = "Image";
public:
    typedef UIItemPool<__UINAME__,20> uiPool_Image; //ui控件池
};

SOUPLE_REGISTER_CLASS(AnchorObj_Image,DATE_ID(202511202138))

#endif // ANCHOROBJ_IMA
