#include "freeobj.h"

FreeObj::FreeObj() {

}

void FreeObj::dealLayout()
{
    if(top_hline) { //上
        y = top_hline->y + top_margin;
        if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setY(y);
    }
    if(bottom_hline) { //下
        if(! top_hline) {
            y = bottom_hline->y - bottom_margin - height; //下标线确定y坐标
            if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setY(y);
        } else {
            height = bottom_hline->y - bottom_margin - y; //上下标线确定高度
            if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setHeight(height);
        }
    }
    if(left_vline) { //左
        x = left_vline->x + left_margin;
        if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setX(x);
    }
    if(right_vline) { //右
        if(! left_vline) {
            x = right_vline->x - right_margin - width;
            if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setX(x);
        } else {
            width = right_vline->x - right_margin - x;
            if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setWidth(width);
        }
    }
    if(Helper::isQmlItemValid(qmlItem)) qmlItem->setZ(z);
}

int FreeObj::dealCommandFromQmlItem(int command,const QVariant& arg)
{
    switch(command) {
    case Helper::X_UP: x = arg.toDouble(); break;
    case Helper::Y_UP: y = arg.toDouble(); break;
    case Helper::WIDTH_UP: width = arg.toDouble(); break;
    case Helper::HEIGHT_UP: height = arg.toDouble();
    case Helper::TOP_MARGIN_UP: top_margin = arg.toDouble(); break;
    case Helper::BOTTOM_MARGIN_UP: bottom_margin = arg.toDouble(); break;
    case Helper::LEFT_MARGIN_UP: left_margin = arg.toDouble(); break;
    case Helper::RIGHT_MARGIN_UP: right_margin = arg.toDouble(); break;
    case Helper::Z_UP: z = arg.toDouble(); break;
    case Helper::TOPLINE_UP: {
        if(top_hline && top_hline->getName() == arg.toString()) return 0;
        auto it = AnchorObj_HLine::hash_hline.find(arg.toString());
        if(it == AnchorObj_HLine::hash_hline.end()) {
            return Helper::Error_Invalid_Data;
        } else {
            top_hline = (*it)->as<AnchorObj_HLine*>();
        }
        break;
    }
    case Helper::BOTTOMLINE_UP: {
        if(bottom_hline && bottom_hline->getName() == arg.toString()) return 0;
        auto it = AnchorObj_HLine::hash_hline.find(arg.toString());
        if(it == AnchorObj_HLine::hash_hline.end()) {
            return Helper::Error_Invalid_Data;
        } else {
            bottom_hline = (*it)->as<AnchorObj_HLine*>();
        }
        break;
    }
    case Helper::LEFTLINE_UP: {
        if(left_vline && left_vline->getName() == arg.toString()) return 0;
        auto it = AnchorObj_VLine::hash_vline.find(arg.toString());
        if(it == AnchorObj_VLine::hash_vline.end()) {
            return Helper::Error_Invalid_Data;
        } else {
            left_vline = *it;
        }
        break;
    }
    case Helper::RIGHTLINE_UP: {
        if(right_vline && right_vline->getName() == arg.toString()) return 0;
        auto it = AnchorObj_VLine::hash_vline.find(arg.toString());
        if(it == AnchorObj_VLine::hash_vline.end()) {
            return Helper::Error_Invalid_Data;
        } else {
            right_vline = *it;
        }
        break;
    }
    }

    return 0;

    // if(command == "xUP") {
    //     x = arg.toDouble();
    // } else if(command == "yUP") {
    //     y = arg.toDouble();
    // } else if(command == "wUP") {
    //     width = arg.toDouble();
    // } else if(command == "hUP") {
    //     height = arg.toDouble();
    // } else if(command == "tmUP") {
    //     top_margin = arg.toDouble();
    // } else if(command == "bmUP") {
    //     bottom_margin = arg.toDouble();
    // } else if(command == "lmUP") {
    //     left_margin = arg.toDouble();
    // } else if(command == "rmUP") {
    //     right_margin = arg.toDouble();
    // } else if(command == "zUP") {
    //     z = arg.toDouble();
    //     qDebug() << "z更新=>" << z;
    // }
    // else if(command == "topLineUP") {
    //     if(top_hline && top_hline->getName() == arg.toString()) return 0;
    //     auto it = AnchorObj_HLine::hash_hline.find(arg.toString());
    //     if(it == AnchorObj_HLine::hash_hline.end()) {
    //         return Helper::Error_Invalid_Data;
    //     } else {
    //         top_hline = *it;
    //     }
    // } else if(command == "bottomLineUP") {
    //     if(bottom_hline && bottom_hline->getName() == arg.toString()) return 0;
    //     auto it = AnchorObj_HLine::hash_hline.find(arg.toString());
    //     if(it == AnchorObj_HLine::hash_hline.end()) {
    //         return Helper::Error_Invalid_Data;
    //     } else {
    //         bottom_hline = *it;
    //     }
    // } else if(command == "leftLineUP") {
    //     if(left_vline && left_vline->getName() == arg.toString()) return 0;
    //     auto it = AnchorObj_VLine::hash_vline.find(arg.toString());
    //     if(it == AnchorObj_VLine::hash_vline.end()) {
    //         return Helper::Error_Invalid_Data;
    //     } else {
    //         left_vline = *it;
    //     }
    // } else if(command == "rightLineUP") {
    //     if(left_vline && left_vline->getName() == arg.toString()) return 0;
    //     auto it = AnchorObj_VLine::hash_vline.find(arg.toString());
    //     if(it == AnchorObj_VLine::hash_vline.end()) {
    //         return Helper::Error_Invalid_Data;
    //     } else {
    //         left_vline = *it;
    //     }
    // }
}

