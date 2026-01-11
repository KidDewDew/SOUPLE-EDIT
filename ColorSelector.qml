import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
//颜色选择器

AbstractButton {
    id: control
    implicitHeight: 26
    implicitWidth: 26
    property color color: "black"

    Text {
        text: "×"
        font.pixelSize: 14
        color: "#B3B3B3"; font.bold: true
        anchors.centerIn: parent
        visible: control.enabled == false
    }

    onClicked: {
        color_dialog.open()
        color_dialog.callback = function(color) { control.color = color }
    }

    background: Rectangle {
        color: control.hovered ? "#B8B8B8" : "white"
        radius: 4
    }
    Rectangle {
        id: r1
        color: control.color
        radius: 4
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width - 8
        height: width
        border.width: 0.8
        border.color: "#404040"
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
