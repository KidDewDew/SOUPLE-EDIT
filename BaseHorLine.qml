import QtQuick
import QtQuick.Layouts
import QtQuick.Shapes
import QtQuick.Controls.Material
import com.custom 1.0

//自由的水平标线
Item {
    id: horLine

    DashLine {
        height: 2
        //lineWidth: 0.3
        width: parent.width + 14
        x: -6
        color: triangle.fillColor // "#D9BFBA"
    }

    property int data_id   //c++数据id

    property alias cp_propertyBar: cp_propertyBar

    Component {
        id: cp_propertyBar //属性栏
        GridLayout {
            columns: 2
            columnSpacing: parent.width * 0.05
            Text { text: "元素类别"; font.pixelSize: 16 }
            RowLayout {
                Text {
                    text: "自由标线";
                    font.pixelSize: 16
                }
                RectButton {
                    Layout.alignment: Qt.AlignVCenter
                    text: "?"
                    accent: "#C9A350"
                    ToolTip {
                        visible: parent.hovered
                        text: "自由标线是什么?"
                    }
                }
            }
            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "基本信息"
            }

            Text { text: "X坐标"; font.pixelSize: 16 }
            MyDoubleSpinBox {
                id: spin_1
                step: 0.01
                precision: 2
                initialValue: Helper.pixel2cm(SoupleManager.qmlGetData(data_id,Helper_Type.X))
                Binding on value {
                    when: horLine.xChanged
                    value: Helper.pixel2cm(horLine.x)
                }
                onValueChanged: {
                    SoupleManager.sendCommandToData(data_id,Helper_Type.X_UP,`${Helper.cm2pixel(value)}`)
                }
            }

            Text { text: "Y坐标"; font.pixelSize: 16 }
            MyDoubleSpinBox {
                id: spin_2
                step: 0.01
                precision: 2
                initialValue: Helper.pixel2cm(SoupleManager.qmlGetData(data_id,Helper_Type.Y))
                Binding on value {
                    when: horLine.yChanged
                    value: Helper.pixel2cm(horLine.y)
                }
                onValueChanged: {
                    SoupleManager.sendCommandToData(data_id,Helper_Type.Y_UP,`${Helper.cm2pixel(value)}`)
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
        x: -19
        dir: Triangle.Dir.Right
        fillColor: mArea.pressed ? "#92E310" : "#B7E8C6"
        Behavior on fillColor { ColorAnimation { duration: 150 } }
        onFillColorChanged: requestPaint()
        MouseArea {
            id: mArea
            anchors.fill: parent
            drag.target: horLine
            drag.threshold: 0

            onPressed: {
                horLine.focus = true
                selectedObj = horLine
            }

            onMouseYChanged: {
                SoupleManager.sendCommandToData(data_id,Helper_Type.Y_UP,horLine.y + horLine.height/2)
            }

            onMouseXChanged: {
                SoupleManager.sendCommandToData(data_id,Helper_Type.X_UP,horLine.x)
            }

            // onReleased: {
            //     SoupleManager.sendCommandToData(data_id,"yUP",horLine.y + horLine.height/2)
            // }
        }
    }

}
