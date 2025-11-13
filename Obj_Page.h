#ifndef OBJ_PAGE_H
#define OBJ_PAGE_H

#include "obj.h"
#include "uiitempool.h"

class Obj_Page : public Obj{
public:
    void initFromPage() {
        x = 0;
        y = page->top_y;
        width = page->width;
        height = page->height;
    }
    virtual void discard_qmlItem() override {
        uiPool_Page::returnItem(qmlItem);
        qmlItem = 0;
    }
    virtual QQuickItem* generateQmlItem() override {
        return uiPool_Page::fetchItem();
    }
    virtual void updateDataToQmlItem(QQuickItem* item) override {
        initFromPage();
        z = Helper::Layer_Z::Bottom * 10;
        Obj::updateDataToQmlItem(item);
        item->setProperty("index",page->index);
        item->setProperty("top_margin",page->topMargin);
        item->setProperty("bottom_margin",page->bottomMargin);
    };
public:
    Page *page;
private:
    static constexpr char __UINAME__[] = "Page";
public:
    typedef UIItemPool<__UINAME__,3> uiPool_Page; //ui控件池
};

#endif // OBJ_PAGE_H
