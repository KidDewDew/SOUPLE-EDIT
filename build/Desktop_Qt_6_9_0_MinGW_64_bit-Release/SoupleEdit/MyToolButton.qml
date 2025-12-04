import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
//颜色选择器

AbstractButton {
    id: control
    implicitWidth: r1.implicitWidth + 8
    implicitHeight: r1.implicitHeight + 8
    scale: down ? 0.9 : 1.0
    property color color: "black"
    background: Rectangle {
        color: control.hovered ? "#B8B8B8" : "transparent"
        radius: 4
    }
    Text {
        id: r1
        text: control.text
        font: control.font
        color: control.color
        anchors.verticalCenter: parent.verticalCenter
    }
    Text {
        text: "◢"
        color: "#383838"
        //anchors.verticalCenter: parent.verticalCenter
        anchors.bottom: parent.bottom
        width: 8
        anchors.right: parent.right
    }
}
