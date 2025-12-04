#include "turnbackmanager.h"
#include "horline_base.h"
#include "souplemanager.h"

#define MERGE_INTERVAL 300

Turnback* TurnbackManager::addTurnback() {
    turnback_list.push_back(new Turnback);
    qDebug() << "Turnback Num: " << turnback_list.size();
    /** 清除重做列表 */
    for(auto tb : redo_list) {
        if(tb->action == Turnback::AC_Content_Flow) {
            auto obj = (AnchorObj*)SoupleManager::getObjById(tb->attach_obj_id);
            if(obj) obj->removeFlowAttacher(tb); //移除附着符
        }
    }
    redo_list.clear(); //清除重做列表
    if(turnback_list.size() > max_num) {
        auto tb = turnback_list.front();
        turnback_list.pop_front();
        auto obj = (AnchorObj*)SoupleManager::getObjById(tb->attach_obj_id);
        if(obj) obj->removeFlowAttacher(tb); //移除附着符
    }
    turnback_list.back()->time_stamp = Helper::ptime;
    return turnback_list.back();
}

// 撤回一次
void TurnbackManager::undo() {
    if(turnback_list.empty()) return;
    int last_time = turnback_list.back()->time_stamp;
    auto __undo = [=] {
        Turnback* tb = turnback_list.back();
        turnback_list.pop_back();
        redo_list.push_back(tb);
        qDebug() << "UNDO: ";
        tb->print();
        Obj* obj = SoupleManager::getObjById(tb->attach_obj_id);
        if(! obj) return;
        tb->undo(tb,obj);
    };
    while(!turnback_list.empty() && turnback_list.back()->trigger_by_software)
        __undo(); //跳过所有“软件”Turnback，直到找到第一个“用户”Turnback
    while(!turnback_list.empty()
           && abs(turnback_list.back()->time_stamp - last_time) < MERGE_INTERVAL) {
        __undo();
    }
}

// 重做一次
void TurnbackManager::redo() {
    if(redo_list.empty()) return;
    int last_time = redo_list.back()->time_stamp;
    auto __redo = [=] {
        Turnback* tb = redo_list.back();
        redo_list.pop_back();
        turnback_list.push_back(tb);
        Obj* obj = SoupleManager::getObjById(tb->attach_obj_id);
        if(! obj) return;
        tb->redo(tb,obj);
    };
    while(!redo_list.empty() && redo_list.back()->trigger_by_software)
        __redo(); //跳过所有“软件”Turnback，直到找到第一个“用户”Turnback
    while(!redo_list.empty()
           && abs(redo_list.back()->time_stamp - last_time) < MERGE_INTERVAL)
    {
        __redo();
    }
}
