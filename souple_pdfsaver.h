#ifndef SOUPLE_PDFSAVER_H
#define SOUPLE_PDFSAVER_H

#include "souplemanager.h"
#include <QThread>
#include <QPdfWriter>
//souple转pdf类，用于输出pdf
//从SoupleManager中获取souple文档信息
class Souple_PdfSaver : public QObject
{
    Q_OBJECT
public:
    Souple_PdfSaver();
    // 保存当前文档为pdf文件
    static Q_INVOKABLE void beginSaveAs(const QString& pdfFileName);

    // 渲染指定的objs到painter。给定page。
    static void renederPage(QPainter* painter,Page* page,const std::vector<Obj*>& page_objs);
private:
    static void imp_saveAs(const QString& pdfFileName);
public:
    static inline QHash<QString,FPDF_FONT> used_font_families; //用到的字体族名称，以便统一内嵌
    static inline Souple_PdfSaver *pdfSaver;
};

#endif // SOUPLE_PDFSAVER_H
