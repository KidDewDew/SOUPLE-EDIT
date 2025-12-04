#ifndef SOUPLESERIALIZER_H
#define SOUPLESERIALIZER_H

#include <QVector>
#include <QFont>
#include <QDataStream>
#include <QJsonObject>
// [已废弃】
// souple元素序列化、反序列化类
// 读取.soup文件 / 存储.soup格式文件
// 提供souple文档的数据结构
// 提供 qml 与 c++间souple文档的转换
class SoupleSerializer
{
public:
    struct DocumentInf { //文档信息
        double total_width; //所有页面宽度最大值
        QString author; //......[待补充]
    };
    struct PageInf { //页面信息
        double width, height; //单位像素 notice:内存中全部以像素存储，存储时全部用厘米为单位！
        double y_offset;      //相对于文档顶部的y偏移(y:pixel)
        int rotation;         //旋转[0,90,180,270]
    };
    struct AnchorObj {
        virtual ~AnchorObj() = default; //启用多态
        // virtual QDataStream& operator>>(QDataStream& ds) = 0; //序列化
        // virtual QDataStream& operator<<(QDataStream& ds) = 0; //反序列化
        //目前只提供vmap形式的序列化，以后为了减少存储量，需补充DataStream的存储方法。
        virtual QVariantMap toVariantMap() = 0; //转换为variantMap(类似于json)
        virtual void fromVariantMap(const QVariantMap& vm) = 0; //由variantMap还原
    };
    struct VerLine : public AnchorObj { //垂直标线
        double x_offset;      //相对于文档左边缘的x偏移
        double rotation;      //旋转角度，单位度(deg)
        QString objectName;   //名称
        // virtual QDataStream& operator>>(QDataStream& ds) {
        //     return ds << objectName << x_offset << rotation;
        // }
        // virtual QDataStream& operator<<(QDataStream& ds) = 0; //反序列化
        virtual QVariantMap toVariantMap() {
            return {{"x_offset",x_offset},{"rotation",rotation},{"objName",objectName}};
        }
        virtual void fromVariantMap(const QVariantMap& vm) {
            x_offset = vm["x_offset"].toDouble();
            rotation = vm["rotation"].toDouble();
            objectName = vm["objName"].toString();
        }
    };
    struct HorLine : public AnchorObj {
        double y_offset;      //相对于文档顶部的y偏移
        QString objectName;
        QList<AnchorObj*> list_obj; //从左到右的anchor_obj
        virtual QVariantMap toVariantMap() {
            QVariantMap vm;
            vm["y_offset"] = y_offset;
            vm["objName"] = objectName;
            QVariantList vl;
            for(auto obj : list_obj) {
                vl << obj->toVariantMap(); //序列化依附obj
            }
            vm["list_obj"] = vl;
            return vm;
        }
        virtual void fromVariantMap(const QVariantMap& vm) {
            y_offset = vm["y_offset"].toDouble();
            objectName = vm["objName"].toString();
            QVariantList vl = vm["list_obj"].toList();
            // for(auto& vm_obj : vl) {
            //     AnchorObj * obj = new AnchorObj;
            //     obj->fromVariantMap(vm_obj);
            //     list_obj.push_back(obj);
            // }
        }
    };
    struct AnchorPoint : public AnchorObj {
        bool isLeft;
    };
    struct FlowText : public AnchorObj {
        QString text;
        QFont font;
        uchar vAlignMode;
        double vAlignOffset;
    };
    struct PH_Rect : public AnchorObj {
        double width;
    };
    struct PH_Right : public AnchorObj { };

    SoupleSerializer();
public:
    static SoupleSerializer* sps;
    //QList<AnchorObj
    //QVector<
};

#endif // SOUPLESERIALIZER_H
