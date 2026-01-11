#ifndef FREE_FRAME_H
#define FREE_FRAME_H
#include "freeobj.h"

// This is Class Free_Frame
// 一个“框”,矩形、圆角矩形；纯色填充、渐变填充、图片填充、图案填充；Border；阴影
class Free_Frame : public FreeObj
{
public:
    Free_Frame():background_type(Helper::FillMode::Color_Fill){
        image_fillMode = Helper::FillMode::Repeat_xy;
        width = height = 60;
        background_color = "transparent";
        lineWidth = 1;
        z = Helper::Layer_Z::Bottom;
    }

    virtual QQuickItem* generateQmlItem() override {
        return uiPool_Frame::fetchItem();
    }

    virtual void updateDataToQmlItem(QQuickItem*) override;
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override;

    virtual QVariant qmlGetData(int dataName) override {
        switch(dataName) {
        case Helper::FILL_MODE: return background_type; break;
        case Helper::RADIUS: return radius1; break;
        case Helper::BG_COLOR: return background_color; break;
        case Helper::SOURCE: return background_src; break;
        case Helper::STROKE_COLOR: return border_color; break;
        case Helper::STROKE_WIDTH: return lineWidth; break;
        case Helper::RADIUS_1: return radius1; break;
        case Helper::RADIUS_2: return radius2; break;
        case Helper::RADIUS_3: return radius3; break;
        case Helper::RADIUS_4: return radius4; break;
        }
        return FreeObj::qmlGetData(dataName);
    }
    void qt_paint(QPainter& painter,Page* page) override {};
    virtual void discard_qmlItem() override {
        uiPool_Frame::returnItem(qmlItem);
        qmlItem = 0;
    }
public:
    char     background_type;
    char     image_fillMode;
    float    lineWidth;          //边框宽度
    float    radius1,radius2,radius3,radius4; //圆角半径  //todo
    QColor   border_color;      //边框颜色
    QColor   background_color;  //背景色
    QString  background_src;   //背景图片
private:
    static constexpr char __UINAME__[] = "Frame";
public:
    typedef UIItemPool<__UINAME__,6> uiPool_Frame; //ui控件池
};

#endif // FREE_FRAME_H
