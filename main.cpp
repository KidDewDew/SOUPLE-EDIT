
/// 文件版本: 2025/11/16
/// 新版本: [无]

/// 这里记录了一些重要代码的位置：
/// 1.全局事件过滤器 位于SoupleManager.cpp


#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QDir>
#include <QSettings>
#include "helper.h"
#include "pdf2souple.h"
#include "souplemanager.h"
#include "myimageprovider.h"
#include "souple_pdfsaver.h"
#include <QPdfWriter>
#include <QPainter>
#include <QPainterPath>
#include <QStaticText>
#include <QQuickWindow>
#include "PlatformMethod.h"
#include <QStandardPaths>
#include "quick_callback.h"
#include "JavaPkg.h"
#include "selectionmanager.h"
#include <QAbstractNativeEventFilter>
#include <QFontDatabase>
#include <QFontInfo>
#include <QFont>
#include <QDataStream>
#include <QColor>

#include "magicalcursor.h"

#ifdef WIN32
#include <windows.h>
#include <dwmapi.h>
#endif


QObject* qmlRoot;
QQuickWindow* qmlWindow;
QQmlApplicationEngine* engine;
MyImageProvider *image_provider; //图像提供者
Quick_Callback quick_callback;
//app指针
QGuiApplication *global_app;
//QObject* no_focus_object;

constexpr char nn[] = "FlowText";

#pragma comment(lib, "dwmapi.lib")

void my_terminate()
{
    try {
        throw;
    }
    catch(LLException& e) {
        auto ba = e.getInfo().toLocal8Bit();
        MessageBoxA(0,ba.constData(),"Error: Uncaught Exception!",MB_OK);
        exit(1);
    }
    catch(std::exception& e) {
        MessageBoxA(0,e.what(),"Error: Uncaught Exception!",MB_OK);
        exit(1);
    }
    MessageBoxA(0,"Unknown Error","Error: Program to terminate.",MB_OK);
    exit(1);
}

/**
 * @brief The MyWindowEvenetFilter class
 *        该类用于处理原生平台窗口消息，需要在main函数里，把该类的实例绑定到主窗口。
 *        该类主要负责：隐藏标题栏 恢复窗口调整边框功能
 */
class MyWindowEvenetFilter : public QAbstractNativeEventFilter
{
public:
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr * result) override
    {
#ifdef WIN32
        MSG *msg = static_cast<MSG *>(message);
        if(msg->hwnd == Helper::main_window_hwnd) {
            switch(msg->message) {
            case WM_NCCALCSIZE:
                *result = 0; //总是返回0，使得标题栏不会被绘制。
                return true;
                break;
            case WM_NCHITTEST: {   //主动告诉窗口，哪些地方是边框。
                const LONG borderWidth = 9;
                RECT winrect;
                GetWindowRect(msg->hwnd, &winrect);
                long x = LOWORD(msg->lParam);
                long y = HIWORD(msg->lParam);

                // bottom left
                if (x >= winrect.left && x < winrect.left + borderWidth &&
                    y < winrect.bottom && y >= winrect.bottom - borderWidth)
                {
                    *result = HTBOTTOMLEFT;
                    return true;
                }

                // bottom right
                if (x < winrect.right && x >= winrect.right - borderWidth &&
                    y < winrect.bottom && y >= winrect.bottom - borderWidth)
                {
                    *result = HTBOTTOMRIGHT;
                    return true;
                }

                // top left
                if (x >= winrect.left && x < winrect.left + borderWidth &&
                    y >= winrect.top && y < winrect.top + borderWidth)
                {
                    *result = HTTOPLEFT;
                    return true;
                }

                // top right
                if (x < winrect.right && x >= winrect.right - borderWidth &&
                    y >= winrect.top && y < winrect.top + borderWidth)
                {
                    *result = HTTOPRIGHT;
                    return true;
                }

                // left
                if (x >= winrect.left && x < winrect.left + borderWidth)
                {
                    *result = HTLEFT;
                    return true;
                }

                // right
                if (x < winrect.right && x >= winrect.right - borderWidth)
                {
                    *result = HTRIGHT;
                    return true;
                }

                // bottom
                if (y < winrect.bottom && y >= winrect.bottom - borderWidth)
                {
                    *result = HTBOTTOM;
                    return true;
                }

                // top
                if (y >= winrect.top && y < winrect.top + borderWidth)
                {
                    *result = HTTOP;
                    return true;
                }

                return false;
            }
            }
        }
#endif
        return false;
    }
};


int main(int argc, char *argv[])
{

    set_terminate(my_terminate);
    /// Qt 6.9.2 bug: threaded渲染模式下vsync异常
    /// 解决方法：强制使用Opengl后端。
    qputenv("QSG_RENDER_LOOP", "threaded");
    //qputenv("QSG_INFO", "1");
    //qputenv("QSG_RHI_PROFILE", "1");

    //qputenv("QT_LOGGING_RULES", "qml=false"); //禁用qml输出

    qputenv("QT_QUICK_CONTROLS_MATERIAL_VARIANT","Dense");

    QGuiApplication app(argc, argv);

    ::global_app = &app;

    app.installNativeEventFilter(new MyWindowEvenetFilter);
    //使用opengl api进行渲染
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
    qDebug() << QQuickWindow::graphicsApi();

    //QQuickWindow::

    //QQuickWindow::setTextRenderType(QQuickWindow::NativeTextRendering);

    //QQuickWindow::set

    QCoreApplication::setOrganizationName("None");
    QCoreApplication::setOrganizationDomain("oar.com");
    QCoreApplication::setApplicationName("SoupleEdit");

    QQmlApplicationEngine engine;

    //QQuickWindow::Sch

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    /**在这里进行各种初始化**/

    FPDF_InitLibrary(); //初始化fpdf

    //FPDFFont_GetFont

    Helper::helper->setParent(&app);
    Helper::init();

    if(Helper::DB_Read("FirstStart",true) == true) {
        //第一次使用
        QDir().mkpath("cache/pdf_image/"); //创建路径
        Helper::DB_Set("FirstStart",false);
    }

    SoupleManager::soupleManager = new SoupleManager;
    SoupleManager::soupleManager->setParent(&app);

    SoupleManager::init();

    Souple_PdfSaver _pdfSaver;
    Souple_PdfSaver::pdfSaver = &_pdfSaver;

    qmlRegisterType<Helper>("com.custom",1,0,"Helper_Type");
    qmlRegisterType<Helper>("com.custom",1,0,"HTypes");
    qmlRegisterType<SoupleManager>("com.custom",1,0,"SoupleManager_Type");
    qRegisterMetaType<Quick_Callback>("quick_callback"); //注册元类型: qml回调s

    image_provider = new MyImageProvider;
    engine.addImageProvider("provider",image_provider); //添加图像提供者

    engine.rootContext()->setContextProperty("SoupleManager",SoupleManager::soupleManager);
    engine.rootContext()->setContextProperty("spMgr",SoupleManager::soupleManager);
    engine.rootContext()->setContextProperty("Helper",Helper::helper);
    engine.rootContext()->setContextProperty("PdfSaver",Souple_PdfSaver::pdfSaver);
    engine.rootContext()->setContextProperty("Invoker",&quick_callback);
    engine.rootContext()->setContextProperty("SelectMgr",SelectionManager::getInstance());
    //engine.rootContext()->setContextProperty("PlatformMethod",&platformMethod);
    //engine.addImageProvider("PDF",new MyImageProvider);

    const QStringList moviesLocation = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    const QUrl videoPath =
        QUrl::fromLocalFile(moviesLocation.isEmpty() ?
                                app.applicationDirPath() :
                                moviesLocation.front());
    engine.rootContext()->setContextProperty("videoPath", videoPath);


#ifdef Q_OS_WIN32
    engine.loadFromModule("SoupleEdit", "Main");
#elif defined(Q_OS_ANDROID)
    engine.loadFromModule("SoupleEdit", "Main_mobile");
#endif

    ::engine = &engine;

    //初始化全局变量
    qmlRoot = Helper::qml_main_object = engine.rootObjects().first();

    MagicalCursor::init();

    qDebug() << "qmlRoot:" << qmlRoot;

    qmlWindow = qobject_cast<QQuickWindow*>(qmlRoot);
    qmlWindow->setPersistentGraphics(false);
    qmlWindow->setPersistentSceneGraph(false);

    try {
        if(! JavaPKG::init()) {
            qDebug() << "JVM初始化失败";
            Helper::invokeQmlFunction<void>("errorMsg","JVM初始化失败，请检查java环境。");
        }
    } catch(LLException& e) {
        if(e.getType2() == LLException::NotFound_JAVA_HOME) {
            Helper::invokeQmlFunction<void>("open_dialog_setJAVA_HOME");
        }
    }

    Helper::invokeQmlFunction<void>("open_dialog_setJAVA_HOME");

    QObject::connect(Helper::helper,&Helper::requestTrimCache,[&]{
        engine.trimComponentCache();
    });


#ifdef WIN32

    //qmlWindow->installEventFilter(new MyWindowEvenetFilter);

    HWND hwnd = (HWND)qmlWindow->winId();

    Helper::main_window_hwnd = hwnd;

    LONG_PTR win_style = GetWindowLongPtr(hwnd,GWL_STYLE);
    SetWindowLong(hwnd, GWL_STYLE, win_style | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CAPTION);
    MARGINS margins = {1,1,1,1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    //QAbstractNativeEventFilter

    //qDebug() << "cycaption" << GetSystemMetrics(SM_CYCAPTION);

    qmlWindow->setProperty("cycaption",6);

#endif

    PlatformMethod::init(); //初始化跨平台方法类

    //安装全局事件过滤器，保证获取所有事件
    //SoupleManager::installEventFilter(&app);
    auto soupleEdit = qmlRoot->findChild<QQuickItem*>("qml_soupleEdit");
    //soupleEdit->setFiltersChildMouseEvents(true);
    SoupleManager::setQmlSoupleEdit(soupleEdit);
    SoupleManager::installEventFilter(&app);

    //加载附件字体
    Helper::walk_dir("./back-fonts",[](const QString& filePath)->bool{
        if(filePath.endsWith(".ttf") || filePath.endsWith(".otf")) {
            QFontDatabase::addApplicationFont(filePath);
            qDebug() << "addFont:" << filePath;
        }
        return true;
    });

    //QFontDatabase::addApplicationFont("F:\\QT-projects\\SoupleEdit\\back-fonts\\LMRoman10-bold.ttf");
    //初始化字体
    for(auto& s : QFontDatabase::families()) {
        QFont f1(s),f2(s);
        f1.setPointSizeF(10);
        f2.setPointSizeF(20);
        QFontInfo info1(f1);
        QFontInfo info2(f2);
        // 无法缩放的字体，不纳入替代字体范围。
        if(info1.pointSize() == info2.pointSize()) continue;
        Helper::original_font_list.push_back(s);
    }
    for(auto& s : QFontDatabase::families()) {
        Helper::original_font_set.insert(s);
    }
    // for(auto& fn : Helper::original_font_list) {
    //     qDebug() << "ava-family:" << fn;
    //     qDebug() << "sim:" << Helper::levenshtein("TimesNRM",fn);
    // }
    //程序即将终止
    QObject::connect(&app,&QCoreApplication::aboutToQuit,
            [] {
        Helper::save();
        //清空cache/pdf_image目录
        for(auto& fileName : QDir("cache/pdf_image").entryList()) {
            QFile::remove("cache/pdf_image"+fileName);
        }
        qDebug() << "程序退出";
    });

    // QObject::connect(qmlRoot,&QObject::destroyed,[]{
    //     SoupleManager::paused = true;
    // });

    /**初始化 END**/

    /*以下是各种测试代码**/

    // QPdfWriter w("C:\\Users\\guest0\\Desktop\\eee.pdf");
    // w.setResolution(Helper::getDPI());
    // //w.setPageSize(QPageSize{QSize{400,800}});
    // QPainter pt;

    // //pt.setRenderHint(QPainter::)

    // pt.begin(&w);
    // pt.setPen(QPen(QColor(255, 0, 0), 1)); // 设置描边颜色
    // pt.setBrush(Qt::black);	// 设置填充颜色
    // // pt.drawTextItem()
    // // QPainterPath path;
    // QFont font("黑体",30);
    // //font.setBold(true);
    // pt.setFont(font);

    // QPainterPath path;
    // path.addText(100,200,font,"你好，这是QPainter描边文字。");
    // pt.drawPath(path);
    // //font.setStyle()
    // //pt.drawText(100,200,"你好，这是QPainter描边文字。");
    // // pt.drawPath(path);
    // //pt.drawP

    // pt.end();
    // QList<QQuickItem*> iis;
    // for(int i = 0; i < 0; ++i) {
    //     iis.push_back(UIItemPool<nn,20>::fetchItem());
    // }
    // for(auto item : iis) {
    //     item->setProperty("text","hello,world");
    //     item->setPosition({200,300});
    // }

    // QTimer::singleShot(8000,[&]{
    //     for(auto item : iis) {
    //         UIItemPool<nn,20>::returnItem(item);
    //     }
    //     UIItemPool<nn,20>::scanPool();
    // });

    // QByteArray bytes;
    // QDataStream ds(&bytes,QIODevice::ReadWrite);
    // AnchorObj_FlowText* ft = new AnchorObj_FlowText;
    // ft->text = "你好啊";
    // ft->font.setFamily("宋体");
    // ft->font.setPointSizeF(22);
    // ft->stroke_color = "#123456";
    // QString str;
    // souple::serialization::serialize_xml(ft,&str);
    // souple::serialization::serialize(ft,ds);
    // qDebug() << str.length() << str;
    // qDebug() << bytes.size() << bytes;

    // QDataStream ds2(&bytes,QIODevice::ReadOnly);
    // Obj* ft2 = (Obj*)souple::serialization::unserialize(ds2);
    // qDebug() << ft2->__dstr();

    /* Create a new workbook and add a worksheet. */
        // lxw_workbook  *workbook  = workbook_new("demo.xlsx");
        //lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);

        // /* Add a format. */
        // lxw_format *format = workbook_add_format(workbook);

        // /* Set the bold property for the format */
        // format_set_bold(format);

        // /* Change the column width for clarity. */
        // worksheet_set_column(worksheet, 0, 0, 20, NULL);

        // /* Write some simple text. */
        // worksheet_write_string(worksheet, 0, 0, "Hello", NULL);

        // /* Text with formatting. */
        // worksheet_write_string(worksheet, 1, 0, "World", format);

        // /* Write some numbers. */
        // worksheet_write_number(worksheet, 2, 0, 123,     NULL);
        // worksheet_write_number(worksheet, 3, 0, 123.456, NULL);

        // /* Insert an image. */
        // worksheet_insert_image(worksheet, 1, 2, "logo.png");

        // workbook_close(workbook);


    return app.exec();
}
