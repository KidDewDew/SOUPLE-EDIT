#ifndef JAVAPKG_H
#define JAVAPKG_H


#ifdef WIN32
#include <jni.h>
#endif

#include <QString>
#include <QProcessEnvironment>
#include <QLibrary>
#include <QFile>
#include <QDebug>

#include "helper.h"
#include "LLException.h"

class JavaPKG
{
public:
    //初始化java环境
    static inline bool init()
    {

        QProcessEnvironment sys_env = QProcessEnvironment::systemEnvironment();
        QString JAVA_HOME = sys_env.value("JAVA_HOME");

        if(JAVA_HOME == "") { //无java环境
            throw LLException("未找到JAVA_HOME环境变量",LLException::NotFound_JAVA_HOME);
        }

        qDebug() << "JAVA_HOME: " << JAVA_HOME;

        QString library_path;

#ifdef WIN32
        if(QFile(JAVA_HOME+"/jre/bin/server/jvm.dll").exists())
            library_path = JAVA_HOME+"/jre/bin/server/jvm.dll";
        if(QFile(JAVA_HOME+"/bin/server/jvm.dll").exists())
            library_path = JAVA_HOME+"/bin/server/jvm.dll";
        if(QFile(JAVA_HOME+"/jre/bin/client/jvm.dll").exists())
            library_path = JAVA_HOME+"/jre/bin/client/jvm.dll";
#endif

        QLibrary jvm_library(library_path);

        if( ! jvm_library.load())
            return false;

        qDebug() << "jvm.dll loaded!";

        //jvm_library.

        JavaVMOption options[1];
        options[0].optionString = (char*)"-Djava.class.path=./tools/JavaPKG.jar";

        JavaVMInitArgs vm_args;
        vm_args.version = JNI_VERSION_1_6;
        vm_args.nOptions = 1;
        vm_args.options = options;
        vm_args.ignoreUnrecognized = JNI_TRUE;

        using FUNC_JNI_CreateJavaVM = jint(JNICALL*)(JavaVM **, void **, void *);

        auto func_JNI_CreateJavaVM = (FUNC_JNI_CreateJavaVM)jvm_library.resolve("JNI_CreateJavaVM");
        jint r0 = func_JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
        //JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);

        jvm_library.unload();

        if(!jvm || !env || r0 < 0) return false;

        qDebug() << "jvm 启动成功";

        jclass_pdfTool = env->FindClass("person/liushuo/PdfTool");

        return bool(jclass_pdfTool);
    }

    static inline void exit()
    {
        if (jvm) jvm->DestroyJavaVM();
    }

    template<bool _new_thread = true>
    class ThreadEnv {
    public:
        ThreadEnv() {
            if constexpr(_new_thread) {
                jvm->AttachCurrentThread((void**)&env,NULL);
            } else {
                this->env = JavaPKG::env;
            }
        }
        ~ThreadEnv() {
            if constexpr(_new_thread) {
                jvm->DetachCurrentThread();
            }
        }
        /**
         * @brief jni_extractEmbeddedFonts: 调用jar包中函数，提取pdf_file的内嵌字体及cmap表到save_path目录。
         * @param pdf_file
         * @param save_path
         */
        QString jni_extractEmbeddedFonts(const QString& pdf_file,const QString& save_path) {
            qDebug() << "jni_extractEmbeddedFonts(" << pdf_file << save_path;
            jmethodID mid = env->GetStaticMethodID(jclass_pdfTool,"extractEmbeddedFonts",
                                        "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");

            jobject r = env->CallStaticObjectMethod(jclass_pdfTool,
                mid,
                env->NewStringUTF(pdf_file.toUtf8().data()),
                env->NewStringUTF(save_path.toUtf8().data())
            );

            if(r == 0) {
                emit Helper::helper->errorMsg("错误","加载PDF字体失败！");
                return "";
            }

            jstring r_string = (jstring) r;
            const char* r_cstr = env->GetStringUTFChars(r_string,nullptr);
            QString str = QString::fromUtf8(r_cstr);
            env->ReleaseStringUTFChars(r_string,r_cstr); //告知jvm: 该jstring在c++的引用已经被释放，可以正常回收该jstring。
            return str;
        }

        /**
         * @brief jni_latexFormula2image
         * @param latex: 公式的latex代码
         * @param image_save_path: 输出图像的路径（必须是png）
         * @return ok:成功 其他：失败原因
         */
        QString jni_latexFormula2image(const QString& latex,float fontSize,const QString& image_save_path) {
            qDebug() << "jni_latexFormula2image(" << latex << image_save_path;
            jmethodID mid = env->GetStaticMethodID(jclass_pdfTool,"latexFormula2image",
                                                   "(Ljava/lang/String;FLjava/lang/String;)Ljava/lang/String;");

            if(mid == 0) {
                emit Helper::helper->errorMsg("错误","找不到Latex公式编译方法。");
                return "调用Latex转换功能失败";
            }

            jobject r = env->CallStaticObjectMethod(jclass_pdfTool,
                                                    mid,
                                                    env->NewStringUTF(latex.toUtf8().data()),
                                                    jfloat(fontSize),
                                                    env->NewStringUTF(image_save_path.toUtf8().data())
                                                    );
            qDebug() << "a";
            if(r == 0) {
                emit Helper::helper->errorMsg("错误","调用Latex转换功能失败！");
                return "调用Latex转换功能失败";
            }

            jstring r_string = (jstring) r;
            const char* r_cstr = env->GetStringUTFChars(r_string,nullptr);
            QString str = QString::fromUtf8(r_cstr);
            env->ReleaseStringUTFChars(r_string,r_cstr); //告知jvm: 该jstring在c++的引用已经被释放，可以正常回收该jstring。
            qDebug() << "b:" << str;
            return str;
        }

    private:
        JNIEnv* env;
    };

private:
    static inline jclass  jclass_pdfTool;
    static inline JavaVM* jvm = nullptr;
    static inline JNIEnv* env = nullptr;
};


#endif // JAVAPKG_H
