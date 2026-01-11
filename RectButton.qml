import QtQuick
import QtQuick.Controls

AbstractButton {
    id: but
    width: 16
    height: 16
    scale: down ? 0.8 : 1.0
    property color accent: "#3C8761"
    property alias label: label
    property alias bg: bg_rect
    background: Rectangle {
        id: bg_rect
        radius: 4
        color: but.hovered ? Qt.darker(accent,1.5):accent
    }
    Text {
        id: label
        anchors.centerIn: parent
        text: but.text
        color: "white"
        font.bold: true
        font.pixelSize: 15
    }
}
