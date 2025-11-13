#ifndef FRAME_OFHLINES_H
#define FRAME_OFHLINES_H

#include "anchorobj_phright.h"
#include "anchorobj_phleft.h"
#include "free_frame.h"
#include "safe_pointer.h"
#include "LiveValue.h"

struct Frame_ofHLines_Instance;

// AnchorObj_PHRight_as_FrameEnd
// 描述：行为上等价于换行符。但它始终隐藏自己，不会被用户删除，就仿佛不存在一般。
// 直到其所属的Frame_ofHLines死亡。
class AnchorObj_PHRight_as_FrameEnd: public AnchorObj_PHRight
{

};

// AnchorObj_PHLeft_as_FrameBegin
// 描述：行为上等价于段落占位。但它始终隐藏自己，不会被用户删除，就仿佛不存在一般。
class AnchorObj_PHLeft_as_FrameBegin: public AnchorObj_PHLeft
{
public:
    virtual void dealLayout() override;
public:
    std::shared_ptr<Frame_ofHLines_Instance> instance;
};

// Frame_ofHLines
// 描述：给一组连续的AnchorObj_HLine提供边框、背景色等。
// 只支持最外层的AnchorObj_HLine。
class Frame_ofHLines: public Free_Frame
{
    friend class AnchorObj_PHLeft_as_FrameBegin;
    friend class AnchorObj_PHRight_as_FrameEnd;
public:
    Frame_ofHLines(std::shared_ptr<Frame_ofHLines_Instance>,PCPos pc_pos={}) noexcept;
    void setPCPos(PCPos pc_pos) noexcept {
        this->pc_pos = pc_pos;
    }
    virtual void dealLayout() override;
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override;
protected:
    std::shared_ptr<Frame_ofHLines_Instance> instance; //共享实例
    // Safe_Obj_Pointer<HorLine_Base> hline_start,hline_end;
    //std::vector<Safe_Obj_Pointer<Free_Frame>> free_frames; //生成的free_frame列表
    PCPos pc_pos; //栏位置
};


// 一个Frame_ofHLines的实例
struct Frame_ofHLines_Instance {
    enum {
        Color_Bg,Pattern_Bg,Img_Bg
    };
    LiveValue<float> leftPadding,rightPadding,
                     topPadding,bottomPadding;
    LiveValue<char> background_type;
    LiveValue<float> lineWidth;          //边框宽度
    LiveValue<float> radius;             //圆角半径  //todo
    LiveValue<QColor> border_color;      //边框颜色
    LiveValue<QColor> background_color;  //背景色
    LiveValue<QString> background_src;   //背景图片
    Safe_Obj_Pointer<AnchorObj_PHLeft_as_FrameBegin> ph_begin;
    Safe_Obj_Pointer<AnchorObj_PHRight_as_FrameEnd> ph_end;
    std::vector<Safe_Obj_Pointer<Frame_ofHLines>> frames;
};
#endif // FRAME_OFHLINES_H
