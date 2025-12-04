#include "anchorobj_image.h"
#include "helper.h"
#include <QImageReader>

AnchorObj_Image::AnchorObj_Image() {
    width = 36;
    height = 30; //缺省图片尺寸
    vAlignMode = Helper::AlignBottom;
}


QQuickItem* AnchorObj_Image::generateQmlItem() {
    return uiPool_Image::fetchItem();
    //return Helper::invokeQmlFunction<QQuickItem*>("generateObj","Image");
}

void AnchorObj_Image::updateDataToQmlItem(QQuickItem*item) {
    item->setProperty("source",source.length() == 0 ? "qrc:/image/icon_Image.png":"file:"+source);
    Obj::updateDataToQmlItem(item);
}

int AnchorObj_Image::dealCommandFromQmlItem(int command,const QVariant& arg)
{
    if(command == Helper::WIDTH_UP) {
        width = arg.toFloat();
        qDebug() << "widthUP " << width;
    } else if(command == Helper::HEIGHT_UP) {
        height = arg.toFloat();
        qDebug() << "heightUP " << width;
    } else if(command == Helper::SOURCE_UP) {
        QUrl url(arg.toString());
        source = url.toLocalFile();
        QImageReader reader(source);
        QSize size = reader.size();
        qDebug() << "sourceUP " << source << size;
        float max_width = hline->width;
#ifdef Q_OS_ANDROID
        float scale = std::min((max_width-5)/size.width(),1.0f);
#else
        float scale = std::min((max_width-5)/size.width(),1.0f);
#endif
        width = size.width() * scale;
        height = size.height() * scale;
        if( Helper::isQmlItemValid(qmlItem) ) {
            qmlItem->setWidth(width);
            qmlItem->setHeight(height);
        }
    }
    return 0;
}

AnchorObj* AnchorObj_Image::dropRight(float dropWidth)  //尝试截断并丢弃右边
{
    removeSelf(false);
    return this;
}

AnchorObj* AnchorObj_Image::dropLeft(float dropWidth)   //尝试截断并丢弃左边
{
    if(dropWidth < width) return 0;
    removeSelf(false);
    return this;
}

void AnchorObj_Image::qt_paint(QPainter& painter,Page* page) {
    QImage img{source};
    //qDebug()<<"drawImage(" << source << x << y - page->top_y;
    painter.drawImage(QRectF{x,y-page->top_y,width,height},img);
}
