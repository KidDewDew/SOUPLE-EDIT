#ifndef QUICK_CALLBACK_H
#define QUICK_CALLBACK_H

#include <QObject>
#include <functional>
#include <type_traits>

class Quick_Callback: public QObject
{
    Q_OBJECT
public:
    Quick_Callback(){}
    Quick_Callback(const Quick_Callback& _other): QObject{},m_func(_other.m_func) {}
    Quick_Callback(std::function<void(const QString&)> func): m_func(func) {}
    template<class _Lambda,
             std::enable_if_t<std::is_invocable_r_v<void,_Lambda,const QString&>,int> = 0 >
    Quick_Callback(_Lambda lambda): m_func(lambda) {};
    void setCallbackFunc(std::function<void(const QString&)> func) { m_func = func; }
    void call(const QString& str) const { m_func(str); }
    Q_INVOKABLE static void invoke(Quick_Callback qc,const QString& str) {
        if(qc) qc.call(str);
    }
    operator bool() const {
        return bool(m_func);
    }
private:
    std::function<void(const QString&)> m_func;
};

Q_DECLARE_METATYPE(Quick_Callback)

#endif // QUICK_CALLBACK_H
