#ifndef PDFFONTEXTRACTOR_H
#define PDFFONTEXTRACTOR_H

#include <QString>
#include <QList>
#include "JavaPkg.h"
#include "ToolExecuter.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFontDatabase>
#include <QFontInfo>

struct EmbeddedFontInf {
    QString fontFile;
    QString fontName;
    QString fontFamily;
};

// 从字体名称中提取sign；如果提取到了，则返回true并删除sign
template <bool sensitive = false> //默认大小写不敏感
inline bool extractSignFromFontName(QString& s,const QString& sign) noexcept
{
    int i;
    if constexpr(sensitive) {
        i = s.lastIndexOf(sign);
    } else {
        i = s.lastIndexOf(sign,Qt::CaseInsensitive);
    }
    //qDebug() << s << sign << i;
    if(i == -1 || i == 0) return false;
    if(i > 0 && (s[i-1] == '-'||s[i-1]==' '||s[i-1]==','))
        s.remove(i-1,sign.length()+1);
    else s.remove(i,sign.length());
    return true;
}

// 根据字体名称寻找字体替代family
Helper::Font_Substitution findFontSubstituion(const QString& fontName) {
    // 目标：找到和fontName最像的字体
    // Step1.首先，提取fontName中的
    // Bold Black Regular Italic、It Semibold
    QString s = fontName;
    bool bold,black,regular,italic,semibold,light;
    bold = black = regular = italic = semibold = light = false;

    bold = extractSignFromFontName(s,"bold");
    black = extractSignFromFontName(s,"black");
    italic = extractSignFromFontName(s,"italic");
    italic |= extractSignFromFontName<true>(s,"It");
    semibold = extractSignFromFontName(s,"semibold");
    light = extractSignFromFontName(s,"light");
    regular = extractSignFromFontName(s,"regular");

    QString lower_fontName = s.toLower();
    const QString* best_family = 0;
    float best_sim = 0.5;
    for(auto& maybe_family : Helper::original_font_list) {
        float sim = Helper::levenshtein(lower_fontName,maybe_family.toLower());
        //qDebug() << lower_fontName << maybe_family.toLower() << sim;
        if(sim > best_sim) {
            best_sim = sim;
            best_family = &maybe_family;
        }
    }

    if(best_family) {
        Helper::Font_Substitution sub;
        sub.substituion_family = std::move(*best_family);
        sub.bold = bold|black|semibold;
        sub.light = light;
        sub.italic = italic;
        sub.sim = best_sim;
        qDebug() << fontName << lower_fontName << "-->" << sub.substituion_family
                 << best_sim;
        return sub;
    }
    else return {};
}

/**
 * @brief extractPdfFont: 提取pdf字体文件到save_path，返回所有提取的字体文件的路径。
 * @param pdf_file
 * @param save_path
 */
template <bool _new_thread>
QList<EmbeddedFontInf> extractPdfFont(const QString& pdf_file,const QString& save_path)
{
    QString json_str =
        JavaPKG::ThreadEnv<_new_thread>().jni_extractEmbeddedFonts(pdf_file,save_path);

    QJsonArray arr = QJsonDocument::fromJson(json_str.toUtf8()).array();
    qDebug() << "java return: "<< json_str;

    QStringList fontFileList;
    QStringList args;
    QList<EmbeddedFontInf> fontInf_list;

    for(auto v : arr) {
        auto obj = v.toObject();
        QString fontFile = obj["FontFile"].toString();
        EmbeddedFontInf finf;
        finf.fontFile = fontFile;
        finf.fontName = obj["FontName"].toString();

        fontInf_list.append(std::move(finf));
        fontFileList.append(fontFile);
        if(obj["hasCMapFile"].toBool() == true) {
            args.push_back(fontFile);
            args.push_back(obj["CMapFile"].toString());
        } else {
            args.push_back(fontFile);
            args.push_back("null");
        }
    }


    //调用python模块: sguPythonTool 来修补字体文件
    if(args.size() > 0) {
        ToolExecuter::execute("./tools/sguPythonTool.exe","fontMend",args);
    }

    for(auto& finf : fontInf_list) {
        //取出字体名称
        QString family = finf.fontName.split('+').back();
        extractSignFromFontName(family,"Identity-H");
        if(QFontDatabase::hasFamily(family)) {
            finf.fontFamily = family;
            continue;
        }

        //计算并存储相似替代字体
        auto sub = findFontSubstituion(family);
        if(sub.substituion_family != "")
            Helper::font_substituions[family] = sub;

        int id = QFontDatabase::addApplicationFont(finf.fontFile);
        qDebug() << "id=" << id;
        if(id >= 0) {
            finf.fontFamily = QFontDatabase::applicationFontFamilies(id)[0];
            qDebug() << "addFont:" << QFontDatabase::applicationFontFamilies(id);
        } else {
            qDebug() << "analyse embedded font failed:" << family;
        }
    }

    return fontInf_list;
}




#endif // PDFFONTEXTRACTOR_H
