#include "TableToExcel.h"
#include "tableline.h"  // 包含TableInfo的具体定义
#include <QString>
#include <QDebug>
#include <xlsxwriter.h>  // 依赖libxlsxwriter库
#include <stdexcept>     // 用于异常处理

// 构造函数：仅为QObject继承关系初始化，无需额外逻辑
TableToExcel::TableToExcel(QObject *parent) : QObject(parent) {}

/**
 * @brief 初始化Excel单元格样式（表头和内容样式）
 * @param workbook 工作簿指针
 * @param header_format 表头样式指针（输出）
 * @param content_format 内容样式指针（输出）
 */
static void initExcelStyles(lxw_workbook *workbook, lxw_format **header_format, lxw_format **content_format) {
    if (!workbook) return;

    // 表头样式：加粗、居中、浅灰背景、细边框
    *header_format = workbook_add_format(workbook);
    format_set_bold(*header_format);
    format_set_align(*header_format, LXW_ALIGN_CENTER);         // 水平居中
    format_set_align(*header_format, LXW_ALIGN_VERTICAL_CENTER); // 垂直居中
    format_set_bg_color(*header_format, 0xF0F0F0);              // 浅灰色背景
    format_set_border(*header_format, LXW_BORDER_THIN);          // 细边框
    format_set_border_color(*header_format, 0x000000);          // 边框黑色

    // 内容样式：居中、细边框
    *content_format = workbook_add_format(workbook);
    format_set_align(*content_format, LXW_ALIGN_CENTER);
    format_set_align(*content_format, LXW_ALIGN_VERTICAL_CENTER);
    format_set_border(*content_format, LXW_BORDER_THIN);
    format_set_border_color(*content_format, 0x000000);
}

/**
 * @brief 核心方法：将TableInfo中的表格数据导出到Excel
 * @param ti 表格数据（TableInfo指针，来自tableline.h）
 * @param saveFile 保存路径（支持中文路径）
 * @return 导出成功返回true，失败返回false
 */
bool TableToExcel::extractTableToExcel(TableInfo *ti, const QString &saveFile) {
    // 第一步：参数校验
    if (!ti) {
        qCritical() << "TableToExcel: 表格数据(TableInfo)为空！";
        return false;
    }
    if (saveFile.isEmpty()) {
        qCritical() << "TableToExcel: 保存路径为空！";
        return false;
    }

    // 获取表格行列数（依赖TableInfo已实现的rowCount()和colCount()）
    int rowCount = ti->rowCount();
    int colCount = ti->colCount();
    if (rowCount <= 0 || colCount <= 0) {
        qCritical() << "TableToExcel: 表格无有效数据（行：" << rowCount << "，列：" << colCount << "）";
        return false;
    }

    // 第二步：创建Excel工作簿和工作表
    // 转换QString路径为C字符串（支持中文，使用toLocal8Bit处理编码）
    const char *excelPath = saveFile.toLocal8Bit().constData();
    lxw_workbook *workbook = workbook_new(excelPath);
    if (!workbook) {
        qCritical() << "TableToExcel: 创建工作簿失败！路径：" << saveFile;
        return false;
    }

    // 创建工作表（默认名称"Sheet1"）
    lxw_worksheet *worksheet = workbook_add_worksheet(workbook, nullptr);
    if (!worksheet) {
        qCritical() << "TableToExcel: 创建工作表失败！";
        workbook_close(workbook);  // 释放资源
        return false;
    }

    // 第三步：初始化样式
    lxw_format *headerFormat = nullptr;
    lxw_format *contentFormat = nullptr;
    initExcelStyles(workbook, &headerFormat, &contentFormat);

    // 第四步：设置列宽（优化显示）
    const double columnWidth = 15;  // 列宽适中，可根据内容调整
    for (int col = 0; col < colCount; ++col) {
        worksheet_set_column(worksheet, col, col, columnWidth, nullptr);
    }

    // 第五步：写入表格数据
    try {
        for (int row = 0; row < rowCount; ++row) {
            for (int col = 0; col < colCount; ++col) {
                // 获取单元格文本（依赖TableInfo已实现的getCellText()，返回QString）
                QString cellText = ti->getCellText(row, col);
                // 转换为C字符串（支持中文）
                const char *text = cellText.toLocal8Bit().constData();

                // 表头行（第0行）用表头样式，其他行用内容样式
                if (row == 0) {
                    worksheet_write_string(worksheet, row, col, text, headerFormat);
                } else {
                    worksheet_write_string(worksheet, row, col, text, contentFormat);
                }
            }
        }
    } catch (const std::exception &e) {
        qCritical() << "TableToExcel: 写入数据失败！原因：" << e.what();
        workbook_close(workbook);
        return false;
    }

    // 第六步：保存并释放资源
    if (workbook_close(workbook) != LXW_NO_ERROR) {
        qCritical() << "TableToExcel: 保存Excel文件失败！";
        return false;
    }

    qInfo() << "TableToExcel: 导出成功！文件路径：" << saveFile;
    return true;
}
