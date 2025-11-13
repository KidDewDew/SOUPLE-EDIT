#ifndef ANCHOROBJ_VLINE_H
#define ANCHOROBJ_VLINE_H

#include "anchorobj.h"
#include "uiitempool.h"

class AnchorObj_HLine;

//垂直标线数据管理类
class AnchorObj_VLine : public AnchorObj
{
public:
    friend class AnchorObj_HLine;
    friend class FreeObj;
    AnchorObj_VLine();
    virtual QQuickItem* generateQmlItem() override; //创建用于ui的qml元素
    virtual void updateDataToQmlItem(QQuickItem*) override;    //更新数据到qml元素
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override;  //处理来自对应qml item的指令
    virtual void notifyHLineYChanged(AnchorObj_HLine* hline); //当依附于该对象的hline的y坐标变化时，调用该函数
    const QString& getName() const { return name; }
    virtual QVariant qmlGetData(int dataName) override {
        if(dataName == Helper::NAME) return name;
        return AnchorObj::qmlGetData(dataName);
    }
    virtual void discard_qmlItem() override {
        uiPool_VLine::returnItem(qmlItem);
        qmlItem = 0;
    }
    virtual bool isWordPageLine() const noexcept {
        return false;
    }
    virtual int getColumn() const noexcept {
        return 0;
    }
    virtual const Obj_Global_Info& objInfo() const noexcept override {
        static Obj_Global_Info gi = {.isHelperLine = true};
        return gi;
    }
protected:
    QString name;
    static inline QHash<QString,AnchorObj_VLine*> hash_vline;
    static inline int s_vline_count = 0;
private:
    static constexpr char __UINAME__[] = "VLine";
public:
    typedef UIItemPool<__UINAME__,20> uiPool_VLine; //ui控件池
};

#endif // ANCHOROBJ_VLINE_H
