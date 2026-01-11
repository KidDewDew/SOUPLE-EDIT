import QtQuick
import QtQuick.Layouts
import QtQuick.Shapes
import QtQuick.Controls
import com.custom 1.0

//word风格页面自带的垂直标线，用于分栏
Item {
    id: verLine

    Text {
        text: parent.objectName
        color: "#735C5C"
        font.pixelSize: 11
        anchors.bottom: triangle.top
        anchors.bottomMargin: 2
        anchors.horizontalCenter: parent.horizontalCenter
    }

    DashLine {
        id: dashLine
        isHorizontal: false
        color: "#C9C9C9"
        width: 2
        anchors.horizontalCenter: parent.horizontalCenter
        y: -8
        height: parent.height + 16
    }

    Triangle {
        id: triangle
        dir: Triangle.Dir.Down
        width: 14
        height: 12
        y: -24
        anchors.horizontalCenter: parent.horizontalCenter
        fillColor: mArea.pressed ? "#D96982" : "#8C6869"
        MouseArea {
            id: mArea
            anchors.fill: parent
            drag.target: verLine
            drag.axis: Drag.XAxis
            drag.threshold: 0
            onPressed: {
                soupleEdit.selectedObj = verLine
                horRuler.show_float = true
            }
            onReleased: horRuler.show_float = false
            onMouseXChanged: horRuler.float_x = verLine.x + verLine.width/2
        }
    }

    property int data_id
    property real centerX: x + width/2

    property alias cp_propertyBar: cp_propertyBar

    property int ofPage: soupleEdit.getPageOfY(y)
    property var page: soupleEdit.pages[ofPage]
    property real pageMargin: (soupleEdit.width - page.width)/2

    property real pageLeftMargin: centerX - pageMargin
    property real pageRightMargin: page.width - pageLeftMargin
    property real pageCenterMargin: pageLeftMargin - page.width/2

    function setPageLeftMargin(val) {
        x = val + pageMargin - width/2
        SoupleManager.sendCommandToData(data_id,"xUP",x)
    }

    function setPageRightMargin(val) {
        setPageLeftMargin(page.width - val)
    }

    function setPageCenterMargin(val) {
        setPageLeftMargin(val + page.width/2)
    }

    Component {
        id: cp_propertyBar //属性栏
        GridLayout {
            columns: 2
            columnSpacing: parent.width * 0.18
            Text { text: "元素类别"; font.pixelSize: 16 }
            RowLayout {
                Text {
                    text: "垂直标线";
                    font.pixelSize: 16
                }
                RectButton {
                    Layout.alignment: Qt.AlignVCenter
                    text: "?"
                    accent: "#C9A350"
                    ToolTip {
                        visible: parent.hovered
                        text: "垂直标线是什么?"
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
                    if(text == "") return
                    const r = SoupleManager.sendCommandToData(data_id,Helper_Type.NAME_UP,text)
                    if(r == 0) {
                        rect_invalid.visible = false
                    } else {
                        rect_invalid.visible = true
                        verLine.objectName = text
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
            Text { text: "左边距"; font.pixelSize: 16 }
            MyDoubleSpinBox {
                id: spin_1
                step: 0.01
                precision: 2
                initialValue: Helper.pixel2cm(verLine.pageLeftMargin)
                Binding on value {
                    when: verLine.centerXChanged
                    value: Helper.pixel2cm(verLine.pageLeftMargin)
                }
                onValueChanged: {
                    verLine.setPageLeftMargin(Helper.cm2pixel(value))
                }
            }
            Text { text: "右边距"; font.pixelSize: 16 }
            MyDoubleSpinBox {
                step: 0.01
                precision: 2
                initialValue: Helper.pixel2cm(verLine.pageRightMargin)
                Binding on value {
                    when: verLine.centerXChanged
                    value: Helper.pixel2cm(verLine.pageRightMargin)
                }
                onValueChanged: {
                    verLine.setPageRightMargin(Helper.cm2pixel(value))
                }
            }
            Text { text: "中轴距"; font.pixelSize: 16 }
            MyDoubleSpinBox {
                step: 0.01
                precision: 2
                initialValue: Helper.pixel2cm(verLine.pageCenterMargin)
                Binding on value {
                    when: verLine.centerXChanged
                    value: Helper.pixel2cm(verLine.pageCenterMargin)
                }
                onValueChanged: {
                    verLine.setPageCenterMargin(Helper.cm2pixel(value))
                }
            }
            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "附加信息"
            }
            Text { text: "<font color='red'>*</font>旋转"; font.pixelSize: 16 }
            MyDoubleSpinBox {
                suffix: "°度"
                step: 1.0
                precision: 1
            }
        }
    }
}
