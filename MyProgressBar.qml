import QtQuick
import QtQuick.Controls

ProgressBar {
    id: control
    value: real_value / ori_value
    padding: 2
    property color accent
    property real ori_value: 1.0
    property real real_value
    property bool show_text: true
    property alias control_text: control_text
    property alias bg: bgr
    background: Rectangle {
        id: bgr
        color: "#e6e6e6"
        radius: 5
    }

    contentItem: Item {
        implicitWidth: 200
        implicitHeight: 12
        Rectangle {
            width: control.visualPosition * parent.width
            height: parent.height
            radius: 2
            color: real_value <= ori_value ? accent : Qt.lighter(accent,real_value / ori_value)
        }
        Text {
            id: control_text
            visible: show_text
            text: `${real_value.toFixed(1)}/${ori_value.toFixed(1)}`
            anchors.centerIn: parent
            font.pixelSize: parent.height * 0.4
            color: "white"
            style: Text.Outline
        }
    }
}
