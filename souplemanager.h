#ifndef SOUPLEMANAGER_H
#define SOUPLEMANAGER_H

#include <QObject>
#include <QTimer>
#include <set>
#include <QHash>
#include <QTimer>
#include <QQueue>
#include <QMutex>
#include "anchorobj.h"
#include "anchorobj_hline.h"
#include "ExportHeader.h"
#include "selectionmanager.h"
#include "Obj_Page.h"
#include "tableline.h"
#include "turnbackmanager.h"
#include "obj_start_sign.h"
#ifdef Q_OS_WIN32
#include <ranges>
#endif

#ifdef MULTITHREAD_SOUPLEMANAGER
#include <thread>
#endif

#ifdef ULONGLONG_OBJID
    typedef quint64 OBJID_t;
#else
    typedef quint32 OBJID_t;
#endif

#define Soup_Mgr SoupleManager

///宏：MULTITHREAD_SOUPLEMANAGER
///brief: 多线程分离模式
/// 在cmake中启用后，SoupleManager将自动根据线程ID来存储obj。
/// 主工程禁止启用该宏

//处理一次布局的timer间隔
constexpr int TIMER_DEALUI_INTERVAL = 30;
//每次处理的队列内hline最大数量
constexpr int TIMER_DEALUI_MAX_NUM_PER_TIMEOUT = 600;
//每次处理(预)可见数量
constexpr int TIMER_DEALUI_VISIBLE_NUM = 40;
//constexpr int TIMER_SCANUI_VISIBLE_NUM = 200; //[作废]
constexpr int SCAN_POOL_INTERVAL = 4000;
//扫描可见对象数量
constexpr int TIMER_SCAN_NUM_PER_TIMEOUT = 2000; //old_value: 2000
//缓慢地处理全部对象布局数量
constexpr int TIMER_DEALUI_SCAN_NUM = 30;
constexpr int SELECTION_DEAL_INTERVAL = 400;
constexpr float PRE_VISIBLE_RANGE_cm = 21.16; //预可见高度：21.16cm

//SoupleManager类，用于存储和管理souple文档的所有前后端对象
//SoupleManager处理Souple对象的布局行为，管理ui对象的生命周期


struct CMP_for_AnchorObj { //排序仿函数
    bool operator()(const AnchorObj* o1, const AnchorObj* o2) {
        return o1->y < o2->y; //按y坐标排序即可
    }
};


//本类为纯静态类，支持高效的当前文档访问。也支持多文档访问。
class SoupleManager : public QObject
{
    Q_OBJECT
    friend class Souple_PdfSaver;
    friend class SelectionManager;
    friend class Pdf2Souple;
    Q_PROPERTY(float documentWidth READ documentWidth NOTIFY documentWidthChanged FINAL)
    Q_PROPERTY(float documentHeight READ documentHeight NOTIFY documentHeightChanged FINAL)
    Q_PROPERTY(int pageCount READ getPageCount NOTIFY pageCountChanged FINAL)

    struct _PAGE_INF {
        std::vector<Page*> pages; //页面列表，按顺序排列
        std::vector<float> sum_heights; //页面高度前缀和,[0]=page[.., [1] = pages[0].height, ...，用于快速定位obj所在页面
        std::vector<float> sum_margins;
        //页面上边距+下边距的前缀和,[0] = page[0].topMargin, [1] = [0] + page[0].bottom_margin + page[1].top_margin

        //[2025/11/29 add] 分栏分隔线
        //std::list<ColumnsSeparate> columns_separates;
    };
    enum {
        Current_Document=-2
    };
public:
    Q_INVOKABLE float documentHeight() const {
        return edit_height;
    }
    Q_INVOKABLE float documentWidth() const {
        return edit_width;
    }
    Q_INVOKABLE int getPageCount() const {
        return page_inf.pages.size();
    }
signals:
    void documentWidthChanged();
    void documentHeightChanged();
    void pageCountChanged();
private:
    //Q_PROPERTY(bool showHelpLine NOTIFY showHelpLineChanged FINAL)
    class MyEventFilter : public QObject {
    protected:
        bool eventFilter(QObject *watched, QEvent *event) override;
    };
    class STextInputEventFilter : public QObject {
    protected:
        bool eventFilter(QObject *watched, QEvent *event) override;
    };
    static inline MyEventFilter myEventFilter;
    static inline STextInputEventFilter stEventFilter;
public:
    SoupleManager();
    static void requestUpdateLayoutFrom();
    static void requestDealUIWhenViewChanged(float top,float bottom); //可见区域变化时，更新ui
    static void addObj(Obj* obj) {
        hash_id_obj[obj->id] = obj;
        all_objs.push_back(obj);
    }

    static inline bool enableRegister = true;

    static inline int NUM1 = 0,NUM2 = 0,NUM3=0,NUM4=0,NUM5=0;

    static inline void registerObj(Obj* obj) { //注册obj
#ifdef MULTITHREAD_SOUPLEMANAGER
        ThreadBundle* tb = thread_bundle[std::this_thread::get_id()];
        if(! tb->enableRegister) {
            tb->wait_register_obj_list.push_back(obj);
            return;
        }
        tb->hash_id_obj[obj->id] = obj;
        tb->all_objs.push_back(obj);
#else
        if( ! enableRegister) {
            wait_register_obj_list.push_back(obj);
            return;
        }
        hash_id_obj[obj->id] = obj;
        all_objs.push_back(obj);
#endif
    }

    // 一次性注册多个objs，模板展开;
    template<typename...Ts>
    static inline void registerObjs(Ts...objs) { //注册obj
#ifdef MULTITHREAD_SOUPLEMANAGER
        ThreadBundle* tb = thread_bundle[std::this_thread::get_id()];
        if( ! tb->enableRegister) {
            (tb->wait_register_obj_list.push_back(objs),...);
            return;
        }
        ((tb->hash_id_obj[objs->id] = objs),...);
        (tb->all_objs.push_back(objs),...);
#else
        if( ! enableRegister) {
            (wait_register_obj_list.push_back(objs),...);
            return;
        }
        ((hash_id_obj[objs->id] = objs),...);
        (all_objs.push_back(objs),...);
#endif
    }

#ifdef ULONGLONG_OBJID
    static inline Obj* getObjById(qint64 id) noexcept {
#else
    static inline Obj* getObjById(int id) noexcept {
#endif
#ifdef MULTITHREAD_SOUPLEMANAGER
        ThreadBundle* tb = thread_bundle[std::this_thread::get_id()];
        auto it = tb->hash_id_obj.find(id);
        if(it == tb->hash_id_obj.end()) return 0;
        return *it;
#else
        auto it = hash_id_obj.find(id);
        if(it == hash_id_obj.end()) return 0;
        return *it;
#endif
    }

    static inline Obj* getObjById(int document_id,OBJID_t id)
#ifndef MULTITHREAD_SOUPLEMANAGER
    noexcept
#endif
    {
#ifdef MULTITHREAD_SOUPLEMANAGER
        throw std::runtime_error("MULTITHREAD_SOUPLEMANAGER: getObjById(int document_id,int id)");
#endif
        if(document_id == currentDocumentID || document_id == Current_Document) {
            auto it = hash_id_obj.find(id);
            if(it == hash_id_obj.end()) return 0;
            return *it;
        } else {
            IF NOT(all_documents.contains(document_id)) {
                return nullptr;
            } else {
                auto& h = all_documents[document_id]->hash_id_obj;
                auto it = h.find(id);
                if(it == h.end()) return 0;
                return *it;
            }
        }
    }

    //static inline void removeObj(Obj* obj) {  }
    static void init(); //初始化

    Q_INVOKABLE static int sendCommandToData(qint32 data_id,int command,QVariant arg) {
#ifdef MULTITHREAD_SOUPLEMANAGER
        throw std::runtime_error("MULTITHREAD_SOUPLEMANAGER: no sendCommandToData");
#endif
        auto it = hash_id_obj.find(data_id);
        if(it == hash_id_obj.end()) return 0;
        return it.value()->dealCommandFromQmlItem(command,arg);
    }

    Q_INVOKABLE QVariant qmlGetData(qint32 data_id,int dataName) {
#ifdef MULTITHREAD_SOUPLEMANAGER
        throw std::runtime_error("MULTITHREAD_SOUPLEMANAGER: no qmlGetData");
#endif
        auto it = hash_id_obj.find(data_id);
        if(it == hash_id_obj.end()) return "null";
        return it.value()->qmlGetData(dataName);
    }

    Q_INVOKABLE void requestDeleteObj(qint32 data_id) {
#ifdef MULTITHREAD_SOUPLEMANAGER
        throw std::runtime_error("MULTITHREAD_SOUPLEMANAGER: no requestDeleteObj");
#endif
        auto it = hash_id_obj.find(data_id);
        if(it != hash_id_obj.end() && it.value()) {
            it.value()->removeSelf(true);
        }
    }

    // 暂停：很多情况下可以用定格来代替
    Q_INVOKABLE void pauseDeal() {
        paused = true;
    }

    // 解除暂停
    Q_INVOKABLE void continueDeal() {
        paused = false;
    }

    // 定格：停止处理布局
    Q_INVOKABLE void freeze_frame() {
        frozen = true;
    }

    // 解除定格
    Q_INVOKABLE void unfreeze_frame() {
        frozen = false;
    }

    // 请求撤回
    Q_INVOKABLE void request_undo();

    // 请求重做
    Q_INVOKABLE void request_redo();

    //在hline上创建obj
    Q_INVOKABLE static void createSoupleObj_onHline(const QString& type,qint32 hline_id,bool isInsertOnLeft)
    {
#ifdef MULTITHREAD_SOUPLEMANAGER
        throw std::runtime_error("MULTITHREAD_SOUPLEMANAGER: no createSoupleObj_onHLine");
#endif
#ifdef DEBUG
        qDebug() << "createSoupleObj_onHline(" << type;
#endif
        auto hline_obj = hash_id_obj[hline_id];
        if(! hline_obj) return;
        auto hline = hline_obj->as<HorLine_Base*>();
        if(! hline) return;
        if(isInsertOnLeft) hline->insertOnLeft(createObj(type)->as<AnchorObj*>());
        else hline->insertOnRight(createObj(type)->as<AnchorObj*>());
    }

    //在obj旁边创建obj
    Q_INVOKABLE static void createSoupleObj_byObj(const QString& type,qint32 obj_id,bool isInsertOnLeft) {
#ifdef MULTITHREAD_SOUPLEMANAGER
        throw std::runtime_error("MULTITHREAD_SOUPLEMANAGER: no createSoupleObj_byObj");
#endif
        auto obj = createObj(type);
        auto it = hash_id_obj.find(obj_id);
        if(it == hash_id_obj.end()) return;
        auto aobj = it.value()->as<AnchorObj*>();
        if(! aobj) return;
        if(isInsertOnLeft) aobj->insertOnLeft(obj->as<AnchorObj*>());
        else aobj->insertOnRight(obj->as<AnchorObj*>());
        obj->as<AnchorObj*>()->hline = aobj->hline;
    }

    //创建自由obj
    Q_INVOKABLE static void createSoupleObj_Free(const QString& type,float x,float y) {
#ifdef MULTITHREAD_SOUPLEMANAGER
        throw std::runtime_error("MULTITHREAD_SOUPLEMANAGER: no createSoupleObj_Free");
#endif
#ifdef DEBUG
        qDebug() << "createSoupleObj_Free(" << type << "," << x << "," << y;
#endif
        auto obj = createObj(type);
        obj->x = x, obj->y = y;
    }

    // 创建表格
    Q_INVOKABLE static bool createTable(int n_row,int n_col);

    // 为inner表格特殊处理的createTable
    static bool createTable_inner(int n_row,int n_col,BlockInner_HorLine* innerLine);

    Q_INVOKABLE static void updateViewSize(float top,float bottom) {
        viewUpdated = true;
        view_top = top, view_bottom = bottom;
        //qDebug() << "updateViewSize(" << top << ',' << bottom;
    }

    // 获取更新队列的第一个对象
    static inline Obj* getUpdateQueueFirst() {
        return queue_hline_wait_update.back();
    }

    static inline void requestUpdateHLine(Obj* hline) {
        if(!queue_hline_wait_update.empty()
             && queue_hline_wait_update.front() == hline) return;
#ifdef DEBUG
        //qDebug() << "requestUpdateHLine(" << hline->__dstr();
#endif
        //if(hline)
        //    qDebug() << "requestUpdateHLine(" << hline->id;
        // for(auto obj_inqueue : queue_hline_wait_update) {
        //     if(hline == obj_inqueue) return;
        // }
        queue_hline_wait_update.push_front(hline);
    }

    //static inline QString

    Q_INVOKABLE static inline bool hasCreatedFirstHLine() {
        return AnchorObj_HLine::hash_hline.size() > 0;
    }

    Q_INVOKABLE static qint32 getHLineIdByName(const QString& name);

    Q_INVOKABLE static qint32 getHLineIdByName(int doc_id,const QString& name);

    Q_INVOKABLE static inline void requestRemoveObj(qint32 id) {
        auto it = hash_id_obj.find(id);
        if(it == hash_id_obj.end()) return;
        (*it)->removeSelf(true); //删除对象
    }

    // 尝试创建装饰框
    Q_INVOKABLE static void tryCreateDecorationFrame();

    Q_INVOKABLE static inline Page* addInheritPage() {

#ifdef MULTITHREAD_SOUPLEMANAGER
        //不允许多线程模式的SoupleManager调用该函数
        throw std::runtime_error("MULTITHREAD_SOUPLEMANAGER: no addInheritPage()");
#endif

        int num = SoupleManager::page_inf.pages.size();
        if(0 == num) {
            //addPage()
            return 0;
        }
        auto prev_page = page_inf.pages.back();
        auto new_page = new Page{.width = prev_page->width,.height = prev_page->height,
                                 .topMargin = prev_page->topMargin,
                                 .bottomMargin = prev_page->bottomMargin,
                                 .index=num};

        prev_page->next_page = new_page;
        new_page->prev_page = prev_page;

        new_page->obj_page = new Obj_Page;
        new_page->obj_page->page = new_page;
        registerObj(new_page->obj_page);
        page_inf.pages.push_back(new_page);
        page_inf.sum_heights.push_back((num > 0 ? page_inf.sum_heights.back():0) + new_page->height);
        page_inf.pages.back()->top_y = page_inf.sum_heights.back() - new_page->height;

        if(num > 0) {
            page_inf.sum_margins.push_back(page_inf.sum_margins.back()
                                           + page_inf.pages[num-1]->bottomMargin + new_page->topMargin);
        } else {
            page_inf.sum_margins.push_back(new_page->topMargin);
        }
        edit_height += new_page->height;
        edit_width = qMax(edit_width,new_page->width);
        emit soupleManager->documentWidthChanged();
        emit soupleManager->documentHeightChanged();
        emit soupleManager->newPageAdded(new_page->width,new_page->height,
                                         new_page->topMargin,new_page->bottomMargin); //通知ui增加页面
        emit soupleManager->pageCountChanged();
        new_page->obj_page->initFromPage();

        if(prev_page->page_type == Helper::Word_Page) {
            new_page->page_type = Helper::Word_Page;
            new_page->setWordPageColumnNum(prev_page->columns.size());
        }

        return new_page;
    }

    Q_INVOKABLE static inline Page* addPage(float width,float height,float top_margin,float bottom_margin) {

#ifdef MULTITHREAD_SOUPLEMANAGER
        ThreadBundle* tb = thread_bundle[std::this_thread::get_id()];
        int num = tb->page_inf.pages.size();
        auto new_page = new Page{.width = width,.height = height,.topMargin = top_margin,
                                 .bottomMargin = bottom_margin,.index=num};
        new_page->obj_page = new Obj_Page;
        new_page->obj_page->page = new_page;
        registerObj(new_page->obj_page);
        if(num > 0) {
            tb->page_inf.pages.back()->next_page = new_page;
            new_page->prev_page = tb->page_inf.pages.back();
        }
        tb->page_inf.pages.push_back(new_page);
        tb->page_inf.sum_heights.push_back((num > 0 ? tb->page_inf.sum_heights.back():0) + height);
        tb->page_inf.pages.back()->top_y = tb->page_inf.sum_heights.back() - height;
        if(num > 0) {
            tb->page_inf.sum_margins.push_back(tb->page_inf.sum_margins.back()
                                           + tb->page_inf.pages[num-1]->bottomMargin + top_margin);
        } else {
            tb->page_inf.sum_margins.push_back(top_margin);
        }
        tb->edit_height += height;
        tb->edit_width = qMax(tb->edit_width,width);
        //new_page->obj_page->initFromPage();

        return new_page;
#else
        int num = SoupleManager::page_inf.pages.size();
        auto new_page = new Page{.width = width,.height = height,.topMargin = top_margin,
                                 .bottomMargin = bottom_margin,.index=num};
        new_page->obj_page = new Obj_Page;
        new_page->obj_page->page = new_page;
        registerObj(new_page->obj_page);
        if(num > 0) {
            page_inf.pages.back()->next_page = new_page;
            new_page->prev_page = page_inf.pages.back();
        }
        page_inf.pages.push_back(new_page);
        page_inf.sum_heights.push_back((num > 0 ? page_inf.sum_heights.back():0) + height);
        page_inf.pages.back()->top_y = page_inf.sum_heights.back() - height;
        if(num > 0) {
            page_inf.sum_margins.push_back(page_inf.sum_margins.back()
                                           + page_inf.pages[num-1]->bottomMargin + top_margin);
        } else {
            page_inf.sum_margins.push_back(top_margin);
        }
        edit_height += height;
        edit_width = qMax(edit_width,width);
        emit soupleManager->documentWidthChanged();
        emit soupleManager->documentHeightChanged();
        emit soupleManager->newPageAdded(width,height,top_margin,bottom_margin); //通知ui增加页面
        emit soupleManager->pageCountChanged();
        new_page->obj_page->initFromPage();

        return new_page;
#endif
    }

    Q_INVOKABLE void updatePageTopMargin(int page_index,float margin) {

        if(page_index >= page_inf.pages.size()) return;
        page_inf.pages[page_index]->topMargin = margin;
        //page_inf.pages[page_index]->update_rest_num = 200; //设置更新量
    }

    Q_INVOKABLE void updatePageBottomMargin(int page_index,float margin) {
        if(page_index >= page_inf.pages.size()) return;
        page_inf.pages[page_index]->bottomMargin = margin;
        //page_inf.pages[page_index]->update_rest_num = 200; //设置更新量
    }

    Q_INVOKABLE bool checkHLineValid(const QString& hline_name);
    Q_INVOKABLE bool checkHLineValid(int doc_id,const QString& hline_name);

    Q_INVOKABLE void setShowHelpLine(bool show) noexcept;

    //请求渲染某文档的某页面为图像
    // @param document_id: 文档的id
    // @param page_index: 页码
    // @param: into_image_id 渲染好的图像通过image://provider/${into_image_id}在qml使用
    Q_INVOKABLE static bool request_render_page(int document_id,int page_index,
                                                const QString& into_image_id);

    static Page* getPageByIndex(int page_index) { //从0算起
#ifdef MULTITHREAD_SOUPLEMANAGER
        ThreadBundle* tb = thread_bundle[std::this_thread::get_id()];
        if(page_index >= tb->page_inf.pages.size()) return 0;
        return tb->page_inf.pages[page_index];
#else
        if(page_index >= page_inf.pages.size()) return 0;
        return page_inf.pages[page_index];
#endif
    }

    //修改页面信息
    static inline void modifyPage(float new_width,float new_height,float new_top_margin,float new_bottom_margin)
    {

    }

    // 获取点(x,y)所在的页面，同时返回所在栏。如果所在页面不分栏，则默认column = 0;
    static inline auto get_pt_page_with_coulmn(float x,float y) {
        PCPos inf;
        inf.page = getPage(y);
        if(inf.page->page_type == Helper::Word_Page) {
            inf.column = inf.page->get_x_of_column_id(x);
        }
        return inf;
    }

    // 获取obj的页面，同时返回所在栏。如果所在页面不分栏，则默认column = 0;
    static inline auto get_page_with_column(const Obj* obj) {
        PCPos inf;
        auto aobj = obj->as<AnchorObj*>();
        if(aobj) {
            HorLine_Base *hline = (HorLine_Base*)aobj->hline;
            if(hline->page) inf.page = hline->page;
            else inf.page = getPage(*hline);
            auto aline = hline->as<AnchorObj_HLine*>();
            if(aline && aline->isWordPageLine()) {
                inf.column = inf.page->getLeftLineColumnIndex(aline->leftLine);
            } else {
                inf.column = 0;
            }
        } else {
            inf.page = getPage(*obj);
        }
        return inf;
    }

    //获取obj所在的页面信息
    static inline Page* getPage(const Obj& obj) {
#ifdef MULTITHREAD_SOUPLEMANAGER
        ThreadBundle* tb = thread_bundle[std::this_thread::get_id()];
        if(tb->page_inf.pages.empty()) return 0;
        auto it =  std::lower_bound(tb->page_inf.sum_heights.begin(),tb->page_inf.sum_heights.end(),obj.y);
        if(it == tb->page_inf.sum_heights.end()) return tb->page_inf.pages.back(); //[2025/8/6 修改，保证返回有效Page]
        return tb->page_inf.pages[it - tb->page_inf.sum_heights.begin()];
#else
        if(page_inf.pages.empty()) return 0;
#ifdef Q_OS_WIN32
        auto it = std::ranges::lower_bound(page_inf.sum_heights,obj.y);
#else
        auto it = std::lower_bound(page_inf.sum_heights.begin(),page_inf.sum_heights.end(),obj.y);
#endif
        //if(it == page_inf.sum_heights.end()) return nullptr;
        if(it == page_inf.sum_heights.end()) return page_inf.pages.back(); //[2025/8/6 修改，保证返回有效Page]
        return page_inf.pages[it - page_inf.sum_heights.begin()];
#endif
    }

    static inline Page* getPage(float y) {
#ifdef MULTITHREAD_SOUPLEMANAGER
        ThreadBundle* tb = thread_bundle[std::this_thread::get_id()];
        if(tb->page_inf.pages.empty()) return 0;
        auto it = std::lower_bound(tb->page_inf.sum_heights.begin(),
                                   tb->page_inf.sum_heights.end(),y);
        if(it == tb->page_inf.sum_heights.end())
            return tb->page_inf.pages.back();
        return tb->page_inf.pages[it - tb->page_inf.sum_heights.begin()];
#else
        if(page_inf.pages.empty()) return 0;
#ifdef Q_OS_WIN32
        auto it = std::ranges::lower_bound(page_inf.sum_heights,y);
#else
        auto it = std::lower_bound(page_inf.sum_heights.begin(),page_inf.sum_heights.end(),y);
#endif
        //if(it == page_inf.sum_heights.end()) return nullptr;
        if(it == page_inf.sum_heights.end())
            return page_inf.pages.back(); //[2025/8/6 修改，保证返回有效Page]
        return page_inf.pages[it - page_inf.sum_heights.begin()];
#endif
    }

    static inline Page* getPage(int doc_id,float y) {
#ifdef MULTITHREAD_SOUPLEMANAGER
        throw std::runtime_error("MULTITHREAD_SOUPLEMANAGER: no getPage(doc_id,y)");
#endif
        if(doc_id == Current_Document || doc_id == currentDocumentID) {
            return getPage(y);
        }
        auto& page_inf = all_documents[doc_id]->page_inf;
        auto it = std::ranges::lower_bound(page_inf.sum_heights,y);
        if(it == page_inf.sum_heights.end()) return nullptr;
        return page_inf.pages[it - page_inf.sum_heights.begin()];
    }

    Q_INVOKABLE int getPageIndexOfY(float y) {
        auto page = getPage(y);
        return page ? page->index : 0;
    }

    //计算两个page之间差了多高的空白，不包含page1的上边距和page2的下边距
    static inline float calcPageMargins(const Page* page1,const Page* page2) {
#ifdef MULTITHREAD_SOUPLEMANAGER
        ThreadBundle* tb = thread_bundle[std::this_thread::get_id()];
        if(page2->index > page_inf.pages.size()) return 0;
        return tb->page_inf.sum_margins[page2->index] - tb->page_inf.sum_margins[page1->index];
#else
        //qDebug() << "calcPageMargins(" << page1->index << "," << page2->index;
        if(page2->index > page_inf.pages.size()) return 0;
        //qDebug() << "s2=" << page_inf.sum_margins[page2->index] << " s1=" << page_inf.sum_margins[page1->index];
        return page_inf.sum_margins[page2->index] - page_inf.sum_margins[page1->index];
#endif
    }

    //通知hline死亡，必须从hline队列中删除
    static inline void notifyHLineDead(Obj* hline) {
#ifdef MULTITHREAD_SOUPLEMANAGER
        throw std::runtime_error("MULTITHREAD_SOUPLEMANAGER: no notifyHLineDead(Obj* hline)");
#endif
        int i = 0;
        for(auto obj : queue_hline_wait_update) {
            if(obj == hline)
                queue_hline_wait_update[i] = 0; //打0标记
            ++i;
        }
    }

    //修改obj的y坐标，调用此函数，可以记录y坐标排序的表
    // static inline void changeObjY(Obj& obj,float new_y) {
    //     int ori_i = int(obj.y/500);
    //     int new_i = int(new_y/500);
    //     if(ori_i != new_i) {
    //         y_sort_objs[ori_i].remove(&obj);
    //         y_sort_objs[new_i].insert(&obj);
    //     }
    //     obj.y = new_y;
    // }

    static void recordEmbeddedFont(int fontId) {
        embedded_font_id_list.push_back(fontId);
    }

    static Obj* createObj(const QString& type);

    Q_INVOKABLE static void installEventFilter(QObject* qobj) {
        qobj->installEventFilter(&SoupleManager::myEventFilter);
    }

    Q_INVOKABLE static void efST(QObject* qobj) {
        qobj->installEventFilter(&SoupleManager::stEventFilter);
    }

    Q_INVOKABLE static void requestChangeCurrentPageStyle(int style) {
#ifdef MULTITHREAD_SOUPLEMANAGER
        throw std::runtime_error("MULTITHREAD_SOUPLEMANAGER: no requestChangeCurrentPageStyle(int style)");
#endif
        Page* page = getPage(view_top);
        if(!page || page->page_type == style) return;
        if(page->page_type == Helper::NoFormat_Page) {
            if(style == Helper::Word_Page) {
                page->page_type = Helper::Word_Page;
                page->setWordPageColumnNum(1); //1栏
            }
        }
    }

    Q_INVOKABLE static int getCurrentPageStyle() {
#ifdef MULTITHREAD_SOUPLEMANAGER
        throw std::runtime_error("MULTITHREAD_SOUPLEMANAGER: no getCurrentPageStyle()");
#endif
        Page* page = getPage(view_top);
        if(!page) return 0;
        return page->page_type;
    }

    Q_INVOKABLE static QVariantMap getCurrentPageInfo() {
#ifdef MULTITHREAD_SOUPLEMANAGER
        throw std::runtime_error("MULTITHREAD_SOUPLEMANAGER: no getCurrentPageInfo()");
#endif
        Page* page = getPage(view_top);
        if(!page) return {};
        return {{"width",page->width},{"height",page->height},
                {"style",page->page_type},{"column_num",page->columns.size()},
                {"rotation",0},{"topMargin",page->topMargin},
                {"bottomMargin",page->bottomMargin}};
    }

    // 获取当前文档id,返回-1表示当前无打开的Souple文档
    Q_INVOKABLE static int getCurrentDocumentID() {
        return currentDocumentID;
    }

    Q_INVOKABLE static void requestChangePageColumnNum(int num) {
        Page* page = getPage(view_top);
        if(!page || page->page_type != Helper::Word_Page) return;
        page->setWordPageColumnNum(num);
    }

    Q_INVOKABLE static void addSoupleDocument(int id) {
        qDebug() << "addSoupleDocument(" << id;
#ifdef MULTITHREAD_SOUPLEMANAGER
        throw std::runtime_error("MULTITHREAD_SOUPLEMANAGER: no addSoupleDocument(int id)");
#endif
        all_documents.insert(id,std::make_shared<Document>());
    }

    Q_INVOKABLE static bool switchSoupleDocument(int switch_to_id);

    Q_INVOKABLE static QString getObjHLineName(int id) {
        Obj* obj = getObjById(id);
        if(!obj) return "invalid";
        AnchorObj* aobj = obj->as<AnchorObj*>();
        if(!aobj) return "invalid";
        AnchorObj_HLine* hline = aobj->hline->as<AnchorObj_HLine*>();
        if(!hline) return "invalid";
        return hline->getName();
    }

    // 请求为当前选中的内容进行内容分栏
    Q_INVOKABLE static void requestSetContentColumns(int columns_num,
                                                     const QString& aux_text = "");

    Q_INVOKABLE static void requestClearSelection() {
        SelectionManager::clearSelection();
    }

    // 当selectedObj变动时，qml以此为通知
    Q_INVOKABLE static void changeSelectedObj(int id);

    static std::list<Obj*>& getDocumentObjs(int s) {
        if(s == currentDocumentID) return all_objs;
        return all_documents[s]->all_objs;
    }

    static _PAGE_INF& getDocumentPages(int s) {
        if(s == currentDocumentID) return page_inf;
        return all_documents[s]->page_inf;
    }

    static void saveCurrentDocument();

    // 获取整个文档的第一条水平线。
    // 注意，通常，调用此函数前需要检查一下整个文档是否水平线连续，
    // 从而确保FirstLine可以往下遍历
    // 参数document_id默认值为-2,代表当前文档。
    static HorLine_Base* getDocumentFirstLine(int document_id=Current_Document);

    static void notifyVisible(Obj* obj) noexcept {
#ifndef MULTITHREAD_SOUPLEMANAGER
        all_visible_objs.push_back(obj);
#endif
    }

    static void setQmlSoupleEdit(QQuickItem* qml_instance) noexcept {
        SoupleManager::qml_soupleEdit = qml_instance;
    }

#ifdef MULTITHREAD_SOUPLEMANAGER
    // 初始化SoupleManager，请每个线程都得调用一次。
    static inline void init_for_thread() {
        thread_bundle[std::this_thread::get_id()] = new ThreadBundle;
        clear_for_thread();
    }
    // 清除SoupleManager所有内容
    static inline void clear_for_thread() {
        auto tb = thread_bundle[std::this_thread::get_id()];
        tb->edit_height = 0;
        tb->edit_width = 0;
        tb->enableRegister = true;
        tb->wait_register_obj_list.clear();
        tb->hash_id_obj.clear();
        for(auto obj : tb->all_objs) {
            delete obj;
        }
    }
#endif

public:
    static inline float view_top, view_bottom;
    static inline SoupleManager *soupleManager;
    static inline bool paused = false; //暂停
    static inline bool frozen = false; //定格
    static inline QMutex mutex; //线程锁
    static inline bool showHelpLine = false; //是否显示辅助线
    static inline std::vector<Obj*> wait_register_obj_list; //等待注册的对象
private:
    //static void imp_updateUI(); //真正开始

    //static inline std::vector<std::list<
    static inline QQueue<Obj*> queue_hline_wait_update; //等待更新布局的hline
    static inline std::list<Obj*> all_visible_objs;
    //所有可见Obj(即创建了qml对象的obj) // 含义变更 [2025/9/8]：所有可见或预备可见的obj
    static inline std::list<Obj*> all_objs; //记录所有obj
    static inline QHash<OBJID_t,Obj*> hash_id_obj; //id->obj哈希表
// #ifdef ULONGLONG_OBJID
//         static inline QHash<qint64,Obj*> hash_id_obj; //id->obj哈希表
// #else
//         static inline QHash<qint32,Obj*> hash_id_obj; //id->obj哈希表
// #endif
    //static inline std::vector<Obj*> obj_vector; //2
    static inline std::list<Obj*>::iterator scan_iter = all_objs.begin(); //扫描id
    static inline std::list<Obj*>::iterator scan_iter2 = all_objs.begin(); //扫描id2
    static inline std::list<Obj*>::iterator scan_iter_visble = all_visible_objs.begin(); //扫描可见id
    static inline float edit_width; //编辑区总宽度(=最宽的页面的宽度)
    static inline float edit_height; //编辑区总高度(=所有页面高度和)

    //y_sort_objs[i]存储着所有y属于[i*500,i*500+500)间的obj
    //static inline std::vector<QSet<Obj*>> y_sort_objs{10000,QSet<Obj*>{}}; //最多5000页
    static inline _PAGE_INF page_inf;
    static inline QTimer timer{nullptr};

    static inline QQuickItem* qml_soupleEdit = 0; //qml端的soupleEdit
    static inline bool viewUpdated = false;


    struct Document {
        Safe_Obj_Pointer<Obj_Start_Sign> start_sign;
        uint32_t obj_s_all_id;
        int hline_s_hline_count;
        QHash<QString,HorLine_Base*> horline_s_hash_hline;
        int tline_s_tline_count;
        float edit_width,edit_height;
        float view_top, view_bottom;
        //std::list<Obj*>::iterator scan_iter;
        //std::list<Obj*>::iterator scan_iter2;
        //std::list<Obj*>::iterator scan_iter_visble;
        //std::list<Obj*> all_visible_objs;
        std::list<Obj*> all_objs;
        std::list<Obj*> all_visible_objs;

        QHash<OBJID_t,Obj*> hash_id_obj;
        std::deque<Turnback*> turnback_list;
        std::vector<Turnback*> redo_list;
        _PAGE_INF page_inf;
    };

#ifdef MULTITHREAD_SOUPLEMANAGER
    //每个线程只可能访问自己的键，无插入、修改、删除键值对的可能
    //因此是线程安全的。
    struct ThreadBundle {
        bool enableRegister = true;
        Safe_Obj_Pointer<Obj_Start_Sign> start_sign;
        std::list<Obj*> all_objs;
        QHash<OBJID_t,Obj*> hash_id_obj;
        float edit_width; //编辑区总宽度(=最宽的页面的宽度)
        float edit_height; //编辑区总高度(=所有页面高度和)
        std::vector<Obj*> wait_register_obj_list; //等待注册的对象
        _PAGE_INF page_inf;
    };
    static inline QHash<std::thread::id,ThreadBundle*> thread_bundle;
#endif

    static inline QHash<int,std::shared_ptr<Document>> all_documents;

    static inline int currentDocumentID = -1; //-1代表：无文档
    static inline int selected_qmlItem_id = -1; //被选中的qmlItem的id

    //static inline std::unordered_set<int> inQueue;

    static inline Safe_Obj_Pointer<Obj_Start_Sign> start_sign;
private:
    static inline std::vector<int> embedded_font_id_list; //该文档添加的字体id列表
private slots:
    static void imp_updateUI();
signals:
    void newPageAdded(float width,float height,float top_margin,float bottom_margin);
    void pageIndexChanged(int page_index);
};

#endif // SOUPLEMANAGER_H
