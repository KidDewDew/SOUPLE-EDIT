#ifndef PDF2SOUPLE_H
#define PDF2SOUPLE_H

#include <fpdf_text.h>
#include <fpdf_doc.h>
#include <fpdf_annot.h>
#include <fpdf_edit.h>
#include <fpdf_ext.h>
#include <fpdf_attachment.h>
#include <fpdf_sysfontinfo.h>
#include <fpdfview.h>
// #include <fpdf_dataavail.h>
//#include <fpdf_
#include <QObject>
#include <QRectF>
#include "soupleserializer.h"
#include "anchorobj_flowtext.h"
#include "anchorobj_phrect.h"
#include "anchorobj_vline.h"
#include "anchorobj_phright.h"
#ifndef ANCHOROBJ_HLINE_H
#include "anchorobj_hline.h"
#endif
#include "anchorobj_image.h"
#include <QFuture>
#include <QImage>
#include "tableline.h"
#include "TT_Str.h"
#include "anchorobj_rich.h"

#ifndef MACRO_PDF2SOUPLE
#define MACRO_PDF2SOUPLE
#define Y_LINEOBJ_MAX_MARGIN 2
#define Y_LINEOBJ_MAX_MARGIN_cm 0.053
#define X_LINEOBJ_MAX_MARGIN 5
#define X_LINEOBJ_MAX_MARGIN_cm 0.053
#define X_COVER_OFFSET 2
#define X_COVER_OFFSET_cm 0.053
#define X_COVER_OFFSET_FOR_TEXT 10 //对于两端均文字的情况，只有重叠达到10像素才认为是真重叠
#define X_COVER_OFFSET_FOR_TEXT_cm 0.265
#define HEADER_JUDGE_OFFSET 3 //页眉页脚误差
#define SCALE_OFFSET_JUDGE_HCENTER 0.05 //居中判定时的误差比例
#define TEXT_MERGE_OFFSET 0.4 //文字水平合并的距离最大值
#define PHRECT_MIN_WIDTH 0.1 //中间插入的占位矩形最小宽度
#define PHRECT_MIN_WIDTH_cm 0.004 //中间插入的占位矩形最小宽度
#define PARA_MIN_SPACING_OFFSET_cm 0.2  //段落的最小行距差
#define GLUE_RIGHT_MARGIN_OFFSET_cm 0.3 //认为要加入Glue_Right的行去除换行符后右边距的最大差距
#define GLUE_RIGHT_MAX_MARGIN_cm 1.5 //Glue_Right最大宽度
#define GLUE_RIGHT_SAME_MIN_NUM 2    //最小多少行...才可以添加Glue_Right
#define PH_RIGHT_OFFSET_cm 0.052
#define MAX_LINE_WIDTH_pt 4    //认为线宽最多4pt
#define LINE_INTERSECT_OFFSET 5 //多大间距认为线条相交
#define LINE_SAME_OFFSET_cm 0.06625 //两条线坐标相同的误差
//#define BGRECT_ANALYSE_MIN
#endif

//Pdf2Souple: pdf到souple转换类
class Pdf2Souple: public QObject
{
    Q_OBJECT
public:

    struct PDFOBJ;
    struct Souple_Area;
    struct PDFOBJ_TablePart;
    struct PDFOBJ_FramePart;

    // [2025/8/3 创建] 用于表达PDF解析过程中的表格
    // 该表格不记录列数。到最后才能确定列数。
    // units[y][x] = null ~该单元格被合并了。(右下角单元格为控制单元格)
    struct PDFTable {
        typedef std::vector<std::shared_ptr<PDFOBJ>> Unit_Content;
        float x,y,width; //左上角坐标，宽度
        unsigned char border_style; //边框样式 [目前用不到]
        std::vector<float> rowHeights; //行高
        //std::vector<std::vector<std::optional<Unit_Content>>> units; //单元格信息
        std::vector<TableLine*> tablelines;
        std::vector<std::vector<Free_TableUnit*>> row_units;
        //各行的单元格。注：与Tableinfo的units有区别。 跨行单元格，依旧放在最下方的行内。
        int rowCount() const noexcept {
            return row_units.size();
        }
    };

    struct PDFRef_Rect {
        std::shared_ptr<PDFOBJ> obj_ref; //引用的PDFOBJ_Path
        float x,y,w,h;
        float &x1=x,&y1=y,x2,y2;
    };

    // [2025/8/3 创建该结构体 用于降低页面解析时传递数据的复杂度]
    // 2025/11/10 增加ref_rects，统一解析矩形留后用。
    struct PDFPage {
        unsigned char page_type;
        std::vector<std::shared_ptr<PDFOBJ>> all_objs;  //该页的所有PDF对象列表
        std::vector<std::shared_ptr<Souple_Area>> areas; //该页划分出的Souple_Area列表
        std::vector<std::shared_ptr<PDFOBJ_TablePart>> table_parts; //正在解析的表格
        std::vector<std::shared_ptr<PDFOBJ_FramePart>> frame_parts; //正在解析的Frame
        std::vector<std::shared_ptr<PDFRef_Rect>> ref_rects; //引用的矩形列表
        HorLine_Base* first_hline = 0;  //第1条水平线
        AnchorObj_HLine* the_last_hline_to_continue = 0ull;
        float page_left_margin,page_top_margin,page_width,page_height; //该页的坐标、宽高
        int page_index;
        Page *souple_page;
    };



    struct PDFOBJ { //存储pdf obj
        bool visible = true;
        QRectF rect;
        FPDF_PAGEOBJECT page_obj;
        //Obj* generated_obj = 0;
        int render_id; //渲染id render_id越大，表明该obj渲染越靠后，即深度越小。
                    //目前，默认文本层深度均为1e9，即总在顶层（基本没有pdf会拿有填充色的形状或图片去覆盖文本的）
                    //后期如果要解决该问题，请考虑从obj层先读取一遍赋予每个char一个深度，再去合并相邻且深度一致的文本。
        virtual ~PDFOBJ() = default; //启用多态
        virtual void print() = 0;
        virtual AnchorObj* toAnchorObj(HorLine_Base* hline,float page_top_margin,float& adjustWidth) { return 0; }
        virtual FreeObj* toFreeObj() { return 0; }
        virtual bool selfHBlock() { return false; } //是否必须独占一个HBlock
        //virtual void property_sync() {} //属性追溯
    };


    /**
     * @brief The PDFOBJ_PrePHRect class
     *        [2025/11/28新添加]
     * 为了保证文本的行完整性(不被列切分算法切开)，
     * 允许在解析之初向文档流中就加入PHRect。之后必然被创建为AnchorObj_PHRect.
     */
    struct PDFOBJ_PrePHRect : public PDFOBJ
    {
        virtual void print() override {
            qDebug() << "PDFOBJ_PHRect 预占位矩形. width=" << rect.width();
        }
        virtual AnchorObj* toAnchorObj(HorLine_Base* hline,float page_top_margin,float& adjustWidth)
            override;
    };


    /**
     * @brief The PDFOBJ_FramePart class
     *        存储解析过程中的Frame背景框的部分
     *        这种情况与PDFOBJ_TablePart类似。
     */
    struct PDFOBJ_FramePart : public PDFOBJ {
        char fillMode; //背景填充类型
        QColor fillColor; //填充颜色
        QString fillSrc; //填充图片路径
        float lineWidth;
        float radius; //圆角半径
        QColor borderColor;
        std::vector<std::shared_ptr<PDFOBJ>> invisible_items;
        PDFPage* of_page;

        // 以下属性，在merge和create时才会赋予。
        HorLine_Base* topLine = 0, *bottomLine = 0;
        float topMargin,bottomMargin;

        // 撤回
        void turnback() noexcept {
            for(auto& pdfobj : invisible_items) {
                pdfobj->visible = true;
                of_page->all_objs.push_back(pdfobj);
            }
        }
        void print() override {
            qDebug() << "FramePart:" << rect << fillColor << borderColor;
        }
    };

    // 富内容or区间框，注意它和背景框的不同之处
    // rich: 顺序排列
    // area: 无序排列
    struct PDFOBJ_Rich_or_Area : public PDFOBJ {
        char fillMode; //背景填充类型
        bool isRich = true; //是rich吗？true:是 false:不是rich，是area
        QColor fillColor; //填充颜色
        QString fillSrc; //填充图片路径
        float lineWidth;
        float radius; //圆角半径
        QColor borderColor;
        std::vector<std::shared_ptr<PDFOBJ>> objs_inside; //内部的pdfobjs
        virtual void print() {
            qDebug() << "PDFOBJ_Rich fillColor=" << fillColor;
        };
        virtual AnchorObj* toAnchorObj(HorLine_Base* hline,float page_top_margin,float& adjustWidth) { return 0; }
        virtual FreeObj* toFreeObj() { return 0; }
    };



    /**
     * @brief PDFOBJ_TablePart
     * ///// delete: 实际上，表格由PDFPage中的PDFTable来管理。
     * 但为了正确地建立布局，必须为每页的表格部分创建PDFOBJ。
     * PDFOBJ_TablePart仅包含了该部分表格的TableLine。
     * 解析时，遇到PDFOBJ_TablePart应当单独放到一个HBlock中。(独占)
     * 创建Souple对象时，不需要做任何创建操作。只需要设置TableLine的一些属性。
     * 注意，在解析pdf期间，TableLine的x y width height contentTop都按照实际意义来设定。不受dealLayout影响。
     */
    struct PDFOBJ_TablePart : public PDFOBJ { //(部分)表格
        bool isHeaderPart; //该部分是否是整个表格的开头部分
        std::vector<TableLine*> tablelines;
        std::vector<std::vector<Free_TableUnit*>> row_units;
        //TableInfo *tableinfo;
        bool selfHBlock() override { return true; } //独占
        virtual void print() override {
            qDebug() << "PDFOBJ: TablePart;";
        }
    };

    // 注意
    struct PDFOBJ_RICH : public PDFOBJ { //锚定型内容框
        Anchorobj_Rich* rich_obj;
        virtual void print() override {
            qDebug() << "PDFOBJ: Rich;";
        }
        virtual AnchorObj* toAnchorObj(HorLine_Base* hline,float page_top_margin,float& adjustWidth)
        override
        {
            adjustWidth = rich_obj->width;
            rich_obj->x = rect.left();
            rich_obj->width = rect.width();
            rich_obj->height = rect.height();
            rich_obj->vAlignMode = Helper::AlignBottom;
            rich_obj->vAlignOffset = rich_obj->y + rich_obj->height + page_top_margin
                                     - hline->y;
            rich_obj->y = rect.top() + page_top_margin;
            return rich_obj;
        }
    };

    struct PDFOBJ_TEXT : public PDFOBJ {
        bool isFill,isStroke;
        QString text,family;
        float ascent; //存储top到基线的距离
        float strokeWidth; //描边宽度
        QFont font;
        QColor stroke_color,fill_color;
        void print() override
        { qDebug() << "TEXT" << text; }
        const QFont& toFont() const {
            return font;
        }
        virtual AnchorObj* toAnchorObj(HorLine_Base* hline,float page_top_margin,float& adjustWidth) override;
        virtual FreeObj* toFreeObj() override;
    };

    //图像
    struct PDFOBJ_IMAGE : public PDFOBJ {
        //.?.?.?.
        //QImage image;
        QString source;
        void print() override { qDebug() << "IMAGE"; }
        virtual AnchorObj* toAnchorObj(HorLine_Base* hline,float page_top_margin,float& adjustWidth) override;
        virtual FreeObj* toFreeObj() override;
    };

    //路径
    struct PDFOBJ_PATH : public PDFOBJ {
        QRectF t_rect; //路径紧边框
        QList<Path_Action> list_path_actions;
        bool fill, stroke;
        QColor fillColor,strokeColor;
        float lineWidth;
        void print() override
        {
            qDebug() << "PATH of " << list_path_actions.size() << " actions.";
        }
        AnchorObj* toAnchorObj(HorLine_Base* hline,float,float& adjustWidth) final override;
        FreeObj* toFreeObj() final override;

        struct Line {
            float x1,y1,x2,y2;
        };

        // float getLineWidth_ifLine() noexcept {
        //     float solid = std::min(rect.width(),rect.height());
        //     if(stroke) {
        //         return
        //     }
        // }

        // toLines
        // @brief 尝试把该路径转换为一条或多条线条，前提是该路径近似是矩形（包括圆角矩形、奇形怪状的矩形）
        // 注：该方法返回的Line默认是考虑了线宽的！
        // 显然，该方法的能力强于toSolidRect方法，能够识别一切近似矩形。缺点:...
        template<TT_Str tt = TStr(" ")>
        bool toLinesIfRect(CanPushback<Line> auto& lines);

        QColor getLookColor() const noexcept {
            if(fill) return fillColor;
            return strokeColor;
        }

        // 判断path2和自己的样式是不是一样（看起来一样）
        bool isLookLike_ifrect(const PDFOBJ_PATH* path2) {
            // qDebug() << this->fill << path2->fill;
            // qDebug() << this->stroke << path2->stroke;
            // qDebug() << this->fillColor << path2->fillColor;
            // qDebug() << this->strokeColor << path2->strokeColor;
            if(this->getLookColor() != path2->getLookColor()) return false;
            return true;
        }

        // 转换为实心矩形(一根线也算作实心矩形)，返回是否转换l成功。
        template<TT_Str tt = TStr(" ")>
        bool toSolidRect(float& x1,float& y1,float& x2, float& y2, uint8_t* numLines = 0) const;
    };

    struct Souple_Area { //Souple区域
        float left,right,ex_left,ex_right; //左 右 拓展左 拓展右
        float top_y,bottom_y; //记录最底部y坐标 [注意]
        std::shared_ptr<Souple_Area> top_area = 0;//上方的area
        AnchorObj_HLine *top_line, *bottom_line;
        // bool operator<(Souple_Area const& area2) const {
        //     return
        // }
        //记录其顶部和底部的HLine，方便之后的Souple_Area进行锚定
    };

    struct HBlock {  //水平锚定块
        //int belonging = -1; //属于哪个锚定区域
        bool has_belonging = false;
        float ex_left = 0,ex_right; //扩展左右边界
        QRectF rect;
        std::vector<std::shared_ptr<PDFOBJ>> objs;
    };


    Pdf2Souple(QObject* parent):QObject(parent)  {}

    Q_INVOKABLE static void loadPdf(const QString& pdf_filename);

private:

    //static inline void* temp_memory;

    enum {Left,Right,Top,Bottom};
    enum {Sign_Cover=1,Sign_Expand_Width=2,Sign_Must_Left_Align=4,Sign_No_Cross=8};

    static QRectF getTextFullBound(QFontMetricsF fm,const QString& text,const QRectF& tight_rect);

    // 解析pdf的主过程函数
    static void imp_loadPdf(const QString& pdf_filename);

    // 解析pdf的一页
    static void imp_analysePdfPage(std::shared_ptr<PDFPage> page,std::shared_ptr<PDFPage> prev_page);

    // 尝试解析pdf的某页为word风格page
    static bool analyseWordPage(std::shared_ptr<PDFPage> page,std::shared_ptr<PDFPage> prev_page);

    // 解析页眉页脚
    static void analyseHeader(std::shared_ptr<PDFPage> page);

    // 根据pdfium对象创建PDFOBJ，只适用非文本类型。
    static std::shared_ptr<PDFOBJ> readPdfObj(FPDF_PAGEOBJECT fpdf_pageobj,FPDF_TEXTPAGE textpage,float page_width,float page_height);

    // static std::vector<std::shared_ptr<PDFOBJ>> imp_findPDFOBJAlignON(int onWhat,
    //                                     std::vector<std::shared_ptr<PDFOBJ>>& all_objs,
    //                                     int index,uchar sign = 0);

    // 处理文本层
    static void imp_dealText(FPDF_TEXTPAGE textpage,std::vector<std::shared_ptr<PDFOBJ>>& list,float page_width,float page_height);

    // 预处理：路径切分、合并
    static void imp_path_doSomeMerge(std::vector<std::shared_ptr<PDFOBJ>>& objList);

    //判断两个矩形是否垂直方向上相交
    static inline bool checkRectVCross(const QRectF& r1, const QRectF& r2) {
        return r1.top() < r2.bottom() && r2.top() < r1.bottom();
    }

    //根据objs创建水平块
    template<TT_Str tt = TStr("multi2rich=true")>
    static void createHBlocks(const Iterable<std::shared_ptr<PDFOBJ>> auto& objs,
                              float page_width,CanPushback<HBlock> auto& blocks,
                              bool enable_horizontal_break = true);

    //根据objs创建水平块，这是对word布局的特化版本。在大多情况下都建议使用该函数代替createHBlocks。
    template<TT_Str tt = TStr("multi2rich=true")>
    static bool createHBlocks_specForWord(const RandomAccessCont<std::shared_ptr<PDFOBJ>> auto& objs,
                        CanPushback<HBlock> auto& blocks);

private:
    template<TT_Str tt = TStr("multi2rich=true")>
    static bool impl_createHBlocks_specForWord(const RandomAccessCont<std::shared_ptr<PDFOBJ>> auto& objs,
                                          CanPushback<HBlock> auto& blocks,
                                          float lineYOffset,float lineXOffset);
public:

    // 为水平标线进行布局，同时更新last_hline。
    static void layoutHLine(std::shared_ptr<PDFPage> page,AnchorObj_HLine* hline,AnchorObj_HLine*& last_hline,
                            HBlock& bk,AnchorObj_VLine* leftLine,AnchorObj_VLine* rightLine);

    // 该函数生成一个HLine
    // 功能类似于layoutHLine，但它可以处理非AnchorObj_HLine。
    // 显然，当你尝试创建块内标线时，应当调用该函数。
    static void generateHorLine(const PDFPage* page,HorLine_Base *hline,const HBlock& bk,
                                std::optional<float> leftx,std::optional<float> width);

    /**
     * @brief createRich
     *  根据HBlock创建富文档块。
     *  参数vAlignMode contentHeight firstLine传入对象相应的成员的指针。
     *  根据设计，本函数仅限于确定富文本块的vAlignMode和firstLine(由firstLine即可推出所有line)。
     *  参数fixed_rect 传入 空或有效QRectF。如果传入有效QRectF，将按照固定边框来布局；
     *  否则将创建“完全贴合”的边框，且默认使用“垂直居中对齐”。
     *  注意，此种情况下，内容框的坐标尺寸将赋予到parent。当然，赋予的坐标是页面坐标。
     *  contentHeight似乎无关紧要。。。(错误设计##)
     *  [返回] 是否成功。如果失败，可以尝试创建为自由对象框(todo)。
     *  [模板参数] allowVCross: 允不允许垂直行相交。(采用TT_Str字符串模板，参见头文件TT_Str.h)
     */
    template<TT_Str tt = TStr("allowVCross=false")>
    static bool createRich(const PDFPage* page,Iterable<HBlock> auto& blocks,Obj* parent,
                           unsigned char* vAlignMode,float* contentHeight,
                           HorLine_Base** firstLine,
                           std::optional<QRectF> fixed_rect);

    // 针对页面的analyseTable
    template<class TArg = void>
    static void analyseTable(PDFPage* page, PDFPage* old_page);

    // 根据一个或多个tablepart创建出表格实例
    // @return TableInfo*。可以忽略，已经自动完成了所有注册action。
    template<TT_Str tt = TStr("enable_table_row_num=false")>
    static TableInfo* createTableFromTableParts(
        const Iterable<Pdf2Souple::PDFOBJ_TablePart*> auto& tablepart_list,
        int table_row_num);

    // T参数"inside":是否是在内部提取表格
    // 如表格内嵌的表格、文本框内部的表格。
    // 如果inside=true，则直接创建Table，加入objs。
    // 否则，创建Table_Part加入objs，并在page中记录。
    template<TT_Str tt = TStr("inside=true")>
    static void impl_analyseTable(
        PDFPage* page,
        RandomAccessCont<std::shared_ptr<PDFOBJ>> auto& objs);

    // 合并跨栏跨页的表格
    static void mergeAndCreateTables(Iterable<std::shared_ptr<Pdf2Souple::PDFPage>> auto& pages);

    // 解析页面中的割裂的“跨栏跨页背景框”、可以预先确定的富内容框、area框
    // frame_parts
    // 注意，解析出的frame_parts允许回撤；
    // 解析frame_parts时，函数会设置其背景框pdfobj的visible为false
    // * 该函数会把解析出的rich、area放入文档流，即to_analyse_objs
    //   并从文档流中删除构成rich、area和frame_parts-bg的对象。
    //   但不会把frame_parts记录到page内！
    static void analyse_framepart_or_rich_or_area(
        PDFPage* page,
        RandomAccessCont<std::shared_ptr<PDFOBJ>> auto& to_analyse_objs,
        //const RandomAccessCont<float> auto& column_lines,
        RandomAccessCont<std::shared_ptr<PDFOBJ_Rich_or_Area>> auto& rich_or_areas,
        RandomAccessCont<std::shared_ptr<PDFOBJ_FramePart>> auto& frame_parts);

    static bool findFrameTopAndBottomLine(RandomAccessCont<HorLine_Base*> auto& sorted_hlines,
                                          PDFOBJ_FramePart* framepart,float page_y);

    // 合并跨栏跨页的frames
    static void mergeAndCreateFrames(Iterable<std::shared_ptr<Pdf2Souple::PDFPage>> auto& pages);

    // 找出一条路径所有的邻居路径
    // @TArg参数 onlyLine 是否只查找“线条”类型的邻居。
    template<TT_Str TArg = "onlyLine=false">
    static std::vector<std::pair<int,std::shared_ptr<PDFOBJ_PATH>>>
        find_neighbors_of_path(const Iterable<const std::shared_ptr<PDFOBJ>> auto& all_objs,
                           int path_i,std::shared_ptr<PDFOBJ_PATH> path);

    //static void

    //尝试在hline左边插入占位符，该版本针对主流程进行了优化。
    //ori_left:这一行对应的pdf原始left坐标
    //ori_right:这一行对应的pdf原始right坐标
    static void tryInsertPH_onHLineLeft(AnchorObj_HLine* hline,float ori_left,float ori_right);

    // 调用该函数前，确保hline的x,width被正确设置。
    static void tryInsertPH_onHLineLeft(HorLine_Base* hline);

    //从areas中找到area的顶部area
    static void findTopArea(const std::vector<std::shared_ptr<Souple_Area>>& areas,
                            std::shared_ptr<Souple_Area> area,bool span_page = false);


    //尝试根据hline来判断上一行是否需要在末尾插入换行符，如果需要，则插入，返回1；否则，返回0
    //raw_right为pdf文档中上一行原始右边界坐标
    //template <bool Register = true>
    static bool tryLastHlineAppendBreak(AnchorObj_HLine* hline,float raw_right);

    static bool tryLastHlineAppendBreak(HorLine_Base* hline,HorLine_Base* lastHLine);

    //尝试为一串hline插入GlueRight
    //static bool tryInsertGlueRight();

    //判断是否需要插入左占位，即是否需要加上段落标记。如果需要则插入并返回true。
    //template <bool Register = true>
    static bool tryInsertPHLeft(HorLine_Base* hline);

    static void dealHLines_2(AnchorObj_HLine* first_hline);

    //处理pdf内嵌字体
    static void dealEmbedFont(FPDF_FONT ffont);

    //处理pdf字体，返回字体family
    static QString dealFont(const QString& fontName);

    static inline void setSpanPageHLineTopMargin(AnchorObj_HLine* hline,AnchorObj_HLine* top_hline,PDFPage& page)
    {
        hline->topMargin = page.souple_page->getBottomLineY() - top_hline->getContentBottom() + 1e-3;
    }

public:

    struct Shared {
        static inline int pdf_page_number; //pdf页数
        static inline std::weak_ptr<PDFPage> current_page;
    };

    static inline QFuture<void> future_preload; // preload异步返回值


    //PDF解析建议（由用户给予）
    struct PDF_Load_Advice {
        static inline bool hasUniHeader, hasUniFooter;    //是否指定统一的页眉页脚
        static inline float header_margin, footer_margin; //统一页眉页脚margin
        static inline QHash<int,float> spec_header;       //特定页页眉margin
        static inline QHash<int,float> spec_footer;       //特定页页脚margin (若小于0，表示没有）

        void reset() {
            hasUniFooter = hasUniHeader = false;
            spec_footer.clear();
            spec_header.clear();
        }

    };

    struct PDF_Load_Args {
        static inline bool layoutHLine_adjustWidth = true;
    };

};

#ifndef PDF2SOUPLE_TEMPLATE_IMPL_HPP
#include "pdf2souple_template_impl.hpp"
#endif

#ifndef PDF_TABLE_ANALYSE_H
#include "pdf_table_analyse.hpp"
#endif

#endif // PDF2SOUPLE_H
