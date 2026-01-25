#ifndef OBJUTIL_H
#define OBJUTIL_H

#include "anchorobj.h"
#include "helper.h"

//模板函数，不用担心重定义。
void sort_anchorobj(const Iterable<AnchorObj*> auto& objs,CanPushback<AnchorObj*> auto& result) {
    std::set<AnchorObj*> objs_set;
    for(auto obj : objs) objs_set.insert(obj);
    AnchorObj* start_obj = 0;
    for(auto obj : objs) {
        auto obj_before = obj->get_neighbor_before();
        //qDebug() << "sortat: " << obj->__dstr();
        if( obj_before && !objs_set.contains(obj_before)) {
            start_obj = obj_before;
            break;
        }
    }
    if(!start_obj) return;
    //qDebug() << "start_obj: " << start_obj->__dstr();
    //根据start_obj重新排序
    AnchorObj* cur_obj = start_obj->get_neighbor_after();
    while(true) {
        if( ! objs_set.contains(cur_obj)) {
            break;
        }
        result.push_back(cur_obj);
        cur_obj = cur_obj->get_neighbor_after();
    }
}


#endif // OBJUTIL_H
