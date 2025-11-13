#ifndef LIVEVALUE_H
#define LIVEVALUE_H

#include "safe_pointer.h"
#include <functional>

// 可以产生信号的值
template<typename T>
class LiveValue {
public:
    LiveValue() = default;
    LiveValue(const T& t):value(t) {}
    operator T() noexcept {
        return value;
    }
    LiveValue& operator=(const T& value2) noexcept {
        if(value == value2) return *this;
        value = value2;
        // 产生信号
        for(auto it = listener_list.begin(); it != listener_list.end(); ) {
            auto&[sop,callback] = *it;
            if(sop.valid() == false) { //listener死亡，删除
                it = listener_list.erase(it);
                continue;
            }
            ++it;
            callback();
        }
        return *this;
    }
    const T& getValue() noexcept {
        return value;
    }
    void addListener(const Safe_Obj_Pointer<Obj>& sop,std::function<void(void)> func) {
        listener_list.push_back({sop,func});
    }
    void removeListener(Obj* obj) {
        for(auto it = listener_list.begin(); it != listener_list.end(); ++it) {
            if(it->first.get() == obj) {
                it = listener_list.erase(it);
                continue;
            }
        }
    }
private:
    T value;
    std::list<std::pair<Safe_Obj_Pointer<Obj>,std::function<void(void)>>>
        listener_list;
};

#endif // LIVEVALUE_H
