#ifndef TURNBACKMANAGER_H
#define TURNBACKMANAGER_H

#include <functional>
#include "ExportHeader.h"
#include <deque>
#include "TT_Str.h"

class Obj;

// 请每个HLine对象记录自己的Turnback，并处理Turnback的流动和换行。
struct Turnback : public FlowAttacher {
    enum {
        AC_Single_Obj,  //针对单一对象的动作（保证该对象不会分裂和合并即可）
                        //此动作下，hline_id、flow_position均[无效]。
        AC_Content_Flow //针对内容流的某个位置的动作
    };
    bool trigger_by_software = false; //是由软件自己产生的吗？
    unsigned char action; //操作名称
    int time_stamp; //操作的时间戳(ms)
    //撤回函数 重做函数
    //注：这些参数可以为[无效]
    std::function<void(Turnback*,Obj*)> undo;
    std::function<void(Turnback*,Obj*)> redo;
    void print() {
        qDebug() << "TB" << "ts:" << time_stamp << "flow-pos:" << flow_position;
    }
};
// TurnbackManager
// 撤回/重做管理类
class TurnbackManager
{
    friend class SoupleManager;
public:
    // 添加一个可撤回动作
    static Turnback* addTurnback();

    template<TT_Str tt = TStr("trigger_by_software=false")>
    static inline Turnback* addTurnback(unsigned char type) {
        auto tb = addTurnback();
        tb->action = type;
        if constexpr(tt.getBoolArg("trigger_by_software") == true) {
            tb->trigger_by_software = true;
        }
        return tb;
    }

    // 撤回一次
    static void undo();

    // 重做一次
    static void redo();

    // 模板函数 new_template_walker
    // 该函数可以对_Func func进行内联包装，简化对于需要walk hline内容的redo/undo函数编写。
    template<typename T,typename _Func>
        requires requires(_Func func,T* obj,int start_i,int len){
            {func(obj,start_i,len)}->std::same_as<void>;
        }
    static auto new_template_walker(_Func func,int contentLength);
public:
    static inline int max_num = 1000000; //默认最大撤回存储量
private:
    static inline std::deque<Turnback*> turnback_list;
    static inline std::vector<Turnback*> redo_list;
};

#endif // TURNBACKMANAGER_H
