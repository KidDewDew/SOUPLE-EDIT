#ifndef OBJ_START_SIGN_H
#define OBJ_START_SIGN_H

#include "obj.h"
#include "safe_pointer.h"
#include "horline_base.h"

// 这个组件代表着文档的开始点，
// 它附着的horline即是文档的起始水平线
class Obj_Start_Sign : public Obj{
public:
    virtual QQuickItem* generateQmlItem() override final {
        return Helper::invokeQmlFunction<QQuickItem*>("generateObj","StartSign");
    }
    virtual const Obj_Global_Info& objInfo() const noexcept override final {
        static Obj_Global_Info gi = {.dealLayoutable = true};
        return gi;
    }
public:
    Safe_Obj_Pointer<HorLine_Base> attach_hline;
};

#endif // OBJ_START_SIGN_H
