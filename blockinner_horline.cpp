#include "blockinner_horline.h"
#include "souplemanager.h"

BlockInner_HorLine::BlockInner_HorLine() {}

void BlockInner_HorLine::dealLayout()
{
    BlockInner_HorLine *lastLine = getLastLine();
    if(lastLine) { // 不考虑跨页、跨栏的情况，因为设计上认为不存在这种情况
        y = lastLine->getContentBottom() + topMargin - contentTop;
    }

    if( Helper::isQmlItemValid(qmlItem) ) {
        qmlItem->setPosition({x,y-height/2});
        qmlItem->setWidth(width);
    }

    if( ! leftObj ) {
        if(! nextLine && lastLine) {
            //删除自身
            lastLine->nextLine = 0;
            removeSelf(true); //注：不用担心parent容器使用野指针。所有指针的释放，均在一次布局周期结束才可能进行。
            return;
        }
        if(nextLine && nextLine->leftObj) { //空行要尝试收缩
            auto drop = nextLine->leftObj->dropLeft(width);
            if(drop) { //收缩
                insertOnRight(drop);
                drop->dealLayout();
                if(drop->leftObj) {
                    drop->leftObj->tryMergeRight(); //收缩时，自动尝试合并
                }
            }
        }
        return;
    }

    bool hasHeight = false;
    float min_obj_y = 2e9,max_obj_y = -1e8;
    AnchorObj *obj = leftObj;
    obj->x = x;
    while(1) {
        obj->dealLayout();
        obj->tryMergeRight(); //尝试合并
        if(obj->objInfo().anchorInfo.isRealHeight) { //看它是否具备“真实”高度
            hasHeight = true;
            min_obj_y = std::min(min_obj_y,obj->y);
            max_obj_y = std::max(max_obj_y,obj->y+obj->height);
        }
        if(obj->rightObj == nullptr) {
            rightObj = obj;
            break;
        }
        obj = obj->rightObj;
    }

    if(hasHeight) {
        contentTop = min_obj_y - y;
        contentBottom = max_obj_y - y;
    }

    int limit = 10;
    bool hasDrop = true;

    while(rightObj && getRightX() < rightObj->getRightX()) {
        float drop_width = rightObj->getRightX() - getRightX();
        auto drop = rightObj->dropRight(drop_width);
        if(drop) { //溢出
            if(drop->objInfo().anchorInfo.isSelfWidth
                && drop->width >= width
                && (!rightObj || rightObj->x < getRightX())) {
                this->insertOnRight(drop);
                break; //注：若dropObj宽度大于行宽、且rightObj左边x小于行右边线，则不drop！防止无限drop
            }
            if(! nextLine) {
                //创建下一行
                auto il = new BlockInner_HorLine;
                nextLine = il;
                SoupleManager::registerObj(nextLine);
                il->topMargin = topMargin;
                nextLine->hline = this;
                il->parent = parent;
            }
            nextLine->insertOnLeft(drop);
            hasDrop = true;
        } else break;
        --limit;
        if(! limit) break;
    }

    if(nextLine) {
        limit = 10;
        while(nextLine->leftObj && rightObj->getRightX() < getRightX()) {
            auto drop = nextLine->leftObj->dropLeft(getRightX() - rightObj->getRightX());
            if(drop) { //收缩
                insertOnRight(drop);
                drop->dealLayout();
                if(drop->leftObj) {
                    drop->leftObj->tryMergeRight(); //收缩时，自动尝试合并
                }
                hasDrop = true;
            } else break;
            --limit;
            if(! limit) break;
        }
    }

    //if(hasDrop && nextLine) SoupleManager::requestUpdateHLine(nextLine);

}

QQuickItem* BlockInner_HorLine::generateQmlItem()
{
    //询问一下父容器，看看是否需要自己显示。
    if(parent && parent->getData(Helper::NEED_SHOW) == (int)false)
        return nullptr; //不显示
    return uiPool_ILine::fetchItem(); //显示
}

void BlockInner_HorLine::updateDataToQmlItem(QQuickItem*item)
{
    HorLine_Base::updateDataToQmlItem(item);
}

int BlockInner_HorLine::dealCommandFromQmlItem(int command,const QVariant& arg)
{
    // switch(command) {

    // }

    return HorLine_Base::dealCommandFromQmlItem(command,arg);
}

void BlockInner_HorLine::setPrevLine(HorLine_Base* l) noexcept
{
    if(!l) {
        if(hline) {
            hline->be<HorLine_Base*>()->setNextLine(0);
        }
        hline = 0;
    }
    BlockInner_HorLine* l2 = l->as<BlockInner_HorLine*>();
    if(l2) {
        // 链表插入
        l->hline = this->hline;
        this->hline = l;
        l2->nextLine = this;
    } else {
        //创建兼容线
        NEW_VAR(FitLine_for_AnchorObj_HLine,fitline);
        //connect fitline->this
        this->hline = fitline;
        fitline->nextLine = this;
        //connect l->fitline
        fitline->setPrevLine(l);
    }
}
