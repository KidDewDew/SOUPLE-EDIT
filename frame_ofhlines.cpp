#include "frame_ofhlines.h"
#include "souplemanager.h"
#include <ranges>

using namespace std;

Frame_ofHLines::Frame_ofHLines(
    std::shared_ptr<Frame_ofHLines_Instance> _instance,PCPos pc_pos) noexcept
    : instance(_instance),pc_pos(pc_pos)
{
    background_type = instance->background_type;
    background_color = instance->background_color;
    background_src = instance->background_src;
    border_color = instance->border_color;
    lineWidth = instance->lineWidth;
    radius1 = instance->radius;
    instance->background_type.addListener(this,[this]{
        background_type = instance->background_type;
    });
    instance->background_color.addListener(this,[this]{
        background_color = instance->background_color;
        if(QML_VALID(this))
            qmlItem->setProperty("color",background_color);
    });
    instance->background_src.addListener(this,[this]{
        background_src = instance->background_src;
        if(QML_VALID(this))
            qmlItem->setProperty("source",background_src);
    });
    instance->border_color.addListener(this,[this]{
        border_color = instance->border_color;
        if(QML_VALID(this))
            qmlItem->setProperty("border.color",border_color);
    });
    instance->lineWidth.addListener(this,[this]{
        lineWidth = instance->lineWidth;
        if(QML_VALID(this))
            qmlItem->setProperty("border.width",lineWidth);
    });
    instance->radius.addListener(this,[this]{
        radius1 = instance->radius;
        if(QML_VALID(this))
            qmlItem->setProperty("radius",radius1);
    });
}


int Frame_ofHLines::dealCommandFromQmlItem(int command,const QVariant& arg) {
    switch(command) {
    case Helper::FILL_MODE_UP:
        instance->background_type = arg.toInt();
        break;
    case Helper::BG_COLOR_UP: // 背景色up
    case Helper::COLOR_UP:
        instance->background_color = arg.value<QColor>();
        break;
    case Helper::STROKE_COLOR_UP: // 边框色up
        instance->border_color = arg.value<QColor>();
        break;
    case Helper::STROKE_WIDTH_UP: // 边框宽度up
        instance->lineWidth = arg.toFloat();
        break;
    case Helper::SOURCE_UP:
        instance->background_src = arg.toString();
        break;
    case Helper::RADIUS_UP:
    case Helper::RADIUS_1_UP:
        instance->radius = arg.toFloat();
        break;
    case Helper::RADIUS_2_UP:
        //instance->radius2 = arg.toFloat();
        break;
    case Helper::RADIUS_3_UP:
        //instance->radius3 = arg.toFloat();
        break;
    case Helper::RADIUS_4_UP:
        //instance->radius4 = arg.toFloat();
        break;
    default:
        return Free_Frame::dealCommandFromQmlItem(command,arg);
    }
    return 0;
}


void Frame_ofHLines::dealLayout()
{
    // if(! hline_start.valid() || ! hline_end.valid()) {
    //     removeSelf(true);
    //     return;
    // }

    // if(hline_start->getPCPos() < pc_pos) {
    //     hline_start = hline_start->getNextLine();
    // } else
    // if(auto prev_hline_start = hline_start->getPrevLine();
    //     prev_hline_start->getPCPos() >= pc_pos) {
    //     hline_start = prev_hline_start;
    // }

    // if(hline_start->getPCPos() < pc_pos) {
    //     hline_start = hline_start->getNextLine();
    // } else
    // if(auto next_hline_end = hline_end->getNextLine();
    //     next_hline_end && next_hline_end->getPCPos() <= pc_pos) {
    //     hline_start = next_hline_end;
    // }
}

void AnchorObj_PHLeft_as_FrameBegin::dealLayout()
{
    AnchorObj_PHLeft::dealLayout();
    if(!instance || !instance->ph_end.valid()) {
        qDebug() << "!instance || !instance->ph_end.valid()";
        return;
    }
    auto hline = this->hline->be<HorLine_Base*>();
    HorLine_Base* start_hline = hline,
        *end_hline = instance->ph_end->hline->be<HorLine_Base*>();
    int frame_at = 0;
    while(hline) {
        auto nextLine = hline->getNextLine();
        if(!nextLine || nextLine->getPCPos() > hline->getPCPos()
            || hline == end_hline) { //换栏or换页
            // 安排一个frame去覆盖[start_hline,hline]
            Frame_ofHLines *frame = 0;
            if(instance->frames.size() < frame_at+1) {
                //创建frame实例
                frame = new Frame_ofHLines(instance);
                frame->instance = instance;
                instance->frames.push_back(frame);
                SoupleManager::registerObj(frame);
                qDebug() << "创建Frame";
            } else {
                frame = instance->frames[frame_at].get();
            }

            frame->x = hline->x - instance->leftPadding;
            frame->width = hline->width + instance->leftPadding + instance->rightPadding;
            frame->y = start_hline->getContentTop() - instance->topPadding;
            frame->height = hline->getContentBottom() - frame->y;
            if(QML_VALID(frame)) {
                frame->qmlItem->setSize({frame->width,frame->height});
                frame->qmlItem->setPosition({frame->x,frame->y});
            }
            start_hline = nextLine;
            ++frame_at;
        }
        if(hline == end_hline)
            break;
        hline = nextLine;
    }
    // 隐藏多余的frame
    for(auto& frame : instance->frames | views::drop(frame_at)) {
        if(QML_VALID(frame)) frame->discard_qmlItem();
    }
}
