#ifndef ANCHOROBJ_HLINE_H
#define ANCHOROBJ_HLINE_H

#include "anchorobj.h"
#include "anchorobj_vline.h"
#include "ExportHeader.h"
#include "horline_base.h"
#include "uiitempool.h"
#include "wordpage_vline.h"
#include "LLException.h"

//HLine：水平标线的数据管理类
class AnchorObj_HLine : public HorLine_Base
{
    friend class SoupleManager;
    friend class Pdf2Souple;
    friend class FreeObj;
    friend class AnchorObj_PHLeft;
    //友元的原则是：一个类如果包含创建本对象的过程，则可以成为友元类。(好吧，主要是方便。。。
public:
    AnchorObj_HLine();
    virtual QQuickItem* generateQmlItem() override;
    virtual void updateDataToQmlItem(QQuickItem*) override;
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override;
    virtual void dealLayout() override;
    virtual QVariant qmlGetData(int dataName) override {
        switch(dataName) {
        case Helper::NAME: return name;
        case Helper::LEFTLINE: return leftLine ? leftLine->name : "";
        case Helper::RIGHTLINE: return rightLine ? rightLine->name : "";
        case Helper::DRAGY0: {
            if(! page) return 0;
            return page->getTopLineY() - contentTop;
        }
        case Helper::DRAGY1: {
            if(! page) return 1e28;
            return page->getBottomLineY() - contentBottom;
        }
        case Helper::ANCHOR_LASTHLINE: return hline ? hline->as<AnchorObj_HLine*>()->name : "";
        case Helper::LOGIC_LASTHLINE: return logic_lastHLine ? logic_lastHLine->name : "";
        case Helper::LOGIC_NEXTHLINE: return logic_nextHLine ? logic_nextHLine->name : "";
        case Helper::TOP_MARGIN: return topMargin;
        default: return AnchorObj::qmlGetData(dataName);
        }
    }

    void dealSpan(AnchorObj_HLine*anchor_lastHLine,float min_obj_y,float max_obj_y) noexcept;

    void dealAnchor(AnchorObj_HLine*anchor_lastHLine) noexcept;
    //virtual void insertOnLeft(AnchorObj* obj) override; //从左边插入
    //virtual void insertOnRight(AnchorObj* obj) override; //从右边插入

    const QString& getName() const { return name; }

    void overflow(AnchorObj* obj); //溢出obj
    void setAnchorLastHLine(AnchorObj_HLine* _hline) {
        //qDebug() << "setAnchorLastHLine(" << _hline << ",this = " << this;
        if(hline) {
           // qDebug() << "old_line: " << hline;
            hline->as<AnchorObj_HLine*>()->anchor_nextHLine.remove(this);//删除自身
        }
        hline = _hline;
        if(_hline) _hline->anchor_nextHLine.push_back(this); //记录
    }

    void setLogicLastHLine(AnchorObj_HLine* _hline) {
        if(logic_lastHLine == _hline) return;
        if(logic_lastHLine) {
            logic_lastHLine->logic_nextHLine = 0; //清除原来逻辑上标线的逻辑下标线
        }
        logic_lastHLine = _hline;
        if(_hline) _hline->logic_nextHLine = this;
    }

    void setLogicNextHLine(AnchorObj_HLine* al) {
        if(logic_nextHLine) {
            logic_nextHLine->logic_lastHLine = 0;
        }
        logic_nextHLine = al;
    }

    //注意！：如果dead = true，请保证调用前，该行为空行！即leftObj == 0
    virtual void removeSelf(bool dead) override;

    AnchorObj* dropLeft(float dropWidth) override { return nullptr; } //尝试从左边收缩

    AnchorObj_VLine *getLeftLine() const { return leftLine; }
    AnchorObj_VLine *getRightLine() const { return rightLine; }
    AnchorObj_HLine *getLogicLastLine() const { return logic_lastHLine; }
    HorLine_Base *getAnchorLastLine() const {
        return hline ? hline->as<HorLine_Base*>() : 0;
    }
    virtual HorLine_Base *getNextLine() const noexcept override {
        if(!logic_nextHLine && ! anchor_nextHLine.empty())
            return anchor_nextHLine.front();
        return logic_nextHLine;
    }
    virtual HorLine_Base *getPrevLine() const noexcept override {
        return logic_lastHLine;
    }

    virtual PCPos getPCPos() override {
        PCPos p;
        p.page = page;
        p.column = leftLine->getColumn();
        return p;
    }

    virtual float getTopMargin() const override { return topMargin; }
    virtual void setNextLine(HorLine_Base* l) override {
        auto al = l->as<AnchorObj_HLine*>();
        if(al) {
            setLogicNextHLine(al);
            al->setAnchorLastHLine(this);
        } else {
            throw LLException("AnchorObj_HLine::setNextLine(l): l必须是AnchorObj_HLine或其派生类。");
        }
    }
    virtual void setPrevLine(HorLine_Base* l) override {
        auto al = l->as<AnchorObj_HLine*>();
        if(al) {
            setAnchorLastHLine(al);
            setLogicLastHLine(al);
        } else {
            throw LLException("AnchorObj_HLine::setPrevLine(l): l必须是AnchorObj_HLine或其派生类。");
        }
    }
    auto& getAnchorNextLine() const { return anchor_nextHLine; }
    auto getLogicNextLine() const { return logic_nextHLine; }

    virtual AnchorObj *getLastObj() const override {
        if(logic_lastHLine) return logic_lastHLine->rightObj;
        return 0;
    }

    //实现 注册死亡回调函数
    virtual void register_deleteCallback(std::function<void(void)> callback) override
    {
        onDelete_Functions.push_back(callback);
    }

    virtual void discard_qmlItem() override {
        uiPool_HLine::returnItem(qmlItem);
        qmlItem = 0;
    }

    virtual float getRightX() const override
    {
        return rightLine ? rightLine->x : x + width;
    }

    float calcMarginFromLastHLine(AnchorObj_HLine* lastHLine);

    float getHorizontalScale() const noexcept {
        return horizontal_scale;
    }

    bool isWordPageLine() const noexcept {
        return bool(leftLine) && leftLine->isWordPageLine();
    }

    virtual QString __dstr() const noexcept override {
        return QString("AnchorObj_HLine id=%1 name=%2").arg(id).arg(name);
    }

    // 要求立刻计算水平放缩值（当保存pdf时，必须对每一个HLine调用该函数。）
    void forceCalculateHorizontalScale() noexcept;

    ~AnchorObj_HLine() { hash_hline.remove(name); }
protected:
    void check_after_dealSpan() {
        if(page && page->page_type == Helper::Word_Page) {
            auto word_leftLine = leftLine->be<WordPage_VLine*>();
            if(word_leftLine->page_index != page->index) {
                leftLine = page->columns.front().leftLine->be<AnchorObj_VLine*>();
                rightLine = page->columns.front().rightLine->be<AnchorObj_VLine*>();
            }
        }
    }
private:
    void createNextLine();
    void requestUpdateAnchorNextHLine();
    //void gotoNextPage(); //移动到下一页
    //void gotoPrevPage(); //移动到上一页
    static inline constexpr char __UINAME__[] = "HLine";
public:
    typedef UIItemPool<__UINAME__,90> uiPool_HLine; //ui控件池
protected:
    //bool dealLayout_protect_sign = false;  //保护dealLayout，使得dealLayout不会重复执行
    float topMargin = 50.0;
    float page_topMargin = 0.0; //相对于页面顶部
    float horizontal_scale = 1.0; //水平放缩
    //float contentTop = 0.0, contentBottom = 0.0; //所有位于该hline上的obj相对于hline.y的top_y和bottom_y
    //contentTop = minY_of_obj - hline.y
    //contentBottom = maxY_of_obj - hline.y

    //float topMargin_2 = 0.0; //当本hline和锚定上hline不在一页内，使用topMargin_2来锚定本hline
    QString name;
    AnchorObj_HLine *logic_lastHLine  = 0; //逻辑上标线
    //AnchorObj_HLine *anchor_lastHLine = 0; //锚定上标线，用AnchorObj::hline表示
    AnchorObj_HLine *logic_nextHLine  = 0; //逻辑下标线
    std::list<AnchorObj_HLine*> anchor_nextHLine; //锚定下标线列表

    //本标线死亡时，调用onDelete_Functions的所有回调函数
    //因为Anchor_Obj没有使用安全指针，HLine死亡时必须进行死亡通知。
    //请所有持有hline指针的对象，注册回调函数
    std::vector<std::function<void(void)>> onDelete_Functions;

    AnchorObj_VLine *leftLine = 0, *rightLine = 0; //左右标线
    //const Page *page = 0; //记录所属页面
    //static inline QHash<QString,AnchorObj_HLine*> hash_hline; [2025/8/1 删除，移动到HorLine_Base]
    static inline int s_hline_count = 0; //hline计数
};

class FitLine_for_AnchorObj_HLine : public AnchorObj_HLine {
public:
    FitLine_for_AnchorObj_HLine() {
        topMargin = 0;
    }
    HorLine_Base* lastLine = 0;
    HorLine_Base* nextLine = 0;
    HorLine_Base* getNextLine() const noexcept override {
        return nextLine;
    }
    HorLine_Base* getPrevLine() const noexcept override {
        return lastLine;
    }
    void setNextLine(HorLine_Base* l) noexcept override {
        nextLine = l;
    }
    void setPrevLine(HorLine_Base* l) noexcept override {
        lastLine = l;
    }
    void dealLayout() override;
    virtual QString __dstr() const noexcept override {
        return QString("Fit_HLine id=%1 name=%2").arg(id).arg(name);
    }
};

#endif // ANCHOROBJ_HLINE_H
