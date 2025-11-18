#include "helper.h"
#include "souplemanager.h"
#include "navline.h"
#include "anchorobj_phleft.h"

using namespace std;

// _NavNode 一个目录项
struct _NavNode {
    int level;
    Obj* pointer_to;
    _NavNode* parent;
    vector<_NavNode*> children;
};

void impl_createNavLines(QVariantMap args,vector<_NavNode*> nav_nodes) {

}

void Helper::createNavLines_fromLevels(QVariantMap args)
{
    int doc_id = args["document_id"].toInt();
    auto& all_objs = SoupleManager::getDocumentObjs(doc_id);
    //vector<_NavNode*> nav_nodes; //1级节点列表
    vector<_NavNode*> nav_stack; //当前的遍历栈
    _NavNode __first_node{.level=0};
    nav_stack.push_back(&__first_node);
    // 遍历所有段落标记，其中存储着级数 v_Filter宏(defined in helper.h)
    for(Obj* obj : all_objs | v_Filter(a->template canBe<AnchorObj_PHLeft>()))
    {
        AnchorObj_PHLeft* p = obj->be<AnchorObj_PHLeft*>();
        if(p->level == 0) continue;
        int cur_level = nav_stack.back()->level;
        if(cur_level < p->level) {
            while(cur_level < p->level) {
                _NavNode *node = new _NavNode;
                node->level = cur_level+1;
                nav_stack.back()->children.push_back(node);
                node->parent->parent = nav_stack.back();
                nav_stack.push_back(node);//进入下一级
                ++ cur_level;
            }
        }
        else if(cur_level >= p->level) {
            while(cur_level >= p->level) {
                nav_stack.pop_back();
                -- cur_level;
            }
            //until cur_level = p->level-1
            _NavNode *node = new _NavNode;
            node->level = p->level;
            nav_stack.back()->children.push_back(node);
            nav_stack.push_back(node);
        }
    }
}

void Helper::createNavLines_auto(QVariantMap args)
{
    int doc_id = args["document_id"].toInt();
    auto& all_objs = SoupleManager::getDocumentObjs(doc_id);
}
