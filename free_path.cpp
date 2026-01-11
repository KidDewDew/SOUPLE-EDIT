#include "free_path.h"

void Free_Path::updateDataToQmlItem(QQuickItem*item)
{
    FreeObj::updateDataToQmlItem(item);

    QVariantList vl;
    for(auto& ac : path_actions) {
        vl.push_back(ac.toVM());
    }
    item->setProperty("path",vl);
    item->setProperty("fillStyle",fillStyle);
    item->setProperty("strokeStyle",strokeStyle);
    item->setProperty("lineWidth",lineWidth);
    item->setProperty("stroke",isStroke);
    item->setProperty("fill",isFill);
}



// int Free_Path::dealCommandFromQmlItem(const QString& command,const QVariant& arg)
//{
//    return 0;
//}

