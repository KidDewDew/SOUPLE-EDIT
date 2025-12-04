import QtQuick
import QtQuick.Controls

//实现可以输入小数的spinBox，并包含单位
Item {
    id: control
    property string suffix: "cm"
    property string text: tf.text
    property real initialValue: 0.0
    property alias textFiled: tf
    property real value
    property bool enabled: true //是否启用
    // Binding on value {
    //     when: tf.editingFinished
    //     value: parseFloat(tf.text)
    // }
    property real step: 0.01
    property real precision: 2
    property real from: 0.0
    property real to: 2000.0
    implicitWidth: but_add.x+but_add.implicitWidth+6
    implicitHeight: tf.implicitHeight

    Component.onCompleted: value = initialValue

    // onValueChanged: {
    //     tf.text = `${value}`
    //     //value = Qt.binding(function() { return parseFloat(tf.text) })
    // }
    AbstractButton {
        id: but_sub
        anchors.verticalCenter: parent.verticalCenter
        width: 16
        height: 16
        scale: down ? 0.8 : 1.0
        enabled: parent.enabled
        background: Rectangle {
            radius: 4
            color: but_sub.hovered ? "#8F424E":"#8F1116"
        }
        onClicked: {
            if(value - step < from) return
            value = value - step
        }

        Text {
            anchors.centerIn: parent
            text: "-"
            color: "white"
            font.bold: true
            font.pixelSize: 15
        }
    }
    TextField {
        id: tf
        horizontalAlignment: Text.AlignHCenter
        anchors.left: but_sub.right
        anchors.leftMargin: 2
        anchors.verticalCenter: parent.verticalCenter
        leftPadding: 5; rightPadding: 5
        enabled: parent.enabled
        height: font.pixelSize * 2.1
        width: 65
        validator: DoubleValidator {}
        property string old_text
        Binding on text {
            when: control.valueChanged
            value: `${control.value.toFixed(precision)}`
        }
        onEditingFinished: {
            value = parseFloat(text)
        }
        onFocusChanged: {
            if(focus) {
                old_text = text //记录原来文字
            }
        }
    }
    RectButton {
        id: but_cancel
        visible: tf.focus
        text: "还原"
        accent: "#3644BA"
        anchors.top: tf.bottom
        width: 36
        font.pixelSize: 12
        x: parent.width * 0.1
        onPressed: {
            //tf.text = tf.old_text
            value = parseFloat(tf.old_text)
        }
    }
    RectButton {
        text: "确认"
        visible: tf.focus
        accent: "#25BA60"
        width: 36
        font.pixelSize: 12
        anchors.top: tf.bottom
        anchors.left: but_cancel.right
        anchors.leftMargin: 4
    }

    Text {
        id: text_suf
        text: suffix
        font.pixelSize: 14
        anchors.leftMargin: 2
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: tf.right
        color: theme.fg
    }
    AbstractButton {
        id: but_add
        anchors.verticalCenter: parent.verticalCenter
        x: text_suf.x+Math.max(22,text_suf.width)+2
        enabled: parent.enabled
        width: 16
        height: 16
        scale: down ? 0.8 : 1.0
        background: Rectangle {
            radius: 4
            color: but_add.hovered ? "#31B866":"#268F4F"
        }
        onClicked: {
            if(value + step > to) return
            value = value + step
        }

        Text {
            anchors.centerIn: parent
            text: "+"
            color: "white"
            font.bold: true
            font.pixelSize: 15
        }
    }
}
