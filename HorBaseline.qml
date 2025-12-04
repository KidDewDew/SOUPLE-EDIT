import QtQuick
import QtQuick.Layouts
import QtQuick.Shapes
import QtQuick.Controls.Material
import com.custom 1.0

//HorBaseline: 水平基线
Item {
    id: horLine
    property real biasY //y = realY + biasY 拖动时呢按照 y = y - biasY
    property real centerY: y+1

    function die() {
        obj.destroy()
    }

    DashLine {
        height: 2
        //lineWidth: 0.3
        width: parent.width + 14
        x: -6
        color: triangle.fillColor // "#D9BFBA"
    }

    property int data_id   //c++数据id


    //Behavior on accent { ColorAnimation { duration: 300 } }


    property alias cp_propertyBar: cp_propertyBar

    Component {
        id: cp_propertyBar //属性栏
        GridLayout {
            columns: 2
            columnSpacing: parent.width * 0.05
            Text { text: "元素类别"; font.pixelSize: 16 }
            RowLayout {
                Text {
                    text: "水平标线";
                    font.pixelSize: 16
                }
                RectButton {
                    Layout.alignment: Qt.AlignVCenter
                    text: "?"
                    accent: "#C9A350"
                    ToolTip {
                        visible: parent.hovered
                        text: "水平标线是什么?"
                    }
                }
            }
            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "基本信息"
            }
            Text { text: "名称"; font.pixelSize: 16 }
            TextField {
                leftPadding: 5; rightPadding: 5
                topPadding: 5; bottomPadding: 5
                Layout.preferredHeight: 35
                Component.onCompleted: text = SoupleManager.qmlGetData(data_id,Helper_Type.NAME)
                onTextChanged: {
                    if(text == SoupleManager.qmlGetData(data_id,Helper_Type.NAME)) {
                        rect_invalid.visible = false
                        return
                    }
                    const r = SoupleManager.sendCommandToData(data_id,Helper_Type.NAME_UP,text)
                    if(r == Helper_Type.Error_Repeat) {
                        rect_invalid.visible = true
                    } else if(r == 0) {
                        rect_invalid.visible = false
                        horLine.objectName = text
                    }
                }
                RectButton {
                    id: rect_invalid
                    visible: false
                    text: "重复!"
                    width: 25
                    accent: "red"
                    label.font.pixelSize: 10
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                }
            }
            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "锚定属性"
            }
            Text { text: "左标线"; font.pixelSize: 16 }
            TextField {
                leftPadding: 10; rightPadding: 5
                topPadding: 5; bottomPadding: 5
                Layout.preferredHeight: 35
                Component.onCompleted: text = SoupleManager.qmlGetData(data_id,Helper_Type.LEFTLINE)
                onTextChanged: {
                    if(text == "") { but_zbx.text = "无效"; return }
                    if(text == SoupleManager.qmlGetData(data_id,Helper_Type.LEFTLINE)) return
                    const r = SoupleManager.sendCommandToData(data_id,Helper_Type.LEFTLINE_UP,text)
                    but_zbx.text = r ? "无效" : "有效"
                }
                RectButton {
                    id: but_zbx
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
            Text { text: "右标线"; font.pixelSize: 16 }
            TextField {
                leftPadding: 10; rightPadding: 5
                topPadding: 5; bottomPadding: 5
                Layout.preferredHeight: 35
                Component.onCompleted: text = SoupleManager.qmlGetData(data_id,Helper_Type.RIGHTLINE)
                onTextChanged: {
                    if(text == "") { but_zbx2.text = "无效"; return }
                    if(text == SoupleManager.qmlGetData(data_id,Helper_Type.RIGHTLINE)) return
                    const r = SoupleManager.sendCommandToData(data_id,Helper_Type.RIGHTLINE_UP,text)
                    but_zbx2.text = r ? "无效" : "有效"
                }
                RectButton {
                    id: but_zbx2
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
            RowLayout {
                Text { text: "逻辑上标线"; font.pixelSize: 16 }
                RectButton {
                    Layout.alignment: Qt.AlignVCenter
                    text: "?"
                    accent: "#C9A350"
                    ToolTip {
                        visible: parent.hovered
                        text: "什么是逻辑上标线?"
                    }
                }
            }
            HLineSelector {
                data_id: horLine.data_id
                hline_property: Helper_Type.LOGIC_LASTHLINE
                hline_up_property: Helper_Type.LOGIC_LASTHLINE_UP
            }
            RowLayout {
                Text { text: "锚定上标线"; font.pixelSize: 16 }
                RectButton {
                    Layout.alignment: Qt.AlignVCenter
                    text: "?"
                    accent: "#C9A350"
                    ToolTip {
                        visible: parent.hovered
                        text: "什么是锚定上标线?"
                    }
                }
            }
            HLineSelector {
                data_id: horLine.data_id
                hline_property: Helper_Type.ANCHOR_LASTHLINE
                hline_up_property: Helper_Type.ANCHOR_LASTHLINE_UP
            }
            Text { text: "上边距"; font.pixelSize: 16 }
            MyDoubleSpinBox {
                id: spin_1
                step: 0.01
                precision: 2
                initialValue: Helper.pixel2cm(SoupleManager.qmlGetData(data_id,Helper_Type.TOP_MARGIN))
                Binding on value {
                    when: horLine.centerYChanged
                    value: Helper.pixel2cm(SoupleManager.qmlGetData(data_id,Helper_Type.TOP_MARGIN))
                }
                onValueChanged: {
                    SoupleManager.sendCommandToData(data_id,Helper_Type.TOP_MARGIN_UP,`${Helper.cm2pixel(value)}`)
                }
            }
            Text { text: "下标线"; font.pixelSize: 16 }
            TextField {
                readOnly: true
                enabled: false
                leftPadding: 5; rightPadding: 5
                topPadding: 5; bottomPadding: 5
                Layout.preferredHeight: 35
                Component.onCompleted: {
                    const r = SoupleManager.qmlGetData(data_id,Helper_Type.LOGIC_NEXTHLINE)
                    text = (r == "" ? "[未定义]" : r)
                }
            }
            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "其他属性"
            }
            Text {
                text: "允许调整<font color='grey' size='9px'>以尽量填满一行</font>";
                font.pixelSize: 16
                textFormat: Text.RichText
            }
            CheckBox {
                font.pixelSize: 16
                text: "允许"
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

    // onYChanged: {
    //     console.log(objectName,"y->",y)
    // }

    Triangle {
        id: triangle
        width: 11
        height: 14
        anchors.verticalCenter: parent.verticalCenter
        x: -19
        dir: Triangle.Dir.Right
        fillColor: mArea.pressed ? "#D96982" : "#D9BFBA"
        //scale: mArea.pressed ? 0.85 : 1.0
        Behavior on fillColor { ColorAnimation { duration: 150 } }
        onFillColorChanged: requestPaint()
        MouseArea {
            id: mArea
            anchors.fill: parent
            drag.target: horLine
            drag.axis: Drag.YAxis
            drag.threshold: 0
            //hoverEnabled: true
            onPressed: {
                horLine.focus = true
                selectedObj = horLine
                drag.minimumY = SoupleManager.qmlGetData(data_id,Helper_Type.DRAGY0)
                drag.maximumY = SoupleManager.qmlGetData(data_id,Helper_Type.DRAGY1)
            }
            onMouseYChanged: {
                //console.log("qml yUP: ",horLine.y + horLine.height/2)
                SoupleManager.sendCommandToData(data_id,Helper_Type.Y_UP,horLine.y + horLine.height/2)
            }

            onReleased: {
                SoupleManager.sendCommandToData(data_id,Helper_Type.Y_UP,horLine.y + horLine.height/2)
                // if(anchor_lastHline == null) return
                // lastHlineMargin = horLine.centerY - anchor_lastHline.centerY
                // horLine.y = Qt.binding(function(){ return anchor_lastHline.centerY + lastHlineMargin - horLine.height/2 })
            }
        }
    }

}
