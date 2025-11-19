#include "free_rich.h"
#include "souplemanager.h"

Free_Rich::Free_Rich() {
    width = 160;
    height = 100;
    z = Helper::Layer_Z::Text - 1;
}

// void Free_Rich::updateDataToQmlItem(QQuickItem* item)
// {
//     FreeObj::updateDataToQmlItem(item);
// }


void Free_Rich::dealLayout()
{
    FreeObj::dealLayout();

    //如果启用了vline，需要设置它们的属性
    if(vline_left && vline_right) {
        vline_left->x = x;
        vline_right->x = x+width;
    }

    if(bg) {
        bg->x = x;
        bg->y = y;
        bg->z = z-1;
        bg->width = width;
        bg->height = height;
        if(QML_VALID(bg)) {
            bg->qmlItem->setPosition({x,y});
            bg->qmlItem->setSize({width,height});
            bg->qmlItem->setZ(z-1);
        }
    }

    if(! firstLine) return;
    //富文本框大小不会自适应，除非已经容纳不下。容纳不下时，默认增加高度而不是宽度。
    auto line = firstLine;
    decltype(line) lastLine;
    switch(vAlignMode) {
    case Helper::AlignTop:
        firstLine->y = y - firstLine->contentTop + firstLine->getTopMargin();
        break;
    case Helper::AlignVCenter:
        firstLine->y = y - firstLine->contentTop + (height - contentHeight) * 0.5;
        break;
    case Helper::AlignBottom:
        firstLine->y = y + height - firstLine->contentBottom + firstLine->y;
        break;
    }
    while(line) {
        lastLine = line;
        line->x = x;
        line->width = width;
        line->z = z+1;
        line->dealLayout();
        //qDebug() << "rich:" << line->__dstr() << " Y=[" << line->y;
        //if(line->getPrevLine())
        //    qDebug() << "last-:::" << line->getPrevLine()->__dstr();
        line = line->getNextLine();
    }
    contentHeight = lastLine->getContentBottom() - firstLine->getContentTop(); //计算内容高度
    if(contentHeight > height) { //容纳不下
        height = contentHeight;
        //发送信号
        emitSignal(Helper::HEIGHT_UP,height);
        if( Helper::isQmlItemValid(qmlItem) ) qmlItem->setHeight(height);
        SoupleManager::requestUpdateHLine(this);
    }
}

int Free_Rich::dealCommandFromQmlItem(int command,const QVariant& arg)
{
    switch(command) {
    case Helper::ADDLINE:
        if(firstLine == 0) {
            firstLine = new BlockInner_HorLine;
            SoupleManager::registerObj(firstLine);
            firstLine->be<BlockInner_HorLine*>()->parent = this;
            firstLine->z = this->z + 1;
        } else {
            BlockInner_HorLine *line = new BlockInner_HorLine;
            HorLine_Base *lastLine = findLastLine();
            SoupleManager::registerObj(line);
            line->hline = lastLine;
            //lastLine->setNextLine(line);
            lastLine->connectHLine_down(line);
            line->topMargin = lastLine->getTopMargin() < 1e-6 ?
                                  4 : lastLine->getTopMargin();
            line->parent = this;
        }
        break;
    case Helper::STHCENTER:
        switch(arg.toInt()) {
        case Helper::AlignLeft:
            break;
        case Helper::AlignHCenter:
            break;
        case Helper::AlignRight:
            break;
        }
        break;
    case Helper::STVCENTER:
        break;
    case Helper::CONT_CHANGED: //是否是容器改变
        qDebug() << "CONT_CHANGED: " << arg;
        if(arg == false && firstLine) {
            //隐藏所有innerLine
            auto line = firstLine;
            while(line) {
                if(Helper::isQmlItemValid(line->qmlItem)) {
                    line->discard_qmlItem();
                }
                line = line->getNextLine();
            }
        }
        break;
    case Helper::CREATE:
        if(arg == "bg" && !bg) {
            bg = new Free_Frame;
            SoupleManager::registerObj(bg);
        }
        break;
    case Helper::SET_FOCUS:
        if(arg == "bg") {
            if(QML_VALID(bg)) {
                bg->qmlItem->setProperty("allowEditBound",false);
                bg->qmlItem->setFocus(true);
            }
        }
        break;
    default:
        return FreeObj::dealCommandFromQmlItem(command,arg);
    }
    return 0;
}
