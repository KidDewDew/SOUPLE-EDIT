#ifndef TABLEFACTORY_H
#define TABLEFACTORY_H

#include "souplemanager.h"
#include "tableline.h"

///本文件提供对表格的创建、操作功能。
///但不涉及表格解析。

struct TableUnitIndex {
    int row,col;
};

// 表格工厂
class TableFactory {
public:

    enum { Merge_Left,Merge_Right,Merge_Above,Merge_Below };

    //[静态方法] 创建一个n_row x n_col的表格。
    static TableInfo* newTable(float width,int n_row,int n_col) {

        qDebug() << "TableFactory::newTable(" << width << n_row << n_col;

        TableInfo *ti = new TableInfo;
        ti->alignMode = Helper::AlignHCenter;
        ti->colWidths.resize(n_col,width/n_col);
        ti->rowHeights.resize(n_row,50);
        ti->width = width;
        ti->height = n_row * 50;
        ti->control_rows.resize(n_row);
        for(int i = 0; i < n_row; ++i)
            ti->control_rows[i] = i; //因为没有跨行单元格，所以每行都是控制行
        //ti->control_rows[2] = -1;
        //ti->control_rows[3] = 2;
        ti->units.resize(n_row,std::vector<TableInfo::UnitInfo>(n_col));
        for(int i = 0; i < n_row; ++i)
            for(int j = 0; j < n_col; ++j) {
                Free_TableUnit *u = new Free_TableUnit;
                SoupleManager::registerObj(u);
                u->setSignalReceiver(ti);
                u->tableinfo = ti;
                ti->units[i][j] = {.start_col = j,.start_row = i,.end_col = j,.end_row = i,.u = u};
            }
        //ti->units[3][2].start_row = 2;
        //ti->units[2][2].u = 0;
        //创建 TableLine
        TableLine* lastLine = 0;
        for(int i = 0; i < n_row; ++i) {
            auto line = new TableLine;
            SoupleManager::registerObj(line);
            if(i == 0) ti->firstLine = line;
            else if(i+1 == n_row) ti->endLine = line;
            //line->hline = lastLine;
            line->setAnchorLastHLine(lastLine);
            line->setLogicLastHLine(lastLine);
            line->table_info = ti;
            line->row = i;
            lastLine = line;
        }
        return ti;
    }

    //[静态方法] 检查第row行是否是控制行，如果是还会找出控制的行范围。[控制行 ~所有单元格都由该行来布局的行]
    static bool checkThenFindControlRow(TableInfo* ti,int row,int *top_control_row) {
        auto& rowUnits = ti->units[row];
        for(auto u : rowUnits) {
            if(u.end_row > row) return false;
        }
        while(true) {
            auto& rowUnits = ti->units[row];
            bool ok = true;
            for(auto u : rowUnits) {
                if(u.start_row < row) {
                    ok = false;
                    row = u.start_row;
                }
            }
            if(ok) break;
        }
        *top_control_row = row;
        return true;
    }

    //[静态方法] 根据units，计算并赋予控制行信息
    static void calcControlInfo(TableInfo *ti) {
        if(ti->control_rows.size() != ti->rowCount())
            ti->control_rows.resize(ti->rowCount());
        int noControl_row = -1;
        for(int i = 0; i < ti->rowCount(); ++i)
        {
            auto& row = ti->units[i];
            int cover_n = 0;
            for(int j = 0; j < ti->colCount(); ++j) {
                if(row[j].u) {
                    cover_n += row[j].end_col - row[j].start_col + 1;
                }
            }
            if(cover_n == ti->colCount()) {
                //ti->control_rows[i]
                if(noControl_row == -1) ti->control_rows[i] = i;
                else {
                    ti->control_rows[i] = noControl_row;
                    noControl_row = -1;
                }
            } else {
                ti->control_rows[i] = -1;
                if(noControl_row == -1)
                    noControl_row = i;
            }
        }
    }

    //[静态方法] 获取unit的索引
    static TableUnitIndex getIndex(TableInfo* ti,Free_TableUnit* unit) {
        return {};
    }

    //[静态方法] 合并单元格，上下左右合并。
    static void mergeUnit(TableInfo* ti,TableUnitIndex index,unsigned char dir) {

    }

    //[静态方法] 在row行前面插入行。
    static void insertRow(TableInfo* ti,int row) {

    }

    //[静态方法] 在col列前面插入列。
    static void insertColumn(TableInfo* ti,int col) {

    }

    //[静态方法] 删除行
    static void dropRow(TableInfo* ti,int row) {

    }

    //[静态方法] 删除列
    static void dropColumn(TableInfo* ti,int col) {

    }
};

#endif // TABLEFACTORY_H
