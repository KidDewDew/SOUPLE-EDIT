#include "anchorobj_hline.h"
#include "helper.h"
#include "souplemanager.h"
#include "anchorobj_phright.h"
#include "anchorobj_phleft.h"
#include "wordpage_vline.h"

AnchorObj_HLine::AnchorObj_HLine()
{
    while(hash_hline.contains("H"+QString::number(s_hline_count))) {
        ++s_hline_count;
    }
    name = "H"+QString::number(s_hline_count); //自动命名
    hash_hline[name] = this; //记录hline
    width = 200;
    x = 50;
    z = Helper::Layer_Z::Top; //绝对置顶
    //page = SoupleManager::getPage(y);
}

QQuickItem* AnchorObj_HLine::generateQmlItem()
{
    if(SoupleManager::showHelpLine == false) return (QQuickItem*)Helper::QmlItemStatus::CreatedButHidden;
    return uiPool_HLine::fetchItem();
    //return Helper::invokeQmlFunction<QQuickItem*>("generateObj",u"HLine"_qs);
}

void AnchorObj_HLine::updateDataToQmlItem(QQuickItem* item)
{
    if(! Helper::isQmlItemValid(item)) return;
    item->setProperty("data_id",id);
    item->setProperty("objectName",name);
    item->setPosition({x,y-item->height()/2});
    item->setWidth(width);
    height = item->height(); //hline的高度由qml决定
    //item->setProperty("width",leftLine&&rightLine ? rightLine->x - leftLine->x : 200);
}

void AnchorObj_HLine::createNextLine() {
    //需要让所有以本HLine为锚定上标线的HLine修改锚定上标线
    logic_nextHLine = new AnchorObj_HLine;
    //qDebug() << "nl-size: " << anchor_nextHLine.size();
    while(anchor_nextHLine.size()) {
        auto it = anchor_nextHLine.begin();
        auto nl = *it;
        nl->hline = logic_nextHLine;
        logic_nextHLine->anchor_nextHLine.push_back(nl);
        anchor_nextHLine.erase(it);
    }
    logic_nextHLine->leftLine = leftLine;
    logic_nextHLine->rightLine = rightLine;
    logic_nextHLine->topMargin = topMargin;
    logic_nextHLine->width = width;
    logic_nextHLine->setLogicLastHLine(this);
    logic_nextHLine->setAnchorLastHLine(this);
    logic_nextHLine->page = page;
    logic_nextHLine->y = getContentBottom() + logic_nextHLine->topMargin;
    SoupleManager::registerObj(logic_nextHLine);
}

// void AnchorObj_HLine::insertOnLeft(AnchorObj* obj) //从左边插入
// {
//     obj->hline = this;
//     //qDebug() << "AnchorObj_HLine::insertOnLeft(obj)";
//     if(leftObj) {
//         leftObj->insertOnLeft(obj);
//         return;
//     }
//     leftObj = rightObj = obj;
//     //SoupleManager::requestUpdateHLine(this);
// }

// void AnchorObj_HLine::insertOnRight(AnchorObj* obj) //从右边插入
// {
//     obj->hline = this;
//     if(rightObj) {
//         rightObj->insertOnRight(obj);
//         return;
//     }
//     leftObj = rightObj = obj;
//     //SoupleManager::requestUpdateHLine(this);
// }

void AnchorObj_HLine::requestUpdateAnchorNextHLine() {
    for(auto nextLine : anchor_nextHLine)
        SoupleManager::requestUpdateHLine(nextLine);
}

float AnchorObj_HLine::calcMarginFromLastHLine(AnchorObj_HLine* hline)
{
    const Page *page1 = hline->as<AnchorObj_HLine*>()->page;
    if(page1) {
        return SoupleManager::calcPageMargins(page1, page);
    }
    return 0;
}


/**
 * @brief AnchorObj_HLine::dealSpan
 *        处理跨页、跨栏移动。时间复杂度：平均约8次比较，5次跳转。
 */
void AnchorObj_HLine::dealSpan(AnchorObj_HLine* anchor_lastHLine,float min_obj_y,float max_obj_y) noexcept
{
    //auto anchor_lastHLine = hline->be<AnchorObj_HLine*>();
    //检查是否处于页边距内
    bool bGotoNextPage_or_Column = false;
    if(! page) return;
    if(max_obj_y > page->getBottomLineY())
    { //在页底
        if(page->getContentHeight() < contentBottom - contentTop + page_topMargin) //内容比页面还高
            return;

        /** 检查分栏 */
        if(leftLine->isWordPageLine()) { //该hline是word布局
            auto nextColumn = page->getNextColumn(leftLine); //下一栏
            if(nextColumn) {
                //跳到下一栏
                leftLine = static_cast<AnchorObj_VLine*>(nextColumn->leftLine);
                rightLine = static_cast<AnchorObj_VLine*>(nextColumn->rightLine);
                y = page->getTopLineY() + page_topMargin - contentTop;
            } else {
                if(! page->next_page) {
                    //自动生成下一页
                    if(!SoupleManager::addInheritPage()) return;
//page->width,page->height,page->topMargin,page->bottomMargin);
                }
                y += page->next_page->top_y + page->next_page->topMargin - min_obj_y;
                page = page->next_page;
                if(logic_nextHLine) {
                    leftLine = logic_nextHLine->leftLine;
                    rightLine = logic_nextHLine->rightLine;
                } else {
                    // ...todo
                    if(page->page_type == Helper::Word_Page && page->columns.size() > 0) { //下一页也是Word_Page
                        auto& column = page->columns.front();
                        leftLine = column.leftLine->be<AnchorObj_VLine*>();
                        rightLine = column.rightLine->be<AnchorObj_VLine*>();
                    } else {

                    }
                }
            }
        } else {
            //需要把hline移动到下一页,如果下一页存在(可以自动生成下一页吗?)
            if(! page->next_page) {
                //自动生成下一页
                SoupleManager::addInheritPage();
                qDebug() << "spanPage: " << name << contentBottom - contentTop + page_topMargin;
            }
            y += page->next_page->top_y + page->next_page->topMargin - min_obj_y;
            page = page->next_page;
        }
        bGotoNextPage_or_Column = true;
    } else {
        if(min_obj_y < page->getTopLineY())
        { //在页顶
            //先考虑最上面的hline，在页顶意味着用户操作导致hline处于页顶
            if( ! anchor_lastHLine || anchor_lastHLine->page != page)
            { //本hline为独立hline，即在本页内不受锚定
                y += page->getTopLineY() - min_obj_y; //移动上边沿到页顶
                page_topMargin = 0;
            }
        }
    }


    //检查是否需要和锚定上标线转移到同页(前提是刚才没有进行页、栏转移)
    if(!bGotoNextPage_or_Column) {
        if(leftLine->isWordPageLine()) {
            if(anchor_lastHLine && anchor_lastHLine->page
                && anchor_lastHLine->leftLine != leftLine) { //不在同一栏
                auto prev_column = page->getPrevColumn(leftLine);
                //if(!prev_column) { //不在同一页
                if(!prev_column) {
                    // 这里必须考虑一个情况，即上一页也分栏的情况。如果上一页末尾栏为空，而上一栏满了，则这里会空出一栏却不转移。
                    const Page* prev_page = anchor_lastHLine->page;
                    const Page* this_prev_page = page->prev_page;
                    if( ( anchor_lastHLine->getContentBottom() - contentTop + topMargin + contentBottom
                         <= this_prev_page->getBottomLineY() )  // 条件1~ 有空
                        ||
                        ( prev_page->page_type == Helper::Word_Page  // 条件2~上一页最后一栏是空的！
                            && prev_page->columns.back().leftLine != anchor_lastHLine->leftLine )
                    ) {
                        page = prev_page; //上页转移
                        y = anchor_lastHLine->getContentBottom() - contentTop + topMargin;
                        leftLine = anchor_lastHLine->leftLine;
                        rightLine = anchor_lastHLine->rightLine;
                        //if(qmlItem) qDebug() << name << "上页转移";
                    }
                } else { //同页不同栏
                    if(anchor_lastHLine->getContentBottom() - contentTop + topMargin + contentBottom
                           <= page->getBottomLineY()) {
                        //左栏转移
                        leftLine = static_cast<AnchorObj_VLine*>(prev_column->leftLine);
                        rightLine = static_cast<AnchorObj_VLine*>(prev_column->rightLine);
                        y = anchor_lastHLine->getContentBottom() - contentTop + topMargin; //更新y坐标
                    }
                }
            }
        } else {
            if(anchor_lastHLine && anchor_lastHLine->page != page
                && anchor_lastHLine->page) {
                if(anchor_lastHLine->getContentBottom() - contentTop + topMargin + contentBottom
                    <= anchor_lastHLine->page->getBottomLineY()) {
                    page = anchor_lastHLine->page; //上页转移
                    y = anchor_lastHLine->getContentBottom() - contentTop + topMargin;
                }
            }
        }
    }
}

void AnchorObj_HLine::dealAnchor(AnchorObj_HLine*anchor_lastHLine) noexcept {
    if(! anchor_lastHLine) return;
        //if(anchor_lastHLine->page != page && anchor_lastHLine->y+topMargin <)

        //if(topMargin >= -1e-3) {
    if(page && anchor_lastHLine->page && page->index < anchor_lastHLine->page->index)
    {
        //如果页错位，表明更新错乱
        page = anchor_lastHLine->page; //按照同页处理，[不可删除]
        if(leftLine && leftLine->isWordPageLine()) {
            leftLine = anchor_lastHLine->leftLine;  //2025/8/19添补 [!]
            rightLine = anchor_lastHLine->rightLine;
        }
    }
    //}
    if(leftLine && leftLine->isWordPageLine()) {  //word-line布局

        if(anchor_lastHLine->leftLine == leftLine) { //同栏
            y = anchor_lastHLine->getContentBottom() - contentTop + topMargin; //更新y坐标
            //qDebug() << name << "同栏锚定";
        } else if(page){ //不同栏
            //检查顺序是否错乱
            if(anchor_lastHLine->page &&
                anchor_lastHLine->page->index < page->index) { //异页
                if(page->page_type == Helper::Word_Page
                    && leftLine != page->columns.front().leftLine) {
                    leftLine = page->columns.front().leftLine->be<WordPage_VLine*>();
                    rightLine = page->columns.front().rightLine->be<WordPage_VLine*>();
                }
                y = page->getTopLineY() + page_topMargin - contentTop;
            } else { //同页
                if(anchor_lastHLine->leftLine) {
                    int last_column_id = anchor_lastHLine->leftLine->be<WordPage_VLine*>()->column_id;
                    int column_id = leftLine->be<WordPage_VLine*>()->column_id;
                    if(last_column_id > column_id || column_id - last_column_id > 1) {
                        //恢复到同一栏
                        leftLine = anchor_lastHLine->leftLine;
                        rightLine = anchor_lastHLine->rightLine;
                        y = anchor_lastHLine->getContentBottom() - contentTop + topMargin; //更新y坐标
                        //qDebug() << name << "恢复同栏";
                    } else {
                        y = page->getTopLineY() + page_topMargin - contentTop;
                        //qDebug() << name << "同页异栏";
                    }
                } else {
                    y = page->getTopLineY() + page_topMargin - contentTop;
                }
            }
        }

        //检查leftLine
        // if(page && page->page_type == Helper::Word_Page) {
        //     auto word_leftLine = leftLine->be<WordPage_VLine*>();
        //     if(word_leftLine->page_index != page->index) {
        //         leftLine = page->columns.front().leftLine->be<AnchorObj_VLine*>();
        //         rightLine = page->columns.front().rightLine->be<AnchorObj_VLine*>();
        //     }
        // }

    } else { //常规布局
        if(anchor_lastHLine->page == page) { //同页
            y = anchor_lastHLine->getContentBottom() - contentTop + topMargin; //更新y坐标
        } else { //异页，使用page_topMargin
            if(page)
            {
                y = page->getTopLineY() + page_topMargin - contentTop;
            }
        }
    }
}

void AnchorObj_HLine::dealLayout() //hLine处理布局，实现溢出和收缩
{
    float old_y = y;

    //qDebug() << name << "dealLayout()";
    //qDebug() << name << ".dealLayout()";
    auto anchor_lastHLine = hline->be<AnchorObj_HLine*>();
    auto old_page = page;

    dealAnchor(anchor_lastHLine);
    dealSpan(anchor_lastHLine,y+contentTop,y+contentBottom); //处理跨页、跨栏

    if(!page || abs(old_y - y)>1e-2) page = SoupleManager::getPage(*this);

    //检查leftLine
    check_after_dealSpan();

    if(leftLine) {
        x = leftLine->x;
    }

    //updateDataToQmlItem(qmlItem);

    if(!(leftLine && rightLine)) {
        width = 200;
        updateDataToQmlItem(qmlItem);
        return; //未设定左右标线
    }

    width = rightLine->x - leftLine->x;

    if(leftObj == 0) { //空行
        rightObj = 0;
        contentTop = 0;
        contentBottom = 0; //空行默认占据20高度
        //dealSpan(anchor_lastHLine,y,y); //处理跨页、跨栏
        if(Helper::isQmlItemValid(qmlItem))
            qmlItem->setPosition({x,y-qmlItem->height()/2});
        //if(! logic_nextHLine || ! logic_nextHLine->leftObj) return;
        if(! logic_nextHLine) return;
        if(logic_nextHLine->leftObj) {
            auto dropObj = logic_nextHLine->leftObj->dropLeft(rightLine->x - leftLine->x);
            if(dropObj) { //收缩
                //logic_nextHLine->moveFlowAttachers(-dropObj->contentLength()); //移动附着符
                insertOnRight(dropObj);
                dropObj->dealLayout();
                SoupleManager::requestUpdateHLine(logic_nextHLine); //传递更新信号
            }
        }
        if(abs(y - old_y) > 0.1) {
            requestUpdateAnchorNextHLine();
            //SoupleManager::requestUpdateHLine(logic_nextHLine); //传递更新信号
        }
        //this->chopFlowAttacher(logic_lastHLine,logic_nextHLine);
        if(leftObj == 0)
            return;
    } //else leftObj != nullptr {

    leftObj->x = x;

    auto obj = leftObj;
    float min_obj_y = 2e9, max_obj_y = -1e8; //记录元素上下边y坐标

    //qDebug() << name << ".y = " << y;

    //if(qmlItem) {
        //qDebug() << name << "可见，leftObj:" << leftObj << "isOnline: " <<
        //        ((AnchorObj_HLine*)(leftObj->hline) == this);
        //qDebug() << name << "y:" << y;
    //}

    float impact_hscale_width = 0, solid_width = 0;

    bool isHscaleKilled = false;

    //int this_contentLength = 0; //内容索引

    while(1) {
        obj->dealLayout();    //处理布局
        obj->tryMergeRight(); //尝试合并
        //this_contentLength += obj->contentLength(); //内容索引rd
        auto& objInfo = obj->objInfo();
        if(objInfo.anchorInfo.isRealHeight) { //看它是否具备“真实”高度
#ifdef Q_OS_ANDROID
            min_obj_y = std::min(min_obj_y,obj->y);
            max_obj_y = std::max(max_obj_y,obj->y+obj->height);
#else
            min_obj_y = std::min(min_obj_y,obj->y);
            max_obj_y = std::max(max_obj_y,obj->y+obj->height);
#endif
        }
        if(qmlItem) {
            if(objInfo.anchorInfo.impact_hscale) {
                impact_hscale_width += obj->width;  //影响水平放缩的obj的宽度
            }
            if(objInfo.anchorInfo.isSelfWidth){
                solid_width += obj->width;
            }
            if(objInfo.anchorInfo.kill_hscale) {
                isHscaleKilled = true; //取消水平放缩
            }
        }
        if(obj->rightObj == nullptr) {
            rightObj = obj;
            break;
        }
        obj = obj->rightObj;
    }

    // Horizontal_Scale原则
    // 不允许修改原有的任何数据，只可以修改可见的Qml对象。也就是说，水平放缩仅仅影响视觉效果。
    // 注意，在输出PDF时，需要考虑水平放缩。

    // solid_width: 真实内容宽度
    if(isHscaleKilled) { //水平放缩受到禁用，但万一上一次应用了水平放缩呢？
        if(this->horizontal_scale > 1.000001) {
            this->horizontal_scale = 1.0;  //撤回水平放缩
            obj = leftObj;
            while(obj) {
                if(obj->objInfo().anchorInfo.impact_hscale) {
                   obj->showHScale(1.0,0);
                } else {
                    if(Helper::isQmlItemValid(obj->qmlItem)) {
                        obj->qmlItem->setX(obj->x);
                    }
                }
                obj = obj->rightObj;
            }
        }
    } else if(qmlItem && logic_nextHLine) {
        //计算水平放缩
        float new_horizontal_scale = (width - solid_width + impact_hscale_width) / impact_hscale_width;
        //if(qAbs(new_horizontal_scale - this->horizontal_scale) > 0.01) {
            this->horizontal_scale = std::max(new_horizontal_scale,1.0f);
            //赋予水平放缩
            obj = leftObj;
            float addX = 0;
            while(obj) {
                if(obj->objInfo().anchorInfo.impact_hscale) {
                    addX += obj->showHScale(this->horizontal_scale,addX);
                } else {
                    if(Helper::isQmlItemValid(obj->qmlItem)) {
                        obj->qmlItem->setX(obj->qmlItem->x() + addX);
                    }
                }
                obj = obj->rightObj;
            }
        //}
    }

    //更新contentTop/Bottom
    if(min_obj_y < 1.9e9) { //hline上有占据真实高度的obj
        contentTop = min_obj_y - y;
        contentBottom = max_obj_y - y;
    }

    // dealSpan(anchor_lastHLine,min_obj_y,max_obj_y); //处理跨页、跨栏


    if(abs(y - old_y) > 1e-2) {
        //y坐标变化，需要更新nextHLine
        requestUpdateAnchorNextHLine();
        //if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setY(y - qmlItem->height()/2);
        if(leftLine) leftLine->notifyHLineYChanged(this);
        if(rightLine) rightLine->notifyHLineYChanged(this);
    }

    //获取页面:复杂度约为常数3~12之间，可以放心调用，但还是节俭些好
    //if(! page || abs(y - old_y) > 1) {
    //    page = SoupleManager::getPage(*this);
    //}

    //updateDataToQmlItem(qmlItem);

    if( Helper::isQmlItemValid(qmlItem) ) {
        //qDebug() << "hline.y: " << y;
        //qDebug() << "qmlItem.y: " << qmlItem->y();
        qmlItem->setPosition({x,y-qmlItem->height()/2});
        qmlItem->setWidth(width);
        height = qmlItem->height();
    }

    // 考虑一下溢出、收缩对turnback的影响

    bool hasDrop = false;
    int drop_contentLength = 0; //记录drop的内容长度
    while(rightObj && rightObj->x + rightObj->width > rightLine->x) {
        auto dropObj = rightObj->dropRight(rightObj->x + rightObj->width - rightLine->x);
        //qDebug() << "尝试溢出";
        if(dropObj) {  //溢出
            if(! dropObj->canSplit()) {
                if(dropObj->objInfo().anchorInfo.isSelfWidth
                    && dropObj->width >= width &&
                    (!rightObj || rightObj->x < getRightX())) {
                    this->insertOnRight(dropObj);
                    break; //注：若dropObj宽度大于行宽、且rightObj左边x小于行右边线，则不drop！防止无限drop
                }
            }
            hasDrop = true;
            drop_contentLength += dropObj->contentLength();
            if(logic_nextHLine == nullptr) { //未定义下标线
                //创建下标线
                createNextLine();
            }
            logic_nextHLine->insertOnLeft(dropObj); //从左端插入
            dropObj->tryMergeRight(); //溢出时，自动尝试合并
            dropObj->dealLayout();

        } else break;
    }

    //上面溢出，这里不可能再收缩的，!hasDrop
    if(!hasDrop && logic_nextHLine && rightObj) {
        while(logic_nextHLine->leftObj && rightObj->x + rightObj->width < rightLine->x) {
            auto dropObj = logic_nextHLine->leftObj->dropLeft(rightLine->x - rightObj->x - rightObj->width);
            if(dropObj) { //收缩
                drop_contentLength -= dropObj->contentLength();
                hasDrop = true;
                insertOnRight(dropObj);
                dropObj->dealLayout();
                if(dropObj->leftObj) {
                    auto o = dropObj->leftObj;
                    o->tryMergeRight(); //收缩时，自动尝试合并
                    o->dealLayout();
                }
                //SoupleManager::requestUpdateHLine(logic_nextHLine); //传递更新信号
            } else break;
        }
    }


    if(hasDrop && logic_nextHLine) {
        //logic_nextHLine->moveFlowAttacher(drop_contentLength); //移动附着符
        SoupleManager::requestUpdateHLine(logic_nextHLine); //传递更新信号 [2025/7/16修改]
    }

    // 截断附着符
    //this->chopFlowAttacher(logic_lastHLine,logic_nextHLine);

    //if(! page || abs(y - old_y) > 1) {
       //page = SoupleManager::getPage(*this);
    //}

    // if(page != old_page){
    //     qDebug() << name <<"page changed:"<<(old_page ? old_page->index : -1)
    //              << page->index << "old_y: " << old_y << "new_y: " << y;
    // }

}

void AnchorObj_HLine::forceCalculateHorizontalScale() noexcept {

}

void AnchorObj_HLine::removeSelf(bool dead) {

    if(dead_sign) return; //保护

    //执行注册的死亡回调函数，通知死亡。请保持一开始就做这件事。
    for(auto func : onDelete_Functions)
        func();

    auto *anchor_lastHLine = hline->be<AnchorObj_HLine*>();

    //HLine移除需要考虑到其他HLine引用自身的情况
    for(auto aline : anchor_nextHLine) {
        //aline->hline = hline; //转移锚定引用
        //if(hline) hline->as<AnchorObj_HLine*>()->anchor_nextHLine.push_back(aline);
        qDebug() << "aline: " << aline->getName();
        //aline->setAnchorLastHLine(getAnchorLastLine()); //转移锚定引用
        aline->hline = hline;
        if(anchor_lastHLine)
            anchor_lastHLine->anchor_nextHLine.push_back(aline); //转移锚定引用
    }

    anchor_nextHLine.clear();

    if(logic_nextHLine)
        logic_nextHLine->setLogicLastHLine(logic_lastHLine); //转移逻辑下标线的逻辑上标线

    if(anchor_lastHLine)
        anchor_lastHLine->anchor_nextHLine.remove(this); //删除记录

    if(dead) {
        dead_sign = true;
        SoupleManager::notifyHLineDead(this); //hline死亡必须通知SoupleManager
    }
}

int AnchorObj_HLine::dealCommandFromQmlItem(int command,const QVariant& arg)
{

    switch(command) {
    case Helper::Y_UP: {
        //float old_y = y;
        //qDebug() << "old_y: " << old_y;
        y = arg.toFloat();
        //qDebug() << "Y_UP:" << y;
        //const Page *old_page = page;
        page = SoupleManager::getPage(*this);
        // if(old_page && page && old_page->index > page->index) {
        //     y = old_y;
        //     break;
        // }
        auto anchor_lastHLine = hline->be<AnchorObj_HLine*>(); //<?>
        if(leftLine && leftLine->isWordPageLine()) { //word布局hline(考虑分栏)
            if(anchor_lastHLine && anchor_lastHLine->leftLine == leftLine) { //同一栏
                topMargin = getContentTop() - anchor_lastHLine->getContentBottom();  //更新topMargin
            } else if(page){ //同页不同栏 或 异页
                //qDebug() << "page->index=" << page->index;
                //qDebug() << "page-Height = " << page->height;
                //qDebug() << "y=>" << y;
                page_topMargin = getContentTop() - page->getTopLineY();
                //qDebug() << name << ".page_topMargin => " << page_topMargin;
            }
            requestUpdateAnchorNextHLine();
        } else { // 常规布局hline
            if(hline && anchor_lastHLine && page == anchor_lastHLine->page) {
                topMargin = getContentTop() - anchor_lastHLine->getContentBottom();  //更新topMargin
            } else if(page){
                page_topMargin = getContentTop() - page->getTopLineY();
            }
            //SoupleManager::requestUpdateHLine(this);
            //qDebug() << "y_up!";
            requestUpdateAnchorNextHLine(); //更新信号
            if(leftLine) leftLine->notifyHLineYChanged(this);
            if(rightLine) rightLine->notifyHLineYChanged(this);
        }
        break;
    }
    case Helper::LOGIC_LASTHLINE_UP: {
        if(logic_lastHLine && logic_lastHLine->name == arg.toString()) return 0;
        auto it = hash_hline.find(arg.toString());
        if(it == hash_hline.end()) return Helper::Error_Invalid_Data;
        setLogicLastHLine((*it)->be<AnchorObj_HLine*>());
        // logic_lastHLine = *it;
        // logic_lastHLine->logic_nextHLine = this; //更新逻辑上标线的逻辑下标线
        if( ! hline) {
            setAnchorLastHLine((*it)->be<AnchorObj_HLine*>());
            //hline = logic_lastHLine; //锚定上标线以逻辑上标线为缺省值
            //hline->as<AnchorObj_HLine*>()->anchor_nextHLine = this; //更新锚定上标线的锚定下标线
            SoupleManager::requestUpdateHLine(this); //请求更新布局
        }
        break;
    }
    case Helper::ANCHOR_LASTHLINE_UP: {
        if(hline && hline->be<AnchorObj_HLine*>()->name == arg.toString()) return 0;
        auto it = hash_hline.find(arg.toString());
        if(it == hash_hline.end()) return Helper::Error_Invalid_Data;
        setAnchorLastHLine((*it)->as<AnchorObj_HLine*>());
        // hline = *it;
        // hline->as<AnchorObj_HLine*>()->anchor_nextHLine = this; //更新锚定上标线的锚定下标线
        SoupleManager::requestUpdateHLine(this); //请求更新布局
        break;
    }
    case Helper::TOP_MARGIN_UP: {
        qDebug() << name << "top_margin_up";
        topMargin = arg.toFloat();
        if(leftLine && leftLine->isWordPageLine()) {
            if(hline && hline->be<AnchorObj_HLine*>()->leftLine == leftLine) {
                y = hline->be<AnchorObj_HLine*>()->getContentBottom() + topMargin - contentTop;
            }
        } else if(hline && hline->be<AnchorObj_HLine*>()->page == page) {
            y = hline->be<AnchorObj_HLine*>()->getContentBottom() + topMargin - contentTop;
        }
        if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setY(y - qmlItem->height()/2);
        SoupleManager::requestUpdateHLine(this); //请求更新布局

        /*用户是否希望这一行作为[段落]的首行？*/
        //判断依据：1.该行行距大于上一行 2.上一行末尾是换行 3.本行无段落标志
        if(logic_lastHLine
            && dynamic_cast<AnchorObj_PHLeft*>(leftObj) == nullptr
            && dynamic_cast<AnchorObj_PHRight*>(logic_lastHLine->rightObj) != nullptr) {
            if( logic_lastHLine->topMargin+0.001 < topMargin) {
                qDebug() << logic_lastHLine->topMargin << topMargin;
                Helper::showChooseBoard(id,QPoint{(int)x,(int)y},Helper::Dir_LEFT|Helper::Dir_TOP,
                                        "要添加\"段落\"标记吗?",{"添加","不添加"},[](const QString& arg){
                    int i = arg.toInt();
                    if(i == 0) {
                        qDebug() << "添加！！！";
                    }
                });
            } else {
                Helper::tryHideChooseBoard(id); //隐藏【选择】板
            }

        }

        //qDebug() << "top_margin_up!";
        break;
    }
    case Helper::NAME_UP: {
        if(hash_hline.contains(arg.toString()))
            return Helper::Error_Repeat; //重复
        hash_hline.remove(name);
        hash_hline[name = arg.toString()] = this;
    }
    case Helper::LEFTLINE_UP: {
        if(leftLine && leftLine->name == arg.toString()) return 0;
        auto it = AnchorObj_VLine::hash_vline.find(arg.toString());
        if(it == AnchorObj_VLine::hash_vline.end()) {
            return Helper::Error_Invalid_Data;
        } else {
            leftLine = *it;
            SoupleManager::requestUpdateHLine(this);
            //updateDataToQmlItem(qmlItem); //更新数据
        }
    }
    case Helper::RIGHTLINE_UP: {
        if(leftLine && leftLine->name == arg.toString()) return 0;
        auto it = AnchorObj_VLine::hash_vline.find(arg.toString());
        if(it == AnchorObj_VLine::hash_vline.end()) {
            return Helper::Error_Invalid_Data;
        } else {
            rightLine = *it;
            SoupleManager::requestUpdateHLine(this);
            //updateDataToQmlItem(qmlItem); //更新数据
        }
    }
    default:
        return Helper::Error_Invalid_Command; //无效指令
    }
    return 0; //成功
}

// void AnchorObj_HLine::gotoNextPage() //移动到下一页
// {

// }

// void AnchorObj_HLine::gotoPrevPage() //移动到上一页
// {

// }

void FitLine_for_AnchorObj_HLine::dealLayout() {
    qDebug() << "fitline:y=" << y;
    if(lastLine) {
        if(nextLine && abs(y-lastLine->y)>0.1) {
            SoupleManager::requestUpdateHLine(nextLine);
        }
        y = lastLine->y;
        if(lastLine->page)
            page = lastLine->page; //[important]
        else page = SoupleManager::getPage(y);
    }
}
