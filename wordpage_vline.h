#ifndef WORDPAGE_VLINE_H
#define WORDPAGE_VLINE_H

#include "anchorobj_vline.h"

/**
 * @brief The WordPage_VLine class
 *        word类型页面使用的页面VLine。
 */
class WordPage_VLine : public AnchorObj_VLine
{
public:
    WordPage_VLine();
    void notifyHLineYChanged(AnchorObj_HLine* hline) override { return; }
    virtual void updateDataToQmlItem(QQuickItem* item) override {
        //Page* page = SoupleManager::getPageByIndex(page_index);
        //if(! page) return;
        item->setX(x - item->width()/2);
        item->setY(y);
        item->setHeight(height);
        item->setProperty("data_id",id);
        item->setObjectName(name);
        item->setProperty("line_color",QColor::fromRgb(0xC9,0xC9,0xC9));
        item->setProperty("triangle_color",QColor::fromRgb(0x8C,0x68,0x69));
    }
    // virtual int getColumn() const noexcept override {
    //     return column_id;
    // }
public:
    int page_index/*, column_id*/;
    //以下为支持任意分栏实验内容：

private:
    int s_wordpage_vline_count = 0;
};

#endif // WORDPAGE_VLINE_H
