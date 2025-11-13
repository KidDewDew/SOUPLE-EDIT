#include "free_tableunit.h"
#include "tableline.h"

Free_TableUnit::Free_TableUnit() {}

int Free_TableUnit::dealCommandFromQmlItem(int command,const QVariant& arg)
{
    switch(command) {
    case Helper::CONT_CHANGED:
        emitSignal(Helper::CONT_CHANGED,arg.toBool());
        Free_Rich::dealCommandFromQmlItem(command,arg);
        break;
    case Helper::TABLE_ALIGNMODE_UP:
        qDebug() << "tableinfo->am: " << arg.toInt();
        tableinfo->alignMode = arg.toInt();
        break;
    default:
        return Free_Rich::dealCommandFromQmlItem(command,arg);
    }
    return 0;
}

QVariant Free_TableUnit::qmlGetData(int dataName) {
    if(dataName == Helper::TABLE_ALIGNMODE) {
        return tableinfo->alignMode;
    }
    return Free_Rich::qmlGetData(dataName);
}

