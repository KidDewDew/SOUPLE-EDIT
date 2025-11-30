#include "souplemanager.h"
#include "anchorobj_flowtext.h"
#include "anchorobj_phright.h"
#include "anchorobj_phrect.h"
#include "anchorobj_hline.h"
#include "anchorobj_vline.h"
#include "anchorobj_image.h"
#include "anchorobj_jzrect.h"
#include "anchorobj_path.h"
#include "freeobj.h"
#include "free_text.h"
#include "free_image.h"
#include "free_path.h"
#include "free_rich.h"
#include "anchorobj_phleft.h"
#include "anchorobj_glue.h"
#include "TableFactory.h"
#include "anchorobj_latexformula.h"
#include <QQuickWindow>
#include "turnbackmanager.h"
#include "free_frame.h"
#include "helper.h"
#include "frame_ofhlines.h"
#include "souple_pdfsaver.h"
#include "myimageprovider.h"
#include "navline.h"
#include "anchorobj_spring.h"
#include "magicalcursor.h"
#include "columns_separate.h"
#include <QGuiApplication>

using namespace std;

extern QQuickWindow* qmlWindow;
extern MyImageProvider *image_provider;
extern QGuiApplication *global_app;

SoupleManager::SoupleManager() {}

void SoupleManager::init()
{
    timer.setInterval(TIMER_DEALUI_INTERVAL);
    timer.start();
    connect(&timer,&QTimer::timeout,[]{ SoupleManager::imp_updateUI(); });
}

void SoupleManager::imp_updateUI()  //更新ui
{
    //static int pile_1 = 0;

    Helper::ptime += TIMER_DEALUI_INTERVAL;
    //pile_1 += TIMER_DEALUI_INTERVAL;
    if(paused || currentDocumentID == -1) return;

    if(! mutex.tryLock()) return; //不抢占，所以在保存等操作时，可能会停止刷新界面


    static Obj* last_visible_dealui_obj = 0; //上一次处理可见obj布局处理到了哪一个
    static int pile_time_1 = 0;
    static int pile_time_selectionDeal = 0;

    // 计算预可见高度(in 像素)
    float pre_visible_height = Helper::cm2pixel(PRE_VISIBLE_RANGE_cm);

    //int selected_data_id = Helper::invokeQmlFunction<int>("getSelectedID"); //前端被选中的对象的data_id
    selected_qmlItem_id = Helper::invokeQmlFunction<int>("getSelectedID");
    bool begin_deal_visible_ui = ! last_visible_dealui_obj;

    int n_deal = 0, n_deal_2 = 0; //更新操作计数
    int n_deal_visble = 0;

    //qDebug() << "所有对象num = " << all_objs.size();

    //for(auto ii : queue_hline_wait_update)

    Obj* last_deal_obj = 0;

    static int __debug_count = 0;
    ++__debug_count;

    if(frozen == false) {

        // while(queue_hline_wait_update.size() > 3) {
        //     queue_hline_wait_update.pop_front();
        // }

        //qDebug() << queue_hline_wait_update.size();

        // qDebug() << "queue.size=" << queue_hline_wait_update.size();
        // for(auto hline : queue_hline_wait_update) {
        //     qDebug() << hline->__dstr();
        // }

        while(n_deal < TIMER_DEALUI_MAX_NUM_PER_TIMEOUT
               && ! queue_hline_wait_update.empty()  )
        {
            //auto hline = queue_hline_wait_update.front();
            //queue_hline_wait_update.pop_front();
            auto hline = queue_hline_wait_update.back();
            queue_hline_wait_update.pop_back();
            if(hline == 0) continue; //0标记，意味着该hline已经死亡。

            last_deal_obj = hline;

            //qDebug() << "queue " << hline->__dstr();
            //if(hline->y + hline->height + 500 < view_top
            //    || hline->y - 500 > view_bottom) { //超出可见范围的hline降低处理优先级
            //    if(TIMER_DEALUI_MAX_NUM_PER_TIMEOUT - n_deal < queue_hline_wait_update.length())
            //        continue;
            //}
            hline->dealLayout(); //调用hline处理ui
            //qDebug() << "view.top=" << view_top << " view.bottom=" << view_bottom;
            //qDebug() << "hline-deal at " << hline->getName() << ".y = " << hline->y;
            ++ n_deal;
        }

    }

    //qDebug() << "可见NUm: " << all_visible_objs.size();

    //去除all_visible_objs中死亡的obj(千万不要在这里delete)，并扫描可见性
    //@add 检测其中预备可见obj的可见性，若可见则创建qml对象。
    //注意，即便一个obj位于该队列中，也有可能实际不可见，即qmlItem == 0。
    //因为一个对象自己也可以管理自己的可见性
    for(auto it = all_visible_objs.begin(); it != all_visible_objs.end(); ) {
        auto obj = *it;

        // if(__debug_count % 200 == 0) {
        //     if(obj->qmlItem == (QQuickItem*)Helper::QmlItemStatus::NotCreated)
        //         qDebug() << "NotCreated:" << obj->__dstr();
        //     else if(obj->qmlItem == (QQuickItem*)Helper::QmlItemStatus::PreVisibleButHidden)
        //         qDebug() << "PreVisibleButHidden:" << obj->__dstr();
        //     else if(obj->qmlItem == (QQuickItem*)Helper::QmlItemStatus::CreatedButHidden)
        //         qDebug() << "CreatedButHidden:" << obj->__dstr();
        //     else
        //         qDebug() << "**visible:" << obj->__dstr();
        // }

        if(obj->dead_sign) {
            if(Helper::isQmlItemValid(obj->qmlItem)) obj->discard_qmlItem();
            if(it == scan_iter_visble)
                scan_iter_visble = it = all_visible_objs.erase(it);
            else
                it = all_visible_objs.erase(it);
        }  else if(( obj->y + obj->height + 200 < view_top
                             || obj->y - 200 > view_bottom )
                   && obj->id != selected_qmlItem_id //被选中的ui对象有“免死金牌”
                   ) { //不可见
            if(Helper::isQmlItemValid(obj->qmlItem)) {
                obj->discard_qmlItem(); //移除qmlItem
                // 标记qmlItem为“预可见”
                obj->qmlItem = (QQuickItem*)Helper::QmlItemStatus::PreVisibleButHidden;
            } else if(obj->qmlItem != nullptr) {
                // 标记qmlItem为“预可见”
                obj->qmlItem = (QQuickItem*)Helper::QmlItemStatus::PreVisibleButHidden;
            }

            if(obj->y + obj->height < view_top - pre_visible_height
                || obj->y > view_bottom + pre_visible_height) {
                // 这个对象超出预可见范围了，得从这个队列里删掉。
                if(obj->qmlItem != nullptr) { //qmlItem无效但又被标记为【已创建】
                    // 标记qmlItem为“未创建”
                    obj->qmlItem = (QQuickItem*)Helper::QmlItemStatus::NotCreated;
                }
                if(it == scan_iter_visble) //防止无效指针
                    scan_iter_visble = it = all_visible_objs.erase(it);
                else
                    it = all_visible_objs.erase(it);
            }
            else {
                ++it;
            }
        } else  { //这个对象可见，所以我们要看看它有没有创建qml对象
            //if()

            if(obj->qmlItem == (QQuickItem*)Helper::QmlItemStatus::NotCreated
            || obj->qmlItem == (QQuickItem*)Helper::QmlItemStatus::PreVisibleButHidden) {
                //对于这两种情况，我们要为它们创建qml对象
                obj->qmlItem = obj->generateQmlItem();
                if(Helper::isQmlItemValid(obj->qmlItem)) {
                    obj->updateDataToQmlItem(obj->qmlItem);//更新ui对象
                } else if(obj->qmlItem == 0) {
                    obj->qmlItem = (QQuickItem*)Helper::QmlItemStatus::PreVisibleButHidden;
                }
            }
            ++it;
        }
    }


    //qDebug() << "可见NUm: " << all_visible_objs.size();

    if(all_objs.size() > 0) {
        //扫描对象，检测死亡和视野
        if(scan_iter == all_objs.end()) scan_iter = all_objs.begin();
        if(scan_iter2 == all_objs.end()) scan_iter2 = all_objs.begin();
        int n_scan = 0;
        bool isIter2Danger = false;
        while(n_scan < TIMER_SCAN_NUM_PER_TIMEOUT
               && n_scan < all_objs.size()) {
            auto obj = *scan_iter;
            if(obj->dead_sign) {
                hash_id_obj.remove(obj->id);
                if((uintptr_t)obj->qmlItem >= Helper::QmlItemStatus::Valid) //[2025/8/12修改]
                    obj->discard_qmlItem(); //[更改 2025/7/14 原内存泄漏]
                if(!isIter2Danger && obj == *scan_iter2) {
                    isIter2Danger = true;
                }
                delete obj;
                //++NUM2;
                //qDebug() << "注册:" << NUM1 << "删除:" << NUM2;
                scan_iter = all_objs.erase(scan_iter);
            } else {
                if(obj->y + obj->height >= view_top - pre_visible_height
                    && obj->y <= view_bottom + pre_visible_height) { //(预)可见
                    if(obj->y + obj->height >= view_top
                        && obj->y <= view_bottom)   //真的可见
                    {
                        if(obj->qmlItem == (QQuickItem*)Helper::QmlItemStatus::NotCreated)
                        {  //创建ui
                            obj->qmlItem = obj->generateQmlItem(); //创建ui对象
                            if(obj->qmlItem != (QQuickItem*)Helper::QmlItemStatus::NotCreated) {
                                all_visible_objs.push_back(obj); //加入(预）可见队列
                                if(Helper::isQmlItemValid(obj->qmlItem)) { //注意，有可能创建失败(故意失败，或者uiPool限制数量)。
                                    //obj->qmlItem->setProperty("data_id",obj->id); //传递数据id
                                    obj->updateDataToQmlItem(obj->qmlItem);//更新ui对象
                                }
                            }
                        }
                    } else {
                        if(obj->qmlItem == (QQuickItem*)Helper::QmlItemStatus::NotCreated) {
                            all_visible_objs.push_back(obj); //加入(预）可见队列
                            obj->qmlItem = (QQuickItem*)Helper::QmlItemStatus::PreVisibleButHidden;
                        }
                    }
                } //可见 end
                ++scan_iter;
            }
            if(scan_iter == all_objs.end()) {
                scan_iter = all_objs.begin();
            }
            ++ n_scan;
        } //扫描 END

        //if(n_deal_2 == 0) {
        //    last_visible_dealui_obj = 0; //下一次直接开始扫描可见ui
        //}


        //扫描可见ui

        int N_deal_visble = min(TIMER_DEALUI_VISIBLE_NUM,(int)all_visible_objs.size());

        if(scan_iter_visble == all_visible_objs.end())
            scan_iter_visble = all_visible_objs.begin();

        int nn = 0;

        if(frozen == false) {
            while(n_deal_visble < N_deal_visble && nn < all_visible_objs.size()) {
                auto obj = *scan_iter_visble;

                if(obj->objInfo().dealLayoutable) { //[2025/7/30]修改 更具有广适性
                    if(!queue_hline_wait_update.empty()
                        && queue_hline_wait_update.front() == obj) {
                        queue_hline_wait_update.pop_front(); //[2025/11/29 add]
                    }
                    obj->dealLayout();
                    ++ n_deal_visble;
                }
                ++scan_iter_visble;
                if(scan_iter_visble == all_visible_objs.end())
                    scan_iter_visble = all_visible_objs.begin();
                ++nn;
            }

            if(isIter2Danger)
                scan_iter2 = all_objs.begin();

            n_scan = 0;
            while(n_scan < TIMER_DEALUI_SCAN_NUM
                   && n_scan < all_objs.size()) {
                auto& obj = *scan_iter2;
                //auto aobj = obj->as<AnchorObj_HLine*>();
                //if(aobj->) {
                //    aobj->dealLayout();
                //}
                if(obj->objInfo().dealLayoutable) { //[2025/7/30]修改 更具有广适性
                    if(!queue_hline_wait_update.empty()
                        && queue_hline_wait_update.front() == obj) {
                        queue_hline_wait_update.pop_front(); //[2025/11/29 add]
                    }
                    obj->dealLayout();
                }
                ++n_scan;
                ++scan_iter2;
                if(scan_iter2 == all_objs.end()) {
                    scan_iter2 = all_objs.begin();
                }
            }
        }
    }

    //计算页码，根据view垂直中心y坐标来确定
    float cy = (view_top + view_bottom)/2;
    const Page *page = getPage(cy);
    static int old_page_index = 0;
    int page_index = page ? page->index : page_inf.pages.size();
    if(old_page_index != page_index) {
        old_page_index = page_index;
        emit soupleManager->pageIndexChanged(page_index); //通知ui
    }

    pile_time_1 += TIMER_DEALUI_INTERVAL;
    if(pile_time_1 >= SCAN_POOL_INTERVAL) { //回收ui控件池的多余item
        pile_time_1 = 0;
        AnchorObj_FlowText::uiPool_flowText::scanPool();
        AnchorObj_HLine::uiPool_HLine::scanPool();
        AnchorObj_VLine::uiPool_VLine::scanPool();
        AnchorObj_Image::uiPool_Image::scanPool();
        AnchorObj_PHRect::uiPool_PHRect::scanPool();
        AnchorObj_PHRight::uiPool_PHRight::scanPool();
        AnchorObj_Path::uiPool_Path::scanPool();
        Free_Image::uiPool_FImage::scanPool();
        Free_Path::uiPool_FPath::scanPool();
        Free_Text::uiPool_FText::scanPool();
        Free_Rich::uiPool_FRich::scanPool();
        HorLine_Base::uiPool_BLine::scanPool();
        BlockInner_HorLine::uiPool_ILine::scanPool();
        NavItem::uiPool_Nav::scanPool();
        AnchorObj_Spring::uiPool_Spring::scanPool();
        qmlWindow->releaseResources();
    }

    pile_time_selectionDeal += TIMER_DEALUI_INTERVAL;
    if(pile_time_selectionDeal >= SELECTION_DEAL_INTERVAL) { //处理选择
        SelectionManager::notifyAllVisible();
    }

    mutex.unlock();

}

// [[old]] 注该函数已经过时，但能正常使用。
// 因为它对HLine进行了各种特判，现在推荐使用connectHLine_down/up方法来完成！
bool SoupleManager::createTable_inner(int n_row,int n_col,BlockInner_HorLine* iline)
{
    Obj* parent = iline->getParent();
    auto vline_any = parent->getAnyData("vline");
    auto lastline = parent->getAnyData("lastline");
    if(! vline_any.has_value()) return false; //无效的父容器对象
    try {
        auto[vline_left,vline_right] =
            std::any_cast<pair<AnchorObj_VLine*,AnchorObj_VLine*>>(vline_any);
        TableInfo* ti = TableFactory::newTable(iline->width - 40, n_row, n_col);
        auto line2 = iline->getNextLine();
        auto line = ti->endLine;
        for(int i = 0; i < ti->rowCount(); ++i) {
            line->leftLine = vline_left;
            line->rightLine = vline_right;
            line = line->hline->as<TableLine*>();
        }
        FitLine_for_AnchorObj_HLine* fit1 = new FitLine_for_AnchorObj_HLine,
                                    *fit2 = new FitLine_for_AnchorObj_HLine;
        //SoupleManager::registerObj()
        fit1->leftLine = vline_left;
        fit1->rightLine = vline_right;
        fit2->leftLine = vline_left;
        fit2->rightLine = vline_right;
        // iline->fit1->ti1->...->ti2->fit2->iline2
        ti->firstLine->setPrevLine(fit1);
        ti->endLine->connectHLine_down(fit2);
        fit2->lastLine = ti->endLine;
        if(line2) fit2->nextLine = line2;
        fit1->lastLine = iline;
        fit1->nextLine = ti->firstLine;
        if(line2) line2->setPrevLine(fit2);
        iline->connectHLine_down(fit1);
        return true;
    } catch(std::bad_any_cast& e) {
        return false;
    } catch(LLException& e) {
        qDebug() << e.getInfo();
        return false;
    }
}

// [[old]] 注该函数对innerline的处理方法已经过时，但能正常使用。
// 因为它对HLine进行了各种特判，现在推荐使用connectHLine_down/up方法来完成！
bool SoupleManager::createTable(int n_row,int n_col)
{
    int selected_id = Helper::invokeQmlFunction<int>("getSelectedID");
    auto selected_obj = getObjById(selected_id); //当前选择的obj
    if( ! selected_obj ) return false; //不知道在哪儿插入表格
    //获取selected_obj所在的水平标线
    auto aobj = selected_obj->as<AnchorObj*>();
    if( ! aobj ) return false;
    auto hline = aobj->as<AnchorObj_HLine*>();  //选中的就是水平标线
    if( ! hline ) hline = aobj->hline->as<AnchorObj_HLine*>(); //否则，获取其所在的水平标线
    if( ! hline ) {
        auto iline = aobj->hline->as<BlockInner_HorLine*>();
        if(iline) {
            return createTable_inner(n_row,n_col,iline);
        }
        return false;
    }

    //在hline后面插入 n_row * n_col 表格
    TableInfo* ti = TableFactory::newTable(hline->width - 40, n_row, n_col);
    //ti->endLine->logic_nextHLine = hline->getNextLine();
    auto ori_nextLine = hline->logic_nextHLine;


    if(ori_nextLine) {
        ori_nextLine->setLogicLastHLine(0); //消除逻辑关联
        ori_nextLine->setAnchorLastHLine(ti->endLine); //设置锚定关联
    } else {
        //需要插入到anchorLine中间
        auto lines = hline->anchor_nextHLine;
        for(auto al : lines)
            al->setAnchorLastHLine(ti->endLine); //转移anchorLine
    }
    ti->firstLine->setAnchorLastHLine(hline);

    auto line = ti->endLine;
    for(int i = 0; i < ti->rowCount(); ++i) {
        line->leftLine = hline->leftLine;
        line->rightLine = hline->rightLine;
        line = line->hline->as<TableLine*>();
    }

    requestUpdateHLine(ti->firstLine);
    if(ori_nextLine) requestUpdateHLine(ori_nextLine);

    return true;
}

Obj* SoupleManager::createObj(const QString& type)
{
    Obj* r = 0;

    if(type.length() < 2) return 0;

    switch(type[0].toLatin1()) {
    case 'F':
        switch(type[1].toLatin1()) {
        case 'P':
            r = new Free_Path; // FPath
            break;
        case 'I':
            r = new Free_Image; //FImage
            break;
        case 'T':
            r = new Free_Text; //FText
            break;
        case 'R':
            r = new Free_Rich; //FRich
            break;
        case 'r':
            r = new Free_Frame; //Frame
            break;
        case 'B':
            r = new HorLine_Base; //FBLine
            break;
        case 'l':
            r = new AnchorObj_FlowText; //FlowText
            break;
        }
        break;

    case 'H':
        r = new AnchorObj_HLine; //HLine
        break;

    case 'P':
        switch(type[1].toLatin1()) {
        case 'H':
            if(type == "PH_Right") { //PH_Right
                r = new AnchorObj_PHRight;
            } else if(type == "PH_Rect") {
                r = new AnchorObj_PHRect;
            } else if(type == "PH_Left") {
                r = new AnchorObj_PHLeft;
            }
            break;
        case 'a':
            r = new AnchorObj_Path;
            break;
        }
        break;

    case 'I':
        r = new AnchorObj_Image; //Image
        break;

    case 'G':
        if(type == "GlueL") {
            r = new AnchorObj_Glue;
            r->be<AnchorObj_Glue*>()->glue_left = true;
        } else if(type == "GlueR") {
            r = new AnchorObj_Glue;
            r->be<AnchorObj_Glue*>()->glue_left = false;
        }
        break;

    case 'L':
        r = new AnchorObj_LatexFormula; //Latex
        break;

    case 'V':
        r = new AnchorObj_VLine; //VLine
        break;

    case 'J':
        r = new AnchorObj_JZRect;
        break;

    case 'N':
        r = new NavItem;
        break;

    case 'S':
        r = new AnchorObj_Spring;
        break;
    }

    if(r) {
        registerObj(r); //注册
    }
    return r;
}

bool SoupleManager::checkHLineValid(const QString& hline_name) {
    auto it = AnchorObj_HLine::hash_hline.find(hline_name);
    if(it == AnchorObj_HLine::hash_hline.end()) return Helper::Error_Invalid_Data;
    return 0;
}

bool SoupleManager::checkHLineValid(int doc_id,const QString& hline_name) {
    return getHLineIdByName(doc_id,hline_name) == -1;
}

qint32 SoupleManager::getHLineIdByName(const QString& name) {
    auto it = AnchorObj_HLine::hash_hline.find(name);
    if(it == AnchorObj_HLine::hash_hline.end()) return -1;
    return (*it)->id;
}

qint32 SoupleManager::getHLineIdByName(int doc_id,const QString& name) {
    if(doc_id == currentDocumentID || doc_id == Current_Document) {
        auto it = AnchorObj_HLine::hash_hline.find(name);
        if(it == AnchorObj_HLine::hash_hline.end()) return -1;
        return (*it)->id;
    }
    IF NOT(all_documents.contains(doc_id))
    {
        return -1;
    }
    auto& h = all_documents[doc_id]->horline_s_hash_hline;
    auto it = h.find(name);
    if(it == h.end()) return -1;
    return (*it)->id;
}

void SoupleManager::setShowHelpLine(bool show) noexcept {
    Helper::showHelpLine = showHelpLine = show;
    if(show == false) { //隐藏.....
        for(Obj* obj : all_objs | v_Filter(a && Helper::isQmlItemValid(a->qmlItem) && a->objInfo().isHelperLine)) {
            obj->discard_qmlItem(); //隐藏qml对象
        }
    } else {
        for(Obj* obj : all_objs | v_Filter(a
            &&(uintptr_t)a->qmlItem == Helper::QmlItemStatus::CreatedButHidden
            && a->objInfo().isHelperLine))
        {
            obj->qmlItem = 0;
        }
    }
}

//S_TextInput事件过滤器
bool SoupleManager::STextInputEventFilter::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* me = static_cast<QMouseEvent*>(event);
        QPointF p = SoupleManager::qml_soupleEdit->mapFromGlobal(me->globalPosition());
        SelectionManager::beginSelect(
            dynamic_cast<QQuickItem*>(watched)->property("data_id").toInt(),
            p.x(),p.y());
        return false;
    } else if(event->type() == QEvent::MouseMove) {
        return true; //过滤掉鼠标移动事件！
    }
    return false;
}

// 全局事件过滤器
bool SoupleManager::MyEventFilter::eventFilter(QObject *watched, QEvent *event)
{
    //qDebug() << "eventFilter " << event;
    switch(event->type()) {
    case QEvent::MouseMove:
        if(SelectionManager::hasBegun()) { //选择模式,=>鼠标按下状态=true
            QMouseEvent *me = dynamic_cast<QMouseEvent*>(event);
            //获取相对于SoupleEdit的坐标
            QPointF p = SoupleManager::qml_soupleEdit->mapFromGlobal(me->globalPosition());
            SelectionManager::moveSelect(p.x(),p.y());
            return true; //过滤掉
        }
        break;
    case QEvent::MouseButtonRelease:
        if(SelectionManager::hasBegun()) {
            SelectionManager::stopSelect(); //停止选择
        }
        break;
    case QEvent::MouseButtonPress:
        if(SelectionManager::isSelectionKeep) {
            QMouseEvent *me = dynamic_cast<QMouseEvent*>(event);
            if(me->button() == Qt::LeftButton) { //左键按下
                //获取相对于SoupleEdit的坐标
                QPointF p = SoupleManager::qml_soupleEdit->mapFromGlobal(me->globalPosition());
                if(SoupleManager::qml_soupleEdit->contains(p)&&
                    p.y() >= SoupleManager::view_top
                    && p.y() <= SoupleManager::view_bottom) {
                    SelectionManager::clearSelection();
                }
            }
        }
        break;
    case QEvent::KeyPress:{
        QKeyEvent *ke = (QKeyEvent*)event;
        qDebug() << ke;
        if(SelectionManager::isSelectStopButKeep()) {
            //对选择内容进行键盘操作
            //QObject* focusObject = global_app->focusObject(); //获取焦点对象
            //qDebug() << "focus:" << focusObject;
            QObject* focusObject = global_app->focusObject(); //获取焦点对象
            qDebug() << "focus:" << focusObject;
            if(!focusObject || focusObject->isWindowType()) { //windowType表明它没有具体焦点
                Qt::KeyboardModifiers km = ke->modifiers();
                //if(selected_qmlItem_id
                return true; //国旅
            }
        } else {
            Obj *who = MagicalCursor::at_who();
            if(who && QML_VALID(who) && who->qmlItem->hasFocus())
            { //检查它到底有没有焦点
                return true;
            }
        }
        break;
    }
    default: return false;
    }

    return false; // 返回false表示事件未被处理，继续传递
}

void SoupleManager::saveCurrentDocument() {
    if(currentDocumentID == -1)
        return;
    /** 回收所有可见对象 */
    for(auto obj : all_visible_objs) {
        if(Helper::isQmlItemValid(obj->qmlItem)) {
            obj->discard_qmlItem();
        } else obj->qmlItem = (QQuickItem*)Helper::NotCreated;
    }
    //all_visible_objs.clear();

    auto doc = all_documents[currentDocumentID];
    doc->start_sign = start_sign;
    doc->all_objs = all_objs;
    doc->all_visible_objs = all_visible_objs;
    doc->edit_height = edit_height;
    doc->edit_width = edit_width;
    doc->hash_id_obj = hash_id_obj;
    doc->page_inf = page_inf;
    //doc->scan_iter = scan_iter;
    //doc->scan_iter2 = scan_iter2;
    //doc->scan_iter_visble = scan_iter_visble;
    doc->view_top = view_top;
    doc->view_bottom = view_bottom;
    /** 记录类静态成员 */
    doc->obj_s_all_id = Obj::s_all_id;
    doc->hline_s_hline_count = AnchorObj_HLine::s_hline_count;
    doc->tline_s_tline_count = TableLine::s_tline_count;
    doc->horline_s_hash_hline = HorLine_Base::hash_hline;
    doc->turnback_list = TurnbackManager::turnback_list;
    doc->redo_list = TurnbackManager::redo_list;
}

bool SoupleManager::switchSoupleDocument(int switch_to_id) {
    qDebug() << "switchSoupleDocument(" << switch_to_id;
    if(currentDocumentID == switch_to_id) return true;
    if(switch_to_id == -1) {
        saveCurrentDocument();
        currentDocumentID = -1;
        return true;
    }
    auto pdoc = all_documents.find(switch_to_id);
    if(pdoc == all_documents.end()) {
        emit Helper::helper->errorMsg("错误","切换到不存在的文档。");
        return false;
    }
    saveCurrentDocument(); //保存当前文档
    auto doc = *pdoc;
    start_sign = doc->start_sign;
    all_objs = doc->all_objs;
    all_visible_objs = doc->all_visible_objs;
    edit_width = doc->edit_width;
    edit_height = doc->edit_height;
    hash_id_obj = doc->hash_id_obj;
    page_inf = doc->page_inf;
    scan_iter = all_objs.begin();
    scan_iter2 = all_objs.begin();
    scan_iter_visble = all_visible_objs.begin();
    view_top = doc->view_top;
    view_bottom = doc->view_bottom;
    TurnbackManager::turnback_list = doc->turnback_list;
    TurnbackManager::redo_list = doc->redo_list;
    emit soupleManager->documentWidthChanged();
    emit soupleManager->documentHeightChanged();
    emit soupleManager->pageCountChanged();
    Helper::invokeQmlFunction<void>("changeViewTop",view_top); //通知qml调整滚动条位置
    /** 恢复类静态成员 */
    Obj::s_all_id = doc->obj_s_all_id;
    AnchorObj_HLine::s_hline_count = doc->hline_s_hline_count;
    TableLine::s_tline_count = doc->tline_s_tline_count;
    HorLine_Base::hash_hline = doc->horline_s_hash_hline;

    currentDocumentID = switch_to_id;

    qDebug() << "switch success";
    return true;
}

// 请求撤回
void SoupleManager::request_undo() {
    TurnbackManager::undo();
}

// 请求重做
void SoupleManager::request_redo() {
    TurnbackManager::redo();
}

// 尝试创建装饰框
void SoupleManager::tryCreateDecorationFrame() {
    qDebug() << "SoupleManager::tryCreateDecorationFrame()";
    if(SelectionManager::hasSelection()==false) {
        emit Helper::helper->errorMsg("错误","请先选中一段文本，再添加装饰框。");
        return;
    }
    auto[h1,h2] = SelectionManager::getSelectedHLineRange();
    if(h1==0 || h2==0) {
        emit Helper::helper->errorMsg("错误","请选中连续内容。");
        return;
    }
    qDebug() << "选中区间：" << h1 << h2;
    //装饰框范围：[h1,h2]
    Frame_ofHLines_Instance *instance = new Frame_ofHLines_Instance;
    AnchorObj_PHLeft_as_FrameBegin* ph_left = new AnchorObj_PHLeft_as_FrameBegin;
    AnchorObj_PHRight_as_FrameEnd* ph_right = new AnchorObj_PHRight_as_FrameEnd;
    if(h1->leftObj && h1->leftObj->as<AnchorObj_PHLeft*>())
        h1->leftObj->removeSelf(true);
    if(h2->rightObj && h2->rightObj->as<AnchorObj_PHRight*>())
        h2->rightObj->removeSelf(true);
    h1->insertOnLeft(ph_left);
    h2->insertOnRight(ph_right);
    SoupleManager::registerObj(ph_left);
    SoupleManager::registerObj(ph_right);
    instance->topPadding = 5;
    instance->leftPadding = 5;
    instance->rightPadding = 5;
    instance->bottomPadding = 5;
    instance->ph_begin = ph_left;
    instance->ph_end = ph_right;
    instance->background_type = Helper::Color_Fill;
    instance->background_color = QColor::fromRgb(245,234,255);
    instance->border_color = Qt::transparent;
    ph_left->instance = instance;
}

bool SoupleManager::request_render_page(int document_id,int page_index,
                                            const QString& into_image_id)
{
    if(document_id != currentDocumentID
            && all_documents.contains(document_id) == false) {
        QString ds;
        for(int s : all_documents.keys())
            ds.append(QString::number(s)+' ');
        emit Helper::helper->errorMsg("错误",
            QString("渲染的文档ID=%1无效,\n文档列表为:%2")
            .arg(document_id).arg(ds));
        return false;
    }
    //auto& doc = all_documents[document_id];
    auto &d_page_inf = getDocumentPages(document_id);
    auto &d_all_objs = getDocumentObjs(document_id);
    if(d_page_inf.pages.size() <= page_index || page_index < 0) {
        emit Helper::helper->errorMsg("错误",QString("渲染的页面(索引=%1)无效。").arg(page_index));
        return false;
    }
    std::vector<Obj*> page_objs;
    Page* page = d_page_inf.pages[page_index];
    for(auto obj : d_all_objs) {
        // 找出所有位于该页的obj
        if(obj->y+obj->height >= page->top_y && obj->y <= page->top_y+page->height) {
            page_objs.push_back(obj);
        }
    }
    QImage image(page->width,page->height,QImage::Format_BGR888);
    image.fill(QColor("white"));
    QPainter painter(&image);
    Souple_PdfSaver::renederPage(&painter,page,page_objs);
    image_provider->addImage(into_image_id,image);
    return true;
}

HorLine_Base* SoupleManager::getDocumentFirstLine(int document_id)
{
    IF(((document_id == Current_Document or document_id == currentDocumentID)
         and currentDocumentID == -1)
       or (not all_documents.contains(document_id)))
    THEN(return 0;)
    auto ss = (document_id == Current_Document || document_id == currentDocumentID)
                  ? start_sign : all_documents[document_id]->start_sign;
    IF NOT(ss.valid() and ss->attach_hline.valid())
    THEN(return nullptr;)
    return ss->attach_hline.get();
}

void SoupleManager::requestSetContentColumns(int columns_num,const QString& aux_text)
{

    if(columns_num <= 0 || columns_num > 4) {
        emit Helper::helper->errorMsg("提示","内容分栏最多允许分4栏。");
        return;
    }

    auto[startHLine0,endHLine0] = SelectionManager::getSelectedHLineRange();
    if(! startHLine0 || ! endHLine0) {
        emit Helper::helper->errorMsg("提示","请选择一段内容后，再进行内容分栏。");
        return;
    }

    auto startHLine = startHLine0->as<AnchorObj_HLine*>();
    auto endHLine = endHLine0->as<AnchorObj_HLine*>();

    //if(startHLine->hline
    //    && ! startHLine->hline->canBe<AnchorObj_HLine>()
    //|| )

    if(! startHLine || ! endHLine) {
        emit Helper::helper->errorMsg("提示","只可以为最外层的内容设置内容分栏；\n"
                                "表格的单元格、文本框等对象内部的内容，无法设置内容分栏。");
        return;
    }

    // 开始创建 内容分栏
    ColumnsSeparate *sep_line_1 = new ColumnsSeparate,
                    *sep_line_2 = new ColumnsSeparate;
    sep_line_1->isTop = true; //一上
    sep_line_2->isTop = false; //一下
    sep_line_1->pal = sep_line_2;
    sep_line_2->pal = sep_line_1;
    sep_line_1->y = startHLine->getContentTop();
    sep_line_2->y = endHLine->getContentBottom();
    // 创建栏
    sep_line_1->changeColumnsNum(columns_num);
    // 设置栏的宽度
    float sum_width = startHLine->page->width - Helper::cm2pixel(2.4);
    float start_x = Helper::cm2pixel(1.2);
    float column_width = sum_width / columns_num;
    float spacing = std::min<float>(column_width * 0.2f,Helper::cm2pixel(0.6));
    for(auto& column : sep_line_1->getColumns())
    {
        column.leftLine->x = start_x;
        column.rightLine->x = start_x + column_width - spacing;
        start_x += column_width;
    }

    // 赋予相邻的水平标线一些属性
    startHLine->top_sep_line = sep_line_1;
    if(startHLine->hline) {
        startHLine->hline->be<AnchorObj_HLine*>()
            ->bottom_sep_line = sep_line_1;
    }
    endHLine->bottom_sep_line = sep_line_2;
    if(endHLine->getNextLine()) {
        endHLine->getNextLine()->be<AnchorObj_HLine*>()
            ->top_sep_line = sep_line_2;
    }

    sep_line_1->hline_up = startHLine->hline->be<AnchorObj_HLine*>();
    sep_line_1->hline_down = startHLine;
    sep_line_2->hline_up = endHLine;
    sep_line_2->hline_down = endHLine->getNextLine()->be<AnchorObj_HLine*>();

    //排列这些hline到新的栏
    AnchorObj_HLine* hline = startHLine;
    while(hline != endHLine)
    {
        hline->leftLine = sep_line_1->columns[0].leftLine->be<AnchorObj_VLine*>();
        hline->rightLine = sep_line_1->columns[0].rightLine->be<AnchorObj_VLine*>();
        hline = hline->getNextLine()->be<AnchorObj_HLine*>();
    }
}
