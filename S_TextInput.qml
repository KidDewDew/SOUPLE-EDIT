import QtQuick
import QtQuick.Controls

//这是本软件使用的自定义文本输入控件，用来支持软件连续选择的功能

TextInput {
    id: control
    property int data_id

    function selectw() {
        control.selectAll()
    }

    function selectp(x1,x2) {
        control.select(positionAt(x1-x),positionAt(x2-x))
    }

    function selectcl() {
        control.select(0,0)
    }

    function selecti(i1,i2) {
        control.select(i1,i2+1)
    }

    Component.onCompleted: {
        //安装事件过滤器
        //SoupleManager.efST(this)
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.IBeamCursor
        onPressed: function(mouse){
            let p = control.mapToItem(soupleEdit,mouse.x,mouse.y)
            SelectMgr.beginSelect(data_id,p.x,p.y)
            mouse.accepted = false
        }
    }

}
