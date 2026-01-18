#include "wordpage_vline.h"

WordPage_VLine::WordPage_VLine() {
    this->isWordLine = true;
#ifndef MULTITHREAD_SOUPLEMANAGER
    while(hash_vline.contains("W"+QString::number(s_wordpage_vline_count))) {
        ++s_wordpage_vline_count;
    }
    name = "W"+QString::number(s_wordpage_vline_count++); //自动命名
    hash_vline[name] = this; //记录hline
#endif
    height = 100;
    z = Helper::Layer_Z::Top; //绝对置顶
}
