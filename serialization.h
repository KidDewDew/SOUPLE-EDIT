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
#include <QColor>
#include <QFont>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include "LLException.h"

class Register_ID_Repeat_Error : std::exception {};

#define DATE_ID(date) (((long long)date)%30013)

//如果该变量可以引用，且只实现serialize，使用该宏
#define SOUPLE_PP(id) souple::serialization::KV(#id,id)
//如果id不允许修改，使用该宏
#define SOUPLE_PPC(id) souple::serialization::KVC(#id,id)
#define SOUPLE_PK(key,id) souple::serialization::KV(key,id)
#define SOUPLE_PKC(key,id) souple::serialization::KVC(key,id)

// 使用该宏来注册序列化类型
#define SOUPLE_REGISTER_CLASS(_class_name,_id) \
struct _class_name##_souple_register_inf {\
static inline int _ =\
([](){\
if(souple::serialization::get_serialization_class_map().contains(_id)){\
throw Register_ID_Repeat_Error();\
}\
souple::serialization::get_typename_id_map()[typeid(_class_name).name()]=\
souple::serialization::get_serialization_class_map()[_id]=\
{\
.id=_id,\
.class_name=#_class_name,\
.serialize=[](void*obj,souple::serialization::Serial_Output&serial_output){\
    ((_class_name*)obj)->serialize(serial_output);\
},\
.serialize_xml_br=[](void*obj,souple::serialization::Serial_Output_XML<true>&serial_output){\
        ((_class_name*)obj)->serialize(serial_output);\
},\
.serialize_xml_nobr=[](void*obj,souple::serialization::Serial_Output_XML<false>&serial_output){\
        ((_class_name*)obj)->serialize(serial_output);\
},\
.unserialize=[](souple::serialization::Serial_Input&serial_input){\
_class_name *obj = new _class_name;\
souple::serialization::do_Unserialize(obj,serial_input);\
return (void*)obj;\
}\
};\
}()\
,0);\
};

namespace souple {

    template<typename T,class Serial>
    concept NoUnSerialize = !requires(T* t,Serial& serial) {
        t->unserialize(serial);
    };

    class serialization {
    public:
        static inline QString escapeXml(const QString& input) {
            QString result;
            result.reserve(input.length() * 1.2); // 预留一些额外空间以提高性能

            for (const QChar& ch : input) {
                switch (ch.unicode()) {
                case '&':  result += "&amp;";  break;
                case '\"': result += "&quot;"; break;
                case '\'': result += "&apos;"; break;
                case '<':  result += "&lt;";   break;
                case '>':  result += "&gt;";   break;
                default:   result += ch;       break;
                }
            }

            return result;
        }

        static inline QString to_string(const QString& t) {
            return t;
        }

        static inline QString to_string(const QColor& t) {
            return t.name();
        }

        static inline QString to_string(const QFont& font) {
            QJsonObject json;
            json.insert("family",font.family());
            json.insert("pointSizeF",font.pointSizeF());
            json.insert("bold",font.bold());
            json.insert("italic",font.italic());
            json.insert("underline",font.underline());
            return QJsonDocument(json).toJson(QJsonDocument::Compact);
        }

#define NUMBER_TO_STRING(type)\
        static inline QString to_string(const type& t) {\
            return QString::number(t);\
        }

        NUMBER_TO_STRING(int)
        NUMBER_TO_STRING(unsigned int)
        NUMBER_TO_STRING(long long)
        NUMBER_TO_STRING(float)
        NUMBER_TO_STRING(double)
        NUMBER_TO_STRING(short)
        NUMBER_TO_STRING(unsigned char)
        NUMBER_TO_STRING(bool)
        NUMBER_TO_STRING(unsigned long long)

        template<class T>
        struct KV {
            KV(const char*K,T& val):K(K),val(val){}
            const char* K;
            T &val;
        };

        template<class T>
        struct KVC {
            KVC(const char*K,const T& val):K(K),val(val){}
            const char* K;
            const T &val;
        };

        struct Serial_Input {
            Serial_Input(QDataStream* ds):ds(ds) {}
            template<typename T>
            Serial_Input& operator /(KV<T> kv) {
                (*ds) >> kv.val;
                return *this;
            }
            template<typename T>
            Serial_Input& operator /(T& v) {
                (*ds) >> v;
                return *this;
            }
            QDataStream*ds;
        };

        struct Serial_Input_XML {
            Serial_Input_XML(const QString& str):str(str) {}
            template<typename T>
            Serial_Input_XML& operator /(KV<T> kv) {
                while(i<str.size() && str[i] != '>') ++i;
                while(i<str.size() && (str[i] == '\n'||str[i]=='\t')) ++i;
                ++i;
                int j = i;
                while(j<str.size() && str[j] != '<') ++j;
                //[i,j-1]即value
                if constexpr(std::is_same_v<T,int>)
                    kv.val = str.sliced(i,j-i).toInt();
                else if constexpr(std::is_same_v<T,float>)
                    kv.val = str.sliced(i,j-i).toFloat();
                else if constexpr(std::is_same_v<T,double>)
                    kv.val = str.sliced(i,j-i).toDouble();
                else if constexpr(std::is_same_v<T,QString>)
                    kv.val = str.sliced(i,j-i);
                else if constexpr(std::is_same_v<T,long long>)
                    kv.val = str.sliced(i,j-i).toLongLong();
                else if constexpr(std::is_same_v<T,unsigned int>)
                    kv.val = str.sliced(i,j-i).toUInt();
                else if constexpr(std::is_same_v<T,short>)
                    kv.val = str.sliced(i,j-i).toShort();
                else if constexpr(std::is_same_v<T,unsigned short>)
                    kv.val = str.sliced(i,j-i).toUShort();
                i = j;
                return *this;
            }
            template<typename T>
            Serial_Input_XML& operator /(T& v) {
                (*this)/KV("",v);
                return *this;
            }
            int i = 0;
            const QString& str;
        };

        struct Serial_Output {
            Serial_Output(QDataStream* ds):ds(ds) {}
            template<typename T>
            Serial_Output& operator /(KV<T> kv) {
                (*ds) << kv.val;
                return *this;
            }
            template<typename T>
            Serial_Output& operator /(KVC<T> kv) {
                (*ds) << kv.val;
                return *this;
            }
            QDataStream*ds;
        };

        template <bool br>
        struct Serial_Output_XML {
            Serial_Output_XML(QString* str):str(str) {}
            template<typename T>
            Serial_Output_XML& operator /(KV<T> kv) {
                QString s = escapeXml(to_string(kv.val));
                if constexpr(br) {
                    str->append(QStringLiteral("  <%1>%2</%1>\n").arg(kv.K).arg(s));
                } else {
                    str->append(QStringLiteral("<%1>%2</%1>").arg(kv.K).arg(s));
                }
                return *this;
            }
            template<typename T>
            Serial_Output_XML& operator /(KVC<T> kv) {
                QString s = escapeXml(to_string(kv.val));
                if constexpr(br) {
                    str->append(QStringLiteral("  <%1>%2</%1>\n").arg(kv.K).arg(s));
                } else {
                    str->append(QStringLiteral("<%1>%2</%1>").arg(kv.K).arg(s));
                }
                return *this;
            }
            template<typename T>
            Serial_Output_XML& operator /(const T& v) {
                QString s = escapeXml(to_string(v));
                if constexpr(br) {
                    str->append(QStringLiteral("  <unknown>%1</unknown>\n").arg(s));
                } else {
                    str->append(QStringLiteral("<unknown>%1</unknown>").arg(s));
                }
                return *this;
            }
            QString* str;
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
            std::string class_name;
            std::function<void(void*,Serial_Output&)> serialize; //序列化
            std::function<void(void*,Serial_Output_XML<true>&)> serialize_xml_br; //序列化xml
            std::function<void(void*,Serial_Output_XML<false>&)> serialize_xml_nobr; //序列化xml
            std::function<void*(Serial_Input&)> unserialize; //反序列化
        };

        static inline QHash<short, serialization_class_info>& get_serialization_class_map() {
            static QHash<short, serialization_class_info> instance;
            return instance;
        }

        static inline QHash<std::string, serialization_class_info>& get_typename_id_map() {
            static QHash<std::string, serialization_class_info> instance;
            return instance;
        }

        template<typename T>
        // 只对单对象进行序列化，返回QByteArray
        static inline QByteArray serialize(T* obj) {
            QByteArray bytes;
            QDataStream ds(&bytes,QIODevice::WriteOnly);
            serialize(obj,ds);
            return bytes;
        }

        template<typename T>
        static inline void serialize(T* obj,QDataStream& ds) {
            Serial_Output serial_output(&ds);
            auto it_si = get_typename_id_map().find(typeid(*obj).name());
            if(it_si == get_typename_id_map().end()) {
                throw LLException(QString("souple::serialization: 类型%1未注册")
                                      .arg(typeid(*obj).name()));
            }
            ds << it_si->id;
            qDebug() << "serialize:" << typeid(*obj).name() << it_si->id;
            it_si->serialize(obj,serial_output);
        }

        static inline void* unserialize(QDataStream& ds) {
            Serial_Input serial_input(&ds);
            short id;
            ds >> id;
            qDebug() << "unserialize(" << "id=" << id;
            auto it_si = get_serialization_class_map().find(id);
            if(it_si == get_serialization_class_map().end()) {
                throw LLException(QString("souple::unserialize: 未注册的类型ID=%1")
                                      .arg(id));
            }
            //qDebug() << "unserialize:" << id << si.id;
            void* obj = it_si->unserialize(serial_input); //反序列化
            return obj;
        }

        // inline void* unserialize_xml(const QString& str,int &i) {
        //     return obj;
        // }

        template<bool br=true,typename T>
        static inline void serialize_xml(T* obj,QString* xml) {
            Serial_Output_XML<br> serial_output(xml);
            auto it_si = get_typename_id_map().find(typeid(*obj).name());
            if(it_si == get_typename_id_map().end()) {
                throw LLException(QString("souple::serialization: 类型%1未注册")
                                      .arg(typeid(*obj).name()));
            }
            if constexpr(br) xml->append(QStringLiteral("<%1>\n").arg(it_si->class_name));
            else xml->append(QStringLiteral("<%1>").arg(it_si->class_name));
            //qDebug() << "serialize:" << typeid(*obj).name() << si.id;
            if constexpr(br)
                it_si->serialize_xml_br(obj,serial_output);
            else it_si->serialize_xml_nobr(obj,serial_output);
            if constexpr(br) xml->append(QStringLiteral("</%1>\n").arg(it_si->class_name));
            else xml->append(QStringLiteral("</%1>").arg(it_si->class_name));
        }
    };
}

#endif // SERIALIZATION_H
