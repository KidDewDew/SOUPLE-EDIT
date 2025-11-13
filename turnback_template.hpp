#ifndef TURNBACK_TEMPLATE_HPP
#define TURNBACK_TEMPLATE_HPP

#include "souplemanager.h"

class TurnbackManager;

template<typename T,typename _Func>
    requires requires(_Func func,T* obj,int start_i,int len){
        {func(obj,start_i,len)}->std::same_as<void>;
    }
auto TurnbackManager::new_template_walker(_Func func,int contentLength) {
    return [=](Turnback*tb,Obj*obj)->void {
        HorLine_Base* hline = (HorLine_Base*)(((AnchorObj*)obj)->hline);
        if(! hline) return;
        for(auto[obj,start_i,len] : hline->getWalker((AnchorObj*)obj,tb->flow_position,contentLength)) {
            T* t = obj->as<T*>();
            if(! t) return;
            func(t,start_i,len);
        }
    };
}

#endif // TURNBACK_TEMPLATE_HPP
