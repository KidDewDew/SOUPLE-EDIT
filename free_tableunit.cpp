#include "free_tableunit.h"
#include "tableline.h"
#include <QPainterPath>

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
    if (dataName == Helper::TEXT) {
        return getText();
    }
    return Free_Rich::qmlGetData(dataName);
}

void Free_TableUnit::qt_paint(QPainter& painter,Page* page) {
    if(!tableinfo) return;
    QPainterPath path;
    QPen pen;
    pen.setColor("black");
    pen.setWidthF(Helper::point2pixel(0.6));
    path.moveTo(x,y-page->top_y+height);
    path.lineTo(x+width,y-page->top_y+height);
    path.lineTo(x+width,y-page->top_y);
    painter.strokePath(path,pen);
}


QString Free_TableUnit::getText() const {
    // 检查依赖是否有效
    if (!tableinfo || m_row < 0 || m_col < 0) {
        return QString();
    }
    // 直接获取QString（无需转换为std::string）
    try {
        // TableInfo::getCellText() 本身返回QString，直接返回即可
        return tableinfo->getCellText(m_row, m_col);
    } catch (const std::out_of_range& e) {
        // 处理行列越界的情况（如果TableInfo的方法会抛出异常）
        return QString();
    } catch (...) {
        // 捕获其他可能的异常，提高健壮性
        return QString();
    }

}
