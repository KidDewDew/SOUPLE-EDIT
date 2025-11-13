import QtQuick
import QtQuick.Layouts

Item {
    property string text
    //property alias label: label
    implicitHeight: label.implicitHeight
    implicitWidth: label.implicitWidth * 2.5
    Layout.leftMargin: -5
    Rectangle {
        width: 58
        height: 0.8
        color: "grey"
        anchors.verticalCenter: parent.verticalCenter
    }
    Text {
        id: label
        x: 60
        color: "grey"
        text: parent.text
        anchors.verticalCenter: parent.verticalCenter
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
