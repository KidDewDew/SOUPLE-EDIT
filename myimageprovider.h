#ifndef MYIMAGEPROVIDER_H
#define MYIMAGEPROVIDER_H
#include <QQuickImageProvider>
//#include "souplemanager.h"
//用于为qml提供使用内置图像的渠道
class MyImageProvider: public QQuickImageProvider
{
    Q_OBJECT
public:
    MyImageProvider():QQuickImageProvider{QQuickImageProvider::Image} {}
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override
    {
        qDebug() << "requestImage(" << id << requestedSize;
        auto it = image_table.find(id);
        if(it == image_table.end()) return {};
        auto image = *it;
        *size = image.size();
        return image;
    }
    //删除图像
    Q_INVOKABLE void removeImage(const QString& id) {
        qDebug() << "MyImageProvider::removeImage(" << id;
        image_table.remove(id);
    }
    //添加图像
    Q_INVOKABLE void addImage(const QString& id, QImage image) {
        image_table[id] = image;
        qDebug() << "******addImage";
    }
private:
    QHash<QString,QImage> image_table;
};

#endif // MYIMAGEPROVIDER_H
