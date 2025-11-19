#ifndef BLOCKINNER_HORLINE_H
#define BLOCKINNER_HORLINE_H

#include "horline_base.h"

// BlockInner_HorLine: 块内部水平标线
// 该水平标线继承自HorLine_Base
// 功能上与AnchorObj_HLine类似，但它默认由Free_Rich(或其他)来设置x、width，且不处理上标线跨页的情况。
class BlockInner_HorLine : public HorLine_Base
{
    friend class Free_Rich;
    friend class Pdf2Souple;
public:
    BlockInner_HorLine();
    BlockInner_HorLine *getLastLine() const noexcept {
        return hline->as<BlockInner_HorLine*>();
    }
    virtual void dealLayout() override;
    virtual QQuickItem* generateQmlItem() override;
    virtual void updateDataToQmlItem(QQuickItem*) override;
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override;
    virtual AnchorObj *getLastObj() const override {
        if(hline) return hline->rightObj;
        return 0;
    }
    virtual void discard_qmlItem() override {
        uiPool_ILine::returnItem(qmlItem);
        qmlItem = 0;
    }
    virtual const Obj_Global_Info& objInfo() const noexcept override {
        //该标线由父容器来调用dealLayout,不需要SoupleManager来处理。
        static Obj_Global_Info gi = {.dealLayoutable = false};
        return gi;
    }
    virtual HorLine_Base *getNextLine() const noexcept override {
        return nextLine;
    }
    virtual void setNextLine(HorLine_Base* nextLine) noexcept override {
        this->nextLine = nextLine;
    }
    virtual void setPrevLine(HorLine_Base* prevLine) noexcept override;
    virtual QString __dstr() const noexcept override {
        return QString("BlockInner_HLine %1").arg(id);
    }
    //virtual HorLine_Base *getPrevLine() const noexcept override {
    //    return 0;
    //}
    virtual float getTopMargin() const override { return topMargin; }
    Obj* getParent() noexcept {
        return parent;
    }

private:
    static inline constexpr char __UINAME__[] = "ILine";
protected:
    //bool needNewLine = false; //是否需要创建新行
    float topMargin = 0; //上边距
    Obj *parent = 0;    //父容器对象
public:
    typedef UIItemPool<__UINAME__,20> uiPool_ILine; //ui控件池
protected:
    //BlockInner_HorLine *lastLine = 0; 注: lastLine引用自AnchorObj::hline
    HorLine_Base *nextLine = 0;
};

#endif // BLOCKINNER_HORLINE_H
