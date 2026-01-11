#ifndef MAGICALCURSOR_H
#define MAGICALCURSOR_H

#define mcursor MagicalCursor

#include "Obj.h"

#include "safe_pointer.h"

#define Mgc MagicalCursor

// 魔法光标
class MagicalCursor
{
public:
    // 光标位于哪个obj上,注意该函数不是O(1)复杂度。
    static Obj* at_who() {
        return obj_at.valid() ? obj_at.get() : nullptr;
    }
    // 光标位于obj的哪个位置
    static int at_pos() {
        return pos;
    }

    static void set_cursor(Obj* obj,int pos) {
        obj_at = obj;
        Mgc::pos = pos;
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
