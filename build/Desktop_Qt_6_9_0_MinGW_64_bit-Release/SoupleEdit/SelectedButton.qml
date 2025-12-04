import QtQuick
import QtQuick.Controls

AbstractButton {
    id: control
    property alias label: label
    property bool selected: false
    property color selectedColor: "#C9C9C9"
    leftPadding: 5
    rightPadding: 5
    topPadding: 3
    bottomPadding: 3
    implicitWidth: label.width + leftPadding + rightPadding
    implicitHeight: label.height + topPadding + bottomPadding
    background: Rectangle {
        radius: 5
        color: control.selected ? selectedColor :
                    (control.down ? "#C7C7C7" :"#00000000")
        scale: control.down ? 0.9 : 1.0
        border.color: control.hovered || control.selected ? "#858585" : "#00000000"
    }
    onClicked: {
        selected = selected ^ 1
    }
    Text {
        id: label
        anchors.centerIn: parent
        color: theme.fg
    }
}
