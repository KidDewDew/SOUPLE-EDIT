#ifndef OBJ_H
#define OBJ_H

#include <QQuickItem>
#include <fpdf_doc.h>
#include <fpdf_text.h>
#include <fpdf_edit.h>
#include <QPainter>
#include "helper.h"
#include "DecorationInterface.h"
#include "ExportHeader.h"
#include <any>
#include "serialization.h"

#define DEBUG

//共有信息，类型相关
struct Obj_Global_Info {
    bool dealLayoutable = false; //dealLayout()函数是否包含内容
    bool allowSelect = true,allowPartSelect = false; //是否允许选择；是否允许部分选择
    bool isTransparent = false;
    struct  {
        bool isSelfWidth = true; //是否自己决定自己的宽度，而不是由别的因素决定（典型如右占位符，其isSelfWidth = false)
        bool isRealHeight = true;
        bool impact_hscale = true; //是否影响水平标线的水平放缩
        bool kill_hscale = false; //是否破坏水平放缩
    }  anchorInfo;
    bool isHelperLine = false;
};


// 注意，selfDeal_input=true的对象不仅得自己处理键盘事件，还得自己绘制光标
struct Obj_KeyEvent_Info {
    bool selfDeal_backspace; //是否自己处理退格
    bool selfDeal_input;     //是否自己处理输入(除退格外的按键)
};


// Obj是所有Souple对象的数据端(后端)的基类
// 负责存储对象数据、定义对象行为以及管理ui对象
/** 关于4个有些相似的函数：qmlGetData getData selectionGetData
 *  qmlGetData 一般用于qml对象向c++端获取数据。返回QVariant
 *  getData 一般用于c++对象之间请求数据。返回std::optional<int>
 *  对于 getData，如果需要传递超出int表达范围的数据，可以使用Helper提供的临时数据存取功能。或直接改用qmlGetData
 *  selectionGetData 用于SelectionManager请求选择属性数据
 *  getAnyData 用于c++对象间请求数据，用于“很少”发生的请求。它的功能最强。
 * */



/**
 * 另外，dealCommandFromQmlItem函数不仅可以处理qml对象的命令，c++对象也可以相互调用。
 */
class Obj
{
//public:
    //static constexpr inline int C_MAX_Z = (int)Helper::Layer_Z::Top; //最大z坐标,绝对置顶
    //static constexpr inline int C_COVER_Z = (int)Helper::Layer_Z::Cover; //遮盖文本层
    //static constexpr inline int C_TEXT_Z = (int)Helper::Layer_Z::Text; //文本层默认z坐标
    //static constexpr inline int C_MIN_Z = (int)Helper::Layer_Z::Bottom;      //默认最小z坐标，最底部
public:
    Obj();

    /**
     * @brief generateQmlItem 创建qml对象
     * @return 创建好的qml对象，可以为无效值
     * 不同的对象可以自定义如何创建qml对象，
     * 通常的两种方法是：1.使用UIItemPool(ui组件池)来获取对象。2.调用qml generateObj创建对象
     * 对于显示数量较多的对象，建议使用方法1，以减少qml对象创建的开销。
     *
     * 参加discardQmlItem()回收qml对象
     */
    virtual QQuickItem* generateQmlItem() = 0;

    virtual void updateDataToQmlItem(QQuickItem* item) {
        item->setProperty("data_id",id);
        item->setPosition({x,y});
        //item->setX(x); item->setY(y);
        item->setSize({width,height});
        item->setZ((qreal)z);
    };

    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) { return 0; };

    virtual std::optional<int> getData(int dataName) {
        return {};
    }

    virtual std::any getAnyData(std::string key) noexcept {
        return {};
    }

    virtual QVariant qmlGetData(int dataName) {
        if(dataName == Helper::X) return x;
        if(dataName == Helper::Y) return y;
        if(dataName == Helper::WIDTH) return width;
        if(dataName == Helper::HEIGHT) return height;
        if(dataName == Helper::Z) return z;
        return QVariant{};
    }

    /**
     * @brief dealLayout 对象处理布局
     * dealLayout仅处理数据层面，不会使ui更新。[2025/12/2 added]
     */
    virtual void dealLayout() {}

    // [2025/12/2 added]
    // 把dealLayout和dealUI区分。
    // dealUI将导致ui更新，因此需注意调用时机。
    virtual void dealUI() noexcept {
        if(Helper::isQmlItemValid(qmlItem)) {
            qmlItem->setPosition({x,y});
            qmlItem->setSize({width,height});
            if(z != qmlItem->z()) qmlItem->setZ(z);
        }
    }

    /**
     * @brief selectionCommand
     *   本对象作为本选中的内容的一部分，接受并处理用户对选中内容的操作命令。
     * @param command  命令名称
     * @param arg  命令参数
     */
    virtual void selectionCommand(int command,const QVariant& arg) {}


    /**
     * @brief selectionGetData 获取选中内容的统计数据
     * @param dataName: 要获取的数据名称
     * @return 返回空或具体数据，返回空表示本对象不含有此种数据。
     */
    virtual QVariant selectionGetData(int dataName) {
        return {};
    }

    /**
     * @brief contentLength
     * @return 返回对象的内容长度。对于一体化的对象，通常长度为1。
     */
    virtual int contentLength() const noexcept {
        return 1;
    }

    /**
     * @brief positionToIndex
     * 根据x坐标返回对应的内容索引区间，注：x区间即便只覆盖了某索引内容的一部分，也算被选中
     * @param x1: 左边x
     * @param x2: 右边x
     * @param begin_index: 起始索引
     * @param end_index: 结束索引(含)
     */
    virtual void positionToIndex(float x1,float x2,int& begin_index,int& end_index) noexcept
    {
        begin_index = end_index = 0;
    }

    //根据内容索引返回该段内容的起始X坐标(，相对于本对象的x坐标)。
    virtual float x_ofIndex(int index) {
        return index == 0 ? 0.0f : width; //对于内容长度为1者，即可这样计算
    }

    /**
     * @brief removeSelf
     *    从布局中移除自身
     * @param dead  是否删除自身
     *    removeSelf能保证使本对象离开布局，并重建布局。
     *    因此，移除对象的最佳方法就是使用该函数。
     */
    virtual void removeSelf(bool dead = true) {
        if(dead) dead_sign /*= hidden_sign*/ = true;
    }


    // 获取x+width的值。
    virtual float getRightX() const { return x + width; }

    /**
     * @brief showHScale
     * @param hscale  水平缩放值
     * @param addX  平移量
     * @param justQueryAddWidth  是否只是获取addWidth，而不真正改变对象。
     * @return 返回addWidth，即本对象应用了hscale后增加的宽度。
     *         默认返回 0，即对象不受hscale影响。
     * 凡是想要实现行拉伸的对象，应重载该方法。
     */
    virtual float showHScale(float hscale,float addX,bool justQueryAddWidth) { return 0.0; }

    // 该对象是否还可以继续切分
    virtual bool canSplit() const noexcept {
        return false;
    }

    // 所有 update?? 函数：修改某个值，并同步通知前端对象。
    // 可以按需要在此或派生类处添加。

    void updateX(float x) noexcept {
        if(qAbs(this->x - x) < 1e-2) return;
        this->x = x;
        if(Helper::isQmlItemValid(qmlItem)) qmlItem->setX(x);
    }

    void updateY(float y) noexcept {
        if(qAbs(this->y - y) < 1e-2) return;
        this->y = y;
        if(Helper::isQmlItemValid(qmlItem)) qmlItem->setY(y);
    }

    void updateWidth(float width) noexcept {
        if(qAbs(this->width - width) < 1e-2) return;
        this->width = width;
        if(Helper::isQmlItemValid(qmlItem)) qmlItem->setWidth(width);
    }

    void updateHeight(float height) noexcept {
        if(qAbs(this->height - height) < 1e-2) return;
        this->height = height;
        if(Helper::isQmlItemValid(qmlItem)) qmlItem->setHeight(height);
    }

    //void setHidden(bool hidden) noexcept {
    //    this->hidden_sign = hidden;
    //}

    // 该函数指明一个对象如何“丢弃”qml对象
    // 典型的，如果使用了UIItemPool，在这里回收qml对象
    // 不使用UIItemPool的，比如qml对象实例预期很少的，可以直接删除qml对象
    virtual void discard_qmlItem() {}

    //注册死亡时的回调函数，当然该函数可以留空。
    //notice,如果被调用者未实现该函数，将抛出RuntimeError.
    virtual void register_deleteCallback(std::function<void(void)> callback) {
        throw std::runtime_error("Call register_deleteCallback But Not Resolved.");
    }

    //写入pdf page(pdfium版本的绘制函数)
    virtual void writeToPDFPage(FPDF_DOCUMENT document,FPDF_PAGE pdf_page, const Page* page){};

    virtual void qt_paint(QPainter& painter,Page* page) {}; //qt版本的绘制函数

    /**
     * @brief objInfo 获取一个Obj对象的类型相关的info，
     *         显然，该函数利用多态性实现。返回一个静态常引用，避免了或许额外的开销。
     * @return
     */
    virtual const Obj_Global_Info& objInfo() const noexcept {
        static Obj_Global_Info gi = {.dealLayoutable = false};
        return gi;
    }

    virtual const Obj_KeyEvent_Info& keyInfo() const noexcept {
        static Obj_KeyEvent_Info ki =
            {.selfDeal_backspace=false,.selfDeal_input=false};
        return ki;
    }

    template<typename T> //多态转换
    requires std::is_pointer_v<T>
    inline T as() noexcept { return dynamic_cast<T>(this); }

    template<typename T> //多态转换(const)
    requires std::is_pointer_v<T>
    inline auto as() const noexcept
    { return dynamic_cast<
        std::add_pointer_t<
            std::add_const_t<std::remove_pointer_t<T>>
        >
     >(this); }

    template<typename T> //静态转换
        requires std::is_pointer_v<T>
    inline T be() noexcept { return static_cast<T>(this); }

    template<typename T> //静态转换(const)
    inline auto be() const noexcept
    requires std::is_pointer_v<T>
    { return static_cast<
            std::add_pointer_t<
                std::add_const_t<std::remove_pointer_t<T>>
                >
            >(this); }

    template<class T>
    bool canBe() noexcept{
        return bool(dynamic_cast<T*>(this));
    }

    template<class T>
    bool exactlyBe() noexcept{
        return bool(typeid(*this) == typeid(T));
    }

    // 序列化
    // 向bytes从at处写入，并更新at
    // n为目前bytes剩余的空间大小
    // 返回：true:成功 false:需要更多空间来存储
    //virtual bool serialize(char* bytes,int rest_length,int& at) {
    //    return true;
    //}

    // 序列化函数
    // 该函数模仿boost库的序列化设计。具体请查看serialization.h
    template<typename Serial>
    void serialize(Serial& serial) {
        serial /SOUPLE_PP(id)
            /SOUPLE_PP(x)
            /SOUPLE_PP(y)
            /SOUPLE_PP(z);
    }

    // 反序列化
    //virtual Obj* unserialize(const char* bytes,int& at) { return 0; }

    // 序列化类型表
    // enum {
    //     Serialize_AnchorObj_FlowText = 1,
    //     Serialize_AnchorObj_Glue = 2,
    //     Serialize_AnchorObj_HLine = 3,
    //     Serialize_AnchorObj_Image = 4,
    //     Serialize_AnchorObj_JZRect = 5,
    //     Serialize_AnchorObj_LatexFormula = 6,
    //     Serialize_AnchorObj_Path = 7,
    //     Serialize_AnchorObj_PHLeft = 8,
    //     Serialize_AnchorObj_PHRect = 9,
    //     Serialize_AnchorObj_PHRight= 10,
    //     Serialize_AnchorObj_Rich = 11,
    //     Serialize_AnchorObj_VLine = 12,
    //     Serialize_BlockInner_HorLine = 13,
    //     Serialize_Frame_ofHLines = 14,
    //     Serialize_Free_Image = 15,
    //     Serialize_Free_Path = 16,
    //     Serialize_Free_TableUnit = 17,
    //     Serialize_Free_Text = 18,
    //     Serialize_HorLine_Base = 19,
    //     Serialize_Obj_Page = 20,
    //     Serialize_TableLine = 21,
    //     Serialize_WordPage_VLine = 22
    // };

    // 反序列化: 从bytes[at]处读取一个Obj，并更新at
    //static inline Obj* unserialize_all(const char* bytes,int& at);

    // 返回一个debug字符串
    virtual QString __dstr() const noexcept {
        return QString("Obj id=%1").arg(id);
    }

    virtual ~Obj();
public:

    //注意，如果dead_sign=true则hidden_sign一定=true
    bool dead_sign = false; //死亡标志(置位后，由SoupleManager负责删除obj)
    //bool hidden_sign = false; //隐藏标志(置位后，对象将被移除出文档流，但不被删除)
    float x=0,y=-1000,width=0,height=0;
#ifdef ULONGLONG_OBJID
    uint64_t id;
#else
    uint32_t id;  //唯一标识符，id
#endif
    int z {Helper::Layer_Z::Text};        //渲染层级
    QQuickItem* qmlItem = 0;
    static inline uint32_t s_all_id = 0; //记录下一个obj的id
protected:
    //赋予矩阵变换到pdf obj
    void endowMatrixToPDFObj(FPDF_PAGEOBJECT obj,const Page* page) {
        FS_MATRIX mat{.a=1.0,.b=0.0,.c=0.0,.d=1.0,.e=(float)Helper::pixel2point(x),
                      .f=(float)Helper::pixel2point(page->height - y + page->top_y)};
        FPDFPageObj_SetMatrix(obj,&mat);
    }
private:
    //std::list<Obj*>::iterator iter_in_y_sort_objs;
};

#endif // OBJ_H
