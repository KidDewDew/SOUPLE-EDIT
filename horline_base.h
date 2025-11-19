#ifndef HORLINE_BASE_H
#define HORLINE_BASE_H

#include "anchorobj.h"
#include "uiitempool.h"
#include <deque>
#include "anchorobj_glue.h"
#include "anchorobj_phleft.h"
#include "LLException.h"

//水平标线基类，继承自AnchorObj
class HorLine_Base : public AnchorObj
{
    friend class SoupleManager;
    friend class Pdf2Souple;
    friend class FreeObj;
    friend class AnchorObj_PHLeft;
    friend class SoupleManager;
    //友元的原则是：一个类如果包含创建本对象的过程，则可以成为友元类。(好吧，主要是方便。。。
public:
    HorLine_Base();
    virtual QQuickItem* generateQmlItem() override;
    virtual void updateDataToQmlItem(QQuickItem*) override;
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override;
    virtual void dealLayout() override;

    virtual void insertOnLeft(AnchorObj* obj) override; //从左边插入
    virtual void insertOnRight(AnchorObj* obj) override; //从右边插入

    virtual int getContentIndex(Obj* obj) const noexcept {
        int ci = 0;
        auto obj1 = leftObj;
        while(obj1 && obj1 != obj) {
            ci += obj1->contentLength();
            obj1 = obj1->rightObj;
        }
        return ci;
    }

    virtual void getObj_atContentIndex(int ci,Obj*& obj,int& ci_obj) const noexcept {
        int ci2 = 0;
        auto obj1 = leftObj;
        while(obj1 && ci2 < ci) {
            ci2 += obj1->contentLength();
            obj1 = obj1->rightObj;
        }
        obj = obj1;
        ci_obj = ci2 - ci;
    }

    virtual float getHScale() const noexcept {
        return 1.0f;
    }

    virtual float getObjX_atHScale(AnchorObj* obj) const noexcept {
        return obj->x;
    }

    virtual HorLine_Base *getNextLine() const noexcept {
        return 0;
    }

    virtual HorLine_Base *getPrevLine() const noexcept {
        return 0;
    }

    // 【注意】setNextLine和setPrevLine有很大区别
    // setNextLine仅仅是简单的设置了下一条线，不会保证接续。
    [[deprecated("该函数易导致歧义。请使用connectHLine_down/up或setPrevLine。")]]
    virtual void setNextLine(HorLine_Base* l) {}

    // setPrevLine
    // 【注意】setPrevLine会确保接续性，会自动处理接续。
    virtual void setPrevLine(HorLine_Base* l) {}

    virtual const Obj_Global_Info& objInfo() const noexcept override {
        static Obj_Global_Info gi = {.dealLayoutable = true,.isHelperLine = true};
        return gi;
    }

    float getContentTop() const { return y + contentTop; }

    float getContentBottom() const { return y + contentBottom; }

    virtual float getTopMargin() const { return 0; }

    //注意！：如果dead = true，请保证调用前，该行为空行！即leftObj == 0
    virtual void removeSelf(bool dead) override;

    virtual void discard_qmlItem() override {
        uiPool_BLine::returnItem(qmlItem);
        qmlItem = 0;
    }

    virtual int contentLength()  const noexcept override {
        auto obj = leftObj;
        int length = 0;
        while(obj) {
            length += obj->contentLength();
            obj = obj->rightObj;
        }
        return length;
    }

    enum {HLT_Same=1,HLT_Anchor=2,HLT_Inner=4};

    // 在这条线下面创建一条接续的水平线
    // @param hline_type 取HLT_???，表示创建的水平线类型。
    virtual HorLine_Base* insertHLine_down(int hline_type) {
        return nullptr;
    }
    // 在这条线上面创建一条接续的水平线
    virtual HorLine_Base* insertHLine_up(int hline_type) {
        return nullptr;
    }

    /* connectHLine_up和connectHLine_down
     * 是对setPrevLine和setNextLine的调用+额外一些属性设置。
     **/

    // 向上连接new_line
    bool connectHLine_up(HorLine_Base* new_line) noexcept {
        try {
            setPrevLine(new_line);
            //new_line->setNextLine(this);
        } catch(LLException& e) {
            return false;
        }
        return true;
    }
    // 向下连接new_line
    bool connectHLine_down(HorLine_Base* new_line) noexcept {
        try {
            new_line->setPrevLine(this);
            //setNextLine(new_line);
        } catch(LLException& e) {
            return false;
        }
        return true;
    }

    virtual PCPos getPCPos();

    float getPHLeftWidth() {
        if(leftObj == 0) return 0;
        if(auto ph = leftObj->as<AnchorObj_Glue*>(); ph && ph->glue_left) {
            return ph->width;
        } else  if(auto ph = leftObj->as<AnchorObj_PHLeft*>(); ph) {
            return ph->width;
        }
        return 0.0;
    }

    // 自动设置段落属性
    void autoSetPara();

    // void addFlowAttacher(FlowAttacher* attacher) {
    //     qDebug() << "addFlowAttacher" << attacher->flow_position;
    //     auto it = flowAttacher_list.begin();
    //     while(it != flowAttacher_list.end()) {
    //         if((*it)->flow_position >= attacher->flow_position)
    //             break;
    //         ++it;
    //     }
    //     //从i处插入
    //     flowAttacher_list.insert(it,attacher);
    // }

    // void removeFlowAttacher(FlowAttacher* attacher) {
    //     std::ranges::remove(flowAttacher_list,attacher);
    // }

    // getWalker
    // 获取一个可迭代容器
    // 迭代内容：tuple[obj,start_cl,max_len]
    // [使用示例：]   '''      ''''''''''''''''''''''
    //    for(auto[obj,at,len] : getWalker(2,60)) {
    //       obj->slice(at,len)->fontSize = 16;
    //    }
    // 一个典型的例子，在实例化Turnback时，多多使用这个函数，可以简化代码。
    // @param start_content_length: 从这条水平标线的哪儿开始遍历，
    //            可以超出本水平标线范围，甚至允许是负数。
    // 在遍历过程中，允许切分obj（slice)。条件：obj右边最多插入一个新对象，左边随意。
    //         并保证接下来的contentLength不变化。
    auto getWalker(int start_content_length,int walk_content_length) noexcept
    {
        class _Walker_Cont {
        public:
            HorLine_Base* hline_at;
            int s_cl,w_cl;
            Iterator_Walk begin() noexcept {
                return Iterator_Walk(hline_at,s_cl,w_cl);
            }
            Iterator_Walk end() noexcept {
                return Iterator_Walk();
            }
        };
        return _Walker_Cont{this,start_content_length,walk_content_length};
    }

    // 重载版本：从start_obj[start_i]开始遍历。
    // 同样，start_i可超过范围，可负。
    auto getWalker(AnchorObj* start_obj,int start_i,int walk_length) noexcept
    {
        class _Walker_Cont_2 {
        public:
            AnchorObj* obj;
            int s_cl,w_cl;
            Iterator_Walk begin() noexcept {
                return Iterator_Walk(obj,s_cl,w_cl);
            }
            Iterator_Walk end() noexcept {
                return Iterator_Walk();
            }
        };
        return _Walker_Cont_2{start_obj,start_i,walk_length};
    }



    // 内容遍历迭代器类，貌似是getWalker的辅助类。
    class Iterator_Walk {
        HorLine_Base* hline_at;
        AnchorObj* obj_at = 0, *expect_right_obj = 0;
        int rest_content_length;
        int content_length_at_obj;
        int this_obj_content_length;
    public:
        Iterator_Walk() {
            hline_at = 0;
            rest_content_length = 0;
        }
        Iterator_Walk(AnchorObj* start_obj,int start_i,int content_length)
        : content_length_at_obj(0){
            hline_at = start_obj->hline->be<HorLine_Base*>();
            obj_at = start_obj;
            rest_content_length = content_length;
            walk_by(start_i);
            expect_right_obj = obj_at->rightObj;
            this_obj_content_length = obj_at->contentLength();
        }
        Iterator_Walk(HorLine_Base* start_hline,
                      int start_content_length,
                      int content_length)
            :hline_at(start_hline),rest_content_length(content_length),
            content_length_at_obj(0)
        {
            if(rest_content_length == 0) return; //this is END-iter.
            while(hline_at && !hline_at->leftObj) { //跳过所有空行
                hline_at = hline_at->getNextLine();
            }
            if(hline_at) {
                obj_at = hline_at->leftObj;
                walk_by(start_content_length);
                if(obj_at) {
                    expect_right_obj = obj_at->rightObj;
                    this_obj_content_length = obj_at->contentLength();
                }
            }
            else rest_content_length = 0;
        }
        bool operator==(const Iterator_Walk& i2) const noexcept {
            if(rest_content_length == 0) return i2.rest_content_length == 0;
            return obj_at == i2.obj_at && content_length_at_obj == i2.content_length_at_obj;
        }
        // 注：无法提供->重载，因为没有可以返回的左值。
        std::tuple<AnchorObj*,int,int> operator*() noexcept {
            return {obj_at,content_length_at_obj,rest_content_length};
        }
        Iterator_Walk& operator++() {
            walk_to_next();
            return *this;
        }
    private:
        void walk_to_next() {
            rest_content_length = rest_content_length +
                                  content_length_at_obj - this_obj_content_length;
            qDebug() << "walk_to_next(rest:"<<rest_content_length;
            content_length_at_obj = 0;
            if(rest_content_length <= 0) {
                rest_content_length = 0;
                return;
            }
            to_next_obj();
            qDebug() << "at " << obj_at;
            //必须提前计算这个obj的内容长度
            if(obj_at) {
                this_obj_content_length = obj_at->contentLength();
                expect_right_obj = obj_at->rightObj; //记录预期的rightObj
                qDebug() << "this_content_Length: " << this_obj_content_length;
            }
        }
        void to_next_obj() {
            if(expect_right_obj) { //使用预期的rightObj以防止用户进行了obj的插入、断裂
                obj_at = expect_right_obj;
            }
            else if(gotoNextLine()) {
                qDebug() << "gotoNextLine()";
                obj_at = hline_at->leftObj;
            } else {
                rest_content_length = 0; //设置剩余长度为0 ~ 迫使迭代终止
            }
        }
        // 走step步
        void walk_by(int step) {
            qDebug() << "walk by " << step;
            if(step > 0) goto STEP_GE_0;
            else if(step < 0)goto STEP_LE_0;
            else return;
        STEP_GE_0:
            step += content_length_at_obj;
            content_length_at_obj = 0;
            while(step > 0 && hline_at) {
                int cl = obj_at->contentLength();
                expect_right_obj = obj_at->rightObj;
                if(cl > step) {
                    content_length_at_obj = step;
                    break;
                }
                step -= cl;
                to_next_obj();
            }
            return;
        STEP_LE_0:
            step = step + content_length_at_obj - obj_at->contentLength();
            content_length_at_obj = 0;
            while(step < 0 && hline_at) {
                int cl = obj_at->contentLength();
                if(cl+step >= 0) {
                    content_length_at_obj = cl+step;
                    break;
                }
                step += cl;
                if(obj_at->leftObj) obj_at = obj_at->leftObj;
                else if(gotoPrevLine()) {
                    obj_at = hline_at->rightObj;
                } else {
                    throw;
                }
            }
        }

        HorLine_Base* gotoNextLine() {
            hline_at = hline_at->getNextLine();
            while(hline_at && !hline_at->leftObj) { //跳过所有空行
                hline_at = hline_at->getNextLine();
            }
            return hline_at;
        }

        HorLine_Base* gotoPrevLine() {
            hline_at = hline_at->getPrevLine();
            while(hline_at && !hline_at->rightObj) { //跳过所有空行
                hline_at = hline_at->getPrevLine();
            }
            return hline_at;
        }
    };
protected:

    // 移动所有流附着符
    // @param step: 移动数量 负数表示左移，正数表示右移
    // 显然，在 行左边流入/流出 时，必须调用该函数。
    // void moveFlowAttacher(int step) {
    //     for(auto attacher : flowAttacher_list) {
    //         attacher->flow_position += step;
    //         qDebug() << "attchar->flow_pos => " << attacher->flow_position;
    //     }
    // }

    //void innerFlowChange(int )

    // void chopFlowAttacher(HorLine_Base* lastLine,HorLine_Base* nextLine) {
    //     chopFlowAttacher(contentLength(),lastLine,nextLine);
    // }

    // 砍掉超出右边范围的流附着符
    // void chopFlowAttacher(int this_contentLength,HorLine_Base* lastLine,HorLine_Base* nextLine) {
    //     if(lastLine) {
    //         while(!flowAttacher_list.empty() && flowAttacher_list.front()->flow_position < 0)
    //         {
    //             flowAttacher_list.front()->flow_position += lastLine->contentLength();
    //             lastLine->flowAttacher_list.push_back(flowAttacher_list.front());
    //             flowAttacher_list.pop_front();
    //             //qDebug() << "FlowAttacher pop_front";
    //         }
    //     }
    //     if(nextLine) {
    //         while(!flowAttacher_list.empty() && flowAttacher_list.front()->flow_position >= this_contentLength)
    //         {
    //             flowAttacher_list.back()->flow_position -= this_contentLength;
    //             nextLine->flowAttacher_list.push_front(flowAttacher_list.back());
    //             flowAttacher_list.pop_back();
    //             //qDebug() << "FlowAttacher pop_backr";
    //         }
    //     }
    // }
private:
    static inline constexpr char __UINAME__[] = "BLine";
public:
    static inline QHash<QString,HorLine_Base*> hash_hline;
    const Page *page = 0; //记录所属页面，当然派生类也可以忽略;
    float contentTop = 0.0, contentBottom = 0.0; //所有位于该hline上的obj相对于hline.y的top_y和bottom_y

    //(暂时)位于该hline的所有附着符    //Turnback动作
    //所有hline必须正确传递附着符   //turnback
    // std::deque<FlowAttacher*> flowAttacher_list; [错误的设计]

    typedef UIItemPool<__UINAME__,20> uiPool_BLine; //ui控件池
private:

};

#endif // HORLINE_BASE_H
