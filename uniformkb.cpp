#include "uniformkb.h"
#include "selectionmanager.h"
#include "magicalcursor.h"
#include "anchorobj_flowtext.h"
#include "souplemanager.h"

extern QGuiApplication *global_app;
extern QObject* qmlRoot;

bool UniformKB::dealKeyPressed(const QKeyEvent* ke)
{
    //qDebug() << "ktext:"<<ke->text();
    if(SelectionManager::isSelectStopButKeep()) { // 有选中的内容
        QObject* focusObject = global_app->focusObject(); //获取焦点对象
        if(!focusObject || focusObject->parent() == qmlRoot) {
            qDebug() << ke;
            Qt::KeyboardModifiers km = ke->modifiers();
            if(km.testAnyFlag(Qt::ControlModifier)) {
                //按下Ctrl
                switch(ke->key()) {
                case Qt::Key_C:
                    //复制选中内容
                    break;
                case Qt::Key_V:
                    //粘贴内容
                    break;
                case Qt::Key_X:
                    //剪贴选中内容
                    break;
                default: return false; //其余情况不处理。
                }
                return true; //凡是被switch处理的情况，一律过滤
            }
            else if(ke->key() == Qt::Key_Backspace) {
                //退格
                qDebug() << "Selection - 退格";
                SelectionManager::deleteAllSelectedObjs();
                return true;
            }
            else {
                //可能是输入文本
                QString text = ke->text();

                if(! text.isEmpty()) {
                    SelectionManager::deleteAllSelectedObjs();
                }
            }
        }
    }
    else [[likely]]
    {
        Obj *who = MagicalCursor::at_who();
        if(who && QML_VALID(who) && who->qmlItem->hasFocus())
        { //有焦点
            auto& keyInfo = who->keyInfo();
            if(keyInfo.selfDeal_input)
                return false; //自处理输入按键，则不进行统一处理
            Qt::KeyboardModifiers km = ke->modifiers();
            if(km.testAnyFlag(Qt::ControlModifier)) {
                //按下Ctrl
                switch(ke->key()) {
                case Qt::Key_V:
                    //粘贴内容
                    break;
                default: return false; //其余情况不处理。
                }
                return true; //凡是被switch处理的情况，一律过滤
            }
            else if(ke->key() == Qt::Key_Backspace) {
                //退格
                qDebug() << "Cursor - 退格";
                who->removeSelf(true);
                return true;
            }
            else {
                //可能是输入文本
                QString text = ke->text();
                if(! text.isEmpty()) {
                    AnchorObj* anchor_who = who->as<AnchorObj*>();
                    if(anchor_who) {

                        AnchorObj_FlowText* right_text_obj =
                                anchor_who->get_neighbor_after()
                                          ->as<AnchorObj_FlowText*>();

                        if(right_text_obj) { //如果右边就有文本对象
                            right_text_obj->text = text + right_text_obj->text;
                            if( ! QML_VALID(right_text_obj)) {
                                right_text_obj->qmlItem = right_text_obj->generateQmlItem();
                                SoupleManager::notifyVisible(right_text_obj);
                            }
                            if(QML_VALID(right_text_obj)) { //谨防qmlItem创建失败
                                right_text_obj->qmlItem->setFocus(true);
                                right_text_obj->qmlItem->setProperty("text",right_text_obj->text);
                                right_text_obj->qmlItem->setProperty("cursorPosition",text.length());
                            }
                            return true;
                        }

                        //如果who是anchorobj，那么就可以处理了
                        //向anchor_who后面插入文本
                        AnchorObj_FlowText* text_obj = new AnchorObj_FlowText;
                        SoupleManager::registerObj(text_obj);
                        anchor_who->insertOnRight(text_obj);
                        text_obj->text = text;

                        text_obj->font.setPointSize(11);

                        //向前遍历，找到第一个也是AnchorObj_FlowText的对象
                        AnchorObj* a = anchor_who;
                        unsigned int ct = 32; //最多向前找32次

                        while(a && (ct--) > 0) {
                            AnchorObj_FlowText* ta = a->as<AnchorObj_FlowText*>();
                            if(ta) {
                                text_obj->font = ta->font;
                                text_obj->vAlignMode = ta->vAlignMode;
                                text_obj->vAlignOffset = ta->vAlignOffset;
                                break;
                            }
                            a = a->get_neighbor_before();
                        }

                        // 转移光标和focus
                        text_obj->qmlItem = text_obj->generateQmlItem();
                        // 通知SM：该对象已经可见了；如果不通知，该对象将无法正常调控。
                        SoupleManager::notifyVisible(text_obj);

                        if(QML_VALID(text_obj)) { //谨防qmlItem创建失败
                            text_obj->qmlItem->setFocus(true);
                            text_obj->qmlItem->setProperty("cursorPosition",text.length());
                        }

                        return true;
                    }
                }
            }
        }
    }
    return false;
}


