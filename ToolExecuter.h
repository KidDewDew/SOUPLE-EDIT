#ifndef TOOLEXECUTER_H
#define TOOLEXECUTER_H

#include <QProcess>
#include <QCoreApplication>

//外部程序调用器
class ToolExecuter {
public:
    template<bool wait = true,typename...Args>
    static inline void execute(const QString& exe_name,Args...args) {
        QProcess *process = new QProcess;
        QString path = QCoreApplication::applicationDirPath() + "/" + exe_name;
        QStringList arg_list;
        [[maybe_unused]] int __[] = {0,((arg_list << args),0)...};
        process->start(path,arg_list);
        if constexpr(wait) {
            process->waitForFinished();
        }
    }
    template <bool wait = true>
    static inline void execute(const QString& exe_name,const QStringList& arg_list) {
        QProcess *process = new QProcess;
        QString path = QCoreApplication::applicationDirPath() + "/" + exe_name;
        process->start(path,arg_list);
        if constexpr(wait) {
            process->waitForFinished();
        }
    }
};


#endif // TOOLEXECUTER_H
