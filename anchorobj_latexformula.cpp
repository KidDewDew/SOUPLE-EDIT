#include "anchorobj_latexformula.h"
#include <QImageReader>

AnchorObj_LatexFormula::AnchorObj_LatexFormula() {
    image_id = Helper::cache_image_index++;
    font_size = 24;
    width = 24;
    height = 16;
}

QQuickItem* AnchorObj_LatexFormula::generateQmlItem() {
    return uiPool_Latex::fetchItem();
}

void AnchorObj_LatexFormula::updateDataToQmlItem(QQuickItem*item) {
    AnchorObj::updateDataToQmlItem(item);
}
int AnchorObj_LatexFormula::dealCommandFromQmlItem(int command,const QVariant& arg) {
    switch(command) {
    case Helper::TEXT_UP:
        latex_code = arg.toString();
        break;
    case Helper::FONT_SIZE_UP:
        font_size = arg.toFloat();
        break;
    case Helper::Request_Sync: {
        latex_code = arg.toString();
        QString img_path = QString("cache/pdf_image/latex_%1.png").arg(image_id);
        Helper::requestRenderLatexFormula(latex_code,font_size,
                                          img_path);
        QImageReader reader(img_path);
        QSize size = reader.size();
        width = size.width();
        height = size.height();
        if(Helper::isQmlItemValid(qmlItem)) {
            qmlItem->setWidth(width);
            qmlItem->setHeight(height);
        }
        break;
    }
    }
    return 0;
}
AnchorObj* AnchorObj_LatexFormula::dropRight(float dropWidth) {
    removeSelf(false);
    return this;
}
AnchorObj* AnchorObj_LatexFormula::dropLeft(float dropWidth) {
    if(dropWidth >= width) {
        removeSelf(false);
        return this;
    }
    return nullptr;
}
