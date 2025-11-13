#include "free_frame.h"

void Free_Frame::updateDataToQmlItem(QQuickItem*item){
    switch(background_type){
    case Helper::No_Fill:
        item->setProperty("color","transparent");
        break;
    case Helper::Color_Fill:
        item->setProperty("color",background_color);
        break;
    }
    FreeObj::updateDataToQmlItem(item);
}
int Free_Frame::dealCommandFromQmlItem(int command,const QVariant& arg) {
    switch(command) {
    case Helper::FILL_MODE_UP:
        background_type = arg.toInt();
        break;
    case Helper::BG_COLOR_UP: // 背景色up
    case Helper::COLOR_UP:
        background_color = arg.value<QColor>();
        break;

    case Helper::STROKE_COLOR_UP: // 边框色up
        border_color = arg.value<QColor>();
        break;
    case Helper::STROKE_WIDTH_UP: // 边框宽度up
        lineWidth = arg.toFloat();
        break;
    case Helper::SOURCE_UP:
        background_src = arg.toString();
        break;
    case Helper::RADIUS_UP:
    case Helper::RADIUS_1_UP:
        radius1 = arg.toFloat();
        break;
    case Helper::RADIUS_2_UP:
        radius2 = arg.toFloat();
        break;
    case Helper::RADIUS_3_UP:
        radius3 = arg.toFloat();
        break;
    case Helper::RADIUS_4_UP:
        radius4 = arg.toFloat();
        break;
    default: FreeObj::dealCommandFromQmlItem(command,arg);
    }
    return 0;
}
