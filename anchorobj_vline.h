#ifndef ANCHOROBJ_VLINE_H
#define ANCHOROBJ_VLINE_H

#include "anchorobj.h"
#include "uiitempool.h"

class AnchorObj_HLine;
class ColumnsSeparate;

//垂直标线数据管理类
class AnchorObj_VLine : public AnchorObj
{
public:
    friend class Pdf2Souple;
    friend class AnchorObj_HLine;
    friend class FreeObj;
    friend class ColumnsSeparate;
    friend class Page;

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

    // 获取该vline是否是word风格vline.
    bool isWordPageLine() const noexcept {
        return this->isWordLine;
    }

    // virtual int getColumn() const noexcept { [作废]
    //     return 0;
    // }

    int getColumn() const noexcept { //内联函数，代替virutal函数。
        return column_id;
    }

    // 获取顶部的分栏分隔线，注意对于页面topLine，该函数会返回 0 (~nullptr)。
    // 因此，该函数返回0时代表使用页面topLine.
    ColumnsSeparate *getTopColumnsSeparate() noexcept {
        return top_columns_separate;
    }

    // 获取底部的分栏分隔线，注意对于页面bottomLine，该函数会返回 0 (~nullptr)。
    ColumnsSeparate *getBottomColumnsSeparate() noexcept {
        return bottom_columns_separate;
    }

    // 获取下一栏
    const PageColumn& getNextColumn() noexcept {
        if(next_column.leftLine == 0) {
            //没有下一栏了
            createNextColumn();
        }
        return next_column;
    }

    Page* getPage() noexcept {
        return page;
    }

    virtual const Obj_Global_Info& objInfo() const noexcept override {
        static Obj_Global_Info gi = {.isHelperLine = true};
        return gi;
    }
    virtual QString __dstr() const noexcept override {
        return QString("AnchorObj_VLine id=%1 isWordPageLine=%2").arg(id).arg(isWordPageLine());
    }
protected:
    void createNextColumn() noexcept;

protected:
    bool isWordLine;
    QString name;

    /** 任意分栏 实验内容 BEGIN */
    int              column_id = 0;
    Page *           page = 0;
    ColumnsSeparate *top_columns_separate = 0;
    ColumnsSeparate *bottom_columns_separate = 0;
    PageColumn       next_column;
    /** 任意分栏 实验内容 END */

    static inline QHash<QString,AnchorObj_VLine*> hash_vline;
    static inline int s_vline_count = 0;

private:
    static constexpr char __UINAME__[] = "VLine";
public:
    typedef UIItemPool<__UINAME__,20> uiPool_VLine; //ui控件池
};

#endif // ANCHOROBJ_VLINE_H
