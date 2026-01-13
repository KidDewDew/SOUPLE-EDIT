#include "magicalcursor.h"
#include "souplemanager.h"

void MagicalCursor::init()
{
    cursor_quickitem = Helper::invokeQmlFunction<QQuickItem*>("getSoupleEditObject","uniformCursor");
    if(cursor_quickitem) {
        qDebug() << "MagicalCursor::init() OK";
    } else {
        qDebug() << "MagicalCursor::init() FAILED";
    }
}
