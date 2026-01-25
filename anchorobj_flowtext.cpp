#include "anchorobj_flowtext.h"
#include "helper.h"
#include "souplemanager.h"
#include "anchorobj_phright.h"
#include <QFontMetrics>
#include <fpdf_edit.h>
#include <QFontDatabase>
#include <QFontInfo>
#include <QRawFont>
#include <QPainterPath>
#include "souple_pdfsaver.h"
#include "turnbackmanager.h"
#include "turnback_template.hpp"
#include "magicalcursor.h"

AnchorObj_FlowText::AnchorObj_FlowText()
{
    text = "#文本";
    font.setPointSize(18);
    vAlignMode = Helper::AlignVBaseLine; //默认基线对齐
    z = Helper::Layer_Z::Text; //默认为文本层深度
}

QQuickItem* AnchorObj_FlowText::generateQmlItem()
{
    //qDebug() << "FlowText(" << text << ") generateQmlItem()";
    return uiPool_flowText::fetchItem();
    //return Helper::invokeQmlFunction<QQuickItem*>("generateObj","FlowText");
}

bool AnchorObj_FlowText::tryMergeRight()
{
    if(! rightObj) return false;
    auto r2 = rightObj->as<AnchorObj_FlowText*>();
    if(!r2 || r2->font != font || r2->vAlignOffset != vAlignOffset
        || r2->vAlignMode != vAlignMode || r2->isStroke != isStroke || r2->isFill != isFill
        || abs(r2->strokeWidth - strokeWidth) > 1e-3) return false;
    if(isFill && fill_color != r2->fill_color) return false;
    if(isStroke && stroke_color != r2->stroke_color) return false;
    //qDebug() << "Merge: " << text << r2->text;

    int old_text_length = text.length();
    QString new_text = text + r2->text;
    width += r2->width; //这个时候r2并没真正死亡

    //merge_flowAttachers(r2,text.length()); //合并attacher [2025/9/16]
    text = new_text;


    //qmlItem合并 [2025/7/16添加，目的：减少qmlItem峰值数量]
    if(Helper::isQmlItemValid(r2->qmlItem) && ! Helper::isQmlItemValid(qmlItem)) { //此时可以直接替代qmlItem
        qmlItem = r2->qmlItem;
        r2->qmlItem = 0;
        int old_cursor = qmlItem->property("cursorPosition").toInt();
        qmlItem->setProperty("data_id",id); //[2025/7/18 添加]
        qmlItem->setProperty("text",text);
        if(qmlItem->hasFocus()) {
            qmlItem->setProperty("cursorPosition",old_text_length+old_cursor);
        }
        SoupleManager::notifyVisible(this); //告诉SM本对象自己让自己从不可见变为可见了
    }
    else if( Helper::isQmlItemValid(qmlItem) ) {
        int old_cursor = qmlItem->property("cursorPosition").toInt();
        qmlItem->setProperty("text",text);
        if(qmlItem->hasFocus()) {
            qmlItem->setProperty("cursorPosition",old_cursor); //恢复光标
            //qDebug() << text;
            //qDebug() << "old_cursor:" << old_cursor;
        }
    }

    r2->removeSelf(true);

    //维护选择内容
    if(SelectionManager::isSelectStopButKeep()) {
        auto si = SelectionManager::getSelectionItem(id);
        auto si2 = SelectionManager::getSelectionItem(r2->id);
        if(si2) {
            SelectionManager::removeSelectionItem(r2->id);
            int addv = text.length()-r2->text.length();
            if(!si) {
                SelectionManager::putSelectionItem(id,
                        SelectionManager::SelectionItem(id,
                            si2->begin_index+addv,
                            si2->end_index+addv));
            } else {
                SelectionManager::putSelectionItem(id,
                                    SelectionManager::SelectionItem(id,
                                        si->begin_index,
                                        si2->end_index+addv));
            }
        }
    }

    return true;
}

AnchorObj* AnchorObj_FlowText::dropRight(float dropWidth) //尝试截断并丢弃右边
{
    //() << "FlowText dropRight(" << dropWidth;
    //qDebug() << "selfWidth=" << width;
    if(text.length() == 0 || dropWidth >= width) { //完全截断
        removeSelf(false); //移除自身(不删除)
        //qDebug() << "removeSelf(false)";
        return this;
    }

    if(dropWidth < 1.0) return 0;
    QFontMetrics metrics{font};
    int dropN = 0, pile_width = 0; //丢弃多少字符
    int ori_length = text.length();
    if(dropWidth*2 < width) { //若截断宽度较小
        while(dropN < text.length() && pile_width < dropWidth) {
            pile_width += metrics.horizontalAdvance(text[text.length() - dropN - 1]);
            //qDebug() << "pile_width = " << pile_width;
            ++dropN;
        }
    } else { //若截断宽度较大，咱们应该从左边向右遍历
        int resN = 0; //保留多少个字符
        float toReserveWidth = width - dropWidth;
        while(resN < text.length()) {
            toReserveWidth -= metrics.horizontalAdvance(text[resN]);
            if(toReserveWidth < 1e-2) break;
            ++resN;
        }
        dropN = text.length() - resN;
        //if(resN < text.length())
        //    pile_width -= metrics.horizontalAdvance(text[resN]);
        pile_width = dropWidth - toReserveWidth;
    }

    if(dropN <= 0) return nullptr; //无法截断s
    if(dropN == text.length()) { //完全截断
        //qDebug() << "FlowText(" << text << ") 完全截断";
        removeSelf(false); //移除自身(不删除)
        return this;
    }
    QString new_text = text.sliced(text.length() - dropN);
    //text = text.chopped(dropN);
    text.chop(dropN);
    //if( Helper::isQmlItemValid(qmlItem) )
    //    QMetaObject::invokeMethod(qmlItem,"cpp_updateText",Q_ARG(QVariant,text));

    width -= pile_width; //注意更新width
    auto obj = new AnchorObj_FlowText;
    obj->text = std::move(new_text);
    obj->font = font;
    obj->vAlignMode = vAlignMode;
    obj->vAlignOffset = vAlignOffset;
    obj->isStroke = isStroke;
    obj->isFill = isFill;
    obj->strokeWidth = strokeWidth;
    obj->fill_color = fill_color;
    obj->stroke_color = stroke_color;
    obj->width = pile_width; //[2025/12/2 added]
    SoupleManager::registerObj(obj);

    if( Helper::isQmlItemValid(qmlItem) ) {
        if(qmlItem->hasFocus()) {
            int old_cursor = qmlItem->property("cursorPosition").toInt();
            //qDebug() << "old_cursor: " << old_cursor;
            qmlItem->setProperty("text",text);
            /**下面这段代码即将作废，MagicCursor正在实现*/
            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            if(old_cursor <= text.length())
                qmlItem->setProperty("cursorPosition",old_cursor);
            else { //光标需要转移到overflow_textEdit
                obj->qmlItem = obj->generateQmlItem();
                obj->updateDataToQmlItem(obj->qmlItem);
                SoupleManager::notifyVisible(obj);
                obj->qmlItem->forceActiveFocus();
                obj->qmlItem->setProperty("cursorPosition",old_cursor - text.length());
            }
        } else {
            qmlItem->setProperty("text",text);
        }
    }

    //维持选择内容
    if(SelectionManager::isSelectStopButKeep()) {
        auto si = SelectionManager::getSelectionItem(id);
        if(si) {
            if(si->begin_index < text.length() && si->end_index >= text.length()) {
                qDebug() << "dropRight Selecion 1!";
                SelectionManager::putSelectionItem(id,SelectionManager::SelectionItem(id,si->begin_index,text.length()-1));
                SelectionManager::putSelectionItem(obj->id,
                            SelectionManager::SelectionItem(obj->id,0,si->end_index-text.length()));
            } else if(si->begin_index >= text.length()) {
                qDebug() << "dropRight Selecion 2!";
                SelectionManager::removeSelectionItem(id);
                SelectionManager::putSelectionItem(obj->id,
                        SelectionManager::SelectionItem(obj->id,
                                                    si->begin_index-text.length(),
                                                    si->end_index-text.length()));
            }
        }
    }

    //     //通知ui端，文本溢出
    //     if(! obj->qmlItem) {
    //         obj->qmlItem = obj->generateQmlItem();
    //         obj->updateDataToQmlItem(obj->qmlItem);
    //     }
    //     QMetaObject::invokeMethod(qmlItem,"cpp_overflow",Q_ARG(QVariant,text),
    //                QVariant::fromValue(obj->qmlItem));
    // }

    return obj;
}

AnchorObj* AnchorObj_FlowText::dropLeft(float dropWidth)
{
    QFontMetrics metrics{font};
    int drop_N = 0, pile_width = 0;
    while(drop_N < text.length()) {
        int _a = metrics.horizontalAdvance(text[drop_N]);
        pile_width += _a;
        if(pile_width > dropWidth) {
            pile_width -= _a;
            break;
        }
        ++ drop_N;
    }
    if(drop_N <= 0) return nullptr; //未截断
    if(drop_N == text.length()) {
        removeSelf(false);
        return this;
    }

    QString new_text = text.first(drop_N);
    text = text.sliced(drop_N);
    width -= pile_width;

    //qDebug() << "dropLeft: " << drop_N << ",new_text:" << new_text;
    if( Helper::isQmlItemValid(qmlItem) )
       QMetaObject::invokeMethod(qmlItem,"cpp_updateText",Q_ARG(QVariant,text));

    auto obj = new AnchorObj_FlowText;
    obj->text = std::move(new_text);
    obj->font = font;
    obj->vAlignMode = vAlignMode;
    obj->vAlignOffset = vAlignOffset;
    obj->fill_color = fill_color;
    obj->stroke_color = stroke_color;
    obj->strokeWidth = strokeWidth;
    obj->isStroke = isStroke;
    obj->isFill = isFill;
    obj->width = pile_width;
    SoupleManager::registerObj(obj);

    //moveFlowAttachers(-obj->text.length()); //移动attacher，但不用转移。

    //维持选择内容
    if(SelectionManager::isSelectStopButKeep()) {
        auto si = SelectionManager::getSelectionItem(id);
        int L = obj->text.length();
        if(si) {
            if(si->begin_index >= L) {
                SelectionManager::putSelectionItem(id,
                        SelectionManager::SelectionItem(id,si->begin_index-L,si->end_index-L)
                        );
            } else {
                if(si->end_index - L >= 0) {
                    SelectionManager::putSelectionItem(id,
                            SelectionManager::SelectionItem(id,0,si->end_index-L));
                }
                else SelectionManager::removeSelectionItem(id);

                SelectionManager::putSelectionItem(obj->id,
                            SelectionManager::SelectionItem(obj->id,si->begin_index,
                                                        qMin(L-1,si->end_index)));
            }
        }
    }

    return obj;
}

void AnchorObj_FlowText::updateDataToQmlItem(QQuickItem* item)
{
    AnchorObj::updateDataToQmlItem(item);
    //item->setProperty("data_id",id);
    //item->setPosition({x,y});
    //item->setX(x); item->setY(y);
    //item->setSize({width,height});
    item->setProperty("text",text);
    item->setProperty("color",fill_color); //填充色
    item->setProperty("sColor",stroke_color); //描边色
    item->setProperty("font",font);
    item->setProperty("bFill",isFill);
    item->setProperty("bStroke",isStroke);
    item->setProperty("sWidth",strokeWidth);
}

void AnchorObj_FlowText::getCursorFromRight(int RN)
{
    if(!Helper::isQmlItemValid(qmlItem) || RN > 32) return;
    qmlItem->forceActiveFocus(); //获取焦点
    qmlItem->setProperty("cursorPosition",text.length()); //设置光标
}

void AnchorObj_FlowText::getCursorFromLeft(int RN)
{
    if(!Helper::isQmlItemValid(qmlItem) || RN > 32) return;
    qmlItem->forceActiveFocus(); //获取焦点
    qmlItem->setProperty("cursorPosition",0); //设置光标
}

float AnchorObj_FlowText::showHScale(float hscale,float addX,bool justQueryAddWidth)
{
    if(justQueryAddWidth) {
        return width * (hscale - 1.0);
    } else {
        if( ! Helper::isQmlItemValid(qmlItem) || text.length() == 0)
            return 0.0;
        qmlItem->setX(x+addX);
        float addW = width * (hscale - 1.0);
        QMetaObject::invokeMethod(qmlItem,"setLetterSpacing",QVariant(addW / text.length()));
        qmlItem->setWidth(width * hscale);
        return addW;
    }
}

void AnchorObj_FlowText::userUpdateText(const QString& new_text)
{
    if(!hline || text == new_text) return;

    //1112asdad1111111
    //eeeee

    //int
    //for(change_pos = 0; change_pos)

    // // 序列化更新前的自己
    // QByteArray old_bytes = souple::serialization::serialize(this);

    QString old_text = text;
    text = new_text;
    calcWidth();

    // 处理redo、undo

    Turnback* tb = TurnbackManager::addTurnback(Turnback::AC_Content_Flow);
    //[不需要设置了]tb->attach_obj_id = id;
    tb->flow_position = 0;

    this->addFlowAttacher(tb); //添加附着符

    int old_length = old_text.length();
    int new_length = text.length();

    // undo原理：从attacher开始的一段文本(len=new_length)删除掉，然后反序列化生成old_text.
    tb->undo = [new_length,old_text,font=font](Turnback*tb,Obj*start_obj)->void {
        //咱们给第一个text修改为old_text
        int i = 0,rest_length = new_length;
        auto hline = start_obj->be<AnchorObj*>()->hline->be<HorLine_Base*>();
        for(auto[obj,start_i,len] : hline->getWalker((AnchorObj*)start_obj,tb->flow_position,new_length)) {
            AnchorObj_FlowText* t = obj->as<AnchorObj_FlowText*>();
            if(! t) {
                if(i>0 || ! obj) return;
                t = new AnchorObj_FlowText;
                t->font = font;
                t->text = "";
                SoupleManager::registerObj(t);
                obj->insertOnLeft(t);
                return;
            }
            qDebug() << "undo-textup-walk:" << t->text;
            if(start_i == 0)
                t->text.slice(qMin(len,t->text.length()));
            else {
                auto leftstr = t->text.left(start_i);
                t->text = leftstr + t->text.slice(qMin(start_i+len,t->text.length()));
            }
            if(i == 0) {
                //第一个文本
                t->text.push_front(old_text);
            }
            if(t->text.length() == 0) {
                t->removeSelf(true); //to die
            }
            else if(Helper::isQmlItemValid(t->qmlItem))
                t->qmlItem->setProperty("text",t->text);
            ++i;
        }
    };
    tb->redo = [old_length,font=font,new_text=text](Turnback*tb,Obj*start_obj)->void {
        //咱们给第一个text修改为new_text
        qDebug() << "tredo: " << start_obj->__dstr() << tb->flow_position;
        int i = 0,rest_length = old_length;
        auto hline = start_obj->be<AnchorObj*>()->hline->be<HorLine_Base*>();
        for(auto[obj,start_i,len] : hline->getWalker((AnchorObj*)start_obj,tb->flow_position,old_length)) {
            AnchorObj_FlowText* t = obj->as<AnchorObj_FlowText*>();
            qDebug() << "walk-at: " << t->__dstr() << start_i << len;
            if(! t) {
                return;
            }
            int end_i = qMin(start_i + len,t->text.length());
            if(i == 0) {
                t->text = t->text.sliced(0,start_i) +
                          new_text +
                          t->text.sliced(end_i,t->text.length()-end_i);
            } else {
                t->text = t->text.sliced(0,start_i) +
                          t->text.sliced(end_i,t->text.length()-end_i);
            }
            if(Helper::isQmlItemValid(t->qmlItem))
                t->qmlItem->setProperty("text",t->text);
            ++i;
        }
    };
}

int AnchorObj_FlowText::dealCommandFromQmlItem(int command,const QVariant& arg)
{
    //qDebug() << "FlowText 接收: " << command << arg;
    switch(command) {
    case Helper::TEXT_UP: {
        userUpdateText(arg.toString());
        break;
    }
    case Helper::CURSOR_CHANGE: { //光标移动
        MagicalCursor::set_cursor(this,arg.toInt()); //告诉魔法光标
        break;
    }
    case Helper::FONT_SIZE_UP: {
        float old_ps = font.pointSizeF();
        if(old_ps == arg.toFloat()) break;
        font.setPointSizeF(arg.toFloat());
        if(!hline) break;
        Turnback* tb = TurnbackManager::addTurnback(Turnback::AC_Content_Flow);
        tb->attach_obj_id = id;
        tb->flow_position = 0;
        this->addFlowAttacher(tb); //添加附着符
        int content_length = contentLength();
        tb->undo = TurnbackManager::new_template_walker<AnchorObj_FlowText>(
            [=](AnchorObj_FlowText* obj_text,int start_i,int len){
                obj_text
                    ->slice(start_i,len)
                    ->be<AnchorObj_FlowText*>()
                    ->font.setPointSizeF(old_ps);
                qDebug() << "undo-walk:" <<obj_text->text << start_i << len;
                if(QML_VALID(obj_text)) {
                    obj_text->qmlItem->setProperty("font",obj_text->font);
                }
            },
            content_length);
        tb->redo = TurnbackManager::new_template_walker<AnchorObj_FlowText>(
            [=,new_ps = font.pointSizeF()](AnchorObj_FlowText* obj_text,int start_i,int len){
                obj_text
                    ->slice(start_i,len)
                    ->be<AnchorObj_FlowText*>()
                    ->font.setPointSizeF(new_ps);
                qDebug() << "redo-walk:" <<obj_text->text << start_i << len;
                if(QML_VALID(obj_text)) {
                    obj_text->qmlItem->setProperty("font",obj_text->font);
                }
            },
            content_length);
        break;
    }
    case Helper::FONT_FAMILY_UP: {
        auto family_list = font.families();
        qDebug() << "Helper::FONT_FAMILY_UP:" << family_list;
        if(family_list.size() == 0) font.setFamily(arg.toString());
        else {
            family_list[0] = arg.toString();
            font.setFamilies(family_list);
        }
        if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setProperty("font",font);
        break;
    }
    case Helper::FONT_BOLD_UP:
        font.setBold(arg.toBool());
        break;
    case Helper::FONT_ITALIC_UP:
        font.setItalic(arg.toBool());
        break;
    case Helper::GOTOLEFT: {  //向左转移光标
        auto lastObj = getLastObj();
        if(lastObj) lastObj->getCursorFromRight();
        break;
    }
    case Helper::KEY_RETURN: { //按下回车键
        if(!Helper::isQmlItemValid(qmlItem)) return 0;
        int cursorPosition = qmlItem->property("cursorPosition").toInt(); //获取当前光标位置
        if(cursorPosition == 0) {
            AnchorObj_PHRight *phright = new AnchorObj_PHRight;
            SoupleManager::registerObj(phright);
            phright->hline = hline;
            insertOnLeft(phright);
            break;
        }
        QString chop_text = text.sliced(cursorPosition);
        AnchorObj_FlowText *new_obj = 0;
        if(chop_text.length() > 0) {
            float my_old_rightx = getRightX();
            text.chop(text.length() - cursorPosition);
            if( Helper::isQmlItemValid(qmlItem) )
                QMetaObject::invokeMethod(qmlItem,"cpp_updateText",Q_ARG(QVariant,text));

            new_obj = new AnchorObj_FlowText;
            SoupleManager::registerObj(new_obj);
            new_obj->text = std::move(chop_text);
            new_obj->font = font;
            new_obj->vAlignMode = vAlignMode;
            new_obj->vAlignOffset = vAlignOffset;
            new_obj->fill_color = fill_color;
            new_obj->stroke_color = stroke_color;
            new_obj->strokeWidth = strokeWidth;
            new_obj->isStroke = isStroke;
            new_obj->isFill = isFill;

            //[2025/12/2 added 维护坐标和尺寸]
            new_obj->x = my_old_rightx - new_obj->width;
            this->width -= new_obj->width;

            //立即创建ui对象，以便转移光标
            new_obj->qmlItem = new_obj->generateQmlItem();
            SoupleManager::notifyVisible(new_obj);//注意，通知SoupleManager可见性
            new_obj->updateDataToQmlItem(new_obj->qmlItem);
            new_obj->getCursorFromLeft(); //转移光标
            new_obj->hline = hline;
            new_obj->dealLayout();
        }

        AnchorObj_PHRight *phright = new AnchorObj_PHRight;
        SoupleManager::registerObj(phright);

        phright->hline = hline;

        insertOnRight(phright); //插入换行
        if(new_obj) phright->insertOnRight(new_obj);
        break;
    }
    case Helper::COLOR_UP:
        fill_color = arg.value<QColor>();
        break;
    case Helper::STROKE_COLOR_UP:
        stroke_color = arg.value<QColor>();
        break;
    case Helper::STROKE_WIDTH_UP:
        strokeWidth = arg.toFloat();
        break;
    case Helper::BOOL_STROKE_UP:
        isStroke = arg.toBool();
        break;
    case Helper::VALIGNOFFSET_UP:
        vAlignOffset = arg.toDouble();
        break;
    case Helper::INSERTOBJ: { //插入obj
        if(!Helper::isQmlItemValid(qmlItem)) SoupleManager::createSoupleObj_byObj(arg.toString(),id,false);
        else {
            int pos = qmlItem->property("cursorPosition").toInt();
            if(pos == 0) SoupleManager::createSoupleObj_byObj(arg.toString(),id,true);
            else if(pos == text.length()) SoupleManager::createSoupleObj_byObj(arg.toString(),id,false);
            else { //中间插入
                //从光标pos处截断
                QString chop_text = text.sliced(pos);
                text = text.first(pos);
                if( Helper::isQmlItemValid(qmlItem) )
                    QMetaObject::invokeMethod(qmlItem,"cpp_updateText",Q_ARG(QVariant,text));
                AnchorObj_FlowText* new_obj = clone();
                new_obj->text = chop_text;
                SoupleManager::registerObj(new_obj);
                auto insert_obj = SoupleManager::createObj(arg.toString())->as<AnchorObj*>(); //已经注册好了
                if(0 == insert_obj) { return Helper::Error_Failed; }
                new_obj->hline = insert_obj->hline = hline;
                insertOnRight(insert_obj);
                insert_obj->insertOnRight(new_obj);
            };
        }
    }
    }
    return 0;
}

AnchorObj_FlowText* AnchorObj_FlowText::clone()
{
    AnchorObj_FlowText *new_obj = new AnchorObj_FlowText;
    new_obj->font = font;
    new_obj->vAlignMode = vAlignMode;
    new_obj->vAlignOffset = vAlignOffset;
    new_obj->fill_color = fill_color;
    new_obj->stroke_color = stroke_color;
    new_obj->strokeWidth = strokeWidth;
    new_obj->isStroke = isStroke;
    new_obj->isFill = isFill;
    return new_obj;
}

void AnchorObj_FlowText::selectionCommand(int command,const QVariant& arg)
{
    auto si = SelectionManager::getSelectionItem(this->id);
    if(!si) return;

    auto chop = [&]{
        if(si->begin_index > 0) {
            AnchorObj_FlowText* new_left = clone();
            new_left->text = text.left(si->begin_index);
            SoupleManager::registerObj(new_left);
            this->insertOnLeft(new_left);
            qDebug() << "chop.insertOnLeft(" << new_left->text;
            //new_left->merge_flowAttachers(this,0); //左截断，合并attachers
        }
        if(si->end_index < text.length() - 1) {
            AnchorObj_FlowText* new_right = clone();
            new_right->text = text.sliced(si->end_index+1);
            SoupleManager::registerObj(new_right);
            this->insertOnRight(new_right);
            qDebug() << "chop.insertOnRight(" << new_right->text;
        }
        if(si->begin_index > 0 || si->end_index < text.length() - 1) {
            text = text.sliced(si->begin_index,si->end_index - si->begin_index + 1);
            if(Helper::isQmlItemValid(qmlItem)) qmlItem->setProperty("text",text);
            SelectionManager::removeSelectionItem(id);
            SelectionManager::putSelectionItem(id,
                                SelectionManager::SelectionItem(id,0,text.length()-1));
        }
    };
    switch(command) {
    case Helper::SP_Family_Set: {
        QString s = arg.toString();
        if(font.family() != s) {
            chop();
            auto family_list = font.families();
            if(family_list.size() == 0) font.setFamily(s);
            else {
                family_list[0] = s;
                font.setFamilies(family_list);
            }
            if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setProperty("font",font);
        }
        break;
    }
    case Helper::SP_FontSize_Add:
        chop();
        dealCommandFromQmlItem(Helper::FONT_SIZE_UP,font.pointSizeF() + arg.toFloat());
        if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setProperty("font",font);
        break;
    case Helper::SP_FontSize_Set:
        if(font.pointSizeF() == arg.toFloat()) break;
        chop();
        dealCommandFromQmlItem(Helper::FONT_SIZE_UP,arg.toFloat());
        if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setProperty("font",font);
        break;
    case Helper::SP_FontBold_Set:
        if(font.bold() != arg.toBool()) {
            chop();
            font.setBold(font.bold()^1);
            if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setProperty("font",font);
        }
        break;
    case Helper::SP_FontItalic_Set:
        if(font.italic() != arg.toBool()) {
            chop();
            font.setBold(font.italic()^1);
            if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setProperty("font",font);
        }
        break;
    }
}

void AnchorObj_FlowText::dealLayout()
{

    //if(hline && hline->qmlItem) {
    //   qDebug() << text << "y: " << y << " hline.y: " << hline->y;
   // }

    // if(x < 10) {
    //     qDebug() << "leftObj is " << leftObj;
    //     if(leftObj) {
    //         qDebug() << leftObj->__dstr();
    //     }
    //     if(hline) qDebug() << "hline.x = " << hline->x;
    //     qDebug() << __dstr() << "x = " << x;
    // }

    if(font.letterSpacing() < 0) qDebug() << "fw:" << font.letterSpacing();
    QFontMetricsF metrics{font};
    width = metrics.horizontalAdvance(text);
    height = metrics.height();
    if(hline && vAlignMode == Helper::AlignVBaseLine) { //基线对齐
        y = hline->y - metrics.ascent() + vAlignOffset;
        //qDebug() << text << "...ascent2: " << metrics.ascent();
    }
    // if(qmlItem && qmlItem->property("length") != text.length()) {
    //     qmlItem->setProperty("text",text);
    // }
    AnchorObj::dealLayout();
    //if(qmlItem && qmlItem->hasFocus()) qDebug() << text << ".x = " << x << ".w = " << width;
}


void AnchorObj_FlowText::qt_paint(QPainter& painter,Page* page) {
    if(text.length() == 0){
        return;
    }
    QFontMetricsF fm(font);
    auto hline = this->hline->be<HorLine_Base*>();
    float draw_x = x,hscale = 1.0f;
    if(hline) {
        // 使用应用了hscale的x坐标
        draw_x = hline->getObjX_atHScale(this);
        hscale = hline->getHScale();
        qDebug() << "qt_paint(" << __dstr() << ": hscale=" << hscale;
    }
    font.setLetterSpacing(QFont::AbsoluteSpacing,
                          (hscale-1.0f)*width/text.length());
    if(isStroke) { //需要描边
        QPainterPath path;
        path.addText(QPointF{draw_x,y - page->top_y+fm.ascent()},font,text);
        QPen pen(stroke_color,strokeWidth);
        painter.setPen(stroke_color);
        painter.setBrush(fill_color);
        painter.drawPath(path);
    } else { //不需要描边
        // if(isStroke) { //证明描边和填充颜色一样，按粗体处理
        //     QFont f2(font);
        //     f2.setBold(true);
        //     painter.setFont(f2);
        // }
        painter.setFont(font);
        painter.setPen(stroke_color);
        painter.drawText(QPointF{draw_x,y - page->top_y+fm.ascent()},text);
    }
    font.setLetterSpacing(QFont::AbsoluteSpacing,0);
}

void AnchorObj_FlowText::writeToPDFPage(FPDF_DOCUMENT document,FPDF_PAGE pdf_page, const Page* page)
{
    FPDF_FONT ffont = Souple_PdfSaver::used_font_families[font.family()];
    if(!ffont) {
        QRawFont rf = QRawFont::fromFont(font);
        QByteArray data = rf.fontTable("OTTO");
        qDebug() << "data.length = " << data.size();
        ffont = FPDFText_LoadFont(document,(uint8_t*)data.data(),data.size(),FPDF_FONT_TYPE1,true);
        Souple_PdfSaver::used_font_families[font.family()] = ffont;
        qDebug() << "loadFont: " << ffont;
    }
    //Souple_PdfSaver::used_font_families.insert(font.family()); //set
    auto pdf_textobj = FPDFPageObj_CreateTextObj(document,ffont,font.pointSizeF());
    //QString ss = "abcde";
    FPDFText_SetText(pdf_textobj,text.utf16());
    qDebug() << "AnchorObj_FlowText::writeToPDFPage(";
    endowMatrixToPDFObj(pdf_textobj,page); //应用矩阵
    FPDFPage_InsertObject(pdf_page,pdf_textobj);
}

// 重载slice截取函数
AnchorObj* AnchorObj_FlowText::slice(int start_i,int max_len)
{
    if(start_i >= text.length() || start_i + max_len <= 0) {
        // 对于错误的参数，本函数仍然返回自身 todo
        return this;
    }
    if(start_i <= 0) {
        if(max_len >= text.length()) return this;
        AnchorObj_FlowText* right_part = clone();
        SoupleManager::registerObj(right_part);
        right_part->text = text.sliced(max_len);
        text.slice(0,max_len);
        if(Helper::isQmlItemValid(qmlItem)) qmlItem->setProperty("text",text);
        this->insertOnRight(right_part);
        qDebug() << "right-slice: " << text << right_part->text;
    } else {
        if(max_len + start_i >= text.length()) {
            AnchorObj_FlowText* left_part = clone();
            SoupleManager::registerObj(left_part);
            left_part->text = text.sliced(0,start_i);
            text.slice(start_i);
            if(Helper::isQmlItemValid(qmlItem)) qmlItem->setProperty("text",text);
            this->insertOnLeft(left_part);
            qDebug() << "left-slice: " << left_part->text << text;
            //moveFlowAttachers(-left_part->text.length()); //移动attacher
        } else { //切成3段
            AnchorObj_FlowText *left_part = clone(), *right_part = clone();
            SoupleManager::registerObj(left_part);
            SoupleManager::registerObj(right_part);
            left_part->text = text.sliced(0,start_i);
            right_part->text = text.sliced(start_i+max_len);
            text.slice(start_i,max_len);
            if(Helper::isQmlItemValid(qmlItem)) qmlItem->setProperty("text",text);
            this->insertOnLeft(left_part);
            this->insertOnRight(right_part);
            qDebug() << "lr-slice: " << left_part->text << text << right_part->text;
            //moveFlowAttachers(-left_part->text.length()); //移动attacher
        }
    }
    return this;
}
