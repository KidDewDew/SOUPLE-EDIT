#ifndef TABLELINE_H
#define TABLELINE_H

#include "horline_base.h"
#include "free_tableunit.h"

class TableLine;

// 表格数据结构。一个表格的多个TablePart共享一个TableInfo(ti)
struct TableInfo : public Signal_Receiver{
    // 获取表格行数
    int rowCount() const;
    // 获取表格列数
    int colCount() const;
    // 获取指定单元格的文本（row=行号，col=列号，从0开始）
    QString getCellText(int row, int col) const;
    // 其他已有成员...
    struct UnitInfo {
        int start_col,start_row; //所在(合并)单元格的左上角的列、行坐标(index from 0)
        int end_col,end_row; //所在(合并)单元格的右下角的列、行坐标(index from 0)
        Free_TableUnit *u = 0;
    };

    TableLine *firstLine, *endLine;
    unsigned char alignMode;
    float width,height;
    float table_top_margin = Helper::cm2pixel(0.1);
    std::vector< std::vector< UnitInfo > > units;
    std::vector< float > colWidths, rowHeights; //列宽、行高
    std::vector< int > control_rows; //控制行信息
    bool tableActive = false;
private:
    virtual int dealSignal(int signal,const std::variant<bool,int,float,double,QString>& arg)
    {
        if(signal == Helper::CONT_CHANGED) {
            bool cont = std::get<bool>(arg);
            if(cont) tableActive = true;
            else {
                tableActive = false;
            }
            return 1;
        }
        return 0;
    }
};

// 请确保TableLine和TableLine_Iversion同步修改。

// 表格的一行。如你所见，它继承自AnchorObj_HLine，这让它可以和AnchorObj_HLine及其他派生类很好地配合。
class TableLine : public AnchorObj_HLine, public Signal_Receiver
{
    friend class TableFactory;
    friend class Pdf2Souple;
    // friend void mergeAndCreateTables(auto);
    // template <typename T1,typename T2>
    // friend void analyseTable(T1 page, T2 old_page);
public:
    TableLine();
    virtual void dealLayout() override;
    virtual void updateDataToQmlItem(QQuickItem*) override;
    virtual int dealCommandFromQmlItem(int command,const QVariant& arg) override;
    // virtual HorLine_Base *getNextLine() const noexcept override {
    //     return nextLine;
    // }
    // virtual HorLine_Base *getPrevLine() const noexcept override {
    //     return hline->as<HorLine_Base*>();
    // }
    // virtual void setNextLine(HorLine_Base* l) noexcept override {
    //     nextLine = l;
    // }
    // virtual void setPrevLine(HorLine_Base* l) noexcept override {
    //     hline = l;
    // }
    virtual ~TableLine() { hash_hline.remove(getName()); }
    virtual int dealSignal(int signal,const std::variant<bool,int,float,double,QString>& arg) override {
        return 0;
    }
    virtual QQuickItem* generateQmlItem() override {
        return uiPool_TLine::fetchItem();
    }
    virtual void discard_qmlItem() override {
        uiPool_TLine::returnItem(qmlItem);
        qmlItem = 0;
    }
    virtual QString __dstr() const noexcept override {
        return QString("TableLine id=%1 name=%2").arg(id).arg(name);
    }
    virtual void qt_paint(QPainter& painter,Page* page) override;
private:
    //float topMargin = 0; //topMargin仅仅对第一行的TableLine有意义。
    int16_t row; //行索引 index from 0
    TableInfo *table_info = 0;
    //std::vector<Free_TableUnit*> table_units;
    //int tline_id; //与anchorObj_hline不同，不允许修改名称。名称总为："T" + tline_id
public:
    static inline uint32_t s_tline_count = 0;
private:
    static constexpr char __UINAME__[] = "TLine";
public:
    typedef UIItemPool<__UINAME__,10> uiPool_TLine; //ui控件池
};

//【暂时废弃】参考FitLine，兼容水平标线。
//TableLine的块内标线特化
//这样实现是有原因的，毕竟两种定位的TableLine，一个需要跨栏跨页，一个则不需要。
class TableLine_Iversion : public BlockInner_HorLine, public Signal_Receiver
{
    friend class TableFactory;
    friend class Pdf2Souple;
public:
    virtual void dealLayout() override;
    virtual QQuickItem* generateQmlItem() override {
        return TableLine::uiPool_TLine::fetchItem();
    }
    virtual void discard_qmlItem() override {
        TableLine::uiPool_TLine::returnItem(qmlItem);
        qmlItem = 0;
    }
};

#endif // TABLELINE_H
