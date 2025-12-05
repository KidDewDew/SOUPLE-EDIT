#ifndef TABLETOEXCEL_H
#define TABLETOEXCEL_H

#include <QObject>
#include "tableline.h" // 包含TableInfo定义

// 必须继承QObject，且有Q_OBJECT宏
class TableToExcel : public QObject
{
    Q_OBJECT // 必须保留，否则无法注册到QML
public:
    explicit TableToExcel(QObject *parent = nullptr);

    // 修正参数为QObject*，确保QML能传递TableInfo
    Q_INVOKABLE static bool extractTableToExcel(QObject* tableInfoObj, const QString& filePath);

private:
    // 从TableInfo中获取单元格内容
    static QString getCellContent(Free_TableUnit* unit);
    // 将QObject*转换为TableInfo*（安全转换）
    static TableInfo* toTableInfo(QObject* obj);
};

#endif // TABLETOEXCEL_H
