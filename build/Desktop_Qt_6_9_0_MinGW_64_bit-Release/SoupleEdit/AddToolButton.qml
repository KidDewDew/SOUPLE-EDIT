import QtQuick
import QtQuick.Controls.Material

//表达“添加”含义的工具按钮
Item {
    id: control
    property string text
    property bool enableList: false
    property list<string> listText
    property alias label: label
    implicitWidth: label.implicitWidth + but.width + 1
    implicitHeight: label.implicitHeight + 2
    signal clicked()

    signal selected(int index,string str)

    Component {
        id: cp_menuItem
        MenuItem {
            property int index
            onTriggered: control.selected(index,text)
        }
    }

    onClicked: {
        if(enableList) {
            //创建列表菜单
            const menu = cp_menu.createObject(Overlay.overlay)
            var index = 0
            for(const text of listText) {
                const mitem = cp_menuItem.createObject(null,{index:index,text:text})
                menu.addItem(mitem)
                ++index
            }
            const p = control.mapToItem(Overlay.overlay,0,height)
            menu.x = p.x
            menu.y = p.y
            menu.open()
            menu.closed.connect(function() { menu.destroy() })
        }
    }

    Text {
        id: label
        text: control.text
        color: theme.fg
    }
    AbstractButton {
        id: but
        anchors.bottom: label.bottom
        anchors.left: label.right
        anchors.leftMargin: 1
        width: 16
        height: 16
        scale: down ? 0.8 : 1.0
        onClicked: control.clicked()
        background: Rectangle {
            radius: 4
            color: but.hovered ? Qt.darker(theme.accent_light,1.3):theme.accent_light
        }
        Text {
            anchors.centerIn: parent
            text: enableList ? "▾" : "+"
            color: "white"
            font.bold: true
            font.pixelSize: 15
        }
    }
    Rectangle {
        width: but.hovered ? parent.width : 10
        height: 2
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        color: theme.accent_light
        //opacity: but.hovered ? 1.0 : 0.0
        Behavior on width { NumberAnimation { duration: 500;
                easing.type: Easing.InOutQuad } }
    }
}
