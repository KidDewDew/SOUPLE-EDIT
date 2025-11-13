import QtQuick

Rectangle {
    implicitWidth: 30
    implicitHeight: 20
    property Component contentItem
    property color background_color: "#808F8F8F"
    property bool hovered: marea.containsMouse
    property bool down: marea.pressed
    signal clicked()
    signal pressed()
    color: marea.pressed ? Qt.lighter(background_color,1.4) :
                           (marea.containsMouse ? background_color : "#00000000")
    MouseArea {
        id: marea
        hoverEnabled: true
        anchors.fill: parent
        onClicked: parent.clicked()
        onPressed: parent.pressed()
    }
    Loader {
        anchors.centerIn: parent
        sourceComponent: contentItem
    }
}
