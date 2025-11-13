import QtQuick
import QtQuick.Controls
//ObjContainer
//可插入对象的排列容器
ScrollView {
    id: control
    onWidthChanged: {
        flow.width = 0
        flow.width = width + 50
    }
    Flow {
        id: flow
        width: parent.width + 50
        Repeater {
            id: repeater
            property bool _UPV
            model:
                [{type:"VerLine",img:"qrc:/image/img_VerLine.png"},
                 {type:"HorLine",img:"qrc:/image/img_HorLine.png"},
                 {type:"TextEdit",img:"qrc:/image/img_TextEdit.png"},
                 {type:"PlaceHolder",img:"qrc:/image/img_占位符.png"},
                 {type:"FlowRect",img:"qrc:/image/img_FlowRect.png"},
                 {type:"Image",img:"qrc:/image/img_Image.png"},
                 {type:"Rect",img:"qrc:/image/img_Rect.png"},
                 {type:"Circle",img:"qrc:/image/img_Circle.png"},
                 {type:"Triangle",img:"qrc:/image/img_Triangle.png"}]
            delegate: ItemDelegate {
                contentItem: Image {
                    source: modelData.img
                    width: 30
                    fillMode: Image.PreserveAspectFit
                }
            }
        }
    }
}
