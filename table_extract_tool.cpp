#include "table_extract_tool.h"


lxw_workbook *spt_new_workbook(const char* filename)
{
    return workbook_new(filename);
}

void spt_free_workbook(lxw_workbook* workbook)
{
    workbook_close(workbook);
}

bool spt_add_worksheet_of_table(lxw_workbook* workbook,
                            const char* sheet_name,
                            TableInfo* table_info)
{
    lxw_worksheet *worksheet = workbook_add_worksheet(workbook, sheet_name);
    if(NULL == worksheet) {
        return false;
    }
    //每列最长的字符串的字符数
    std::vector<int> max_char_len_ofline_ofcol(table_info->colCount(),8);
    for(int i = 0; i < table_info->rowCount(); ++i) {
        for(int j = 0; j < table_info->colCount(); ++j) {
            const TableInfo::UnitInfo& unit = table_info->units[i][j];
            if(!unit.u) continue; //本单元格被其他单元格合并了
            lxw_format* format = workbook_add_format(workbook);
            const char* unit_text;

            //获取单元格文本内容
            QString text_impl;
            HorLine_Base* hline = unit.u->getFirstLine();

            bool spanCol = (unit.start_col != unit.end_col);

            int n_char_col = 0;

            while(hline) {
                auto lineText = hline->get_merged_line_text(false);
                if(!spanCol) {
                    //计算字符数
                    for(auto ch : lineText) {
                        if(ch.unicode() >= 0x4e00 && ch.unicode() <= 0x9fff) {
                            //中文字符
                            n_char_col += 2;
                        } else {
                            ++n_char_col;
                        }
                    }
                }
                text_impl += lineText;
                hline = hline->getNextLine();
                //if(hline) text_impl += '\n';
            }

            max_char_len_ofline_ofcol[j] = std::max(max_char_len_ofline_ofcol[j],n_char_col);

            auto stdstr_unit_text = text_impl.toStdString();
            unit_text = stdstr_unit_text.c_str();

            if(unit.start_col != unit.end_col || unit.start_row != unit.end_row) {
                //需要合并单元格
                worksheet_merge_range(worksheet,unit.start_row,unit.start_col,
                                      unit.end_row,unit.end_col,unit_text,format);
            } else {
                worksheet_write_string(worksheet,unit.start_row,unit.start_col,unit_text,format);
            }
        }
    }

    for(int j = 0; j < table_info->colCount(); ++j) {
        //调整列宽
        worksheet_set_column(worksheet,j,j,max_char_len_ofline_ofcol[j],NULL);
    }

    return true;
}
