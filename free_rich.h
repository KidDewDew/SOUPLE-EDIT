#ifndef FREE_RICH_H
#define FREE_RICH_H
#include "freeobj.h"
#include "blockinner_horline.h"
#include "free_frame.h"
#include "anchorobj_vline.h"

//富文本
//~ BlockInner_HorLine的容器类
//该对象可以统一为内部HLine设置一些属性(如居中对齐等...)

//getAnyData提供key:  vline  返回pair<vline_left,vline_right>

//[2025/11/4：新增对AnchorObj_HLine及其派生类的兼容！]
class Free_Rich : public FreeObj, public Single_Signal_Emitter
{
    friend class Pdf2Souple;
public:

    enum {
        Has_Border=0,      //是否有边框
        Has_Background=1,  //是否填充背景
        Prefer_Extend_Width=2 //自动调整时要优先调整宽度么？
    };

    Free_Rich();

    bool getSign(uint8_t sign_id) const {
        return sign&(1<<sign_id);
    }

    void setSign(uint8_t sign_id,bool value) {
        if(value) sign |= (1<<sign_id);
        else sign &= ~(1<<sign_id);
    }

    virtual std::any getAnyData(std::string key) noexcept override {
        if(key == "vline") {
            createVLine_IfNotCreated();
            return std::pair(vline_left,vline_right);
        } else if(key == "lastline") {
            return findLastLine();
        }
        return {};
    }

    virtual void dealLayout() override;

    virtual QQuickItem* generateQmlItem() override {
        return uiPool_FRich::fetchItem();
    }

    //virtual void updateDataToQmlItem(QQuickItem*) override;
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override;

    virtual std::optional<int> getData(int dataName) override {
        if(dataName == Helper::NEED_SHOW)
            return (int)(Helper::isQmlItemValid(qmlItem) && qmlItem->property("iscont") == true);
        return {};
    }

    virtual void updateDataToQmlItem(QQuickItem* item) override {
        item->setProperty("drawBorder",getSign(Has_Border));
        item->setProperty("hasBg",bool(bg != nullptr));
        FreeObj::updateDataToQmlItem(item);
    }

    virtual QVariant qmlGetData(int dataName) override {
        if(dataName == Helper::NEED_SHOW) {
            return qmlItem && qmlItem->property("iscont") == true;
        }
        return FreeObj::qmlGetData(dataName);
    }

    void qt_paint(QPainter& painter,Page* page) override {}

    virtual void discard_qmlItem() override {
        uiPool_FRich::returnItem(qmlItem);
        qmlItem = 0;
    }

    //尝试创建vline
    void createVLine_IfNotCreated() {
        if(vline_left) return;
        vline_left = new AnchorObj_VLine;
        vline_right = new AnchorObj_VLine;
        //无需注册...
    }

protected:
    HorLine_Base *findLastLine() noexcept {
        if(! firstLine) return 0;
        HorLine_Base *line = firstLine;
        while(1) {
            auto nl = line->getNextLine();
            if(!nl) return line;
            line = nl;
        }
    }
protected:
    unsigned char vAlignMode = Helper::AlignVCenter; //默认垂直方向中心对齐
                    //注意：除了AlignVTop，其他对齐方式都会忽略firstLine的topMargin。
                    //free_rich无权限制各个标线的水平对齐方式。统一修改是可以的，但不能限制。
    float contentHeight = 0;
    HorLine_Base *firstLine = 0; //第一条HLine
    Free_Frame *bg = 0; //背景对象，初始为NULL
    //为了兼容AnchorObj_HLine，添加两个vline对象
    //注：当需要时才会创建；创建也不会注册对象，自然不会显示qml对象。
    AnchorObj_VLine *vline_left=0,*vline_right=0;
private:
    uint8_t sign = "00000001"_8bits;
    static constexpr char __UINAME__[] = "FRich";
public:
    typedef UIItemPool<__UINAME__,2> uiPool_FRich; //ui控件池
};

#endif // FREE_RICH_H
