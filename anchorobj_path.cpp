#include "anchorobj_path.h"
#include <QPainterPath>
#include <QPainter>

QQuickItem* AnchorObj_Path::generateQmlItem() {
    return uiPool_Path::fetchItem();
}

void AnchorObj_Path::updateDataToQmlItem(QQuickItem*item)
{
    AnchorObj::updateDataToQmlItem(item);
    //上传path_action到qml对象
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

int AnchorObj_Path::dealCommandFromQmlItem(int command,const QVariant& arg)
{
    return 0;
}

AnchorObj *AnchorObj_Path::dropRight(float dropWidth)
{
    removeSelf(false);
    return this;
}

AnchorObj *AnchorObj_Path::dropLeft(float dropWidth)
{
    if(dropWidth < width) return 0;
    removeSelf(false);
    return this;
}

void AnchorObj_Path::qt_paint(QPainter& painter,Page* page)
{
    //绘制路径到PDF
    QPainterPath path;
    for(int i = 0; i < path_actions.size(); ++i)
    {
        auto &a = path_actions[i];
        switch(a.type) {
        case Path_Action::MoveTo: path.moveTo(x+a.x,y+a.y-page->top_y); break;
        case Path_Action::LineTo: path.lineTo(x+a.x,y+a.y-page->top_y); break;
        case Path_Action::BezierTo: {
            if(i+2 >= path_actions.size()) break;
            auto &cp1 = a;
            auto &cp2 = path_actions[++i];
            auto &p = path_actions[++i];
            path.cubicTo({x+cp1.x,y+cp1.y-page->top_y}, //cp1
                         {x+cp2.x,y+cp2.y-page->top_y}, //cp2
                         {x+p.x,y+p.y-page->top_y});    //aim
            break;
        }
        }
    }
    //目前忽略描边
    painter.fillPath(path,fillStyle);
}
