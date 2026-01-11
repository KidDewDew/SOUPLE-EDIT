import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material
import com.custom 1.0

//这是左占位符，用于“段落”占位。
Rectangle {
    id: obj
    property int data_id
    property real tab //缩进
    property bool showLevel //是否显示level
    property int level //级别
    onFocusChanged: {
        if(focus) selectedObj = obj
    }
    property alias cp_propertyBar: cp_propertyBar
    color: "white"

    border.width: 1
    border.color: "#963B3D"

    onTabChanged: {
        tab_canvas.requestPaint()
    }

    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        height: parent.height
        width: parent.width * 0.36
        color: "#963B3D"
    }

    function die() {
        obj.destroy()
    }

    function selecti(i1,i2) {
        //selected = true
    }

    function selectw() {}

    function selectp(x1,x2) {}

    function selectcl() {}

    // onWidthChanged:
    //     if(width != 16) width = 16
    // onHeightChanged:
    //     if(height != 16) height = 16
    Component {
        id: cp_propertyBar //属性栏
        GridLayout {
            id: propertyBar
            columns: 2
            columnSpacing: parent.width * 0.05
            Component.onCompleted: {
                cb_showLevel.checked = SoupleManager.qmlGetData(data_id,Helper_Type.SHOW_LEVEL)
                spinbox_level.value = Helper.pixel2cm(
                            SoupleManager.qmlGetData(data_id,Helper_Type.LEVEL))
                spinbox_firstline_tab.value = Helper.pixel2cm(
                        SoupleManager.qmlGetData(data_id,Helper_Type.FIRSTLINE_TAB))
                spinbox_etc_tab.value = Helper.pixel2cm(
                        SoupleManager.qmlGetData(data_id,Helper_Type.ETC_TAB))
            }
            Text { text: "元素类别"; font.pixelSize: 16 }
            Text {
                text: "左占位符(段落标记)";
                font.pixelSize: 16
            }
            Text {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "这是段落标记符，标识着新段落的开始。"
                color: "gray"
                wrapMode: Text.WrapAnywhere
                font.pixelSize: 14
            }
            CheckBox {
                id: cb_showLevel
                Layout.columnSpan: 2
                text: "显示(所有)段落级别"
                Component.onCompleted: checked = obj.showLevel
                Binding on checked {
                    when: obj.showLevelChanged
                    value: obj.showLevelChanged
                }
                onCheckedChanged: {
                    spMgr.sendCommandToData(data_id,HTypes.SHOW_LEVEL_UP,checked)
                    obj.showLevel = checked
                }
            }
            LineText {
                text: "段落属性"
                Layout.columnSpan: 2
            }
            TText {
                text: "首行缩进"
            }
            MyDoubleSpinBox{
                id: spinbox_firstline_tab
                step: 0.01
                suffix: "cm"
                initialValue: obj.tab
                Binding on value {
                    when: obj.tabChanged
                    value: obj.tab
                }
                onValueChanged: {
                    const new_tab = Helper.cm2pixel(value)
                    spMgr.sendCommandToData(data_id,HTypes.FIRSTLINE_TAB_UP,new_tab)
                    obj.tab = value
                }
            }
            TText {
                text: "其余行缩进"
            }
            MyDoubleSpinBox{
                id: spinbox_etc_tab
                step: 0.01
                suffix: "cm"
                onValueChanged: {
                    spMgr.sendCommandToData(data_id,HTypes.ETC_TAB_UP,Helper.cm2pixel(value))
                }
            }
            TText {
                text: "段落级别"
            }
            MyDoubleSpinBox{
                id: spinbox_level
                step: 1
                suffix: "级"
                initialValue: obj.level
                Binding on value {
                    when: obj.levelChanged
                    value: obj.level
                }
                onValueChanged: {
                    spMgr.sendCommandToData(data_id,HTypes.LEVEL_UP,value)
                    obj.level = value
                }
            }
        }
    }
    MouseArea {
        anchors.fill: parent
        onClicked: obj.focus = true
    }
    Text {
        visible: showLevel && level > 0
        text: `${level}`
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.left
        anchors.rightMargin: 3
        color: "#963B3D"
    }

    Canvas {
        id: tab_canvas
        x:parent.width
        height:3
        width: Helper.cm2pixel(parent.tab)
        y:parent.height*0.5
        onPaint: {
            var ctx = getContext('2d')
            ctx.clearRect(0,0,width,height)
            ctx.beginPath()
            ctx.moveTo(0,1)
            ctx.lineTo(width,1)
            ctx.lineWidth = 1
            ctx.setLineDash([4,4])
            ctx.strokeStyle = "#963B3D"
            ctx.stroke()
        }
    }
}
