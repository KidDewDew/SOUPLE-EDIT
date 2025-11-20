#ifndef ANCHOROBJ_LATEXFORMULA_H
#define ANCHOROBJ_LATEXFORMULA_H

#include "anchorobj.h"
#include "uiitempool.h"

// AnchorObj_LatexFormula
// Latex公式锚定类组件
// 导出的公式图片命名：cache/
class AnchorObj_LatexFormula: public AnchorObj
{
public:
    AnchorObj_LatexFormula();
    virtual QQuickItem* generateQmlItem() override;
    virtual void updateDataToQmlItem(QQuickItem*) override;
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override;
    virtual AnchorObj* dropRight(float dropWidth) override; //尝试截断并丢弃右边
    virtual AnchorObj* dropLeft(float dropWidth) override;  //尝试截断并丢弃左边
    virtual QVariant qmlGetData(int dataName) override {
        if(dataName == Helper::TEXT) {
            return latex_code;
        } else if(dataName == Helper::FONT_SIZE) {
            return font_size;
        } else if(dataName == Helper::SOURCE) {
            return getImagePath();
        }
        return AnchorObj::qmlGetData(dataName);
    }
    virtual const Obj_Global_Info& objInfo() const noexcept override {
        static Obj_Global_Info gi = {.dealLayoutable = false,.allowSelect = true,.allowPartSelect = false,
                                     .anchorInfo = {.isSelfWidth = true,
                                                    .isRealHeight = true,
                                                    .impact_hscale = false}};
        return gi;
    }
    virtual void discard_qmlItem() override {
        uiPool_Latex::returnItem(qmlItem);
        qmlItem = 0;
    }
    QString getImagePath() const noexcept {
        QString ipath = QString("cache/pdf_image/latex_%1.png").arg(image_id);
        if(QFile(ipath).exists())
            return "file:"+ipath;
        else return "qrc:/image/icon_Image.png";
    }
    //void qt_paint(QPainter& painter,Page* page) override;
private:
    static constexpr char __UINAME__[] = "Latex";
public:
    typedef UIItemPool<__UINAME__,1> uiPool_Latex; //ui控件池
protected:
    int image_id;
    float font_size;
    QString latex_code; //latex代码
};

SOUPLE_REGISTER_CLASS(AnchorObj_LatexFormula,DATE_ID(202511202140))

#endif // ANCHOROBJ_LATEXFORMULA_H
