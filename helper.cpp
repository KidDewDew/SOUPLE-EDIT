#include "helper.h"
#include "pdf2souple.h"
#include "souplemanager.h"
#include <QGuiApplication>
#include <QScreen>
#include <QtConcurrent/QtConcurrent>
#include "myimageprovider.h"
#include "PdfFontExtractor.h"
#include "safe_pointer.h"
#include <QtConcurrent/QtConcurrent>
#include "souple_pdfsaver.h"

Helper* Helper::helper = new Helper(nullptr);

extern MyImageProvider *image_provider;

Helper::Helper(QObject *parent)
    : QObject{parent}
{}

void Helper::init() //初始化
{
    { //获取dpi
        auto screen = QGuiApplication::primaryScreen();
        if(screen) {
            dpi = screen->logicalDotsPerInch();
        } else dpi = 96.0; //缺省值: 96.0
    } //获取dpi END
    qDebug() << "1像素 = " << Helper::pixel2cm(1) << "厘米";
    if(!QDir("./cache").exists()) {
        QDir(".").mkdir("./cache");
    }
    QFile file("./cache/database");
    if(file.exists()) {
        file.open(QIODevice::ReadOnly);
        QDataStream ds(&file);
        ds >> little_DB;
        qDebug() << "读入little_DB：Num of K-V:" << little_DB.size();
        file.close();
    }
}

void Helper::removeImageFromProvider(const QString& id)
{
    image_provider->removeImage(id);
}

void Helper::requestLoadSoup(const QString& filename) {
    //TODO Notify-Load-Soup-File
}

void Helper::requestLoadPdf(const QString& filename,const QVariantMap &options) {
    //从options中读取pdf解析建议
    qDebug() << "读取建议advice: " << options;
    Pdf2Souple::PDF_Load_Advice::hasUniHeader = options["uniHeader"].toBool();
    qDebug() << "advice hasUniHeader: " << Pdf2Souple::PDF_Load_Advice::hasUniHeader;
    //qDebug() << "PDF_Load_Advice ptr: " << &Pdf2Souple::PDF_Load_Advice;
    Pdf2Souple::PDF_Load_Advice::hasUniFooter = options["uniFooter"].toBool();
    if(Pdf2Souple::PDF_Load_Advice::hasUniHeader)
        Pdf2Souple::PDF_Load_Advice::header_margin = options["headerMargin"].toFloat();
    if(Pdf2Souple::PDF_Load_Advice::hasUniFooter)
        Pdf2Souple::PDF_Load_Advice::footer_margin = options["footerMargin"].toFloat();

    Pdf2Souple::loadPdf(filename);
}

QString Helper::requestRenderLatexFormula(const QString& latex,float fontSize,
                                      const QString& image_save_path)
{
    return JavaPKG::ThreadEnv<false>().jni_latexFormula2image(latex,fontSize,image_save_path);
}

void Helper::requestPreLoadPdf(const QString& filename,QJSValue callback) {
    qDebug() << "Helper::requestPreLoadPdf(" << filename;
    Pdf2Souple::future_preload = QtConcurrent::run([=]{
        QDir dir("./TEMP_embedded_font");
        if(! dir.exists())
            dir.mkdir(".");
        // true: 标识异步
        extractPdfFont<true>(filename,dir.absolutePath()); //提取内嵌字体
        Helper::invokeJSFunc(callback,{{"progress",1.2}});
    });
}

static void __deletePdfiumImage(void*info) {
    qDebug() << "释放临时渲染的Pdfium图像。";
    FPDFBitmap_Destroy((FPDF_BITMAP)info); //释放pdfium图片缓存
}

void Helper::request_renderPdf_as_image(const QString& filename,
                                int page_index,
                                const QString& into_image_id,
                                QJSValue callback)
{
    //qDebug() << "fileName::::" << filename;
    auto result = QtConcurrent::run([=]{
        auto document = FPDF_LoadDocument(filename.toUtf8(),""); //加载document
        int page_count = FPDF_GetPageCount(document); //获取pdf页数
        if(page_index < 0 || page_index > page_count) {

            QMetaObject::invokeMethod(helper,"mainThread_do",Q_ARG(std::function<void(void)>,[=]{
                callback.call({false,"页码超出范围"}); //回调
            }));

            FPDF_CloseDocument(document);
            return;
        }

        auto page = FPDF_LoadPage(document,page_index);

        int page_width = Helper::point2pixel(FPDF_GetPageWidth(page));
        int page_height = Helper::point2pixel(FPDF_GetPageHeight(page));

        auto fpdf_bitmap = FPDFBitmap_Create(page_width,page_height,1);

        FPDFBitmap_FillRect(fpdf_bitmap, 0, 0, page_width,page_height, 0xFFFFFFFF); // 白色背景

        qDebug() << "渲染pdf图像";

        //渲染图片
        FPDF_RenderPageBitmap(fpdf_bitmap,page,0,0,page_width,page_height,0,0);

        uchar* buffer = (uchar*)FPDFBitmap_GetBuffer(fpdf_bitmap);

        QImage image(buffer,page_width,page_height,QImage::Format_ARGB32,&__deletePdfiumImage,(void*)fpdf_bitmap);

        //image.save("D:/uuuu.png");

        //把图片给provider(异步)
        QMetaObject::invokeMethod(image_provider,"addImage",Q_ARG(QString,into_image_id),Q_ARG(QImage,image));

        QMetaObject::invokeMethod(helper,"mainThread_do",Q_ARG(std::function<void(void)>,[=]{
            callback.call({true,"success"}); //回调
        }));

        //FPDFBitmap_Destroy(fpdf_bitmap); //释放pdfium图片缓存
        FPDF_ClosePage(page);
        FPDF_CloseDocument(document);
    });

}

float Helper::levenshtein(const QString& str1,const QString& str2) noexcept
{
    int len1 = str1.length(),
        len2 = str2.length();
    if(len1 == 0 || len2 == 0)
        return 0;
    std::vector<std::vector<int>> dif(len1+1,std::vector<int>(len2+1));
    for (int a = 0; a <= len1; ++a) {
        dif[a][0] = a;
    }
    for (int a = 0; a <= len2; ++a) {
        dif[0][a] = a;
    }
    //计算两个字符是否一样，计算左上的值
    int temp;
    for (int i = 1; i <= len1; ++i) {
        for (int j = 1; j <= len2; ++j) {
            if (str1[i-1] == str2[j-1]) {
                temp = 0;
            } else {
                temp = 1;
            }
            //取三个值中最小的
            dif[i][j] = qMin(dif[i - 1][j - 1] + temp,
                        qMin(dif[i][j - 1] + 1,
                             dif[i - 1][j] + 1));
        }
    }
    float similarity =1 - (float)dif[len1][len2]/qMax(len1,len2);
    return similarity;
}

QStringList Helper::getFontFamilyList()
{
    return QFontDatabase::families();
}

void Helper::doTest_1(QVariant arg)
{
    //2025-11-13 测试“渲染pdf页面为provider图像的功能”
    SoupleManager::request_render_page(arg.toInt(),0,"test_page");
}
