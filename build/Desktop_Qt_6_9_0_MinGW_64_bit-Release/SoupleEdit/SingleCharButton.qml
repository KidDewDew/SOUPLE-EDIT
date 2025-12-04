import QtQuick

//单字符按钮
Text {
    id: item
    signal clicked()
    property color hover_color: Qt.darker(ori_color,1.5)
    property color ori_color: "#CC5358"
    color: ma.containsMouse ? hover_color : ori_color
    scale: ma.down ? 0.7 : 1.0
    font.pixelSize: 14
    MouseArea {
        id: ma
        hoverEnabled: true
        anchors.fill: parent
        onClicked: {
            item.clicked()
        }
    }
}
