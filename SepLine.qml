import QtQuick

// 分栏分隔线 qml对象
Rectangle {
    id: obj
    property int data_id
    height: 1
    color: "#2334AD"
    Triangle {
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.left
        width: 9
        height: 12
        fillColor: theme.accent_dark
        dir: Triangle.Dir.Right
    }
}
