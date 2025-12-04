import QtQuick
import QtQuick.Layouts

// 可以折叠/展开的LineText
Item {
    id: root
    property string text
    property bool expanded: false
    //property alias label: label
    implicitHeight: t1.implicitHeight + (expanded ? 0 : 25)
    implicitWidth: label.implicitWidth * 2.5
    Layout.leftMargin: -5
    Row {
        y: t1.height + 18
        visible: root.expanded == false
        spacing: 10
        x: 60 + t1.implicitWidth
        Rectangle {
            color: "grey"
            width: 6
            height: 6
            radius: 3
        }
        Rectangle {
            color: "grey"
            width: 6
            height: 6
            radius: 3
        }
        Rectangle {
            color: "grey"
            width: 6
            height: 6
            radius: 3
        }
    }
    Rectangle {
        width: 58
        height: 0.8
        color: "grey"
        anchors.verticalCenter: parent.verticalCenter
    }
    MouseArea {
        id: label
        hoverEnabled: true
        x: 60
        anchors.verticalCenter: parent.verticalCenter
        width: t1.width + 16
        height: t1.height
        onClicked: {
            root.expanded ^= 1
        }
        Text {
            id: t1
            x: 4
            color: "grey"
            text: root.text
            anchors.verticalCenter: parent.verticalCenter
        }
        Triangle {
            dir: root.expanded ? Triangle.Dir.Down : Triangle.Dir.Right
            width: 8
            height: 8
            x: t1.width+6
            fillColor: "grey"
            anchors.verticalCenter: parent.verticalCenter
        }
        Rectangle {
            visible: parent.containsMouse
            border.color: "grey"
            anchors.fill: parent
            color: parent.pressed ? "grey" : "#00000000"
        }
    }
    Rectangle {
        anchors.left: label.right
        anchors.right: parent.right
        anchors.leftMargin: 3
        height: 0.8
        color: "grey"
        anchors.verticalCenter: parent.verticalCenter
    }
}
