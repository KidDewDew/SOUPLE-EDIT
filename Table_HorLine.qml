import QtQuick

import QtQuick
import QtQuick.Layouts
import QtQuick.Shapes
import QtQuick.Controls.Material
import com.custom 1.0

//内部水平标线
Item {
    id: horLine

    property bool isFirstLine: false
    property bool isLastLine: false
    property bool tableActive: false
    property real rightButtonY

    DashLine {
        height: 2
        lineWidth: 0.75
        width: parent.width + 14
        x: -6
        color: triangle.fillColor // "#D9BFBA"
    }

    property int data_id   //c++数据id

    property alias cp_propertyBar: cp_propertyBar

    property bool showDrag: false //是否显示左边的drag按钮

    Component {
        id: cp_propertyBar //属性栏
        GridLayout {
            columns: 2
            columnSpacing: parent.width * 0.05
            Text { text: "元素类别"; font.pixelSize: 16 }
            RowLayout {
                Text {
                    text: "表格行线";
                    font.pixelSize: 16
                }
                RectButton {
                    Layout.alignment: Qt.AlignVCenter
                    text: "?"
                    accent: "#C9A350"
                    ToolTip {
                        visible: parent.hovered
                        text: "表格行线是什么?"
                    }
                }
            }
            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "基本信息"
            }

            Text { text: "上边距"; font.pixelSize: 16 }
            MyDoubleSpinBox {
                id: spin_1
                step: 0.01
                precision: 2
                initialValue: Helper.pixel2cm(SoupleManager.qmlGetData(data_id,Helper_Type.TOP_MARGIN))
                //Binding on value {
                //    when: horLine.xChanged
                //    value: Helper.pixel2cm(horLine.x)
                //}
                onValueChanged: {
                    SoupleManager.sendCommandToData(data_id,Helper_Type.TOP_MARGIN_UP,`${Helper.cm2pixel(value)}`)
                }
            }

        }
    }


    function terminateDrag() {
        // mArea.enabled = false
        // mArea.enabled = true
        mArea.visible = false
        mArea.drag = null
        mArea.drag = horLine
        mArea.visible = true
    }

    SquareButton {
        visible: isFirstLine && tableActive //第一行显示添加列按钮
        text: "+"
        font.pixelSize: 14
        height: 45
        x: parent.width+2
        y: rightButtonY - height/2
    }

    SquareButton {
        visible: isLastLine && tableActive //最后一行显示添加行按钮
        text: "+"
        font.pixelSize: 14
        anchors.horizontalCenter: parent.horizontalCenter
        y: 4
        width: 45
    }

    Text {
        text: parent.objectName
        font.pixelSize: 11
        color: "#735C5C"
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: triangle.left
        anchors.rightMargin: 2
    }

    Triangle {
        id: triangle
        width: 11
        height: 14
        anchors.verticalCenter: parent.verticalCenter
        x: -11
        dir: Triangle.Dir.Right
        fillColor: mArea.pressed ? "#E08B38" : "#E0D490"
        Behavior on fillColor { ColorAnimation { duration: 150 } }
        onFillColorChanged: requestPaint()
        MouseArea {
            id: mArea
            anchors.fill: parent
            drag.target: horLine
            drag.threshold: 0
            drag.axis: Drag.YAxis
            onPressed: {
                horLine.focus = true
                selectedObj = horLine
            }

            onMouseYChanged: {
                SoupleManager.sendCommandToData(data_id,Helper_Type.Y_UP,horLine.y + horLine.height/2)
            }

            // onMouseXChanged: {
            //     SoupleManager.sendCommandToData(data_id,Helper_Type.X_UP,horLine.x)
            // }

            // onReleased: {
            //     SoupleManager.sendCommandToData(data_id,"yUP",horLine.y + horLine.height/2)
            // }
        }
    }

}

