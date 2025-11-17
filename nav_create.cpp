#include "helper.h"
#include "souplemanager.h"
#include "navline.h"
#include "anchorobj_phleft.h"

using namespace std;

void Helper::createNavLines_fromLevels(QVariantMap args)
{
    int doc_id = args["document_id"].toInt();
    auto& all_objs = SoupleManager::getDocumentObjs(doc_id);
    for(Obj* obj : all_objs | v_Filter(a->template canBe<AnchorObj_PHLeft>())) {
        AnchorObj_PHLeft* p = obj->be<AnchorObj_PHLeft*>();
        //p->level
    }
}

void Helper::createNavLines_auto(QVariantMap args)
{
    int doc_id = args["document_id"].toInt();
    auto& all_objs = SoupleManager::getDocumentObjs(doc_id);
}
