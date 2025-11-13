import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import com.custom 1.0

//胶着占位矩形 为了突显它和PH_Rect有些不一样，另外做一个ui
Canvas {
    id: obj
    property bool isLeft  //是左胶着还是右胶着
    property bool selected: false

    property real old_z

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
        if(focus) {
            selectedObj = obj
            old_z = z
            z = Helper_Type.Top
        } else {
            z = old_z
        }
    }

    Keys.onPressed: function(e) {
        SoupleManager.sendCommandToData(data_id,Helper_Type.KEY_PRESSED,e.key)
    }

    Component {
        id: cp_propertyBar //属性栏
        GridLayout {
            columns: 2
            columnSpacing: parent.width * 0.18
            Text { text: "元素类别"; font.pixelSize: 16 }
            RowLayout {
                Text {
                    text: `${isLeft ? '左':'右'}占位`;
                    font.pixelSize: 16
                }
                RectButton {
                    Layout.alignment: Qt.AlignVCenter
                    text: "?"
                    accent: "#C9A350"
                    ToolTip {
                        visible: parent.hovered
                        text: "什么是左/右占位?"
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
        fillColor: "#138EF2"
        width: 16
        height: 12
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
                //drag.maximumX = SoupleManager.qmlGetData(data_id,Helper_Type.MAX_DRAG_X)
            }
            onReleased: horRuler.show_float = false
        }
    }


    onIsLeftChanged: requestPaint()
    onPaint: {
        var ctx = getContext('2d')
        ctx.clearRect(0,0,width,height)
        ctx.beginPath()
        ctx.moveTo(2,height/2)
        ctx.lineTo(width-2,height/2)
        ctx.lineWidth = 1
        ctx.setLineDash([4,4])
        ctx.strokeStyle = "#919191"

        ctx.stroke()

        ctx.beginPath()
        const aw = Math.min(6,Math.max(width*1/3,12))
        if(isLeft) {
            ctx.moveTo(width-2,height/2)
            ctx.lineTo(width-aw,height*0.25)
            ctx.moveTo(width-2,height/2)
            ctx.lineTo(width-aw,height*0.75)
            ctx.moveTo(2,3)
            ctx.lineTo(2,height-3)
        } else {
            ctx.moveTo(2,height/2)
            ctx.lineTo(aw,height*0.25)
            ctx.moveTo(2,height/2)
            ctx.lineTo(aw,height*0.75)
            ctx.moveTo(width-2,3)
            ctx.lineTo(width-2,height-3)
        }
        ctx.setLineDash([])
        ctx.stroke()
    }
}
