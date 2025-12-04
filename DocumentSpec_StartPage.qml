import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

// 欢迎页界面
Flickable {
    id: flick
    clip: true
    //contentHeight: content.height
    Binding on contentWidth {
        when: content.width > flick.width
        value: content.width
    }
    Binding on contentHeight {
        when: content.height > flick.height
        value: content.height
    }
    ScrollBar.horizontal: ScrollBar {}
    ScrollBar.vertical: ScrollBar {
        policy: ScrollBar.AlwaysOn
        visible: contentHeight > flick.height
    }

    SequentialAnimation {
        id: start_ani
        ParallelAnimation {
            OpacityAnimator {
                target: row_title
                from: 0.0; to: 1.0
                duration: 800
                easing.type: Easing.InOutCubic
            }
            PropertyAnimation {
                target: rect_leftTitle
                property: "anchors.rightMargin"
                from: row_title.x; to: -130
                duration: 900
                easing.type: Easing.InOutCubic
            }
            PropertyAnimation {
                target: rect_rightTitle
                property: "anchors.leftMargin"
                from: content.width - row_title.x - row_title.width; to: 0
                duration: 900
                easing.type: Easing.InOutCubic
            }
        }
    }

    Component.onCompleted: {
        start_ani.start()
    }

    Rectangle {
        id: content
        anchors.horizontalCenter:
            width < parent.width ? parent.horizontalCenter : undefined
        anchors.verticalCenter: height < parent.height ? parent.verticalCenter : undefined
        width: 750
        height: 360
        color: theme.page_bg
        Rectangle {
            id: rect_leftTitle
            anchors.left: parent.left
            anchors.right: row_title.left
            anchors.rightMargin: -130
            anchors.bottom: row_title.bottom
            anchors.bottomMargin: 8
            height: 6
            color: "#402A0C"
        }
        Rectangle {
            id: rect_rightTitle
            anchors.left: row_title.right
            anchors.right: parent.right
            anchors.bottom: row_title.bottom
            anchors.bottomMargin: 8
            height: 6
            color: "#402A0C"
        }
        RowLayout {
            id: row_title
            opacity: 0.0
            y: 20
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 30
            Image {
                id: image
                source: "qrc:/image/嗖谷熊_big.png"
                sourceSize.width: 100
                sourceSize.height: 130
                Layout.alignment: Qt.AlignBottom
                Layout.bottomMargin: 10
                MouseArea {
                    anchors.fill: parent
                    drag.target: image
                    onReleased: {
                        image.Layout.bottomMargin = 0
                        image.Layout.bottomMargin = 10
                    }
                }
            }
            Rectangle {
                implicitWidth: 20
                implicitHeight: 60
                color: "#402A0C"
                Layout.alignment: Qt.AlignBottom
                Layout.bottomMargin: 8
            }
            Column {
                Layout.alignment: Qt.AlignBottom
                Layout.bottomMargin: 8
                spacing: 15
                Text {
                    text: "欢迎使用"
                    font.pixelSize: 34
                    color: "#402A0C"
                    font.italic: true
                    textFormat: Text.RichText
                }
                Text {
                    text: "嗖谷文档编辑软件"
                    font.pixelSize: 28
                    font.bold: true
                    color: "#402A0C"
                }

            }
            Rectangle {
                implicitWidth: 10
                implicitHeight: 60
                color: "#402A0C"
                Layout.alignment: Qt.AlignBottom
                Layout.bottomMargin: 8
            }
        }
        RectButton {
            id: button_newdoc
            anchors.top: row_title.bottom
            anchors.topMargin: 30
            anchors.right: row_title.right
            anchors.rightMargin: 25
            font.pixelSize: 18
            text: "新建文档"
            width: 200
            height: 40
            accent: "#574009"
            scale: down ? 0.9 : 1.0
        }
        RectButton {
            id: button_opendoc
            anchors.top: button_newdoc.bottom
            anchors.topMargin: 25
            anchors.right: row_title.right
            anchors.rightMargin: 25
            font.pixelSize: 18
            text: "打开文档"
            accent: "#574009"
            width: 200
            height: 40
            scale: down ? 0.9 : 1.0
        }
    }
}
