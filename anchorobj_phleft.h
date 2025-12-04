#ifndef ANCHOROBJ_PHLEFT_H
#define ANCHOROBJ_PHLEFT_H

#include "anchorobj.h"
#include "uiitempool.h"
#include "LiveValue.h"

class AnchorObj_PHLeft;

// 段落数据
class ParaInf {
    friend class AnchorObj_PHLeft;
public:
    bool multi_etc_tab;
    AnchorObj_PHLeft* phleft; //段落开始符
    float first_tab; //第一行缩进
    float etc_tab;   //其余行缩进
private:
    ParaInf(){}
};

//左占位，保证左边没有任何锚定对象。
//若其左边有对象，则创建中间行，并把左边对象放到中间行；若缺少换行，还会补一个换行符哦。
//仅当(逻辑)上一行为空行时收缩，收缩方式为直接删除上一空行。
//综上，左占位用于维护“段落”布局。
class AnchorObj_PHLeft : public AnchorObj
{
public:
    AnchorObj_PHLeft();
    virtual QQuickItem* generateQmlItem() override;
    virtual void dealLayout() override;
    virtual AnchorObj* dropRight(float dropWidth) override; //尝试截断并丢弃右边
    virtual AnchorObj* dropLeft(float dropWidth) override;  //尝试截断并丢弃左边
    virtual void updateDataToQmlItem(QQuickItem* item) override;
    // virtual const AnchorObj_Global_Info& global_info() const override {
    //     static AnchorObj_Global_Info _global_info = {.isSelfWidth = false,.isRealHeight=false};
    //     return _global_info;
    // }

    virtual QVariant qmlGetData(int dataName) override {
        switch(dataName) {
        //case Helper::ETC_TAB:
        //    return getPra
        default:
            return AnchorObj::qmlGetData(dataName);
        }
    }

    void doPHLeft() noexcept;

    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override;

    virtual const Obj_Global_Info& objInfo() const noexcept override {
        static Obj_Global_Info gi = {.dealLayoutable = false,.allowSelect = true,.allowPartSelect = false,
                                     .isTransparent = true,
                                     .anchorInfo = {.isSelfWidth = true,.isRealHeight = false,
                                                    .impact_hscale = false}};
        return gi;
    }

    virtual void discard_qmlItem() override {
        uiPool_PHLeft::returnItem(qmlItem);
        showLevel.removeListener(this);
        qmlItem = 0;
    }

    ParaInf getParaInf() noexcept;

    // [static] 获取hline所属的段落的信息。返回{}表示从文档开头到此行没有段落被标记。
    // [!notice]该方法返回的ParaInf中的etc_tab不一定等于段落的真实etc_tab。
    static std::optional<ParaInf> findBelongPara(HorLine_Base* hline);
    //static

    template<typename Serial>
    void serialize(Serial& serial) {
        serial / SOUPLE_PP(id)
            / SOUPLE_PP(level)
            / SOUPLE_PK("first_tab",width);
    }

public:
    int level; //段落级别 0代表尚未有级别。
private:
    static constexpr char __UINAME__[] = "PH_Left";
public:
    typedef UIItemPool<__UINAME__,20> uiPool_PHLeft; //ui控件池
    static inline LiveValue<bool> showLevel = true; //是否显示段落级别
};

SOUPLE_REGISTER_CLASS(AnchorObj_PHLeft,DATE_ID(202511202144))

#endif // ANCHOROBJ_PHLEFT_H
