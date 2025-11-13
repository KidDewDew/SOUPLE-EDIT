#ifndef UIITEMPOOL_H
#define UIITEMPOOL_H

#include <QQuickItem>
#include "helper.h"
#include "selectionmanager.h"

// struct UIItem {
//     bool unused = false; //空闲标志
//     QQuickItem *qmlItem;
// };

// 当总的qmlItem数量超过max_total_num时，
// UIItemPool将在正常回收后：再次额外回收空闲的items，但每次最多回收掉一半。
// 因此，max_total_num只是一个“建议”，可以使得items的分布更加均衡。
class __UIItemPool_Shared {
public:
    static inline int max_total_num = 500;
    static inline int total_item_num = 0; //总计的item数量
    static inline void addOne() noexcept { ++total_item_num; }
    static inline void delOne() noexcept { --total_item_num; }
    static inline int overflow() noexcept {
        return total_item_num > max_total_num ?
            total_item_num - max_total_num : 0;
    }
};

//ui控件池，由于ui控件需要不断地删除和创建，为了重复利用，使用控件池的方法。
template<const char *UINAME,int POOL_SIZE = 50>
class UIItemPool
{
public:
    UIItemPool();

    static void scanPool() //进行一次扫描,以回收超出POOL_SIZE数量的item
    {
        while(list_items_unused.size() > POOL_SIZE * 0.25
               && getPoolItemNum() > POOL_SIZE) {
            auto item = list_items_unused.back();
            //QMetaObject::invokeMethod(item, "die");
            //item->deleteLater();
            item->releaseResources();
            item->deleteLater();
            list_items_unused.pop_back();
            __UIItemPool_Shared::delOne();
        }
        int delNum = std::min<int>(__UIItemPool_Shared::overflow(),
                              list_items_unused.size()/2);
        while(delNum--) {
            auto item = list_items_unused.back();
            item->releaseResources();
            item->deleteLater();
            list_items_unused.pop_back();
            __UIItemPool_Shared::delOne();
        }
        qDebug() << UINAME << "回收，池大小：" << getPoolItemNum() << ",使用数量：" << used_num;
        //qDebug() << "曾经峰值：" << max_allocated_num;
        //for()
    }

    static inline QQuickItem *fetchItem() //取走一个Item
    {
        //if(used_num >= POOL_SIZE) return 0;
        ++used_num;
        max_allocated_num = qMax(max_allocated_num,used_num);
        if( ! list_items_unused.empty()) {
            auto item = list_items_unused.back();
            list_items_unused.pop_back();
            item->setVisible(true);
            return item;
        }
        //ui池中没有item了
        return _createQmlItem(); //新建
    }

    static void returnItem(QQuickItem* item) //归还item
    {
        -- used_num;
        item->setVisible(false);
        //item->releaseResources();
        list_items_unused.push_back(item);
        if(SelectionManager::hasSelection()) {
            QMetaObject::invokeMethod(item,"selectcl");
        }
    }

    static int getPoolItemNum() {
        return used_num + list_items_unused.size();
    }
private:
    static inline int used_num = 0; //被取出的item的数量
    static inline int max_allocated_num = 0;
    static inline std::vector<QQuickItem*> list_items_unused; //空闲的item列表
private:
    //创建ui对象
    static inline QQuickItem* _createQmlItem() {
        __UIItemPool_Shared::addOne();
        return Helper::invokeQmlFunction<QQuickItem*>("generateObj",UINAME);
    }
};

#endif // UIITEMPOOL_H
