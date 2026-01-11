#include "uniformkb.h"
#include "selectionmanager.h"
#include "magicalcursor.h"

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

        }
    }
    return false;
}


