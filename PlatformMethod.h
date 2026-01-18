#ifndef PLATFORMMETHOD_H
#define PLATFORMMETHOD_H

#include <QString>
#include <QDebug>
#include <QHash>
#include <QQmlProperty>
#include <QQmlApplicationEngine>
#include <QDebug>

extern QObject* qmlRoot;
extern QQmlApplicationEngine* engine;


//实现跨平台方法


#ifdef Q_OS_ANDROID
#include <QJniObject>
#include <QtCore/private/qandroidextras_p.h>
class MyAndroidActivityReceiver : public QObject, public QtAndroidPrivate::ActivityResultListener {
    Q_OBJECT
signals:
    void call(QJSValue func, QJSValueList args);
public:
    QHash<int, QJSValue> callback_funcs;
    int id = 1220;
public:
    bool handleActivityResult(int receiverRequestCode, int resultCode, jobject data)
    override
    {
        auto it = callback_funcs.constFind(receiverRequestCode);
        if(it != callback_funcs.cend()) {
            QJniObject obj(data); //包装一下
            QJniObject jUri = obj.callObjectMethod("getData", "()Landroid/net/Uri;");
            // QJniObject path = QJniObject::callStaticMethod<jstring>("org/qtproject/qt/MyJava",
            //                                 "getPath",
            //             "(Landroid/content/Context;Landroid/net/Uri;)Ljava/lang/String;",
            //             QtAndroidPrivate::context(), jUri.object());
            //QString fname = "temp.pdf";
            //jstring fname = "temp.pdf";
            QJniObject::callStaticObjectMethod(
                "org/qtproject/qt/MyJava",
                "copyUriToAppDir",
                "(Landroid/content/Context;Landroid/net/Uri;)Ljava/io/File;",
                QtAndroidPrivate::context(),
                jUri.object());
            emit call(*it, QJSValueList() << resultCode << "temp.pdf");
            callback_funcs.erase(it);
        }
        return true;
    }
};
#endif

class __Caller : public QObject{
    Q_OBJECT
public slots:
    void call(QJSValue func, QJSValueList arg) {
        func.call(arg);
        //qDebug() << "call call call  ------------------";
    }
};

class PlatformMethod { //把不同平台的方法统一到一个类里面来
public:
    static void init() { //初始化
#ifdef Q_OS_ANDROID
        receiver = new MyAndroidActivityReceiver;
        QtAndroidPrivate::registerActivityResultListener(receiver); //注册activity消息回调函数
        static __Caller *caller = new __Caller;
        QObject::connect(receiver, &MyAndroidActivityReceiver::call, caller,
                         &__Caller::call, Qt::QueuedConnection);
#endif
    }
    //选择文件 - 跨平台方法
    static void selectFile(const QJSValue& callback_func) { //如果是安卓平台
#ifdef Q_OS_ANDROID
        QJniObject::callStaticMethod<void>  //调用MyJava.java中的原生方法
            ("org/qtproject/qt/MyJava","selectFile","(Landroid/app/Activity;I)V"
             , QtAndroidPrivate::activity(), receiver->id);
        receiver->callback_funcs[receiver->id++] = callback_func;
#else
    //QString fileName = QFileDialog::getOpenFileName(nullptr,"选择文件","/home","所有文件(*.*)"); \
    //if(! fileName.isEmpty()) callback_func(1, fileName);
        QMetaObject::invokeMethod(qmlRoot, "open_fileDialog",
                                  Q_ARG(QVariant, QStringLiteral("选择一张图片")),
                                  Q_ARG(QVariant, QStringLiteral("所有文件(*.*)")),
                                  callback_func.toVariant());
#endif

    }
    static void selectPhoto(const QJSValue& callback_func) { //如果是安卓平台
#ifdef Q_OS_ANDROID
        QJniObject::callStaticMethod<void>  //调用MyJava.java中的原生方法
            ("org/qtproject/qt/MyJava","selectPhoto","(Landroid/app/Activity;I)V"
                            , QtAndroidPrivate::activity(), receiver->id);
        receiver->callback_funcs[receiver->id++] = callback_func;
#else
        //QString fileName = QFileDialog::getOpenFileName(nullptr,"选择一张图片","/home","图像文件(*.bmp *.png *.jpg *.jpeg *.gif)");
        //if(! fileName.isEmpty()) callback_func(1, fileName);
        QMetaObject::invokeMethod(qmlRoot, "open_fileDialog", Q_ARG(QVariant, u"选择一张图片"_qs),
                                  Q_ARG(QVariant, u"图像文件(*.bmp *.png *.jpg *.jpeg *.gif)"_qs),
                                  callback_func.toVariant());
#endif

    }
    static bool requestPermisson(const QString& permisson) {
#ifdef Q_OS_ANDROID //如果是安卓平台
        QtAndroidPrivate::PermissionResult r = QtAndroidPrivate::checkPermission(permisson).result();
        //QFuture在调用result()时将阻塞至获得结果
        if(r == QtAndroidPrivate::PermissionResult::Denied) {
            QtAndroidPrivate::requestPermission(permisson);
            r = QtAndroidPrivate::checkPermission(permisson).result();;
            return r == QtAndroidPrivate::PermissionResult::Authorized;
        }
        return true;
#else
        return true;
#endif
    }
private:
#ifdef Q_OS_ANDROID
    static inline MyAndroidActivityReceiver *receiver;
#endif
};

#endif // PLATFORMMETHOD_H
