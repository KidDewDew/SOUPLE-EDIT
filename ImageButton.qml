import QtQuick

Rectangle {
    id: button
    signal clicked()
    width: img.implicitWidth
    height: img.implicitHeight
    radius: 5
    color: marea.pressed ? "#9C9C9C" : (marea.containsMouse ? "#D9D9D9" : "transparent")
    property string source
    property string text
    property alias image: img
    property bool down: marea.pressed
    property bool hovered: marea.containsMouse
    property real padding: 4
    Image {
        id: img
        anchors.fill: parent
        anchors.margins: button.padding
        source: button.source
        scale: marea.pressed ? 0.95 : 1.0
        fillMode: Image.PreserveAspectFit
    }

    MouseArea {
        id: marea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            button.clicked()
        }
        onEntered: {
            //button.focus = true
            //selectedObj = button
        }
    }
}
