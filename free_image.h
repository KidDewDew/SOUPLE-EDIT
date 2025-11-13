#ifndef FREE_IMAGE_H
#define FREE_IMAGE_H

#include "freeobj.h"

class Free_Image : public FreeObj
{
public:
    Free_Image() {
        width = height = 36;
    }

    virtual QQuickItem* generateQmlItem() override {
        return uiPool_FImage::fetchItem();
    }

    virtual void updateDataToQmlItem(QQuickItem*) override;
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override;

    virtual QVariant qmlGetData(int dataName) override {
        if(dataName == Helper::SOURCE) return source;
        return FreeObj::qmlGetData(dataName);
    }
    void qt_paint(QPainter& painter,Page* page) override {};
    virtual void discard_qmlItem() override {
        uiPool_FImage::returnItem(qmlItem);
        qmlItem = 0;
    }
private:
    QString source = ""; //缺省source
    static constexpr char __UINAME__[] = "FImage";
public:
    typedef UIItemPool<__UINAME__,8> uiPool_FImage; //ui控件池
};

#endif // FREE_IMAGE_H
