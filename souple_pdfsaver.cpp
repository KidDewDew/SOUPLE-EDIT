#include "souple_pdfsaver.h"
#include <fpdf_text.h>
#include <fpdf_doc.h>
#include <fpdf_annot.h>
#include <fpdf_edit.h>
#include <fpdf_save.h>
#include <QPdfWriter>
#include <QFile>
#include <QPainter>

using namespace std;


class CustomFileWriter : public FPDF_FILEWRITE {
public:
    explicit CustomFileWriter(const QString& file_path)
        : output_file(file_path) {
        output_file.open(QIODevice::WriteOnly);
    }
    static int writeBlock(FPDF_FILEWRITE* pThis, const void* data, unsigned long size) {
        auto* writer = static_cast<CustomFileWriter*>(pThis);
        if(writer->output_file.write(static_cast<const char*>(data), size) == -1) return 0;
        return 1; // 返回1表示成功，0表示失败
    }
    ~CustomFileWriter() {
        output_file.close();
    }
private:
    QFile output_file;
};


Souple_PdfSaver::Souple_PdfSaver() {}

void Souple_PdfSaver::beginSaveAs(const QString& pdfFileName) {
    qDebug() << "Souple_PdfSaver::beginSaveAs(" << pdfFileName;
    static bool _protect_var = false;
    if(_protect_var) {
        emit Helper::helper->errorMsg("错误","请等待PDF加载完毕！");
        return;
    }
    _protect_var = true;
    emit Helper::helper->beginSavePdf();
    QThread *t = new QThread;
    QObject::connect(t,&QThread::started,[=]{
        imp_saveAs(pdfFileName);
        _protect_var = false;
        emit Helper::helper->pdfSaved();
        t->exit(); //退出线程
    });
    QObject::connect(t,&QThread::finished,t,&QThread::deleteLater);
    t->start();
}

void Souple_PdfSaver::imp_saveAs(const QString& pdfFileName)
{
    //暂时使用QPdfWriter进行pdf输出
    QPdfWriter pdfWriter(pdfFileName);

    pdfWriter.setResolution(Helper::getDPI()); //设置像素密度

    pdfWriter.setCreator("Souple Editor 相对布局编辑器");

    struct Print_Page {
        vector<Obj*> objs;
        Page *page;
    };

    vector<Print_Page> print_pages; //打印页面

    qDebug() << "尝试上锁";

    SoupleManager::mutex.lock();
    SoupleManager::paused = true;

    qDebug() << "上锁成功";

    for(int i = 0; i < SoupleManager::page_inf.pages.size(); ++i) {
        print_pages.push_back({.page = SoupleManager::page_inf.pages[i]});
    }

    //pdfWriter.setPageRanges()

    //把所有对象安排到对应的print_page中去

    for(auto obj : SoupleManager::all_objs) {
        auto page = SoupleManager::getPage(*obj);
        if(!page || page->index >= print_pages.size()) continue;
        print_pages[page->index].objs.push_back(obj);
    }

    qDebug() << "排页成功";

    if(print_pages.size() > 0) {
        //开始保存
        pdfWriter.setPageSize(QPageSize{QSize{(int)Helper::pixel2point(print_pages[0].page->width),
                                              (int)Helper::pixel2point(print_pages[0].page->height)}});
        QPainter painter(&pdfWriter);

        int i = 0;
        for(auto& print_page : print_pages) {

            qDebug() << "newPage: " << print_page.page->width << print_page.page->height;
            for(auto obj : print_page.objs) {
                obj->qt_paint(painter,print_page.page); //绘制内容
            }

            if(i+1 < print_pages.size()) {
                pdfWriter.setPageSize(QPageSize{QSize{
                                        (int)Helper::pixel2point(print_pages[i+1].page->width),
                                        (int)Helper::pixel2point(print_pages[i+1].page->height)}});
                pdfWriter.newPage();
            }
            ++i;
        }

        painter.end();
    }

    SoupleManager::paused = false;
    SoupleManager::mutex.unlock();

    // for(int i = 0; i < SoupleManager::page_inf.pages.size(); ++i) { //创建所有页面
    //     pages.push_back(
    //         FPDFPage_New(pdf,i,
    //                      Helper::pixel2point(SoupleManager::page_inf.pages[i]->width),
    //                      Helper::pixel2point(SoupleManager::page_inf.pages[i]->height))
    //         );
    // }

    //保存所有souple对象
    // for(auto obj : SoupleManager::all_objs) {
    //     auto page = SoupleManager::getPage(*obj);
    //     if(page->index > pages.size()) continue;
    //     obj->writeToPDFPage(pdf,pages[page->index],page);
    // }



    // for(auto page : pages) {
    //     FPDFPage_GenerateContent(page);
    // }

    // CustomFileWriter writer(pdfFileName);
    // writer.WriteBlock = &CustomFileWriter::writeBlock; // 绑定回调函数
    // bool ok = FPDF_SaveAsCopy(pdf,&writer,FPDF_NO_INCREMENTAL);

    // //QRawFont rf;
    // //FPDFText_LoadCidType2Font()

    // qDebug() << "save ok = " << ok;

    // FPDF_CloseDocument(pdf);
}

void Souple_PdfSaver::renederPage(QPainter* painter,Page* page,const std::vector<Obj*>& __page_objs)
{
    std::vector<Obj*> page_objs = __page_objs;
    ranges::sort(page_objs,[](Obj*a,Obj*b){
        return a->z > b->z;
    });
    for(auto obj : page_objs) {
        obj->qt_paint(*painter,page);
    }
}
