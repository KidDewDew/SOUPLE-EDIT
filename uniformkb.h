#ifndef UNIFORMKB_H
#define UNIFORMKB_H

#include <QKeyEvent>

///统一键盘事件处理类。
/// 所有主软件的键盘事件都应该通过该类的过滤。
class UniformKB
{
public:
    struct Right {};
    struct Left {};
    UniformKB() = delete;
    // dealKeyEvent，返回true:过滤 false:未过滤
    static bool dealKeyPressed(const QKeyEvent* ke);
private:
    // 从ke中获取输入的文本
    // bool getInputText(const QKeyEvent* ke,QString& text);
    // bool isBackSpace(const QKeyEvent* ke,QString& text);
};

#endif // UNIFORMKB_H
