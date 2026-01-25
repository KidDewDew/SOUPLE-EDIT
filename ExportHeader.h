#ifndef EXPORTHEADER_H
#define EXPORTHEADER_H
//ExportHeader.h用来导出一些简单的类
//不依赖其他自定义头文件

#include <QColor>
#include <QMap>
#include <QVariant>
#include "helper.h"
//#include "Obj_Page.h"
//#include "wordpage_vline.h"
#include <ranges>

#define Default_Page_LeftRightMargin_cm 3.6
#define Default_Column_Spacing_cm 1.6

class Obj;
class WordPage_VLine;
class Obj_Page;

// FlowAttacher
// 内容流附着类
// 典型的，它被Turnback(撤回/重做符)继承。
class FlowAttacher {
public:
    OBJID_t attach_obj_id;    //附着的obj id
    int flow_position;    //操作位于行内容流的位置
};

// 内容占位标识
class ContentPlaceHolder {
    ContentPlaceHolder *left = 0,*right = 0;
};

// 页面栏
struct PageColumn {
    Obj *leftLine, *rightLine; //左右标线 保证可以向下转换为wordpage_vline。请使用static_cast以避免多态开销。
};

//Souple文档中的页面类
//页面需要指定上、下边距
struct Page {
    //int update_rest_num = 0;
public:
    float page_left_margin = (float) Helper::cm2pixel(Default_Page_LeftRightMargin_cm);
    float page_right_margin = (float) Helper::cm2pixel(Default_Page_LeftRightMargin_cm);
    unsigned char page_type = Helper::NoFormat_Page;
    float width,height,topMargin,bottomMargin;
    float top_y; //顶部在文档中的y坐标
    int index; //页码
    int rotation = 0; //[旋转 取0 90 180 270中的一个]
    QColor background = Qt::white; //背景色
    Page *next_page = 0, *prev_page = 0;
    Obj_Page *obj_page = 0;
    std::vector<PageColumn> columns; //分栏信息
    void dealLayout();
    // 返回以leftLine为左标线的hline的下一栏
    std::optional<PageColumn> getNextColumn(Obj* leftLine) const noexcept;
    // 返回以leftLine为左标线的hline的上一栏
    std::optional<PageColumn> getPrevColumn(Obj* leftLine) const noexcept;
    float getContentHeight() const noexcept {
        return height - topMargin - bottomMargin;
    }
    float getTopLineY() const {
        return top_y + topMargin;
    }
    float getBottomLineY() const {
        return top_y + height - bottomMargin;
    }
    int getLeftLineColumnIndex(Obj* leftLine) const noexcept {
        for(auto[i,column] : columns | std::views::enumerate)
            if(column.leftLine == leftLine) return i;
        return -1;
    }
    int get_x_of_column_id(float x) const noexcept;
    void setWordPageColumnNum(int num); // 设置页面分栏数量
    void initColumnWidthAndSpacing(); //初始化各栏的宽度和间距
};

struct PCPos {
    const Page* page;
    int column;
    //PCPos(Page* page,int colunm)
    void print() {
        qDebug() << "PCPos:" << page->index << column;
    }

    // 减法运算; 差一页算差距 1， 差
    // int operator-(const PCPos& pos2) const noexcept {
    //     if(page == pos2.page) return column - pos2.column;
    //     else if(page->index > pos2.page->index) {
    //         return
    //     }
    // }

    bool operator<(const PCPos& pos2) const noexcept {
        if(page->index < pos2.page->index) return true;
        if(page->index > pos2.page->index) return false;
        return column < pos2.column;
    }

    bool operator>(const PCPos& pos2) const noexcept {
        if(page->index > pos2.page->index) return true;
        if(page->index < pos2.page->index) return false;
        return column > pos2.column;
    }

    bool operator>=(const PCPos& pos2) const noexcept {
        return !(*this < pos2);
    }

    bool operator<=(const PCPos& pos2) const noexcept {
        return !(*this > pos2);
    }

    bool operator==(const PCPos& pos2) const noexcept {
        return page == pos2.page && column == pos2.column;
    }
};

struct Path_Action {
    enum {MoveTo=0,LineTo,BezierTo,Unknown};
    uchar type;
    float x,y;
    Path_Action(uchar type,float x,float y)
        :type(type),x(x),y(y){}
    QVariantMap toVM() const {
        return {{"x",x},{"y",y},{"t",enum2str()}};
    }
    constexpr char enum2str() const {
        if(type == MoveTo) return 'M';
        else if(type == LineTo) return 'L';
        else if(type == BezierTo) return 'B';
        else return '.';
    }
};

#endif // EXPORTHEADER_H
