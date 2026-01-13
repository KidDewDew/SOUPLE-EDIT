#include "horline_base.h"
#include "souplemanager.h"
#include "anchorobj_phrect.h"

HorLine_Base::HorLine_Base() {
    z = Helper::Text;
}

QQuickItem* HorLine_Base::generateQmlItem()
{
    if(SoupleManager::showHelpLine == false)
        return (QQuickItem*)Helper::QmlItemStatus::CreatedButHidden;
    return uiPool_BLine::fetchItem();
}

PCPos HorLine_Base::getPCPos() {
    PCPos p;
    if(!page) page = SoupleManager::getPage(y);
    p.page = page;
    p.column = 0;
    return p;
}

void HorLine_Base::updateDataToQmlItem(QQuickItem* item)
{
    if(! Helper::isQmlItemValid(item)) return;
    height = item->height(); //hline的高度由qml决定
    AnchorObj::updateDataToQmlItem(item);
    item->setWidth(width);
    item->setPosition({x,y-item->height()/2});
}

int HorLine_Base::dealCommandFromQmlItem(int command,const QVariant& arg)
{
    if(command == Helper::Y_UP) {
        y = arg.toFloat();
    } else if(command == Helper::X_UP) {
        x = arg.toFloat();
    } else return Obj::dealCommandFromQmlItem(command,arg);
    return 0;
}

void HorLine_Base::dealLayout()
{
    if(leftObj)
    {
        auto obj = leftObj;
        obj->x = x;
        while(1) {
            obj->dealLayout();
            obj->dealUI();
            obj->tryMergeRight();
            if(obj->rightObj == nullptr) {
                this->rightObj = obj;
                break;
            }
            obj = obj->rightObj;
        }

        width = rightObj->getRightX() - leftObj->x;

    }

    updateDataToQmlItem(qmlItem);

}

void HorLine_Base::insertOnLeft(AnchorObj* obj)
{
    obj->hline = this;
    if(leftObj) leftObj->insertOnLeft(obj);
    else leftObj = rightObj = obj;
}

void HorLine_Base::insertOnRight(AnchorObj* obj)
{
    obj->hline = this;
    if(rightObj) rightObj->insertOnRight(obj);
    else leftObj = rightObj = obj;
}

//注意！：如果dead = true，请保证调用前，该行为空行！即leftObj == 0
void HorLine_Base::removeSelf(bool dead)
{
    if(dead) {
        dead_sign = true;
    }
}

void HorLine_Base::autoSetPara() {
    auto lastLine = getPrevLine();
    if(!lastLine) {
        //直接应用ph_left段落开始
        return;
    }
    float phw = lastLine->getPHLeftWidth();
    if(phw < 1e-2) return;
    AnchorObj_Glue *glue_left = new AnchorObj_Glue;
    SoupleManager::registerObj(glue_left);
    glue_left->glue_left = true;
    glue_left->width = phw;
    insertOnLeft(glue_left);
}

QString HorLine_Base::get_merged_line_text(bool addSpace) const noexcept
{
    QString str;
    auto obj = leftObj;
    bool to_addspace = false;
    while(obj) {
        std::any text = obj->getAnyData("text");
        if(text.has_value()) {
            if(addSpace && to_addspace) {
                str.append(' ');
                to_addspace = false;
            }
            try {
                str.append(std::any_cast<QString>(text));
            } catch(std::bad_any_cast& e) {}
        } else {
            //if(!obj->canBe<AnchorObj_PHRect>() || obj->width > 15) {
                to_addspace = true;
            //}
        }
        obj = obj->rightObj;
    }
    return str;
}
