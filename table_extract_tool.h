#ifndef TABLE_EXTRACT_TOOL_H
#define TABLE_EXTRACT_TOOL_H

///table_extract_tool.h提供线程安全的表格提取为excel文件的方法

#include "tableline.h"
#include "xlsxwriter.h"

/// notice: lxw库对每个线程独立操作workbook是线程安全的。

/**
 * @brief new_workbook
 * 创建一个新的表格文档
 * 保存路径为filename.
 */
lxw_workbook *spt_new_workbook(const char* filename);

/**
 * @brief free_workbook
 * 释放一个表格文档
 */
void spt_free_workbook(lxw_workbook* workbook);


/**
 * @brief add_worksheet_of_table
 * 向表格文档添加一个工作簿，内容为一个给定的表格
 * @param workbook    待添加的表格文档
 * @param sheet_name  sheet名称，如为NULL(0)，则使用默认名称
 * @param table_info  表格结构体
 * @return false:失败 true:成功
 */
bool spt_add_worksheet_of_table(lxw_workbook* workbook,
                            const char* sheet_name,
                            TableInfo* table_info);

#endif // TABLE_EXTRACT_TOOL_H
