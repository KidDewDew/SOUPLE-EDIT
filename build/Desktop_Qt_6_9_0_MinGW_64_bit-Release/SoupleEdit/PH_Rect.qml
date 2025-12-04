import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import com.custom 1.0

//占位矩形，用于进行间距设置
Rectangle {
    id: obj
    width: 30
    height: 16

    property bool selected: false

    property real sw: 0.0

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
    // function fromVM(vm) {  //反序列化
    //     width = vm.width
    // }

    property alias cp_propertyBar: cp_propertyBar

    onFocusChanged: {
        if(focus) selectedObj = obj
    }

    border.color: "transparent"
    border.width: 0.8
    color: selected ? "#161F80" : "transparent"

    Component {
        id: cp_propertyBar //属性栏
        GridLayout {
            columns: 2
            columnSpacing: parent.width * 0.18
            Text { text: "元素类别"; font.pixelSize: 16 }
            RowLayout {
                Text {
                    text: "占位矩形";
                    font.pixelSize: 16
                }
                RectButton {
                    Layout.alignment: Qt.AlignVCenter
                    text: "?"
                    accent: "#C9A350"
                    ToolTip {
                        visible: parent.hovered
                        text: "什么是占位矩形?"
                    }
                }
            }
            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "基本信息"
            }
            Text { text: "占位宽度"; font.pixelSize: 16 }
            MyDoubleSpinBox {
                id: spin_1
                step: 0.01
                precision: 2
                initialValue: Helper.pixel2cm(obj.width)
                Binding on value {
                    when: obj.widthChanged
                    value: Helper.pixel2cm(obj.width)
                }
                onValueChanged: {
                    obj.width = Helper.cm2pixel(value)
                    SoupleManager.sendCommandToData(data_id,Helper_Type.WIDTH_UP,obj.width)
                }
            }
        }
    }



    // Text {
    //     visible: isSelected
    //     anchors.centerIn: parent
    //     text: "占位"; font.pixelSize: 10
    //     color: "#DB5395"
    // }

    // Canvas {
    //     //绘制双向箭头
    //     id: canvas_arrows
    //     anchors.fill: parent
    //     onPaint: {
    //         var ctx = getContext("2d")
    //         ctx.beginPath()
    //         ctx.strokeStyle = "#707070"
    //         ctx.lineWidth = 1.0
    //         ctx.rect(0,0,width,height)
    //         ctx.setLineDash([4,4])
    //         ctx.stroke()
    //     }
    // }


    Menu {
        id: menu
        MenuItem {
            text: "删除"
            onTriggered: {
                SoupleManager.requestDeleteObj(obj.data_id)
                parent.focus = false
            }
        }
    }


    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onPressed: {
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

    Triangle { //右调整浮标
        id: t_right
        dir: Triangle.Dir.Down
        x: parent.width-width/2
        anchors.bottom: parent.top
        fillColor: "#DB3F41"
        width: 12
        height: 8
        visible: parent.focus
        onXChanged: {
            if(x+width/2 != parent.width) {
                SoupleManager.sendCommandToData(data_id,Helper_Type.WIDTH_UP,(x+width/2))
                horRuler.float_x = obj.x + x + width/2
            }
        }
        MouseArea {
            anchors.fill: parent
            drag.target: parent
            drag.axis: Drag.XAxis
            drag.minimumX: 2
            //drag.maximumX:
            onPressed: {
                horRuler.show_float = true
                drag.maximumX = SoupleManager.qmlGetData(data_id,Helper_Type.MAX_DRAG_X)
            }
            onReleased: horRuler.show_float = false
        }
    }

    Rectangle {
        anchors.left: parent.right
        width: sw
        height: parent.height
        visible: sw > 0.0
        color: "#DBDBDB"
    }

    function dropRight(dropWidth) { //前进
        if(dropWidth < width) return null
        obj.removeSelf(false)
        return obj
    }
    function dropLeft(dropWidth) { //回缩
        if(dropWidth < width) return null
        obj.removeSelf(false)
        return obj
    }

    function getCursorFromLeft() { //直接转移cursor
        rightObj.getCursorFromLeft()
    }

    function getCursorFromRight() {//直接转移cursor
        leftObj.getCursorFromRight()
    }
}
