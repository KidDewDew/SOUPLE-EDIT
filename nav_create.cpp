#include "helper.h"
#include "souplemanager.h"
#include "navline.h"
#include "anchorobj_phleft.h"
#include "anchorobj_hline.h"

using namespace std;

// _NavNode 一个目录项
struct _NavNode {
    int level = 0;
    Obj* pointer_to = 0;
    _NavNode* parent = 0;
    HorLine_Base* hline = 0;
    vector<_NavNode*> children;
};

QFont font;
int showType;
int levels;
float tab;
float radius;
float spacing;
float lineWidth;
float dashWidth;
// 遍历nav_nodes
HorLine_Base* lastHLine;

void impl_dfs_createNavLines(_NavNode* node) {
    if(node->pointer_to) {

    }
    for(_NavNode* child : node->children) {
        impl_dfs_createNavLines(child);
    }
}

// 根据_NavNode创建真正的目录
void impl_createNavLines(QVariantMap args,vector<_NavNode*> nav_nodes) {
    qDebug() << "impl_createNavLines";
    int doc_id = args["document_id"].toInt();
    font = args["font"].value<QFont>();
    showType = args["showType"].toInt();
    levels = args["levels"].toInt();
    tab = args["tab"].toFloat();
    radius = args["radius"].toFloat();
    spacing = args["spacing"].toFloat();
    lineWidth = args["lineWidth"].toFloat();
    dashWidth = args["dashWidth"].toFloat();
    HorLine_Base* at_hline =
        SoupleManager::getObjById(doc_id,args["hline_id"].toInt())
                                 ->as<HorLine_Base*>();
    IF NOT(at_hline)
    {
        emit Helper::helper->errorMsg("错误","传递的水平线ID无效！\n"
                                            "请再检查一下。");
        return;
    }

    bool isDown = args["isDown"].toBool(); //是否在at_hline下方创建目录
    HorLine_Base *nextHLine;
    if(isDown) {
        lastHLine = at_hline;
        nextHLine = at_hline->getNextLine();
    } else {
        nextHLine = lastHLine;
        lastHLine = at_hline->getPrevLine();
    }

    for(_NavNode* node : nav_nodes)
        impl_dfs_createNavLines(node);
}

void Helper::createNavLines_fromLevels(QVariantMap args)
{
    qDebug() << "createNavLines_fromLevels(" << args;
    int doc_id = args["document_id"].toInt();
    auto& all_objs = SoupleManager::getDocumentObjs(doc_id);
    //vector<_NavNode*> nav_nodes; //1级节点列表
    vector<_NavNode*> nav_stack; //当前的遍历栈
    _NavNode __first_node{.level=0};
    nav_stack.push_back(&__first_node);
    // 遍历所有段落标记，其中存储着级数 v_Filter宏(defined in helper.h)
    //for(Obj* obj : all_objs | v_Filter(a->template canBe<AnchorObj_PHLeft>()))
    auto hline = SoupleManager::getDocumentFirstLine(doc_id);
    if(!hline) {
        emit Helper::helper->errorMsg("错误","该文档未指定起始行！\n请补充起始行。");
        return;
    }

    for(;hline;hline = hline->getNextLine())
    {
        qDebug() << hline->__dstr();
        AnchorObj_PHLeft* p = hline->leftObj->as<AnchorObj_PHLeft*>();
        IF NOT(p) THEN(continue;)
        if(p->level == 0 || p->level < 0) continue;
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
            nav_stack.back()->pointer_to = p;
            nav_stack.back()->hline = hline;
        }
        else if(cur_level >= p->level) {
            while(cur_level >= p->level) {
                nav_stack.pop_back();
                -- cur_level;
            }
            //while until cur_level = p->level-1
            _NavNode *node = new _NavNode;
            node->level = p->level;
            nav_stack.back()->children.push_back(node);
            nav_stack.push_back(node);
            nav_stack.back()->pointer_to = p;
            nav_stack.back()->hline = hline;
        }

    }

    impl_createNavLines(args,nav_stack[0]->children);
}

void Helper::createNavLines_auto(QVariantMap args)
{
    int doc_id = args["document_id"].toInt();
    auto& all_objs = SoupleManager::getDocumentObjs(doc_id);
}
