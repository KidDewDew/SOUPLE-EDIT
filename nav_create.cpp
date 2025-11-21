#include "helper.h"
#include "souplemanager.h"
#include "navline.h"
#include "anchorobj_phleft.h"
#include "anchorobj_hline.h"
#include "anchorobj_flowtext.h"
#include "anchorobj_phright.h"
#include "anchorobj_spring.h"
#include "anchorobj_jzrect.h"
#include "anchorobj_glue.h"

using namespace std;

// _NavNode 一个目录项
struct _NavNode {
    int level = 0;
    Obj* pointer_to = 0;
    _NavNode* parent = 0;
    HorLine_Base* hline = 0;
    vector<_NavNode*> children;
};

// file global vars
static int doc_id;
static QFont font;
static int showType;
static int levels;
static float tab;
static float radius;
static float spacing;
static float lineWidth;
static float dashWidth;
// 遍历nav_nodes
HorLine_Base* lastHLine;

void impl_dfs_createNavLines(_NavNode* node) {
    if(node->level > levels) //超过要求的级数了
        return;
    if(node->pointer_to) {
        HorLine_Base* navline = 0;
        if(! lastHLine) {
            // 要创建的是起始行
            auto sl = SoupleManager::getDocumentFirstLine(doc_id)
                                   ->as<AnchorObj_HLine*>();
            if(!sl) {
                emit Helper::helper->errorMsg("错误",
                            "未知错误@nav_create.cpp impl_dfs_createNavLines");
                return;
            }
            navline = sl->insertHLine_up(HorLine_Base::HLT_Same);
        } else {
            navline = lastHLine->insertHLine_down(HorLine_Base::HLT_Same);
        }
        lastHLine = navline;
        // here,navline CREATED
        // 下面填充内容------>
        NavItem *nav = new NavItem;  //导航符(段落的派生类型)
        AnchorObj_FlowText *ft = new AnchorObj_FlowText,  //左文字
                           *ft2 = new AnchorObj_FlowText; //右文字：页码
        AnchorObj_Spring* spring = new AnchorObj_Spring;  //中间的弹簧
        SoupleManager::registerObjs(nav,ft,ft2,spring);
        // 赋予属性--->
        nav->nav_at = node->pointer_to; // 导航点
        nav->width = node->level * tab; //段落符的宽度 ~即首行缩进
        ft->font = ::font;
        ft2->font = ::font;
        spring->showType = showType;
        spring->lineWidth = lineWidth;
        spring->radius = radius;
        spring->spacing = spacing;
        spring->dashWidth = dashWidth;
        Page* to_page = SoupleManager::getPage(doc_id,node->pointer_to->y);
        if(! to_page) {
            ft2->text = "?";
        } else {
            ft2->text = QString::number(to_page->index+1);
        }
        ft->calcWidth();
        ft2->calcWidth();
        // 连接、放置对象。connect_l2r_atHLine函数...
        AnchorObj::connect_l2r_atHLine(navline,nav,ft,spring,ft2);
    }
    for(_NavNode* child : node->children) {
        impl_dfs_createNavLines(child);
    }
}

// 根据_NavNode创建真正的目录
void impl_createNavLines(QVariantMap args,vector<_NavNode*> nav_nodes) {
    qDebug() << "impl_createNavLines";

    if(nav_nodes.empty()) {
        emit Helper::helper->errorMsg("提示","未检测到任何目录项。\n无法创建目录。");
        return;
    }

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
        nextHLine = at_hline;
        lastHLine = at_hline->getPrevLine();
    }

    for(_NavNode* node : nav_nodes)
        impl_dfs_createNavLines(node);

    if(nextHLine) {
        nextHLine->setPrevLine(lastHLine); //缝合
    }

    // 创建“目录”标题
    HorLine_Base* line = lastHLine;
    while(line) {
        auto l2 = line->getPrevLine();
        if(!l2 || ! l2->leftObj->canBe<NavItem>()) {
            auto new_line = line->insertHLine_up(HorLine_Base::HLT_Same);
            if(!new_line) {
                break;
            }
            AnchorObj_FlowText* text = new AnchorObj_FlowText;
            AnchorObj_JZRect* jz = new AnchorObj_JZRect;
            SoupleManager::registerObjs(text,jz);
            text->text = "目录";
            text->font = ::font;
            text->font.setPointSizeF(14);
            AnchorObj::connect_l2r_atHLine(new_line,jz,text);
            break;
        }
        line = l2;
    }
}

void Helper::createNavLines_fromLevels(QVariantMap args)
{
    qDebug() << "createNavLines_fromLevels(" << args;
    doc_id = args["document_id"].toInt();
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
        //qDebug() << hline->__dstr();
        AnchorObj_PHLeft* p = hline->leftObj->as<AnchorObj_PHLeft*>();
        IF NOT(p) THEN(continue;)
        if(p->level == 0 || p->level < 0) continue;
        int cur_level = nav_stack.back()->level;
        if(cur_level < p->level) {
            while(cur_level < p->level) {
                _NavNode *node = new _NavNode;
                node->level = cur_level+1;
                nav_stack.back()->children.push_back(node);
                node->parent = nav_stack.back();
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

inline int chinese2number(QChar c) {
    switch(c.unicode()) {
        case 0x4E00: return 1; break;
        case 0x4E8C: return 2; break;
        case 0x4E09: return 3; break;
        case 0x56DB: return 4; break;
        case 0x4E94: return 5; break;
        case 0x516D: return 6; break;
        case 0x4E03: return 7; break;
        case 0x516B: return 8; break;
        case 0x4E5D: return 9; break;
        case 0x5341: return 10; break;
        default:
            throw LLException(u"nav_create::chinese2number: unknown input="+c);
    }
}

inline QString extractSuffix(const QString& suffix) {
    if(suffix.isEmpty()) return {};
    if(suffix[0] == ')' || suffix[0].unicode() == 0xFF09 //中文'）'
        || suffix[0] == '.' || suffix[0].unicode() == 0x3001)
        return suffix[0];
    int i = 0;
    while(i < suffix.length()) {
        if(suffix[i].unicode() == 0x3001
            || suffix[i] == ':'
            || suffix[i].unicode() == 0xFF1A) {
            return suffix.sliced(0,i+1);
        }
        ++i;
    }
    return suffix;
}

struct WordCount {
    int chineseChars;    // 汉字数量
    int englishWords;    // 英文单词数量
    int all_words;
};

WordCount countWords(const QString& text) {
    WordCount result = {0, 0, 0};
    // 统计汉字
    for (const QChar& ch : text) {
        if (ch.script() == QChar::Script_Han) {
            result.chineseChars++;
        }
    }
    // 统计英文单词（使用正则表达式）
    QRegularExpression englishWordRe("\\b[a-zA-Z]+\\b");
    QRegularExpressionMatchIterator it = englishWordRe.globalMatch(text);

    while (it.hasNext()) {
        it.next();
        result.englishWords++;
    }
    result.all_words = result.englishWords + result.chineseChars;

    return result;
}

// 自动识别目录
void Helper::createNavLines_auto(QVariantMap args)
{
    int doc_id = args["document_id"].toInt();
    auto& all_objs = SoupleManager::getDocumentObjs(doc_id);
    enum {
        Arabic,Roman,Chinese,Lower_Char,Upper_Char
    };
    //特征,描述一个目录项的特征
    struct Feature {
        char number_type; //标号类型
        int number; //标号
        int arabic_level; //数字序号级别: 1:0级 1.1:1级 ...
        float tab; //左缩进
        QString prefix; //前缀
        QString suffix; //后缀
        QString __dstr() const noexcept {
            return QString("Feature(type=%1 number=%2 arabic_level=%3 tab=%4 prefix=%5 suffix=%6")
                .arg((int)number_type).arg(number).arg(arabic_level).arg(tab).arg(prefix).arg(suffix);
        }
    };

    struct PreNav { //导航项
        int level;
        AnchorObj_PHLeft* ph_left;
        Feature feature;
    };

    vector<PreNav> prenavs; //预处理的prenav

    auto hline = SoupleManager::getDocumentFirstLine(doc_id);
    if(!hline) {
        emit Helper::helper->errorMsg("错误","该文档未指定起始行！\n请补充起始行。");
        return;
    }

    /**   举例：
     * 一、xxx         1
     *   (1)...       2
     *   (2)...       2
     *     1 xxx      3
     *     2 xxx      3
     * 二、xxx         1
     *   1 xxx        2
     *   1.1 xxx      3
     *   2 xxx        2
     *   2.1 xxx      3
     *   2.1.2 xxx    4
     *   (a)...       5
     *   (b)...       5
     *   3 xxx        2
     */

    for(;hline;hline = hline->getNextLine())
    {
        if(!hline->leftObj) continue;
        float tab = hline->getPHLeftWidth();
        QString text = hline->get_merged_line_text(true);
        if(text.isEmpty()) continue;

        // 这一行是否是段落起始行，这会影响到之后的判断
        bool hasPHLeft = hline->leftObj->canBe<AnchorObj_PHLeft>();

        static QRegularExpression re("^(?<prefix>.*?)"
                              //match 几十几、十几、几十
                              "(?:(?<chinese_tens>[一二三四五六七八九]?十[一二三四五六七八九]?)|"
                              //macth 几
                              "(?<chinese_single>[一二三四五六七八九])|"
                              //match 数字序号 1  1.2 1.2.10
                              "(?:(?<arabic_prefix>(\\d+\\.)*)(?<arabic>\\d+))|"
                              //match 小写字母
                              "(?<lower_letter>(?<=\\s|\\(|（|^)[a-z](?=\\s|\\)|）|\\.|$))|"
                              //match 大写字母
                              "(?<upper_letter>(?<=\\s|\\(|（|^)[A-Z](?=\\s|\\)|）|\\.|$)))"
                              //后缀
                              "(?<suffix>\\S*)");
        auto m = re.match(text);
        if(! m.hasMatch()) continue;
        QString prefix = m.captured("prefix"),
            chinese_tens = m.captured("chinese_tens"),
            chinese_single = m.captured("chinese_single"),
            arabic_prefix = m.captured("arabic_prefix"),
            arabic = m.captured("arabic"),
            lower_letter = m.captured("lower_letter"),
            upper_letter = m.captured("upper_letter"),
            suffix = m.captured("suffix");

        Feature feature;
        // 计算特征
        feature.prefix = prefix;
        feature.suffix = extractSuffix(suffix);
        if(!chinese_tens.isEmpty() || !chinese_single.isEmpty()) {
            if(countWords(prefix).all_words > 3 ||
                countWords(feature.suffix).all_words > 3 ||
                !prefix.isEmpty() && suffix.isEmpty()) continue;
            feature.number_type = Chinese;
            if(!chinese_single.isEmpty()) {
                feature.number = chinese2number(chinese_single[0]);
            } else {
                if(chinese_tens.length() == 1)
                    feature.number = 10;
                else if(chinese2number(chinese_tens[0]) == 10)
                    feature.number = 10 + chinese2number(chinese_tens[1]);
                else
                    feature.number = chinese2number(chinese_tens[0])*10+
                                     (chinese_tens.length()==3 ? chinese2number(chinese_tens[2]):0);
            }
        }
        else if(!arabic.isEmpty()) {
            auto cw = countWords(prefix);
            if(cw.chineseChars > 4 || cw.englishWords > 2
                || cw.chineseChars&&cw.englishWords) continue;
            feature.number_type = Arabic;
            feature.number = arabic.toInt();
            feature.prefix += arabic_prefix;
            if(!arabic_prefix.isEmpty()) {
                feature.arabic_level = arabic_prefix.count('.');
            }
        }
        else if(!lower_letter.isEmpty()) {
            if(feature.suffix.isEmpty() || !prefix.isEmpty()) continue;
            feature.number_type = Lower_Char;
            feature.number = lower_letter[0].toLatin1() - 'a';
        }
        else if(!upper_letter.isEmpty()) {
            if(feature.suffix.isEmpty() || !prefix.isEmpty()) continue;
            feature.number_type = Upper_Char;
            feature.number = lower_letter[0].toLatin1() - 'A';
        }

        qDebug() << feature.__dstr();
    }
}
