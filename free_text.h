#ifndef FREE_TEXT_H
#define FREE_TEXT_H

#include "freeobj.h"

//Free_Text: 如果您需要使用富文档，请使用Free_Rich
class Free_Text : public FreeObj
{
public:
    // Free_Text() {
    //     //width = height = 36;
    // }

    virtual QQuickItem* generateQmlItem() override {
        return uiPool_FText::fetchItem();
    }

    virtual void updateDataToQmlItem(QQuickItem*) override {}
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override { return 0; }

    virtual QVariant qmlGetData(int dataName) override {
        if(dataName == Helper::TEXT) return text;
        return FreeObj::qmlGetData(dataName);
    }

    virtual std::any getAnyData(std::string key) noexcept override {
        if(key == "text") {
            return text;
        }
        return {};
    }

    void qt_paint(QPainter& painter,Page* page) override {};
    virtual void discard_qmlItem() override {
        uiPool_FText::returnItem(qmlItem);
        qmlItem = 0;
    }
private:
    bool isFill=true,isStroke=false;
    float strokeWidth = 0.0;
    QString text;
    QFont font;
    QColor stroke_color,fill_color;
    static constexpr char __UINAME__[] = "FText";
public:
    typedef UIItemPool<__UINAME__,20> uiPool_FText; //ui控件池
};

#endif // FREE_TEXT_H
