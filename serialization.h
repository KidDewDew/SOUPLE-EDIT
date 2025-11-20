#ifndef SERIALIZATION_H
#define SERIALIZATION_H

// 本头文件提供序列化/反序列化支持
// 开销比正常多态实现的序列化高一些。
// 您可以在任何需要序列化或反序列化的类下面添加一个宏：
// 1.SOUPLE_REGISTER_CLASS(class_name,_id) 来注册该类型
// 其中，class_name是这个类的类名，_id是编号，是一个16位有符号整数。
// 【注意】_id不允许重复，如果重复，软件运行时会throw Register_ID_Repeat_Error
// 推荐使用宏DATE_ID(年月日时分)来生成_id，重复概率很小。
// 2.在类中实现模板函数：
//  template<class Serial>
//  void serialize(Serial& serial)
// 注意，该函数serialize同时能够完成序列化和反序列化，以及xml、二进制序列化。
// 如果您需要非对称的unserialize，则只需要添加一个unserialize函数：
// template<class Serial>
// void unserialize(Serial& serial)
// @author liushuo

#include <QHash>
#include <typeinfo>
#include <QString>
#include <QDataStream>
#include <QDebug>
#include "LLException.h"

class Register_ID_Repeat_Error : std::exception {};

#define DATE_ID(date) (((long long)date)%30013)

// 使用该宏来注册序列化类型
#define SOUPLE_REGISTER_CLASS(class_name,_id) \
struct class_name##_souple_register_inf {\
static inline int _ =\
([](){\
if(souple::serialization::get_serialization_class_map().contains(_id)){\
throw Register_ID_Repeat_Error();\
}\
souple::serialization::get_typename_id_map()[typeid(class_name).name()]=\
souple::serialization::get_serialization_class_map()[_id]=\
{\
.id=_id,\
.serialize=[](void*obj,souple::serialization::Serial_Output&serial_output){\
    ((class_name*)obj)->serialize(serial_output);\
},\
.unserialize=[](souple::serialization::Serial_Input&serial_input){\
class_name *obj = new class_name;\
souple::serialization::do_Unserialize(obj,serial_input);\
return (void*)obj;\
}\
};\
}()\
,0);\
};

namespace souple {

    namespace serialization {

        struct Serial_Input {
            Serial_Input(QDataStream* ds):ds(ds) {}
            template<typename T>
            Serial_Input& operator /(T& value) {
                (*ds) >> value;
                return *this;
            }
            QDataStream*ds;
        };

        struct Serial_Output {
            Serial_Output(QDataStream* ds):ds(ds) {}
            template<typename T>
            Serial_Output& operator /(T&& value) {
                (*ds) << std::forward<T>(value);
                return *this;
            }
            QDataStream*ds;
        };

        struct Serial_Output_XML {
            Serial_Output_XML(QString* str):str(str) {}
            template<typename T>
            Serial_Output_XML& operator /(T&& value) {

                return *this;
            }
            QString* str;
        };

        template<typename T,class Serial>
        concept NoUnSerialize = !requires(T* t,Serial& serial) {
            t->unserialize(serial);
        };

        template <class T,class Serial>
            requires requires(T* t,Serial& serial) {
                t->serialize(serial);
                requires NoUnSerialize<T,Serial>;
            }
        static inline void do_Unserialize(T* t,Serial& serial) {
            t->serialize(serial);
        }

        template <class T,class Serial>
            requires requires(T* t,Serial& serial) {
                t->unserialize(serial);
            }
        static inline void do_Unserialize(T* t,Serial& serial) {
            t->unserialize(serial);
        }


        struct serialization_class_info {
            short id;
            std::function<void(void*,Serial_Output&)> serialize; //序列化
            std::function<void*(Serial_Input&)> unserialize; //反序列化
        };

        inline QHash<short, serialization_class_info>& get_serialization_class_map() {
            static QHash<short, serialization_class_info> instance;
            return instance;
        }

        inline QHash<std::string, serialization_class_info>& get_typename_id_map() {
            static QHash<std::string, serialization_class_info> instance;
            return instance;
        }

        template<typename T>
        inline void serialize(T* obj,QDataStream& ds) {
            Serial_Output serial_output(&ds);
            auto it_si = get_typename_id_map().find(typeid(*obj).name());
            if(it_si == get_typename_id_map().end()) {
                throw LLException(QString("souple::serialization: 类型%1未注册")
                                      .arg(typeid(*obj).name()));
            }
            ds << it_si->id;
            //qDebug() << "serialize:" << typeid(*obj).name() << si.id;
            it_si->serialize(obj,serial_output);
        }

        inline void* unserialize(QDataStream& ds) {
            Serial_Input serial_input(&ds);
            short id;
            ds >> id;
            auto it_si = get_serialization_class_map().find(id);
            if(it_si == get_serialization_class_map().end()) {
                throw LLException(QString("souple::unserialize: 未注册的类型ID=%1")
                                      .arg(id));
            }
            //qDebug() << "unserialize:" << id << si.id;
            void* obj = it_si->unserialize(serial_input); //反序列化
            return obj;
        }
    }
}

#endif // SERIALIZATION_H
