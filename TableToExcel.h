#ifndef TABLETOEXCEL_H
#define TABLETOEXCEL_H

#include <QObject>
#include "tableline.h"  // 直接使用tableline.h中定义的TableInfo

class TableToExcel : public QObject {
    Q_OBJECT
    QML_ELEMENT  // 注册到QML，支持Qt 6+；Qt 5需用qmlRegisterType在main中注册

public:
    explicit TableToExcel(QObject *parent = nullptr);

    // 核心接口：将TableInfo数据导出为Excel
    // Q_INVOKABLE允许QML直接调用静态方法
    Q_INVOKABLE static bool extractTableToExcel(TableInfo* ti, const QString& saveFile);
};

#endif // TABLETOEXCEL_H
