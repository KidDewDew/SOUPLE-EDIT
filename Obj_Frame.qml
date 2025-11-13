import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material
import com.custom 1.0
// 背景框qmlItem
Rectangle {
    id: obj
    //很多的free-qmlitem都有这个标志: isFree
    //表示这个Item是不是真的Free-Obj
    property bool isFree: true
    property int data_id
    property alias cp_propertyBar: cp_propertyBar
    property bool isSelected: false
    property bool allowEditBound: true
    property bool allowChangePosition: true
    property bool allowChangeSize: true
    //radius color borde

    Component {
        id: cp_propertyBar //属性栏
        GridLayout {
            columns: 2
            columnSpacing: parent.width * 0.08
            Text { text: "元素类别"; font.pixelSize: 16 }

            Text {
                text: "背景框";
                font.pixelSize: 16
            }

            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "定位"
                visible: isFree
            }

            FreeObjPositionPropertyGrid {
                aimObj: obj
                data_id: obj.data_id
                Layout.fillWidth: true
                Layout.columnSpan: 2
                visible: isFree
            }

            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "填充"
            }

            Text {
                text: "填充方式"
                font.pixelSize: 16
            }
            ComboBox {
                id: combo_fill
                Layout.preferredHeight: 34
                model: ["无填充","纯色填充","渐变色填充","图片填充","图案填充"]
                Component.onCompleted: {
                    const bg_type = SoupleManager.qmlGetData(data_id,Helper_Type.FILL_MODE)
                    if(bg_type == Helper_Type.No_Fill) currentIndex = 0
                    else if(bg_type == Helper_Type.Color_Fill) currentIndex = 1
                    else if(bg_type == Helper_Type.Gradient_Fill) currentIndex = 2
                    else if(bg_type == Helper_Type.Image_Fill) currentIndex = 3
                    else if(bg_type == Helper_Type.Pattern_Fill) currentIndex = 4
                }
            }
            Text {
                visible: combo_fill.currentIndex == 1
                text: "填充颜色"
                font.pixelSize: 16
            }
            ColorSelector {
                visible: combo_fill.currentIndex == 1
                Component.onCompleted: {
                    this.color = SoupleManager.qmlGetData(data_id,Helper_Type.BG_COLOR)
                }
                onColorChanged: {
                    obj.color = this.color
                    SoupleManager.sendCommandToData(data_id,Helper_Type.BG_COLOR_UP,this.color)
                }
            }
            Text {
                visible: combo_fill.currentIndex == 3
                text: "图片路径"
                font.pixelSize: 16
            }
            TextField {
                visible: combo_fill.currentIndex == 3
                Layout.preferredHeight: 34
                Component.onCompleted: {
                    text = SoupleManager.qmlGetData(data_id,Helper_Type.SOURCE)
                }
                onTextChanged: {
                    SoupleManager.sendCommandToData(data_id,Helper_Type.SOURCE_UP,text)
                }
            }
            Text {
                visible: combo_fill.currentIndex == 4
                text: "图案"
                font.pixelSize: 16
            }
            ComboBox {
                id: combo_pattern
                visible: combo_fill.currentIndex == 4
                Layout.preferredHeight: 34
                model: ["dot-1","dot-2"]
            }
            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "描边"
            }
            Text {
                text: "描边颜色"
                font.pixelSize: 16
            }
            ColorSelector {
                Component.onCompleted: {
                    this.color = SoupleManager.qmlGetData(data_id,Helper_Type.STROKE_COLOR)
                }
                onColorChanged: {
                    obj.border.color = this.color
                    SoupleManager.sendCommandToData(data_id,Helper_Type.STROKE_COLOR_UP,
                                        this.color)
                }
            }
            Text {
                text: "描边宽度"
                font.pixelSize: 16
            }
            MyDoubleSpinBox {
                step: 1.0
                initialValue: SoupleManager.qmlGetData(data_id,Helper_Type.STROKE_WIDTH)
                suffix: "像素"
                onValueChanged: {
                    obj.border.width = value
                    SoupleManager.sendCommandToData(data_id,Helper_Type.STROKE_WIDTH_UP,value)
                }
            }
            CheckBox {
                id: cb_r
                text: "复杂圆角半径"
                Layout.columnSpan: 2
            }
            Text {
                visible: ! cb_r.checked
                text: "圆角半径"
                font.pixelSize: 16
            }
            MyDoubleSpinBox {
                visible: ! cb_r.checked
                step: 1.0
                initialValue: SoupleManager.qmlGetData(data_id,Helper_Type.RADIUS)
                suffix: "像素"
                onValueChanged: {
                    obj.radius = value
                    SoupleManager.sendCommandToData(data_id,Helper_Type.RADIUS_UP,value)
                }
            }
            Text {
                visible: ! cb_r.checked
                text: "圆角半径"
                font.pixelSize: 16
            }
            MyDoubleSpinBox {
                visible: ! cb_r.checked
                step: 1.0
                initialValue: SoupleManager.qmlGetData(data_id,Helper_Type.RADIUS)
                suffix: "像素"
                onValueChanged: {
                    obj.radius = value
                    SoupleManager.sendCommandToData(data_id,Helper_Type.RADIUS,value)
                }
            }
            Text {
                visible: cb_r.checked
                text: "圆角半径(左上)"
                font.pixelSize: 16
            }
            MyDoubleSpinBox {
                visible: cb_r.checked
                step: 1.0
                initialValue: SoupleManager.qmlGetData(data_id,Helper_Type.RADIUS)
                suffix: "像素"
                onValueChanged: {
                    obj.radius = value
                    SoupleManager.sendCommandToData(data_id,Helper_Type.RADIUS,value)
                }
            }
            Text {
                visible: cb_r.checked
                text: "圆角半径(右上)"
                font.pixelSize: 16
            }
            MyDoubleSpinBox {
                visible: cb_r.checked
                step: 1.0
                initialValue: SoupleManager.qmlGetData(data_id,Helper_Type.RADIUS)
                suffix: "像素"
                onValueChanged: {
                    obj.radius = value
                    SoupleManager.sendCommandToData(data_id,Helper_Type.RADIUS,value)
                }
            }
            Text {
                visible: cb_r.checked
                text: "圆角半径(左下)"
                font.pixelSize: 16
            }
            MyDoubleSpinBox {
                visible: cb_r.checked
                step: 1.0
                initialValue: SoupleManager.qmlGetData(data_id,Helper_Type.RADIUS)
                suffix: "像素"
                onValueChanged: {
                    obj.radius = value
                    SoupleManager.sendCommandToData(data_id,Helper_Type.RADIUS,value)
                }
            }
            Text {
                visible: cb_r.checked
                text: "圆角半径(右下)"
                font.pixelSize: 16
            }
            MyDoubleSpinBox {
                visible: cb_r.checked
                step: 1.0
                initialValue: SoupleManager.qmlGetData(data_id,Helper_Type.RADIUS)
                suffix: "像素"
                onValueChanged: {
                    obj.radius = value
                    SoupleManager.sendCommandToData(data_id,Helper_Type.RADIUS,value)
                }
            }
        }
    }

    onFocusChanged: {
        if(focus) {
            selectedObj = obj
            if(obj.allowEditBound) {
                eb_bound.visible = true
                eb_bound.sync_obj = obj
            }
        }
    }

    MouseArea {
        id: marea
        hoverEnabled: true
        anchors.fill: parent
        onClicked: {
            obj.focus = true
            //selectedObj = obj
        }
    }

    Rectangle {
        visible: marea.containsMouse && selectedObj != obj
        anchors.fill: parent
        anchors.margins: -2
        color: "transparent"
        border.color:theme.accent_dark
        radius: 4
    }

}
