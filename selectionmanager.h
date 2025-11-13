#ifndef SELECTIONMANAGER_H
#define SELECTIONMANAGER_H

#include "obj.h"
#include "anchorobj.h"
#include <QObject>
#include <vector>
#include <QQuickItem>

class HorLine_Base;

//该类负责管理"选择"操作
//为前端提供最简洁的选择操作接口
//并根据选择内容通知前端对象被(部分)选择
//[notice] 选择只对锚定对象起作用；对于FreeObj的选择，由对象自己处理。
//2025/7/16 该类正在维护 ... 2025/11/6 该类仍在维护...
class SelectionManager : public QObject
{
    Q_OBJECT
    friend class SoupleManager;
public:

    struct SelectionItem {
        int obj_id,begin_index,end_index;
        SelectionItem() = default;
        SelectionItem(int obj_id,int begin_index,int end_index):obj_id(obj_id),begin_index(begin_index),end_index(end_index){}
    };

    static SelectionManager* getInstance() noexcept {
        static SelectionManager* _instance = new SelectionManager;
        return _instance;
    }

    //返回：是否停止选择，但选择内容保持着。
    static inline bool isSelectStopButKeep() noexcept {
        return !_hasBegun && isSelectionKeep;
    }

    static inline bool hasSelection() noexcept {
        return isSelectionKeep;
    }

    Q_INVOKABLE bool isSelecting() noexcept {
        return _hasBegun && isSelectionKeep;
    }

    //判断并获取obj的被选择信息。
    static inline std::optional<SelectionItem> getSelectionItem(int obj_id) noexcept {
        auto it = selection_items.find(obj_id);
        if(it == selection_items.end()) return {};
        return *it;
    }

    static inline void putSelectionItem(int id,const SelectionItem& si) noexcept {
        selection_items[id] = si;
        qDebug() << "putSelectionItem(" << id;
    }

    static inline void removeSelectionItem(int id) noexcept {
        selection_items.remove(id);
    }

    //@brief 通知SoupleManager，前端开始选择
    //@arg id:开始选择的对象id begin_x:鼠标X坐标(全局) begin_y:鼠标Y坐标(全局)
    Q_INVOKABLE static void beginSelect(int id,float begin_x,float begin_y) noexcept;

    //@brief 通知SoupleManager，前端拖动选择
    Q_INVOKABLE static void moveSelect(float to_x,float to_y) noexcept;

    //@brief 通知SoupleManager，停止本次选择
    Q_INVOKABLE static void stopSelect() noexcept;

    //@brief 清除选择内容
    Q_INVOKABLE static void clearSelection() noexcept;

    //@brief 批量发送命令
    Q_INVOKABLE static void sendCommands(int command,const QVariant& arg);

    //@brief 统计选择的内容的属性，赋予给pobj
    Q_INVOKABLE static void staticProperty(QObject* pobj);

    //@brief 把选择的内容转换为格式化字符串
    Q_INVOKABLE static QString toString() { return ""; };

    //static

    //@brief 要求重新计算选择内容，从头开始。
    static void recalculateSelection() noexcept;

    //@brief 通知所有“可见”的涉及被选择的qmlItem它们被(部分)选中了
    //
    static void notifyAllVisible() noexcept;
    static bool hasBegun() noexcept {
        return _hasBegun;
    }
    // 获取选择的内容所在的hline区间，
    // 该函数时间复杂度极高，O(N(all of objs))
    // @return pair<第一个hline，最后一个hline>
    static std::pair<HorLine_Base*,HorLine_Base*> getSelectedHLineRange();
private:

    static inline bool isRev() noexcept {
        return !(start_pcpos < end_pcpos ||
               start_pcpos == end_pcpos && select_end_y >= select_begin_y);
    }

    static inline int getBeginID() noexcept { return select_queue.front(); }

    static inline int getEndID() noexcept { return select_queue.back(); }

    static inline void push(int obj) noexcept { select_queue.push_back(obj); }

    static inline void pop() noexcept { select_queue.pop_back(); }

    static inline bool isObjInSelection(const Obj* obj) noexcept;

    static inline bool isObjInSelection_rev(const Obj* obj) noexcept;

private: // --- static变量
    static inline bool _hasBegun = false;                //是否开始选择
    static inline bool isSelectionKeep = false;         //选择内容是否保持
    static inline bool isSingleLine = true;              //是否是单行选择
    static inline float select_begin_x, select_begin_y; //一次选择的起始坐标
    static inline float select_end_x, select_end_y; //一次选择的起始坐标 总保证end_y >= begin_y
    static inline HorLine_Base * start_hline = 0, *end_hline = 0;
    static inline PCPos start_pcpos, end_pcpos;
    //注意：存储时存储对象id，切忌直接存储指针。对象id可以避免野指针问题。

    //select_queue:
    //涉及被选择的对象队列(当isSelecting = true时，总保证按选择的先后顺序排列！)
    static inline std::vector<int> select_queue;
    static inline QSet<int> unselect_set;  //被撤回选择的obj集合
    static inline QHash<int,SelectionItem> selection_items;
};


#endif // SELECTIONMANAGER_H
