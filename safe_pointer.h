#ifndef SAFE_POINTER_H
#define SAFE_POINTER_H

#include "obj.h"
#include "NoRef_SoupleManager.h"

// 安全Obj指针
// 该类可以确保某个obj死亡后，其他持有该对象指针的对象可以得知该对象已死亡
// 只需使用valid函数即可验证指针是否有效。
template<class T>
    requires std::is_base_of_v<Obj,T>
class Safe_Obj_Pointer {
public:
    Safe_Obj_Pointer():obj_id(-1),obj(0){}
    Safe_Obj_Pointer(T* obj):obj_id(obj->id),obj(obj){}
    Safe_Obj_Pointer(const Safe_Obj_Pointer& sop)
        :obj_id(sop.obj_id),obj(sop.obj){}
    T* get() noexcept {
        return obj;
    }

    bool valid() noexcept  {
        return obj && nullptr != NoRef_SoupleManager::getObjById(obj_id);
    }

    T* operator->() noexcept {
        return obj;
    }
    void reset() noexcept {
        this->obj_id = -1;
        this->obj = 0;
    }
    Safe_Obj_Pointer& operator=(T* obj2) {
        if(obj2 == nullptr) {
            this->obj_id = -1;
            this->obj = 0;
        } else {
            this->obj_id = obj2->id;
            this->obj = obj2;
        }
        return *this;
    }
    Safe_Obj_Pointer& operator=(const Safe_Obj_Pointer& p2) = default;
    Safe_Obj_Pointer& operator=(Safe_Obj_Pointer&& p2) = default;
private:
    int obj_id;
    T* obj;
};


#endif // SAFE_POINTER_H
