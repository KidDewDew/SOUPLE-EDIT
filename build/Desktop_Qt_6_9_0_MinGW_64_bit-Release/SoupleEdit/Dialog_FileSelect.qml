import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
Dialog {
    id: dialog
    width: 260+leftPadding+rightPadding
    height: 315
    bottomPadding: 20
    anchors.centerIn: Overlay.overlay
    property var nameFilters: ["all(*.*)"]
    property string currentFile: tf.text
    background: Rectangle {
        color: theme.bg
        radius: 5
    }
    title: "文件选择对话框"
    //closePolicy: Popup.NoAutoClose
    onClosed: {
        console.log("Dialog_FileSelect Destroyed.")
        dialog.destroy()
    }
    DropArea {
        id: dropArea
        width: 260
        height: 120
        onDropped: function(drop) {
            if(drop.urls.length > 0) {
                tf.text = Helper.qurl2localfile(drop.urls[0])
            }
        }
    }
    Rectangle {
        anchors.fill: dropArea
        color: theme.bar
        border.width: dropArea.containsDrag ? 2.0 : 1.0
        border.color: theme.accent_dark
        radius: 5
        Text {
            color: "gray"
            anchors.centerIn: parent
            text: "在这里拖入文件..."
        }
    }
    RowLayout {
        anchors.top:dropArea.bottom
        anchors.topMargin:8
        width: 260
        TText {
            text: "路径"
        }
        TTextField {
            id: tf
            Layout.fillWidth: true
            font.pointSize: 9
        }
        FlatButton {
            text: "Select"
            foldV: 0.3
            padding_vertical: 10
            accent: "#7F807F"
            onClicked: {
                file_dialog.nameFilters = dialog.nameFilters
                file_dialog.open()
                file_dialog.callback = function() {
                    tf.text = Helper.qurl2localfile(file_dialog.currentFile)
                }
            }
        }
    }
    Row {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        anchors.right: parent.right
        spacing: 6
        FlatButton {
            width: 60
            foldV: 0.3
            text: "取消"
            accent: "#B0424C"
            padding_vertical: 10
            onClicked: dialog.reject()
        }
        FlatButton {
            width: 130
            foldV: 0.3
            text: "确定(confirm)"
            padding_vertical: 10
            onClicked: {
                dialog.accept()
            }
        }
    }
}
