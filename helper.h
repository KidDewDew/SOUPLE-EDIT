#ifndef HELPER_H
#define HELPER_H

#include <QObject>
#include <QJSValue>
#include <QUrl>
#include <QQuickItem>
#include "PlatformMethod.h"
#include "quick_callback.h"
#include <QFile>
#include <exception>
#include <QDir>
#include <any>

#ifdef WIN32
#include <windows.h>
#endif

// 本头文件提供一些辅助设计和Helper类。
// 注意：对这个文件进行修改将导致大量文件(60%+)需要重新编译。

template<typename T,typename U>
concept Iterable = requires(T cont) {   //Concept: 可迭代容器
    {cont.begin()}; {cont.end()};
    {*cont.begin()} -> std::convertible_to<U>;
};

template<typename T,typename U>
concept RandomAccessCont = requires(T cont) {   //Concept: 随机访问容器
    {cont.begin()}; {cont.end()};
    {*cont.begin()} -> std::convertible_to<U>;
    {cont[0]} -> std::convertible_to<U>;
    {cont.size()};
};

template<typename T,typename U>
concept CanPushback = requires(T cont) {   //Concept: 可迭代容器
   {cont.begin()}; {cont.end()};
   {*cont.begin()} -> std::convertible_to<U>;
    {cont.push_back(std::declval<U>())};
};


// 作用域退出触发器
template<typename Func,std::enable_if<std::is_invocable_v<Func>,int>::type = 1>
class __ScopeExitDoClass {
public:
    __ScopeExitDoClass(Func func):func(func) {}
    ~__ScopeExitDoClass() { func(); }
private:
    Func func;
};

// 检查一个qml指针是否有效
#define QML_VALID(obj) (Helper::isQmlItemValid(obj->qmlItem))

#define NEW_VAR(type,var_name,...) type* var_name = new type(__VA_ARGS__)

#define OF(a,b)

#define SYNC_SET(property,qmlProperty,value) \
if(QML_VALID(qmlItem))qmlItem->setProperty(qmlProperty,value);\
this->property = value;

#define SCOPE_EXIT_DO(codes) \
__ScopeExitDoClass{[&](){\
do{\
    codes \
}while(0);\
}\
};

// #define log(a,...) qDebug() __aux_log(__VA_ARGS__) ;

// #define __aux_log(b,...) << b __aux_log(__VA_ARGS__)

#define ENABLE_LOG true

#if ENABLE_LOG == false
#define qDebug _NonDebug
#endif

#define IF if

#define NOT(pred) (!(pred))

#define THEN(codes) {codes}

#define view_suffix(_suffix) std::views::transform([](auto& _a){ return _a##_suffix; })

#define _Pred(condition) [](auto& e1,auto& e2){ return condition; }

#define _Repeat_Header_2(header1,header2,codes) \
header1 \
codes \
header2 \
codes

#define _Comma ,

#define _Header_Class(className,...) class className: __VA_ARGS__

template<class...T>
void log(T&&...args) {
#if ENABLE_LOG == true
    impl_log(std::forward<T>(args)...);
#endif
}

template<class A1,class...T>
void impl_log(A1&& arg1,T&&...args) {
#if ENABLE_LOG == true
    qDebug() << std::forward<A1>(arg1);
    if constexpr(sizeof...(T) > 0)
        impl_log(std::forward<T>(args)...);
#endif
}

class _NonDebug {};

template<class T>
_NonDebug& operator<<(_NonDebug& nd,T&& t) {
    return nd;
}

// y1 < y2
class __BreakVal {
public:
    static inline bool value = false;
    static inline bool old_value;
};

#define Break(val) \
__BreakVal::value = val; break;

#define BreakVal (__BreakVal::old_value=__BreakVal::value,__BreakVal::value=false,__BreakVal::old_value)

#define v_Filter(pred) std::views::filter([](auto& a){ return pred; })


constexpr uint8_t operator""_8bits(const char* str,size_t n) {
    uint8_t value = 0;
    for(size_t i = 0; i < 8; ++i) {
        value = (value<<1) + (str[i]-'0');
    }
    return value;
}

constexpr uint8_t operator""_16bits(const char* str,size_t n) {
    uint16_t value = 0;
    for(size_t i = 0; i < 16; ++i) {
        value = (value<<1) + (str[i]-'0');
    }
    return value;
}

constexpr uint8_t operator""_32bits(const char* str,size_t n) {
    uint32_t value = 0;
    for(size_t i = 0; i < 32; ++i) {
        value = (value<<1) + (str[i]-'0');
    }
    return value;
}

//Helper类，用于提供一些辅助性和交互性方法，如像素和厘米的转换和一些通知等
class Helper : public QObject
{
    Q_OBJECT
public:
    enum Error { Error_Invalid_Command = 1, Error_Repeat, Error_Invalid_Data,
                 Error_Failed, Error_Unknown, Error_Other};
    Q_ENUM( Error )

    enum Layer_Z { Bottom = -1000, Text = 100000, Cover = 500000, Top = 1000000 };

    enum QmlItemStatus {
        NotCreated = 0,
        CreatedButHidden = 1,
        PreVisibleButHidden = 2,
        Valid = 4};

    Q_ENUM( Layer_Z )

    enum Align {
        AlignVCenter = 1,AlignVBaseLine,AlignTop,AlignBottom,
        AlignHCenter,AlignLeft,AlignRight
    };
    enum DataName {X,Y,Z,WIDTH,HEIGHT,TEXT,VALIGNOFFSET,NAME,LEFTLINE,
                    RIGHTLINE,DRAGY0,DRAGY1,
                    ANCHOR_LASTHLINE,LOGIC_LASTHLINE,LOGIC_NEXTHLINE,
                    TOP_MARGIN,SOURCE,TOPLINE,
                    BOTTOMLINE,BOTTOM_MARGIN,LEFT_MARGIN,
                    RIGHT_MARGIN,MAX_DRAG_X,NEED_SHOW,
                    TABLE_ALIGNMODE,FONT_SIZE,FONT_COLOR,
                    FILL_MODE,LINE_WIDTH,RADIUS,
                    RADIUS_1,RADIUS_2,RADIUS_3,RADIUS_4,STROKE_COLOR,BG_COLOR,
                    STROKE_WIDTH,LEVEL,SHOW_LEVEL,
                    FIRSTLINE_TAB,ETC_TAB};
    enum Command { X_UP,Y_UP,WIDTH_UP,HEIGHT_UP,Z_UP,TEXT_UP,
                   FONT_SIZE_UP,FONT_ITALIC_UP,
                   FONT_BOLD_UP,GOTOLEFT,KEY_RETURN,FONT_FAMILY_UP,
                   COLOR_UP,STROKE_COLOR_UP,
                   VALIGNOFFSET_UP,BOOL_STROKE_UP,INSERTOBJ,
                   STROKE_WIDTH_UP,ANCHOR_LASTHLINE_UP,
                   LOGIC_LASTHLINE_UP,TOP_MARGIN_UP,LEFTLINE_UP,
                   RIGHTLINE_UP,NAME_UP,SOURCE_UP,
                   TOPLINE_UP,BOTTOMLINE_UP,BOTTOM_MARGIN_UP,LEFT_MARGIN_UP,
                   RIGHT_MARGIN_UP,ADDLINE,STHCENTER,STVCENTER,FOCUS_CHANGED,
                   CONT_CHANGED,TABLE_ALIGNMODE_UP,TABLE_MERGE_UNIT,TABLE_DELETE_ROW,
                   TABLE_DELETE_COLUMN,TABLE_SPLIT_UNIT,
                   KEY_PRESSED,KEY_BACKSPACE_PRESSED,Request_Sync,KEY_DOWN,
                   BG_COLOR_UP,FILL_MODE_UP,RADIUS_UP,
                   RADIUS_1_UP,RADIUS_2_UP,RADIUS_3_UP,RADIUS_4_UP,
                   CREATE,SET_FOCUS,LEVEL_UP,SHOW_LEVEL_UP,
                   FIRSTLINE_TAB_UP,ETC_TAB_UP,CLICKED,
                   CURSOR_CHANGE};
    enum SelectionCommand {
        SP_Family_Set,
        SP_FontSize_Add,SP_FontSize_Set,
        SP_FontBold_Set,SP_FontItalic_Set,
        SP_FontEnableStroke_Set,
        SP_FontStrokeColor_Set,
        SP_Color_Set,SP_FontStrokeWidth_Add,
        SP_FontStrokeWidth_Set,SP_UnderLine_Set
    };

    enum ObjType {
        AnchorType,FreeType
    };

    enum SomeMore { Dir_LEFT=1,Dir_RIGHT=2,Dir_TOP=4,Dir_BOTTOM=8,Dir_SKEW = 16 };

    enum SelectionProperty {
        SP_FontSize,SP_FontBold,SP_FontItalic,
        SP_TextEnableStroke,SP_TextStrokeColor,
        SP_TextStrokeWidth,SP_UnderLine,SP_TextColor,
        SP_PathColor,SP_TextEnableFill,
        SP_vTextAlignMode,SP_vTextAlignOffset,
        SP_Family
    };

    enum PageType {
        NoFormat_Page,Word_Page
    };

    enum FillMode {
        No_Fill,
        Color_Fill,
        Gradient_Fill,
        Image_Fill,
        Pattern_Fill,
        //for image
        Repeat_xy,
        No_Repeat
    };

    enum LinePattern {
        None_LinePattern,
        Solid_LinePattern,
        Dash_LinePattern,
        Dot_LinePattern
    };

    Q_ENUM( DataName )
    Q_ENUM( Command )
    Q_ENUM( SomeMore )
    Q_ENUM( SelectionProperty )
    Q_ENUM( SelectionCommand )
    Q_ENUM( Align )
    Q_ENUM( PageType )
    Q_ENUM( FillMode )
    Q_ENUM( LinePattern )

    explicit Helper(QObject *parent = nullptr);

    static void init(); //初始化

    static inline bool isQmlItemValid(QQuickItem* item) {
        return (uintptr_t)item >= Helper::QmlItemStatus::Valid;
    }


    static inline double getDPI() {
        return dpi;
    }


    Q_INVOKABLE static inline qreal pixel2cm(qreal val) //像素 转 厘米
    {
        return val / (dpi / 2.54);
    }

    Q_INVOKABLE static inline qreal cm2pixel(qreal val) //厘米 转 像素
    {
        return val * (dpi / 2.54);
    }

    Q_INVOKABLE static inline qreal point2pixel(qreal val) //point 转 像素
    {
        return val/72.0*dpi;
    }

    Q_INVOKABLE static inline qreal pixel2point(qreal val) //像素 转 point
    {
        return val*72.0/dpi;
    }

    Q_INVOKABLE static QString qurl2localfile(QUrl url) {
        return url.toLocalFile();
    }

    Q_INVOKABLE static void requestLoadSoup(const QString& filename);//请求加载.soup文件

    Q_INVOKABLE static void requestLoadPdf(const QString& filename,const QVariantMap &options = {});//请求加载.pdf文件

    // requestPreLoadPdf 请求预加载PDF文件(此时UI方面用户正在选择PDF读取的页面页脚等)
    // 该函数将异步加载PDF的内嵌字体等
    Q_INVOKABLE static void requestPreLoadPdf(const QString& filename,QJSValue callback);

    // qml调用该函数来渲染latex公式
    Q_INVOKABLE static QString requestRenderLatexFormula(const QString& latex,float fontSize,
                                                      const QString& image_save_path);

    Q_INVOKABLE static void trimCache() {
        emit helper->requestTrimCache();
    }

    Q_INVOKABLE static inline bool isChineseChar(QChar c) {
        ushort u = c.unicode();
        return u>=0x4E00 && u<=0x9FA5;
    }

    //移动端选择文件
    Q_INVOKABLE static void mobile_selectFile(QJSValue callback) {
        PlatformMethod::selectFile(callback);
    }

    //移动端请求权限
    Q_INVOKABLE static void mobile_requestPermission(const QString& permission) {
        PlatformMethod::requestPermisson(permission);
    }

    //请求渲染pdf的某页图像，并存储到provider中; 回调通知callback(bool)
    Q_INVOKABLE static void request_renderPdf_as_image(const QString& filename,
                                                       int page_index,
                                                       const QString& into_image_id,
                                                       QJSValue callback);

    Q_INVOKABLE void mainThread_do(std::function<void(void)> func) {
        func();
    }

    Q_INVOKABLE static QVariant temp_Get(const QString& key,const QVariant& default_value = {})
    {
        auto it = temp_DB.find(key);
        if(it == temp_DB.end()) {
            temp_DB[key] = default_value;
            return default_value;
        }
        return *it;
    }

    Q_INVOKABLE static void temp_Set(const QString& key,const QVariant& value)
    {
        temp_DB[key] = value;
    }

    // 注意DB_Get和DB_Read的区别，详见DB_Read
    Q_INVOKABLE static QVariant DB_Get(const QString& key,const QVariant& default_value = {}) {
        auto it = little_DB.find(key);
        if(it == little_DB.end()) {
            little_DB[key] = default_value;
            return default_value;
        }
        return *it;
    }

    // DB_Read和DB_Get的区别在于：DB_Read不会对不存在的k-v写入默认值
    Q_INVOKABLE static QVariant DB_Read(const QString& key,const QVariant& default_value = {}) {
        auto it = little_DB.find(key);
        if(it == little_DB.end()) {
            return default_value;
        }
        return *it;
    }

    Q_INVOKABLE static void DB_Set(const QString& key,const QVariant& value) {
        little_DB[key] = value;
    }

    Q_INVOKABLE static void removeImageFromProvider(const QString& id);

    Q_INVOKABLE static void platform_maximizeWindow() {
#ifdef WIN32
        ShowWindow(main_window_hwnd,SW_MAXIMIZE);
#endif
    }

    Q_INVOKABLE static void platform_minimizeWindow() {
#ifdef WIN32
        ShowWindow(main_window_hwnd,SW_MINIMIZE);
#endif
    }

    Q_INVOKABLE static void platform_restoreWindow() {
#ifdef WIN32
        ShowWindow(main_window_hwnd,SW_RESTORE);
#endif
    }

    Q_INVOKABLE static QString extractFileNameFromPath(QUrl url) {
        return url.fileName();
    }

    //Q_INVOKABLE static

    //Q_INVOKABLE static QString getFileNameFromPath()
    // 从大纲创建目录
    Q_INVOKABLE static void createNavLines_fromLevels(QVariantMap args);

    // 自动创建目录
    Q_INVOKABLE static void createNavLines_auto(QVariantMap args);

    //测试接口，在qml中手动触发。可以测试任何功能。
    Q_INVOKABLE static void doTest_1(QVariant arg);

    Q_INVOKABLE static QChar number2chinese(int n) {
        if(n < 0 || n > 10) return QChar('?');
        static QString str = "零一二三四五六七八九十";
        return str[n];
    }

    Q_INVOKABLE static QStringList getFontFamilyList();

    // removeAll: 删除一个随机访问容器的所有某个值 O(n)
    template<typename T,typename V>
        requires requires(T t,V v){
            requires RandomAccessCont<T,V>;
            {t.resize(0)};
            {t[0] == t[0]};
        }
    static void removeAll(T& cont,V&& val = std::remove_cvref_t<V>(0)) {
        typename T::size_type n = 0;
        for(typename T::size_type i = 0; i < cont.size(); ++i) {
            if(!(cont[i] == val)) cont[n++] = std::move(cont[i]);
        }
        cont.resize(n);
    }

    //显示【选择】板
    static void showChooseBoard(int id,QPoint position,int dir,const QString& question,const QStringList& options,Quick_Callback callback) noexcept {
        //invokeQmlFunction<void>("showChooseBoard",question,options,callback);
        QMetaObject::invokeMethod(qml_main_object,"showChooseBoard",Qt::AutoConnection,
                                  QVariant{id},QVariant{position},QVariant{dir},QVariant{question},
                                  QVariant{options},QVariant::fromValue(callback));
    }

    static void tryHideChooseBoard(int id) {
        QMetaObject::invokeMethod(qml_main_object,"hideChooseBoard",
                                  QVariant{id});
    }

    static void invokeJSFunc(QJSValue func,const QVariantMap& arg) {
        QMetaObject::invokeMethod(Helper::helper,"__invokeJSFunc",func,arg);
    }


    //Q_INVOKABLE void whenWindowClosed();

    template <typename T,typename... Arg>
    static T invokeQmlFunction(const char* funcName,Arg&&...args) {
        if constexpr(std::is_same_v<T,void>) { //无参调用
            QMetaObject::invokeMethod(qml_main_object,funcName,Qt::AutoConnection,
                                      Q_ARG(QVariant,std::forward<Arg>(args))...);
        }
        else {  //有参调用
            QVariant result;
            QMetaObject::invokeMethod(qml_main_object,funcName,Qt::DirectConnection,Q_RETURN_ARG(QVariant,result)
                                      ,Q_ARG(QVariant,std::forward<Arg>(args))...);
            //if(! result.isValid()) return {};
            return result.value<T>();
        }
        return T{};
    }

    //临时存储数据(一次性放入、取出)
    static inline int tempStoreData(QVariant val) {
        if(tempAvailableIndex.empty()) {
            tempData.push_back(val);
            return tempData.size() - 1;
        }
        int i = tempAvailableIndex.back();
        tempAvailableIndex.pop_back();
        tempData[i] = val;
        return i;
    }

    //取出存储数据
    static inline QVariant fetchTempData(int index) {
        auto data = tempData[index];
        tempData[index] = {};
        tempAvailableIndex.push_back(index); //记录可用位置
        return data;
    }

    static inline void save() {
        QFile file("./cache/database");
        file.open(QIODevice::WriteOnly);
        QDataStream ds(&file);
        ds << little_DB;
        file.close();
    }

    // 编辑距离相似度，返回一个[0,1]的小数，越大表示相似度越高  O(n^2)
    static float levenshtein(const QString& str1,const QString& str2) noexcept;

    template <bool recursive=false,typename Func>
        requires requires(Func func,QString path) {
            {func(path)}->std::same_as<bool>;
        }
    static void walk_dir(QString path,Func func) {
        QDir dir;
        dir.setPath(path);
        QFileInfoList list = dir.entryInfoList();
        qDebug() << "walk_dir" << list;
        for ( int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);
            if (fileInfo.isFile()) {
                if(! func(fileInfo.absoluteFilePath())) return;
            }
            else {
                if (fileInfo.fileName() == "." || fileInfo.fileName() == ".." ) {
                    continue;
                }
                walk_dir<true>(fileInfo.absoluteFilePath(),func); //递归调用
            }
        }
    }

    //static void

public:

    static inline int ptime = 0;

    static inline QHash<QString,QVariant> little_DB;
    static inline QHash<QString,QVariant> temp_DB;
    static inline int cache_image_index = 0;

    static Helper* helper;

    static inline qreal dpi;

    static inline QObject* qml_main_object; //qml第一个对象，即Main

    static inline std::vector<QVariant> tempData;
    static inline std::vector<int> tempAvailableIndex;
    static inline bool showHelpLine = false;

    static inline QList<QString> original_font_list;
    static inline std::unordered_set<QString> original_font_set;
    // 替代字体family <pdf内嵌字体名称(去掉子集前缀)，替代的family>
    struct Font_Substitution {
        bool light, bold, italic; //是否需要设置替代字体的一些属性
        float sim; //相似度
        QString substituion_family;
    };

    static inline QHash<QString,Font_Substitution> font_substituions;
#ifdef WIN32
    static inline HWND main_window_hwnd;
#endif
private:
    Q_INVOKABLE void __invokeJSFunc(QJSValue func,const QVariantMap& arg) {
        QJSValueList args;
        args.append(engine->toScriptValue(arg));
        func.call(args);
    }

signals:
    void requestTrimCache();
    void errorMsg(const QString& title,const QString& text); //错误弹窗
    void beginLoadDocument();
    void fileLoadProgress(qreal pg); //加载进度通知
    void fileLoaded();               //加载完毕
    void beginSavePdf();             //开始保存pdf通知
    void pdfSaved();                 //保存pdf完毕
    void addVLine(const QString& vLineName,double x_offset); //添加一个vLine
    void addHLine(const QString& hLineName,double y_offset,const QString& leftLine,const QString& rightLine,
                  const QString& lastLine,const QString& lastAchorLine,
                  const QVariant& hLine_objs); //添加一个hLine
    void beginSelection();
    void clearSelection();
    void selectionStopButKeep();
};

#endif // HELPER_H
