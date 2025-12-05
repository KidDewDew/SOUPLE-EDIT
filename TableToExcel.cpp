#include "TableToExcel.h"
#include "tableline.h"
#include "free_tableunit.h"
#include <xlsxwriter.h>
#include <QDebug>
#include <any>

TableToExcel::TableToExcel(QObject *parent) : QObject(parent) {}

// 安全转换QObject*到TableInfo*
TableInfo* TableToExcel::toTableInfo(QObject* obj) {
    if (!obj) return nullptr;
    // 假设TableInfo继承自QObject，使用dynamic_cast转换
    return dynamic_cast<TableInfo*>(obj);
}

// 提取单元格内容
QString TableToExcel::getCellContent(Free_TableUnit* unit) {
    if (!unit) return "";
    auto textData = unit->getAnyData("text");
    if (textData.has_value()) {
        try {
            return std::any_cast<QString>(textData);
        } catch (...) {
            qWarning() << "单元格文本转换失败";
        }
    }
    return "";
}

// 导出表格到Excel
bool TableToExcel::extractTableToExcel(QObject* tableInfoObj, const QString& filePath) {
    // 转换表格信息对象
    TableInfo* tableInfo = toTableInfo(tableInfoObj);
    if (!tableInfo) {
        qWarning() << "无效的表格信息对象";
        return false;
    }

    // 创建xlsxwriter工作簿
    lxw_workbook *workbook = workbook_new(filePath.toUtf8().constData());
    if (!workbook) {
        qWarning() << "创建Excel工作簿失败:" << filePath;
        return false;
    }
    lxw_worksheet *worksheet = workbook_add_worksheet(workbook, nullptr);
    if (!worksheet) {
        workbook_close(workbook);
        qWarning() << "创建Excel工作表失败";
        return false;
    }

    // 遍历表格写入数据
    for (int row = 0; row < tableInfo->rowCount(); ++row) {
        for (int col = 0; col < tableInfo->colCount(); ++col) {
            const auto& unitInfo = tableInfo->units[row][col];
            if (!unitInfo.u) continue;

            // 仅写入合并单元格的起始位置
            if (unitInfo.start_row == row && unitInfo.start_col == col) {
                QString cellText = getCellContent(unitInfo.u);

                // 写入单元格内容
                worksheet_write_string(worksheet, row, col,
                                       cellText.toUtf8().constData(), nullptr);

                // 处理合并单元格
                if (unitInfo.start_row != unitInfo.end_row || unitInfo.start_col != unitInfo.end_col) {
                    worksheet_merge_range(worksheet,
                                          unitInfo.start_row, unitInfo.start_col,
                                          unitInfo.end_row, unitInfo.end_col,
                                          cellText.toUtf8().constData(), nullptr);
                }
            }
        }
    }

    // 保存并关闭工作簿
    bool success = (workbook_close(workbook) == LXW_NO_ERROR);
    if (!success) {
        qWarning() << "保存Excel文件失败:" << filePath;
    }
    return success;
}
