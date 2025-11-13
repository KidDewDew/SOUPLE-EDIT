#ifndef LLEXCEPTION_H
#define LLEXCEPTION_H

#include <exception>
#include <QString>
#include <any>

class LLException: public std::exception
{
public:
    enum {
        Other,
        NotFound_JAVA_HOME
    };
    LLException(const char* info,int type2 = Other): info(info),type2(type2) {}
    LLException(QString&& info,int type2 = Other): info(info),type2(type2) {}
    LLException(const QString& info,int type2 = Other): info(info),type2(type2) {}

    void setType2(int type2){
        this->type2 = type2;
    }

    template<typename T>
    void setExtra(T&& extra) {
        this->extra = extra;
    }

    const std::any& getExtra() const {
        return extra;
    }

    int getType2() const {
        return type2;
    }

    const QString& getInfo() const {
        return info;
    }
private:
    int type2;
    QString info;
    std::any extra;
};

#endif // LLEXCEPTION_H
