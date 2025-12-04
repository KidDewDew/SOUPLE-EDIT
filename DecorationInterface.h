#ifndef DECORATIONINTERFACE_H
#define DECORATIONINTERFACE_H

#include <set>
#include <variant>
#include <QString>

// 定义一些装饰性的接口

class Obj;

// 信号接收器
class Signal_Receiver {
    friend class Single_Signal_Emitter;
    friend class Signal_Emitter;
protected:
    virtual int dealSignal(int signal,const std::variant<bool,int,float,double,QString>& arg) = 0;
};

// 单对象信号发生类
class Single_Signal_Emitter {
    Signal_Receiver* signal_receiver = 0;
public:
    void setSignalReceiver(Signal_Receiver* receiver) {
        this->signal_receiver = receiver;
    }
    void emitSignal(int signal,const std::variant<bool,int,float,double,QString>& arg) {
        if(signal_receiver) signal_receiver->dealSignal(signal,arg);
    }
};

// 信号发生类
class Signal_Emitter {
    std::set<Signal_Receiver*> signal_receivers;
};


#endif // DECORATIONINTERFACE_H
