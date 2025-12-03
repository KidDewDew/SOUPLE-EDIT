#include "selectionmanager.h"
#include "souplemanager.h"
#include "anchorobj_phright.h"
#include "anchorobj_phleft.h"
#include "anchorobj_image.h"
#include <QGuiApplication>

extern QGuiApplication *global_app;

void SelectionManager::recalculateSelection() noexcept
{
    int begin_id = select_queue.front();
    select_queue.clear();
    push(begin_id);
    float to_x = select_end_x, to_y = select_end_y;
    select_end_x = select_begin_x;
    select_end_y = select_begin_y;
    moveSelect(to_x,to_y); //让moveSelect从新开始计算
}

void SelectionManager::sendCommands(int command,const QVariant& arg)
{
    for(auto& si : selection_items) {
        Obj *obj = SoupleManager::getObjById(si.obj_id);
        if(! obj) continue;
        obj->selectionCommand(command,arg);
    }
}

//统计属性
void SelectionManager::staticProperty(QObject* pobj)
{
    int para_count,line_count,charNum,punction_Num,image_Num;
    para_count = line_count = charNum = punction_Num = 0;
    bool multi_fontsize,multi_textColor,multi_strokeColor,multi_strokeWidth,
        multi_fillColor,multi_pathColor,multi_bold,multi_italic,multi_underline,
        multi_vTextAlignMode,multi_vTextAlignOffset,multi_vImageAlignMode,
        multi_vImageAlignOffset,multi_enableStroke,multi_enableFill,multi_family;
    multi_fontsize = multi_textColor = multi_strokeColor = multi_strokeWidth =
        multi_fillColor = multi_pathColor = multi_bold = multi_italic = multi_underline
        = multi_vTextAlignMode = multi_vTextAlignOffset = multi_vImageAlignMode =
        multi_vImageAlignOffset = multi_enableStroke = multi_enableFill = multi_family
        = false;
    qreal fontsize = -1, strokeWidth;
    int vTextAlignMode;
    qreal vTextAlignOffset = 0;
    QColor textColor,strokeColor,pathColor;
    bool enableStroke = false, enableFill = false;
    bool italic = false,underline = false,bold = false;
    bool hasTextColor = false, hasStrokeColor = false, hasPathColor = false,
        hasBold = false, hasItalic = false, hasUnderline = false, hasVTextAlignOffset = false,
        hasVTextAlignMode = false, hasEnableStroke = false, hasEnableFill = false, hasFamily = false;
    QString family;

    for(auto& si : selection_items) { //遍历所有选择项
        Obj *obj = SoupleManager::getObjById(si.obj_id);
        if(! obj) continue;

        const std::type_info& ti = typeid(*obj);
        if(typeid(AnchorObj_PHRight).before(ti)) {
            ++line_count;
        } else if(typeid(AnchorObj_PHLeft).before(ti)) {
            ++para_count;
        }

        //QVariant charCount =

        if(! multi_family) {
            QVariant v = obj->selectionGetData(Helper::SP_Family);
            if(! v.isNull()) {
                if(hasFamily == false) {
                    hasFamily = true;
                    family = v.toString();
                } else if(v.toString() != family) {
                    multi_family = true;
                }
            }
        }
        if( ! multi_fontsize) {
            QVariant fontsize2 = obj->selectionGetData(Helper::SP_FontSize);
            if( ! fontsize2.isNull()) {
                if(fontsize < 0)
                    fontsize = fontsize2.toReal();
                else if(std::abs(fontsize - fontsize2.toReal()) > 1e-2)
                    multi_fontsize = true; //多种字体大小
            }
        }
        if( ! multi_textColor) {
            QVariant tc = obj->selectionGetData(Helper::SP_TextColor);
            if( ! tc.isNull()) {
                if(hasTextColor == false) {
                    textColor = tc.value<QColor>();
                    hasTextColor = true;
                }
                else if(tc.value<QColor>() != textColor)
                    multi_textColor = true; //多种文本填充颜色
            }
        }
        if( ! multi_strokeColor) {
            QVariant tc = obj->selectionGetData(Helper::SP_TextStrokeColor);
            if( ! tc.isNull()) {
                if(hasStrokeColor == false) {
                    strokeColor = tc.value<QColor>();
                    hasStrokeColor = true;
                }
                else if(tc.value<QColor>() != strokeColor)
                    multi_strokeColor = true; //多种文本描边颜色
            }
        }
        if( ! multi_bold) {
            QVariant b = obj->selectionGetData(Helper::SP_FontBold);
            if(! b.isNull()) {
                if( ! hasBold) {
                    bold = b.toBool();
                    hasBold = true;
                }
                else if(b.toBool() != bold) multi_bold = true;
            }
        }
        if( ! multi_italic) {
            QVariant b = obj->selectionGetData(Helper::SP_FontItalic);
            if(! b.isNull()) {
                if( ! hasItalic) {
                    italic = b.toBool();
                    hasItalic = true;
                }
                else if(b.toBool() != italic) multi_italic = true;
            }
        }
        if( ! multi_underline) {
            QVariant b = obj->selectionGetData(Helper::SP_UnderLine);
            if(! b.isNull()) {
                if( ! hasUnderline) {
                    underline = b.toBool();
                    hasUnderline = true;
                }
                else if(b.toBool() != underline) multi_underline = true;
            }
        }
        if( ! multi_enableStroke) {
            QVariant b = obj->selectionGetData(Helper::SP_TextEnableStroke);
            if(! b.isNull()) {
                if( ! hasEnableStroke) {
                    enableStroke = b.toBool();
                    hasEnableStroke = true;
                }
                else if(b.toBool() != enableStroke) multi_enableStroke = true;
            }
        }
        if( ! multi_enableFill) {
            QVariant b = obj->selectionGetData(Helper::SP_TextEnableFill);
            if(! b.isNull()) {
                if( ! hasEnableFill) {
                    enableFill = b.toBool();
                    enableFill = true;
                }
                else if(b.toBool() != enableFill) multi_enableFill = true;
            }
        }
        if( ! multi_vTextAlignMode) {
            QVariant b = obj->selectionGetData(Helper::SP_vTextAlignMode);
            if(! b.isNull()) {
                if( ! hasVTextAlignMode) {
                    vTextAlignMode = b.toInt();
                    hasVTextAlignMode = true;
                }
                else if(b.toInt() != vTextAlignMode) multi_vTextAlignMode = true;
            }
        }
        if( ! multi_vTextAlignOffset) {
            QVariant b = obj->selectionGetData(Helper::SP_vTextAlignOffset);
            if(! b.isNull()) {
                if( ! hasVTextAlignOffset) {
                    vTextAlignOffset = b.toDouble();
                    hasVTextAlignOffset = true;
                }
                else if(std::abs(b.toDouble() - vTextAlignOffset) > 1e-3) multi_vTextAlignOffset = true;
            }
        }
    }

    pobj->setProperty("charNum",charNum);
    pobj->setProperty("punctionNum",punction_Num);
    pobj->setProperty("paraNum",para_count);


    if(multi_family) pobj->setProperty("mfamily",true);
    else {
        pobj->setProperty("mfamily",false);
        pobj->setProperty("efamily",hasFamily);
        pobj->setProperty("family",family);
    }
    if(multi_fontsize) pobj->setProperty("mfontsize",true);
    else {
        pobj->setProperty("mfontsize",false);
        pobj->setProperty("efontsize",fontsize > 0);
        pobj->setProperty("fontsize",fontsize);
    }
    if(multi_textColor) pobj->setProperty("mtextcolor",true);
    else {
        pobj->setProperty("mtextcolor",false);
        pobj->setProperty("etextcolor",hasTextColor);
        pobj->setProperty("textcolor",textColor);
    }
    if(multi_strokeColor) pobj->setProperty("mstrokecolor",true);
    else {
        pobj->setProperty("mstrokecolor",false);
        pobj->setProperty("estrokecolor",hasStrokeColor);
        pobj->setProperty("strokecolor",strokeColor);
    }
    if(multi_enableStroke) pobj->setProperty("menablestroke",true);
    else {
        pobj->setProperty("menablestroke",false);
        pobj->setProperty("eenablestroke",hasEnableStroke);
        pobj->setProperty("enablestroke",enableStroke);
    }
    if(multi_enableFill) pobj->setProperty("menablefill",true);
    else {
        pobj->setProperty("menablefill",false);
        pobj->setProperty("eenablefill",hasEnableFill);
        pobj->setProperty("enablefill",enableFill);
    }
    if(multi_bold) pobj->setProperty("mbold",true);
    else {
        pobj->setProperty("mbold",false);
        pobj->setProperty("ebold",hasBold);
        pobj->setProperty("bold",bold);
    }
    if(multi_italic) pobj->setProperty("mitalic",true);
    else {
        pobj->setProperty("mitalic",false);
        pobj->setProperty("eitalic",hasItalic);
        pobj->setProperty("italic",italic);
    }
    if(multi_underline) pobj->setProperty("munderline",true);
    else {
        pobj->setProperty("munderline",false);
        pobj->setProperty("eunderline",hasUnderline);
        pobj->setProperty("underline",underline);
    }
    if(multi_vTextAlignMode) pobj->setProperty("mvtextalignmode",true);
    else {
        pobj->setProperty("mvtextalignmode",false);
        pobj->setProperty("evtextalignmode",hasVTextAlignMode);
        pobj->setProperty("vtextalignmode",vTextAlignMode);
    }
    if(multi_vTextAlignOffset) pobj->setProperty("mvtextalignoffset",true);
    else {
        pobj->setProperty("mvtextalignoffset",false);
        pobj->setProperty("evtextalignoffset",hasVTextAlignOffset);
        pobj->setProperty("vtextalignoffset",vTextAlignOffset);
    }
}

//@brief 通知SelectionManager，前端开始选择
//@arg id:开始选择的对象id begin_x:鼠标X坐标(全局) begin_y:鼠标Y坐标(全局)
void SelectionManager::beginSelect(int id,float begin_x,float begin_y) noexcept
{
    //return;
    select_queue.clear();
    push(id);
    select_begin_x = select_end_x = begin_x;
    select_begin_y = select_end_y = begin_y;
    _hasBegun = true;
    isSelectionKeep = false;
    qDebug() << "beginSelect(" << id << begin_x << begin_y;
}

//@brief 通知SelectionManager，前端拖动选择
void SelectionManager::moveSelect(float to_x,float to_y) noexcept
{
    if(isSelectionKeep == false) {
        emit Helper::helper->beginSelection();
        isSelectionKeep = true;
        //开始选择，我们需要让开始点的qml对象失去focus
        auto begin_obj = SoupleManager::getObjById(getBeginID());
        if(Helper::isQmlItemValid(begin_obj->qmlItem)) {
            begin_obj->qmlItem->setFocus(false);
        }
        start_pcpos = SoupleManager::get_page_with_column(begin_obj);
    }

    select_end_x = to_x;
    select_end_y = to_y;

    // 根据select_end_x/y更新end_page和end_column

    end_pcpos = SoupleManager::get_pt_page_with_coulmn(select_end_x,select_end_y);

    if(SoupleManager::getObjById(getBeginID()) == 0) {
        stopSelect();
    }

    //if(select_end_y >= select_begin_y) {  //正向选择
    if(! isRev()) {
        //处理鼠标移动
        while(select_queue.size() > 1) {
            auto end_obj = SoupleManager::getObjById(getEndID());
            if(end_obj == 0) pop();
            else
            if(! isObjInSelection(end_obj)) {
                //qDebug() << "[pop select] " << end_obj->id;
                unselect_set.insert(getEndID()); //记录撤销
                pop();
            }
            else break;
        }
        auto nex_obj = SoupleManager::getObjById(getEndID())->as<AnchorObj*>();
        while(true) {
            if(nex_obj->rightObj == 0) {
                auto hline = nex_obj->hline->be<HorLine_Base*>();
                auto nex_hline = hline->getNextLine();
                if(nex_hline && nex_hline->leftObj) {
                    nex_obj = nex_hline->leftObj;
                } else break;
            } else nex_obj = nex_obj->rightObj;

            if(isObjInSelection(nex_obj) == false) {
                break;
            }
            select_queue.push_back(nex_obj->id);
            //qDebug() << "[push select] " << getEndID();
        }
    } else {  //反向选择
        return;
        //处理鼠标移动
        while(select_queue.size() > 1) {
            auto end_obj = SoupleManager::getObjById(getEndID());
            if(end_obj == 0) pop();
            else
                if(! isObjInSelection_rev(end_obj)) {
                    //qDebug() << "[pop select] " << end_obj->id;
                    unselect_set.insert(getEndID()); //记录撤销
                    pop();
                }
                else break;
        }
        auto nex_obj = SoupleManager::getObjById(getEndID())->as<AnchorObj*>();
        while(true) {
            if(nex_obj->leftObj == 0) {
                auto hline = nex_obj->hline->as<AnchorObj_HLine*>();
                auto nex_hline = hline->getLogicLastLine();
                if(nex_hline && nex_hline->rightObj) {
                    nex_obj = nex_hline->rightObj;
                } else break;
            } else nex_obj = nex_obj->leftObj;

            if(isObjInSelection(nex_obj) == false) {
                break;
            }
            select_queue.push_back(nex_obj->id);
            //qDebug() << "[push select] " << getEndID();
        }
    }
}

//@brief 通知SoupleManager，停止本次选择
void SelectionManager::stopSelect() noexcept
{
    _hasBegun = false;
    qDebug() << "stopSelect()";
    if(isSelectionKeep) { //需要计算SelectionItem
        recalculateSelection(); //重新计算一遍选择内容
        selection_items.clear();
        int i = 0;
        for(int id : select_queue) { //这里的id不会有无效的
            Obj* obj = SoupleManager::getObjById(id);
            float x1 = obj->x, x2 = obj->getRightX();
            if(isRev() == false) {
                if(i == 0) x1 = select_begin_x;
                if(i == select_queue.size()-1) x2 = select_end_x;
            } else {
                if(i == 0) x2 = select_begin_x;
                if(i == select_queue.size()-1) x1 = select_end_x;
            }
            SelectionItem si;
            si.obj_id = id;
            obj->positionToIndex(x1,x2,si.begin_index,si.end_index); //计算索引
            selection_items[id] = si;
            ++i;
            //qDebug() << "SelectionItem(" << id << si.begin_index << si.end_index;
        }
        emit Helper::helper->selectionStopButKeep(); //信号
    }
}

//@brief 清除选择内容
void SelectionManager::clearSelection() noexcept
{
    qDebug() << "clearSelection()";
    emit Helper::helper->clearSelection();
    _hasBegun = isSelectionKeep = false;
    for(auto id : selection_items.keys()) {
        unselect_set.insert(id);
    }
    select_queue.clear();
    selection_items.clear();
}

//@brief 通知所有“可见”的涉及被选择的qmlItem它们被(部分)选中了
//
void SelectionManager::notifyAllVisible() noexcept
{

    for(auto id : unselect_set) {
        auto obj = SoupleManager::getObjById(id);
        if(obj && Helper::isQmlItemValid(obj->qmlItem))
            QMetaObject::invokeMethod(obj->qmlItem,"selectcl"); //取消选择
    }

    unselect_set.clear();

    if(isSelectStopButKeep()) {
        //qDebug() << "notify - SelectStopButKeep";
        for(auto& si : selection_items) {
            auto obj = SoupleManager::getObjById(si.obj_id);
            //qDebug() << "SelectionItem(" << si.obj_id << si.begin_index << si.end_index;
            if(obj && Helper::isQmlItemValid(obj->qmlItem))
                QMetaObject::invokeMethod(obj->qmlItem,"selecti",QVariant{si.begin_index},QVariant{si.end_index}); //索引选择
        }
    } else {

        if(select_queue.empty() || isSelectionKeep == false) return;
        for(int i = 1; i+1 < select_queue.size(); ++i) {
            //注意：对空指针使用dynamic_cast是不会异常的，所以nullptr->as<>()返回: nullptr
            AnchorObj* obj = SoupleManager::getObjById(select_queue[i])->as<AnchorObj*>();
            if(obj && Helper::isQmlItemValid(obj->qmlItem)) {
                QMetaObject::invokeMethod(obj->qmlItem,"selectw"); //完全选择(wholly select)
            }
        }
        if(select_queue.size() > 1) {
            if(!isRev()) {
                AnchorObj* obj = SoupleManager::getObjById(getBeginID())->as<AnchorObj*>();
                if(obj && Helper::isQmlItemValid(obj->qmlItem))
                    QMetaObject::invokeMethod(obj->qmlItem,
                                              "selectp",
                                              Q_ARG(QVariant,select_begin_x),
                                              Q_ARG(QVariant,obj->getRightX()));
                obj = SoupleManager::getObjById(getEndID())->as<AnchorObj*>();
                if(obj && Helper::isQmlItemValid(obj->qmlItem))
                    QMetaObject::invokeMethod(obj->qmlItem,
                                              "selectp",
                                              Q_ARG(QVariant,obj->x),
                                              Q_ARG(QVariant,select_end_x));
            } else {
                AnchorObj* obj = SoupleManager::getObjById(getBeginID())->as<AnchorObj*>();
                if(obj && Helper::isQmlItemValid(obj->qmlItem))
                    QMetaObject::invokeMethod(obj->qmlItem,
                                              "selectp",
                                              Q_ARG(QVariant,obj->x),
                                              Q_ARG(QVariant,select_begin_x));
                obj = SoupleManager::getObjById(getEndID())->as<AnchorObj*>();
                if(obj && Helper::isQmlItemValid(obj->qmlItem))
                    QMetaObject::invokeMethod(obj->qmlItem,
                                              "selectp",
                                              Q_ARG(QVariant,select_end_x),
                                              Q_ARG(QVariant,obj->getRightX()));
            }
        } else { // size == 1
            AnchorObj* obj = SoupleManager::getObjById(getBeginID())->as<AnchorObj*>();

            if(obj && Helper::isQmlItemValid(obj->qmlItem))
                QMetaObject::invokeMethod(obj->qmlItem,
                                        "selectp",
                                        Q_ARG(QVariant,qMin(select_begin_x,select_end_x)),
                                        Q_ARG(QVariant,qMax(select_end_x,select_begin_x)));
        }
    }
}

bool SelectionManager::isObjInSelection_rev(const Obj* obj) noexcept {
    if(obj->y>select_begin_y || obj->y+obj->height<select_end_y) {
        return false;
    }
    if(obj->y < select_begin_y && obj->y+obj->height > select_begin_y) { //位于起始行
        return select_begin_x > obj->x;
    } else if(obj->y < select_end_y && obj->y+obj->height > select_end_y) { //位于末尾行
        return select_end_x < obj->x;
    }
    return true;
}

//判断一个对象是不是处于选择区间里
bool SelectionManager::isObjInSelection(const Obj* obj) noexcept {
    //考虑分栏
    auto pos = SoupleManager::get_page_with_column(obj);
    pos.print();
    if(pos == start_pcpos) {
        if(obj->y+obj->height < select_begin_y) return false;
        if(pos == end_pcpos) {
            if(obj->y>select_end_y) return false;
            if(obj->y < select_end_y && obj->y+obj->height > select_end_y) { //位于末尾行
                return select_end_x > obj->x;
            } else if(obj->y < select_begin_y && obj->y+obj->height > select_begin_y) { //位于起始行
                return select_begin_x < obj->x;
            }
        } else {
            if(obj->y < select_begin_y && obj->y+obj->height > select_begin_y) { //位于起始行
                return select_begin_x < obj->x;
            }
        }
        return true;
    } else if(pos == end_pcpos) {
        if(obj->y > select_end_y) return false;
        if(pos == start_pcpos) {
            if(obj->y+obj->height < select_begin_y) return false;
            if(obj->y < select_end_y && obj->y+obj->height > select_end_y) { //位于末尾行
                return select_end_x > obj->x;
            } else if(obj->y < select_begin_y && obj->y+obj->height > select_begin_y) { //位于起始行
                return select_begin_x < obj->x;
            }
        } else {
            if(obj->y < select_end_y && obj->y+obj->height > select_end_y) { //位于末尾行
                return select_end_x > obj->x;
            }
        }
        return true;
    }
    return start_pcpos < pos && pos < end_pcpos;
}

std::pair<HorLine_Base*,HorLine_Base*>
SelectionManager::getSelectedHLineRange() {
    //找到选中的hline中PCPos最小、y最小的
    HorLine_Base* h1 = 0, *h2 = 0;
    for(auto& si : selection_items) {
        auto obj = SoupleManager::getObjById(si.obj_id);
        if(auto a = obj->as<AnchorObj*>(); a) {
            HorLine_Base* h = a->hline->be<HorLine_Base*>();
            auto pp1 = h->getPCPos();
            if(!h1 || pp1 < h1->getPCPos()
                || pp1 == h1->getPCPos()&&h->y < h1->y) {
                h1 = h;
            }
            if(!h2 || pp1 > h2->getPCPos()
                || pp1 == h2->getPCPos()&&h->y > h2->y) {
                h2 = h;
            }
        }
    }
    if(!h1 || !h2) return {0,0};
    // 验证h1和h2是否连续
    auto h = h1;
    while(h) {
        if(h == h2) return {h1,h2}; //接续
        h = h->getNextLine();
    }
    return {0,0}; //不接续
}

void SelectionManager::dealKeyEvent(QKeyEvent* keyEvent)
{
    //keyEvent->
}
