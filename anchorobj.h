#ifndef ANCHOROBJ_H
#define ANCHOROBJ_H

#include <QQuickItem>
#include "obj.h"
#include <typeindex>
#include <deque>

//共有信息，类型相关
struct AnchorObj_Global_Info {
    bool isSelfWidth = true; //是否自己决定自己的宽度，而不是由别的因素决定（典型如右占位符，其isSelfWidth = false)
    bool isRealHeight = true;
    bool allowSelect = true,allowPartSelect = false; //是否允许选择；是否允许部分选择
};

class TurnbackManager;

//锚定元素c++基类
class AnchorObj : public Obj
{
    friend TurnbackManager;
public:
    //enum {};
    //AnchorObj() {   id = s_all_id++;    }
    virtual ~AnchorObj() = default;
    //virtual QQuickItem* generateQmlItem() override; //创建用于ui的qml元素
    //virtual void updateDataToQmlItem(QQuickItem*) override;    //更新数据到qml元素
    //virtual void dealCommandFromQmlItem(const QString& command,const QString& args) override {};  //处理来自对应qml item的指令
    virtual void dealLayout() override; //处理布局
    virtual bool tryMergeRight() { return false; } //尝试合并右边
    virtual void insertOnLeft(AnchorObj* obj); //从左边插入
    virtual void insertOnRight(AnchorObj* obj); //从右边插入
    virtual AnchorObj* dropRight(float dropWidth) { return 0; } //尝试截断并丢弃右边
    virtual AnchorObj* dropLeft(float dropWidth) { return 0; }  //尝试截断并丢弃左边
    virtual bool check_dropLeft(float dropWidth) { return false; } //仅检测给定dropWidth是否会截断左边
    virtual void removeSelf(bool dead = true) override;
    //float x,y; //x,y坐标: 为了快速定位AnchorObj，需要暴露并更新y坐标
    virtual void getCursorFromLeft(int RN= 0); //从左边获取光标
    virtual void getCursorFromRight(int RN = 0); //从右边获取光标
    //利用多态获取类型相关的信息 [于2025/7/30删除该函数]
    // virtual const AnchorObj_Global_Info& global_info() const {
    //     static AnchorObj_Global_Info _global_info = {.isSelfWidth = true};
    //     return _global_info;
    // }

    /**
     * @brief slice  截取一段内容，获取独立的对象。
     *         ```同时，截取产生的所有对象都会正常地安放，就仿佛没有截取过一样。
     *         ```该函数如同tryMergeRight()的逆过程
     * @param start_i:  从内容索引的start_i开始截取
     * @param max_len：     最大截取的内容长度
     * @return 截取的AnchorObj
     */
    virtual AnchorObj* slice(int start_i,int max_len) {
        return this;
    }

    virtual const Obj_Global_Info& objInfo() const noexcept override {
        static Obj_Global_Info gi = {.dealLayoutable = false,.allowSelect = true,.allowPartSelect = false,
                                    .anchorInfo = {.isSelfWidth = true,.isRealHeight = true}};
        return gi;
    }

    virtual AnchorObj *getLastObj() const;

    // connect_l2r(至少两个对象才可以使用)
    // 从左到右逐个连接，模板展开。【注】没有递归开销
    template <typename...Ts>
    static inline void connect_l2r(AnchorObj* a,AnchorObj* b,Ts...objs) {
        b->leftObj = a;
        a->rightObj = b;
        if constexpr(sizeof...(objs) > 0) {
            connect_l2r(b,objs...);
        }
    }

    // connect_l2r_atHLine
    // 把一组对象放到hline上(至少2个对象)
    template <typename...Ts>
    static inline void connect_l2r_atHLine(AnchorObj*_hline,Ts...objs) {
        _hline->insertOnRight(
            std::tuple_element<sizeof...(objs)-1,Ts...>(std::tuple(objs...)));
        _hline->insertOnLeft(
            std::tuple_element<0,Ts...>(std::tuple(objs...)));
        ((objs->hline = _hline),...);
        connect_l2r(objs...);
    }

    uchar vAlignMode = Helper::AlignVCenter;
    float vAlignOffset = 0.0;
    AnchorObj *leftObj = 0, *rightObj = 0; //左右obj
    AnchorObj *hline = 0;
protected:
    /**
     * 什么时候需要转移flow_attachers?
     * 当合并右对象时，得把右对象的flow_attachers转移到自己身上。
     * 当左截断时，可以把自己的flow_attachers转移到左边的section身上,
     *      也可以只调用moveFlowAttachers。
     * 而右截断时，可以完全不转移，也可以向右转移。
     * 为了避免出现“单边靠”的情况，可以按概率来左/右截断转移。1/2 : 1/2
     * 【原则】最左边的切片obj负责存储原来的所有flow_attachers
     */
    // 合并右边对象的flowAttachers，在mergeRight时必须使用该函数
    // to_merge_obj: 将要合并掉的对象
    // add_flow_position: 对合并的flow_attacher，增加的偏移量、
    // 注：该函数确保`左对象`合并`右对象`。
    void merge_flowAttachers(AnchorObj* to_merge_obj,int add_flow_position) {
        for(auto attacher : to_merge_obj->flow_attachers) {
            attacher->flow_position += add_flow_position;
            attacher->attach_obj_id = id;
            flow_attachers.push_back(attacher);
        }
    }

    void moveFlowAttachers(int add_flow_position) {
        for(auto attacher : flow_attachers) {
            attacher->flow_position += add_flow_position;
        }
    }

    void addFlowAttacher(FlowAttacher* attacher) {
        flow_attachers.push_back(attacher);
    }

    void removeFlowAttacher(FlowAttacher* attacher) {
        std::ranges::remove(flow_attachers,attacher);
    }

    std::vector<FlowAttacher*> flow_attachers;
private:
};

#endif // ANCHOROBJ_H
