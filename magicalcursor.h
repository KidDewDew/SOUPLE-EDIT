#ifndef MAGICALCURSOR_H
#define MAGICALCURSOR_H

#define mcursor MagicalCursor

#include "Obj.h"

#include "safe_pointer.h"

#define Mgc MagicalCursor


// 魔法光标
// 光标位置？总是位于who的右边缘
// 本类还负责控制通用光标的显隐
class MagicalCursor
{
private:
    static inline QQuickItem * cursor_quickitem;
public:

    // 初始化，请在Main.qml可用后调用
    static void init();

    // 光标位于哪个obj上,注意该函数不是常数1的复杂度。
    static Obj* at_who() {
        return obj_at.valid() ? obj_at.get() : nullptr;
    }
    // 光标位于obj的哪个位置
    static int at_pos() {
        return pos;
    }

    // 如果一个对象知道自己是自行管理光标的，就调用该函数
    static void set_cursor(Obj* obj,int pos) {
        obj_at = obj;
        Mgc::pos = pos;
        cursor_quickitem->setVisible(false);
    }

    // 如果一个对象不调用set_cursor就应该调用set_cursor_sync
    // tArg EnsureUC: 是否确定需要使用统一光标
    template<bool EnsureUC = false>
    static void set_cursor_sync(Obj* obj, int pos) {
        obj_at = obj;
        Mgc::pos = pos;
        if constexpr(! EnsureUC) {
            if(obj->keyInfo().selfDeal_input) return;
        }
        // to-here: EnsureUC || NOT obj->keyInfo().selfDeal_input
        float cursor_cx = obj->x_ofIndex(pos);
        cursor_quickitem->setPosition({cursor_cx+obj->x-1,obj->y});
        cursor_quickitem->setHeight(std::max<double>(obj->height,9));
        cursor_quickitem->setVisible(true);
    }

    // 移动光标时，一件事情是：
    //    *如果移动到新的obj，且这个obj的qml对象未创建，
    //     那么Mgc会直接创建qml对象，并设置其focus=true.

    // (try)光标向右移动一步
    static void move_right() {

    }

    // (try)光标向左移动一步
    static void move_left() {

    }

public:
    static inline Safe_Obj_Pointer<Obj> obj_at;
    static inline int pos;
};

#endif // MAGICALCURSOR_H
