#include "anchorobj_phleft.h"
#include "anchorobj_hline.h"
#include "souplemanager.h"

AnchorObj_PHLeft::AnchorObj_PHLeft():level(0)
{
    z = Helper::Layer_Z::Top;
}

QQuickItem* AnchorObj_PHLeft::generateQmlItem(){
    showLevel.addListener(this,[this](){
        if(Helper::isQmlItemValid(qmlItem)) {
            qmlItem->setProperty("level",level);
            qmlItem->setProperty("showLevel",true);
        }
    });
    return uiPool_PHLeft::fetchItem();
}


void AnchorObj_PHLeft::updateDataToQmlItem(QQuickItem* item) {
    AnchorObj::updateDataToQmlItem(item);
    qmlItem->setProperty("level",level);
    qmlItem->setProperty("showLevel",(bool)showLevel);
    qmlItem->setProperty("tab",Helper::pixel2cm(width)); //首行缩进
};

void AnchorObj_PHLeft::doPHLeft() noexcept
{
    //if( ! hline) return;
    //看看左边是不是有对象
    if(leftObj) {
        //准备弹出左边所有对象
        AnchorObj_HLine *mid_hline = 0;
        //SoupleManager::registerObj(mid_hline); //创建并注册中间水平标线
        auto _hline = hline->as<AnchorObj_HLine*>();
        AnchorObj_HLine *last_hline = 0;
        //last_hline优先选用上逻辑标线
        if((last_hline = _hline->getLogicLastLine())) {
            last_hline->createNextLine(); //创建中间标线，该操作将自动转移锚定关系（如果存在）
            mid_hline = last_hline->logic_nextHLine;
            _hline->logic_lastHLine = mid_hline;
            mid_hline->logic_nextHLine = _hline; //转移接续关系

            // 应用上一个段落属性
            mid_hline->autoSetPara();

        } else if((last_hline = _hline->hline->as<AnchorObj_HLine*>())) {
            last_hline->createNextLine(); //创建中间标线
            mid_hline = last_hline->logic_nextHLine;
        }
        else { //啥也没有，这是个自由行
            //咱们可以什么都不做，因为这种情况的出现是用户操作错误，错误可逆
            return;
        }
        //SoupleManager::requestUpdateHLine(mid_hline); //引导刷新(不引导也没事，就是要等点时间。。
        SoupleManager::requestUpdateHLine(mid_hline);
        //往mid_hline上转移对象
        //float leftWidth = 0.0;
        while(leftObj) {
            auto obj = leftObj;
            obj->removeSelf(false);
            mid_hline->insertOnLeft(obj);
            //if(obj->objInfo().anchorInfo.isSelfWidth) {
            //    leftWidth += obj->width;
            //}
        }

        //qDebug() << "last.nextLine = " << last_hline->logic_nextHLine;
    }
}


void AnchorObj_PHLeft::dealLayout()
{


    //AnchorObj::dealLayout(); //处理基本的相对布局
    //if(leftObj) x = leftObj->x + leftObj->width;
    if(hline) {
        y = hline->y; //2025/9/3修改，保证PH_Left不会受到换行影响导致Glue_Left换行。
        x = hline->x;
    }
    if( Helper::isQmlItemValid(qmlItem) ) {
        qmlItem->setWidth(10);
        qmlItem->setHeight(6);
        qmlItem->setPosition({x-10,y-6});
    }

    doPHLeft();
}

AnchorObj* AnchorObj_PHLeft::dropRight(float dropWidth)
{
    removeSelf(false);
    return this;
    //return 0;
}

AnchorObj* AnchorObj_PHLeft::dropLeft(float dropWidth)
{
    auto lgl = hline->as<AnchorObj_HLine*>()->logic_lastHLine;
    if(! lgl) return 0; //虽然理论上不可能存在该情况,但还是判断一下
    if(lgl->leftObj == 0) { //逻辑上一行为空行的时候，咱们直接删除上一行
        lgl->removeSelf(true);
        //注意喽：如果上一行为空行，那么它只有可能收缩本obj，不用担心随后有obj收缩到上一行的可能
    }
    return 0;
}

int AnchorObj_PHLeft::dealCommandFromQmlItem(int command,const QVariant& arg)
{
    switch(command) {
    case Helper::SHOW_LEVEL_UP:
        AnchorObj_PHLeft::showLevel = arg.toBool(); //showLevel is a LiveValue.
        break;
    case Helper::LEVEL_UP:
        level = arg.toInt();
        break;
    case Helper::FIRSTLINE_TAB_UP:
        width = arg.toFloat();
        break;
    case Helper::ETC_TAB_UP: {
        //改变对其余行缩进，注意其余行缩进不一定全部相等。这里做增量处理
        ParaInf pi = getParaInf();
        float addv = arg.toFloat() - pi.etc_tab;
        auto hline = (HorLine_Base*)(this->hline);
        while(hline) {
            //to-do                    !!!
            hline = hline->getNextLine();
        }
        break;
    }
    default:
        return AnchorObj::dealCommandFromQmlItem(command,arg);
    }
    return 0;
}

//[static] 寻找段落
std::optional<ParaInf> AnchorObj_PHLeft::findBelongPara(HorLine_Base* hline)
{
    ParaInf para;
    while(hline) {
        if(!hline->leftObj) continue;
        auto p = hline->leftObj->as<AnchorObj_PHLeft*>();
        if(p) {
            para.phleft = p;
            para.first_tab = p->width;
            return para;//found para
        }
        if(p->canBe<AnchorObj_Glue>()) {
            para.etc_tab = p->width;
        }
        hline = hline->getPrevLine();
    }
    return {};
}

ParaInf AnchorObj_PHLeft::getParaInf() noexcept
{
    ParaInf para;
    para.phleft = this;
    para.first_tab = width;
    para.multi_etc_tab = false;
    auto hline = ((HorLine_Base*)this->hline)->getNextLine();
    if(hline) {
        auto glue = hline->leftObj->as<AnchorObj_Glue*>();
        if(glue) {
            para.etc_tab = glue->width;
        } else {
            para.etc_tab = 0;
        }
    } else {
        para.etc_tab = para.first_tab;//30274
    }
    return para;
}
