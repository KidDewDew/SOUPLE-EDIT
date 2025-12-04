#ifndef ANCHOROBJ_FLOWTEXT_H
#define ANCHOROBJ_FLOWTEXT_H

#include "anchorobj.h"
#include <QFontMetrics>
#include "uiitempool.h"

class _NavNode;

//流文本数据类
class AnchorObj_FlowText : public AnchorObj
{
    friend class Pdf2Souple;
    friend void impl_dfs_createNavLines(_NavNode* node);
public:
    AnchorObj_FlowText();
    virtual QQuickItem* generateQmlItem() override;
    virtual void updateDataToQmlItem(QQuickItem*) override;
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override;
    virtual void dealLayout() override;
    virtual AnchorObj* dropRight(float dropWidth) override; //尝试截断并丢弃右边
    virtual AnchorObj* dropLeft(float dropWidth) override;  //尝试截断并丢弃左边
    virtual QVariant qmlGetData(int dataName) override {
        if(dataName == Helper::TEXT) return text;
        if(dataName == Helper::VALIGNOFFSET) return vAlignOffset;
        return AnchorObj::qmlGetData(dataName);
    }
    virtual bool canSplit() const noexcept override {
        return text.length() > 1;
    }
    virtual float showHScale(float hscale,float addx,bool justQueryAddWidth) override;
    virtual void getCursorFromRight(int RN = 0) override;
    virtual void getCursorFromLeft(int RN = 0) override;
    virtual bool tryMergeRight() override; //尝试合并右边
    virtual void selectionCommand(int command,const QVariant& arg)
        override;
    virtual int contentLength() const noexcept override {
        return text.length();
    }
    virtual AnchorObj* slice(int start_i,int max_len) override;
    virtual QVariant selectionGetData(int dataName) override {
        switch(dataName) {
        case Helper::SP_Family: return font.family(); break;
        case Helper::SP_FontSize: return font.pointSizeF(); break;
        case Helper::SP_TextEnableStroke: return isStroke; break;
        case Helper::SP_TextStrokeWidth: return isStroke ? strokeWidth : QVariant{}; break;
        case Helper::SP_TextStrokeColor: return isStroke ? stroke_color : QVariant{}; break;
        case Helper::SP_TextColor: return isFill ? fill_color : QVariant{}; break;
        case Helper::SP_vTextAlignMode: return vAlignMode;
        }
        return {};
    }

    virtual std::any getAnyData(std::string key) noexcept override {
        if(key == "text") {
            return text;
        }
        return {};
    }

    virtual void positionToIndex(float x1,float x2,int& begin_index,int& end_index)
        noexcept override
    {
        QFontMetricsF m(font);
        x1 -= x; x2 -= x;
        int i = 0; float px = 0;
        float cw;
        begin_index = end_index = text.length()-1;
        while(i < text.length()) {
            cw = m.horizontalAdvance(text[i]);
            if(px + cw/2 >= x1) {
                begin_index = i++;
                px += cw;
                break;
            }
            px += cw;
            ++i;
        }
        while(i < text.length()) {
            cw = m.horizontalAdvance(text[i]);
            if(px + cw/2 > x2) {
                end_index = i-1;
                px += cw/2;
                break;
            }
            px += cw;
            ++i;
        }
    }

    // virtual const AnchorObj_Global_Info& global_info() const override {
    //     static AnchorObj_Global_Info _global_info = {.isSelfWidth = true,.isRealHeight=true};
    //     return _global_info;
    // }

    virtual const Obj_Global_Info& objInfo() const noexcept override {
        static Obj_Global_Info gi = {.dealLayoutable = false,.allowSelect = true,.allowPartSelect = true,
                                     .anchorInfo = {.isSelfWidth = true,.isRealHeight = true}};
        return gi;
    }

    virtual const Obj_KeyEvent_Info& keyInfo() const noexcept override {
        static Obj_KeyEvent_Info ki =
            {.selfDeal_backspace=true,.selfDeal_input=true};
        return ki;
    }

    virtual void discard_qmlItem() override {
        //qmlItem->releaseResources();
        uiPool_flowText::returnItem(qmlItem);
        qmlItem = 0;
        //qDebug() << "FlowText(" << text << ") discard_qmlItem";
    }

    virtual void writeToPDFPage(FPDF_DOCUMENT document,FPDF_PAGE pdf_page, const Page* page) override;
    virtual void qt_paint(QPainter& painter,Page* page) override;

    virtual QString __dstr() const noexcept override {
        return QString("FlowText \"%1\"").arg(text);
    }

    inline void calcWidth() {
        QFontMetricsF metrics{font};
        width = metrics.horizontalAdvance(text);
    }

    template<typename Serial>
    void serialize(Serial& serial) {
        serial / SOUPLE_PP(id)
            / SOUPLE_PP(text)
            / SOUPLE_PP(font)
            / SOUPLE_PP(isFill)
            / SOUPLE_PP(isStroke)
            / SOUPLE_PP(strokeWidth)
            / SOUPLE_PP(stroke_color)
            / SOUPLE_PP(fill_color);
    }

private:
    static inline constexpr char __UINAME__[] = "FlowText";
private:
    AnchorObj_FlowText* clone();
public:
    typedef UIItemPool<__UINAME__,200> uiPool_flowText; //ui控件池
public:
    bool isFill=true,isStroke=false;
    float strokeWidth = 0.0;
    QString text;
    QFont font;
    QColor stroke_color,fill_color;
public:
};

SOUPLE_REGISTER_CLASS(AnchorObj_FlowText,DATE_ID(202511202136))

#endif // ANCHOROBJ_FLOWTEXT_H
