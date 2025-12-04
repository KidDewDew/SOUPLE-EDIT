import QtQuick
import QtQuick.Controls.Material
import com.custom 1.0
//水平标线选择器
Column {
    property int data_id
    property int hline_property
    property int hline_up_property
    property string hline_name: tf_logicLastLine.text
    property bool sync:true

    function setHLineName(name) {
        tf_logicLastLine.text = name
    }

    TextField {
        id: tf_logicLastLine
        leftPadding: 5; rightPadding: 5
        topPadding: 5; bottomPadding: 5
        height: 35
        Component.onCompleted: {
            if(sync) text = SoupleManager.qmlGetData(data_id,hline_property)
        }
        onTextChanged: {
            but_cf1.visible = false
            if(text == "") { but_zbx3.text = "无效"; return }
            if(sync) {
                if(text == SoupleManager.qmlGetData(data_id,hline_property)) {
                    but_zbx3.text = "有效"
                    return
                }
            }
            const r = SoupleManager.checkHLineValid(tf_logicLastLine.text)
            if(r) { but_zbx3.text = "无效"; return }
            but_zbx3.text = "有效"
            but_cf1.visible = true
        }
        RectButton {
            id: but_zbx3
            text: "有效"
            width: 25
            accent: text == "无效" ? "red" : "green"
            Behavior on accent { ColorAnimation { duration: 600 } }
            label.font.pixelSize: 10
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 10
        }
    }
    Row {
        RectButton {
            id: but_cf1
            visible: false
            text: "确定"
            accent: "green"
            width: 40
            height: 22
            onClicked: {
                if(sync) {
                    const r = SoupleManager.sendCommandToData(data_id,hline_up_property,tf_logicLastLine.text)
                    but_zbx3.text = r ? "无效" : "有效"
                }
                but_cf1.visible = false
            }
        }
        RectButton {
            id: but_cf2
            visible: but_cf1.visible
            text: "取消"
            accent: "red"
            width: 40
            height: 22
            onClicked: {
                but_cf1.visible = false
                if(sync) tf_logicLastLine.text = SoupleManager.qmlGetData(data_id,hline_property)
            }
        }
    }
}
