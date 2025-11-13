#ifndef FREE_PATH_H
#define FREE_PATH_H

#include "freeobj.h"

class Free_Path : public FreeObj
{
    friend class Pdf2Souple;
public:
    Free_Path() {}
    virtual QQuickItem* generateQmlItem() override {
        return uiPool_FPath::fetchItem();
    }

    virtual void updateDataToQmlItem(QQuickItem*) override;
    //virtual int dealCommandFromQmlItem(const QString& command,const QVariant& arg) override;

    // virtual QVariant qmlGetData(const QString& dataName) override {
    //     if(dataName == "source") return source;
    //     return FreeObj::qmlGetData(dataName);
    // }
    void qt_paint(QPainter& painter,Page* page) override {};
    virtual void discard_qmlItem() override {
        uiPool_FPath::returnItem(qmlItem);
        qmlItem = 0;
    }
protected:
    bool isFill = true, isStroke = true;
    QList<Path_Action> path_actions; //路径动作
    QColor fillStyle; //填充模式
    QColor strokeStyle; //描边模式
    float lineWidth;  //描边宽度
private:
    QString source = ""; //缺省source
    static constexpr char __UINAME__[] = "FPath";
public:
    typedef UIItemPool<__UINAME__,12> uiPool_FPath; //ui控件池
};

#endif // FREE_PATH_H
