import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import com.custom 1.0

//obj元素 图片
Image {
    id: obj

    property int data_id
    //isSelected: focus

    cache: false
    property bool isSelected

    property alias cp_propertyBar: cp_propertyBar
    //property alias img: img

    function die() {
        obj.destroy()
    }

    function selecti(i1,i2) {
        //selected = true
    }

    Component {
        id: cp_propertyBar //属性栏
        GridLayout {
            columns: 2
            columnSpacing: parent.width * 0.18
            Text { text: "元素类别"; font.pixelSize: 16 }
            Text {
                text: "图像";
                font.pixelSize: 16
            }
            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "锚定信息"
            }

            Text { text: "垂直对齐"; font.pixelSize: 16 }
            ComboBox {
                model: ["中心对齐","基线对齐","下对齐","上对齐"]
            }

            Text { text: "对齐偏移"; font.pixelSize: 16 }
            MyDoubleSpinBox {
                step: 0.5
                precision: 1
                //initialValue: Helper.pixel2cm(obj.vAlignMargin)
                initialValue: 0
                suffix: "cm"
                // Binding on value {
                //     when: obj.vAlignMarginChanged
                //     value: Helper.pixel2cm(obj.vAlignMargin)
                // }
                onValueChanged: {
                    //obj.vAlignMargin = Helper.cm2pixel(value)
                }
            }
            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "图像信息"
            }
            Text { text: "图片源"; font.pixelSize: 16 }
            RowLayout {
                TextField {
                    id: tf_imgsrc
                    leftPadding: 1; rightPadding: 1; topPadding: 1; bottomPadding: 1
                    Layout.preferredHeight: 35; Layout.preferredWidth: 80
                    Component.onCompleted: {
                        tf_imgsrc.text = obj.source
                    }
                    onTextChanged: {
                        obj.source = text
                    }
                }
                RectButton {
                    text: "选择"
                    accent: "#0030AA"
                    font.pixelSize: 11
                    Layout.preferredWidth: 35
                    Layout.preferredHeight: 20
                    Layout.alignment: Qt.AlignVCenter
                    onClicked: {
                        file_dialog.nameFilters = ["图片 (*.bmp *.png *.jpg *.jpeg)"]
                        file_dialog.open()
                        file_dialog.callback = function(file) {
                            if(file == "") return
                            obj.source = file
                            SoupleManager.sendCommandToData(data_id,
                                        Helper_Type.SOURCE_UP,obj.source)
                        }
                    }
                }
            }
            Text { text: "图片宽度"; font.pixelSize: 16 }
            MyDoubleSpinBox {
                id: spin_1
                step: 0.1
                precision: 2
                initialValue: Helper.pixel2cm(SoupleManager.qmlGetData(data_id,Helper_Type.WIDTH))
                Binding on value {
                    when: obj.widthChanged
                    value: Helper.pixel2cm(obj.width)
                }
                onValueChanged: {
                    obj.width = Helper.cm2pixel(value)
                    SoupleManager.sendCommandToData(data_id,
                                Helper_Type.WIDTH_UP,obj.width)
                }
            }
            Text { text: "图片高度"; font.pixelSize: 16 }
            MyDoubleSpinBox {
                id: spin_2
                step: 0.1
                precision: 2
                initialValue: Helper.pixel2cm(SoupleManager.qmlGetData(data_id,Helper_Type.HEIGHT))
                Binding on value {
                    when: obj.heightChanged
                    value: Helper.pixel2cm(obj.height)
                }
                onValueChanged: {
                    obj.height = Helper.cm2pixel(value)
                    SoupleManager.sendCommandToData(data_id,
                                Helper_Type.HEIGHT_UP,obj.height)
                }
            }
        }
    }

    onFocusChanged: if(focus) selectedObj = this

    MouseArea {
        anchors.fill: parent
        onPressed: {
            parent.focus = true
        }
    }


    source: "qrc:/image/icon_Image.png"


    // Rectangle {
    //     anchors.
    // }


    // function dropRight(dropWidth) { //图片，保证完整性，直接溢出
    //     obj.removeSelf(false)
    //     return obj
    // }
    // function dropLeft(dropWidth) { //回缩
    //     if(dropWidth < width) return null //空间不足容纳图片
    //     obj.removeSelf(false)
    //     return obj
    // }

    // function getCursorFromLeft() { //直接转移cursor
    //     rightObj.getCursorFromLeft()
    // }

    // function getCursorFromRight() {//直接转移cursor
    //     leftObj.getCursorFromRight()
    // }
}
