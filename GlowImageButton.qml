import QtQuick
import QtQuick.Layouts
import QtQuick.Effects

Rectangle {
    id: root
    property string text
    property string img_src
    signal clicked
    Layout.fillHeight: true
    implicitWidth: Math.max(img.paintedWidth,text.width)+8
    color: "#00000000"
    border.color: marea.containsMouse ? "#FFAF33" : "#00000000"
    Image {
        id: img
        scale: marea.pressed ? 0.9 : 1.0
        anchors.horizontalCenter: parent.horizontalCenter
        height: parent.height - text.height
        fillMode: Image.PreserveAspectFit
        source: root.img_src
    }
    Text {
        id: text
        scale: marea.pressed ? 0.9 : 1.0
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        text: root.text
        color: theme.fg
    }
    Rectangle {
        id: src
        anchors.fill: parent
        border.width: 5
        border.color: "#60FFAF33"
        visible: false
        color: "#00000000"
    }
    MultiEffect {
        source: src
        anchors.fill: src
        brightness: 0.8
        saturation: 0.6
        blurEnabled: true
        blurMax: 12
        blur: 0.4
        visible: marea.pressed
    }
    MouseArea {
        id: marea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            root.clicked()
        }
    }
}
