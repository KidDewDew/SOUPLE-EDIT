import QtQuick
import QtQuick.Controls

Label {
    property color bgColor: "white"
    property alias bg: bg
    background: Rectangle {
        id: bg
        radius: 5
        anchors.fill: parent
        anchors.margins: -4
        color: bgColor
    }
}
