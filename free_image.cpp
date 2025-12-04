#include "free_image.h"
#include <QImageReader>

void Free_Image::updateDataToQmlItem(QQuickItem*item)
{
    FreeObj::updateDataToQmlItem(item);
    item->setProperty("source",source.length() == 0 ? "qrc:/image/icon_Image.png":"file:"+source);
}

int Free_Image::dealCommandFromQmlItem(int command,const QVariant& arg)
{
    if(command == Helper::WIDTH_UP) {
        width = arg.toFloat();
        qDebug() << "widthUP " << width;
    } else if(command == Helper::HEIGHT_UP) {
        height = arg.toFloat();
        qDebug() << "heightUP " << height;
    } else if(command == Helper::SOURCE_UP) {
        QUrl url(arg.toString());
        source = url.toLocalFile();
        QImageReader reader(source);
        QSize size = reader.size();
        qDebug() << "sourceUP " << source << size;
        width = size.width();
        height = size.height();
        if( Helper::isQmlItemValid(qmlItem) ) {
            qmlItem->setWidth(width);
            qmlItem->setHeight(height);
        }
    }
    else {
        return FreeObj::dealCommandFromQmlItem(command,arg);
    }
    return 0;
}
