import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import com.custom 1.0

//obj元素 图片
Image {
    id: obj

    property int data_id
    property alias cp_propertyBar: cp_propertyBar
    property bool isSelected

    source: "qrc:/image/icon_Image.png"

    Component {
        id: cp_propertyBar //属性栏
        GridLayout {
            columns: 2
            columnSpacing: parent.width * 0.08
            Text { text: "元素类别"; font.pixelSize: 16 }

            Text {
                text: "图像(自由布局)";
                font.pixelSize: 16
            }

            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "定位"
            }

            FreeObjPositionPropertyGrid {
                aimObj: obj
                data_id: obj.data_id
                Layout.fillWidth: true
                Layout.columnSpan: 2
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


    MouseArea {
        anchors.fill: parent
        onPressed: {
            parent.focus = true
            eb_bound.visible = true
            eb_bound.sync_obj = obj
            selectedObj = obj
        }
    }

}
