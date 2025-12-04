import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import com.custom 1.0

// 富文本框
Rectangle {
    id: obj
    property int data_id
    property alias cp_propertyBar: cp_propertyBar
    property bool iscont: false
    property bool drawBorder: true
    property bool hasBg: false
    property bool allowEditBound: true

    border.color: iscont ? "#378BD6" : (marea.containsMouse ? "#3FD1A2" :
                                    (drawBorder ? "#000000" : "transparent"))
    color: "transparent"

    onIscontChanged: {
        SoupleManager.sendCommandToData(data_id,Helper_Type.CONT_CHANGED,iscont)
    }

    onFocusChanged: {
        if(focus) {
            var old = contObj                //保证，在原来的contObj收到iscont变化前，修改contObj
            contObj = selectedObj = obj
            iscont = true
            if(old) old.iscont = false
            if(allowEditBound) {
                eb_bound.visible = true
                eb_bound.sync_obj = obj
            }
        }
    }

    MouseArea {
        id: marea
        anchors.fill: parent
        anchors.margins: 2
        hoverEnabled: true
        // onPressed: function(m) {
        //     //只接受落在边框上的鼠标事件
        //     if(m.x <= 4 || m.x >= width-4 || m.y <= 4 || m.y >= height-4)
        //         return
        //     m.accepted = false
        // }
        onClicked: {
            obj.focus = true
        }
    }

    //signal contentHeightChanged
    function insertImage() {

    }

    Component {
        id: cp_propertyBar //属性栏
        GridLayout {
            columns: 2
            columnSpacing: parent.width * 0.08
            Text { text: "元素类别"; font.pixelSize: 16 }

            Text {
                text: "文档块(自由布局)";
                font.pixelSize: 16
            }

            Expandable_LineText {
                id: el1
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "定位"
            }

            FreeObjPositionPropertyGrid {
                visible: el1.expanded
                aimObj: obj
                data_id: obj.data_id
                Layout.fillWidth: true
                Layout.columnSpan: 2
            }

            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "装饰"
            }

            Button {
                text: obj.hasBg ? "修改背景对象":"创建背景对象"
                Layout.columnSpan: 2
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                    if(! obj.hasBg) {
                        SoupleManager.sendCommandToData(data_id,Helper_Type.CREATE,"bg")
                        obj.hasBg = true
                    } else {
                        SoupleManager.sendCommandToData(data_id,Helper_Type.SET_FOCUS,"bg")
                    }
                }
            }

            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "插入"
            }
            AddToolButton {
                text: "水平线"
                onClicked: {
                    SoupleManager.sendCommandToData(data_id,Helper_Type.ADDLINE,null)
                }
            }
        }
    }

}
