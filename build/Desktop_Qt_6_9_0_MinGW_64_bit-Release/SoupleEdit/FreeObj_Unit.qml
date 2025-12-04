import QtQuick
import QtQuick.Controls.Windows
import QtQuick.Layouts
import com.custom 1.0

// 富文本框
Item {
    id: obj
    property int data_id
    property alias cp_propertyBar: cp_propertyBar
    property bool iscont: false

    property bool drawRightLine: false
    property bool drawBottomLine: false

    property color borderColor: iscont ? "#378BD6" : (marea.containsMouse ? "#3FD1A2" : "#000000")

    //z: iscont ? Helper_Type.Top : Helper_Type.Text
    Rectangle { //上边线
        color: borderColor
        width: parent.width
        height: 0.8
    }

    Rectangle { //左边线
        color: borderColor
        height: parent.height
        width: 0.8
    }

    Rectangle { //右边线
        visible: drawRightLine || marea.containsMouse || iscont
        color: borderColor
        anchors.right: parent.right
        height: parent.height
        width: 0.8
    }

    Rectangle { //下边线
        visible: drawBottomLine || marea.containsMouse || iscont
        color: borderColor
        width: parent.width
        height: 0.8
        anchors.bottom: parent.bottom
    }

    Component {
        id: cp_grid_tools
        RowLayout {
            ImageButton {
                source: "qrc:/image/image_表格属性.png"
                Layout.preferredHeight: parent.height
                onClicked: {
                    menu_1.popup()
                }
            }
            ImageButton {
                source: "qrc:/image/image_设置表头.png"
                Layout.preferredHeight: parent.height
                onClicked: {
                    menu_1.popup()
                }
            }
            Rectangle {
                implicitWidth: 0.8
                color: "black"
                Layout.preferredHeight: parent.height - 10
                Layout.alignment: Qt.AlignVCenter
                Layout.leftMargin: 10
                Layout.rightMargin: 10
            }
            ImageButton {
                source: "qrc:/image/image_添加新行.png"
                Layout.preferredHeight: parent.height
                onClicked: {
                    menu_1.popup()
                }
            }
            ImageButton {
                source: "qrc:/image/image_添加新列.png"
                Layout.preferredHeight: parent.height
                onClicked: {
                    menu_2.popup()
                }
            }
            ImageButton {
                source: "qrc:/image/image_单元格合并.png"
                Layout.preferredHeight: parent.height
                onClicked: {
                    menu_3.popup()
                }
            }
            ImageButton {
                source: "qrc:/image/image_单元格拆分.png"
                Layout.preferredHeight: parent.height
                onClicked: {
                    menu_4.popup()
                }
            }
            ImageButton {
                source: "qrc:/image/image_删除行.png"
                Layout.preferredHeight: parent.height
                onClicked: {
                    menu_5.popup()
                }
            }
            ImageButton {
                source: "qrc:/image/image_删除列.png"
                Layout.preferredHeight: parent.height
                onClicked: {
                    menu_6.popup()
                }
            }
            Rectangle {
                implicitWidth: 0.8
                color: "black"
                Layout.preferredHeight: parent.height - 10
                Layout.alignment: Qt.AlignVCenter
                Layout.leftMargin: 10
                Layout.rightMargin: 10
            }
            Text {
                text: "表格对齐:"
                font.pixelSize: 12
            }
            ComboBox {
                Layout.preferredHeight: 30
                model: ["左对齐","居中对齐","右对齐"]
                property bool readed: false
                Component.onCompleted: {
                    var m = SoupleManager.qmlGetData(data_id,Helper_Type.TABLE_ALIGNMODE)
                    //console.log("表格对齐：",m)
                    if(m == Helper_Type.AlignLeft) currentIndex = 0
                    else if(m == Helper_Type.AlignHCenter) currentIndex = 1
                    else currentIndex = 2
                    readed = true
                }
                onCurrentIndexChanged: {
                    if(! readed) return;
                    var m
                    if(currentIndex == 0) m = Helper_Type.AlignLeft
                    else if(currentIndex == 1) m = Helper_Type.AlignHCenter
                    else m = Helper_Type.AlignRight
                    SoupleManager.sendCommandToData(data_id,Helper_Type.TABLE_ALIGNMODE_UP,m)
                }
            }
        }
    }

    onIscontChanged: {
        SoupleManager.sendCommandToData(data_id,Helper_Type.CONT_CHANGED,iscont)
        if(iscont) {
            rightToolBar.addTab("表格工具",cp_grid_tools)
            rightToolBar.setTab("表格工具")
        } else {
            if( ! (contObj instanceof FreeObj_Unit)) {
                rightToolBar.removeTab("表格工具")
            }
        }
    }

    onFocusChanged: {
        if(focus) {
            var old = contObj                //保证，在原来的contObj收到iscont变化前，修改contObj
            contObj = selectedObj = obj
            iscont = true
            if(old) old.iscont = false
        }
    }

    MouseArea {
        id: marea
        anchors.fill: parent
        anchors.margins: 2
        hoverEnabled: true
        onClicked: {
            obj.focus = true
        }
    }


    CMenu {
        id: menu_1
        width: 230
        accent: "#783F40"
        Action {
            text: "↑ 在单元格<b>上方</b>插入新行"
        }
        Action {
            text: "↓ 在单元格<b>下方</b>插入新行"
        }
    }

    CMenu {
        id: menu_2
        width: 230
        accent: "#783F40"
        Action {
            text: "←在单元格<b>左方</b>插入新列"
        }
        Action {
            text: "→在单元格<b>右方</b>插入新列"
        }
    }

    CMenu {
        id: menu_3
        width: 180
        accent: "#783F40"
        Action {
            text: "←合并<b>左边</b>单元格"
        }
        Action {
            text: "→合并<b>右边</b>单元格"
        }
        Action {
            text: "↑ 合并<b>上边</b>单元格"
        }
        Action {
            text: "↓ 合并<b>下边</b>单元格"
        }
    }

    CMenu {
        id: menu_4
        width: 120
        accent: "#783F40"
        Action {
            text: "水平拆分"
        }
        Action {
            text: "垂直拆分"
        }
        Action {
            text: "斜拆分"
        }
    }

    CMenu {
        id: menu_5
        width: 180
        accent: "#783F40"
        Action {
            text: "删除单元格上边的行"
        }
        Action {
            text: "删除单元格下边的行"
        }
    }

    CMenu {
        id: menu_6
        width: 180
        accent: "#783F40"
        Action {
            text: "删除单元格左边的列"
        }
        Action {
            text: "删除单元格右边的列"
        }
    }

    Component {
        id: cp_propertyBar //属性栏
        GridLayout {
            columns: 2
            columnSpacing: parent.width * 0.08
            Text { text: "元素类别"; font.pixelSize: 16 }

            Text {
                text: "表格-单元格";
                font.pixelSize: 16
            }

            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "表格操作"
            }

            ImageButton {
                source: "qrc:/image/image_添加新行.png"
                onClicked: {
                    menu_1.popup()
                }
            }

            ImageButton {
                source: "qrc:/image/image_添加新列.png"
                onClicked: {
                    menu_2.popup()
                }

            }

            ImageButton {
                source: "qrc:/image/image_单元格合并.png"
                onClicked: {
                    menu_3.popup()
                }
            }

            ImageButton {
                source: "qrc:/image/image_单元格拆分.png"
                onClicked: {
                    menu_4.popup()
                }
            }

            ImageButton {
                source: "qrc:/image/image_删除行.png"
                onClicked: {
                    menu_5.popup()
                }
            }

            ImageButton {
                source: "qrc:/image/image_删除列.png"
                onClicked: {
                    menu_6.popup()
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
