import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material

//PH_Right
//右占位符
//功能上可以实现“换行”的效果，总是使得右边为空白
Rectangle {
    id: obj
    //width: 32

    property bool selected: false

    color: selected ? "#161F80" : (focus ? "#50BCB4D9" : "#00000000")

    //isSolid: false //非实体，不计入居中等对齐计算

    function die() {
        obj.destroy()
    }

    function selecti(i1,i2) {
        selected = true
    }

    function selectw() {
        selected = true
    }

    function selectp(x1,x2) {
        selected = false
    }

    function selectcl() {
        selected = false
    }


    property int data_id

    onFocusChanged: {
        if(focus) selectedObj = obj
    }

    // width: hLine.rightPoint.x - x + 5 //+5让右边的obj全部溢出！
    // Binding on width {
    //     when: rightObj instanceof AnchorPoint && leftObj != null
    //     value: rightObj.leftX - leftObj.rightX
    // }
    // Binding on rightX {
    //     when: leftObj != null
    //     value: leftObj.rightX
    // }
    //rightX: leftObj.rightX
    // Binding on rightX_for_shrink {
    //     when: rightObj instanceof AnchorPoint
    //     value: rightObj.leftX
    // }
    // // //rightX: leftObj.rightX //继承rightX
    // rightX_for_shrink: 1000000
    //onRightXChanged: console.log("\\n-x:",rightX)

    //onRightXChanged: console.log("ph-right.rightXchanged",rightX)
    //onRightX_for_shrinkChanged: console.log("ph-right.rightX_for_Shrinkchanged",rightX)

    property alias cp_propertyBar: cp_propertyBar
    Component {
        id: cp_propertyBar //属性栏
        GridLayout {
            id: propertyBar
            columns: 2
            columnSpacing: parent.width * 0.05
            Text { text: "元素类别"; font.pixelSize: 16 }
            Text {
                text: "右占位符(换行)";
                font.pixelSize: 16
            }
            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "基本信息"
            }
        }
    }



    // onRightObjChanged: {
    //     if(rightObj == null || protectedMode) return
    //     if(!(rightObj instanceof AnchorPoint))
    //     {  //如果右边不是anchor-point，则需要把它从hline溢出
    //         var r = hLine.rightPoint.leftObj //获取最右边对象
    //         var toAddList = []
    //         protectedMode = true
    //         while(r != null && r != obj) {
    //             var next = r.leftObj
    //             r.removeSelf(false)
    //             toAddList.push(r)
    //             r = next
    //         }
    //         while(toAddList.length > 0) {
    //             hLine.overflowObj(toAddList.shift())
    //         }
    //         protectedMode = false
    //     }
    // }



    // function whenInsertedOnRight() { //当右边被插入时
    //     //if(rightObj == null) return
    //     if(!(rightObj instanceof AnchorPoint))
    //     {  //如果右边不是anchor-point，则需要把它从hline溢出
    //         rightObj.removeSelf(false) //删除，不释放
    //         console.log("PH_Right removeRightObj()")
    //         hLine.overflowObj(rightObj) //溢出
    //     }
    // }

    // function whenInsertedOnLeft() { //当右边被插入时
    //     //if(leftObj == null) return
    //     if(leftObj instanceof )
    //     {  //如果右边不是anchor-point，则需要把它从hline溢出
    //         rightObj.removeSelf(false) //删除，不释放
    //         console.log("PH_Right removeRightObj()")
    //         hLine.overflowObj(rightObj) //溢出
    //     }
    // }

    // function insertOnRight(new_obj) { //在右侧插入
    //     new_obj.leftObj = control
    //     new_obj.rightObj = rightObj
    //     rightObj.leftObj = new_obj
    //     rightObj = new_obj
    //     new_obj.hLine = hLine
    //     tryMergeRight() //尝试合并新obj
    // }

    // function insertOnLeft(new_obj) { //在左侧插入
    //     new_obj.rightObj = control
    //     new_obj.leftObj = leftObj
    //     leftObj.rightObj = new_obj
    //     leftObj = new_obj
    //     new_obj.hLine = hLine
    //     new_obj.tryMergeRight() //尝试合并
    // }
    // onLeftObjChanged: {
    //     // //这里需要进行Obj_TextEdit的特化，为了方便用户进行换行输入
    //     // if(leftObj instanceof Obj_TextEdit) {

    //     // }
    // }

    //onHLineChanged: hLine.setSignUpdateLayout() //请求更新布局


    // function runLayoutAction() {
    //     if(rightObj == null) return
    //     if(!(rightObj instanceof AnchorPoint))
    //     {  //如果右边不是anchor-point，则需要把它从hline溢出
    //         var _rightObj = rightObj
    //         //timer_tasks.push(function() {
    //         _rightObj.removeSelf(false) //删除，不释放
    //         hLine.overflowObj(_rightObj) //溢出
    //         console.log("PH_Right removeRightObj()")
    //         //})
    //     }
    // }

    // function dropRight(dropWidth) { //当右占位需要dropRight时，只需要让自己溢出即可
    //     if(leftObj.rightX > hLine.rightPoint.x) {
    //         obj.removeSelf(false)
    //         console.log("换行-dropRight")
    //         return obj
    //     }
    //     return null
    // }
    // function dropLeft(dropWidth) { //直接回缩
    //     obj.removeSelf(false)
    //     console.log("换行-dropLeft")
    //     return obj
    // }

    // function getCursorFromLeft() { //直接转移cursor
    //     rightObj.getCursorFromLeft()
    // }

    // function getCursorFromRight() {//获取cursor
    //     selectedObj = obj
    //     focus = true //获取焦点
    // }

    Keys.onPressed: function(e) {
        if(e.key == Qt.Key_Backspace) {
            removeSelf()
        } else { //否则需要转移到左边（如果可以）
            const of_obj = cp_obj_textedit.createObject(soupleEdit,{text:e.text})
            if(leftObj instanceof Obj_TextEdit) {
                of_obj.textEdit.font = leftObj.textEdit.font
            }
            hLine.overflowObj(of_obj)
            of_obj.getCursorFromRight()
        }
    }

    // Text {
    //     text: "\\n"
    //     anchors.verticalCenter: parent.verticalCenter
    //     font.pixelSize: 12
    //     font.bold: true
    //     color: "grey"
    // }

    Image {
        id: img
        source: "qrc:/image/icon_回车.svg"
        width: 12; height: 12
        anchors.verticalCenter: parent.verticalCenter
    }

    Menu {
        id: menu
        MenuItem {
            text: "删除"
            onTriggered: {
                SoupleManager.requestDeleteObj(obj.data_id)
                //SoupleManager.sendCommandToData(data_id,"delete")
                parent.focus = false
            }
        }
    }

    MouseArea {
        anchors.fill: img
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onPressed: function(mouse){
            parent.focus = true
        }
        onClicked: function(mouse){
            if(mouse.button == Qt.RightButton) {
                menu.x = mouse.x
                menu.y = mouse.y
                menu.open()
            }
        }
    }
}
