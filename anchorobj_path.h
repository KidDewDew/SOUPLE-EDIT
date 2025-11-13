#ifndef ANCHOROBJ_PATH_H
#define ANCHOROBJ_PATH_H

#include "anchorobj.h"
#include "uiitempool.h"
#include "ExportHeader.h"

//路径类
class AnchorObj_Path : public AnchorObj
{
    friend class Pdf2Souple;
public:
    AnchorObj_Path() = default;
    virtual QQuickItem* generateQmlItem() override;
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override;
    virtual AnchorObj* dropRight(float dropWidth) override; //尝试截断并丢弃右边
    virtual AnchorObj* dropLeft(float dropWidth) override;  //尝试截断并丢弃左边
    virtual void updateDataToQmlItem(QQuickItem*) override;

    // virtual const AnchorObj_Global_Info& global_info() const override {
    //     static AnchorObj_Global_Info _global_info = {.isSelfWidth = true,.isRealHeight=true};
    //     return _global_info;
    // }
    virtual const Obj_Global_Info& objInfo() const noexcept override {
        static Obj_Global_Info gi = {.dealLayoutable = false,.allowSelect = true,.allowPartSelect = false,
                                     .anchorInfo = {.isSelfWidth = true,.isRealHeight = true}};
        return gi;
    }
    virtual void discard_qmlItem() override {
        uiPool_Path::returnItem(qmlItem);
        qmlItem = 0;
    }
    virtual void qt_paint(QPainter& painter,Page* page) override;
protected:
    bool isFill = true, isStroke = true;
    QList<Path_Action> path_actions; //路径动作
    QColor fillStyle; //填充模式
    QColor strokeStyle; //描边模式
    float lineWidth;  //描边宽度
private:
    static constexpr char __UINAME__[] = "Path";
public:
    typedef UIItemPool<__UINAME__,150> uiPool_Path; //ui控件池
};

#endif // ANCHOROBJ_PATH_H
