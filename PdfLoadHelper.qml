import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material

//pdf读取小助手
Window {
    id: window
    title: "PDF读取小助手"
    width: 560
    height: 700
    property string pdfFile
    property list<var> steps: [{title:"页眉页脚读取"}]
    onClosing: {
        console.log("onClosing!")
        Helper.removeImageFromProvider("#pdfrender11") //删除图片
        window.destroy()
    }

    ColumnLayout {
        anchors.fill: parent
        Text {
            font.pixelSize: 20
            color: "#0B6B3E"
            font.bold: true
            text: "步骤 "+(stackLayout.currentIndex+1) + " " + steps[stackLayout.currentIndex].title
        }

        Rectangle {
            Layout.fillWidth: true
            implicitHeight: 1
            color: "#0B6B3E"
        }

        StackLayout {
            id: stackLayout
            Layout.fillWidth: true
            Layout.fillHeight: true
            RowLayout {
                Layout.fillHeight: true
                //Layout.fillWidth: true
                //spacing: 0
                ColumnLayout {
                    height: parent.height
                    Layout.leftMargin: 5
                    spacing: 2
                    Image {
                        id: pdf_image
                        source: "image://provider/#pdfrender11"
                        Layout.fillHeight: true
                        Layout.preferredWidth: sourceSize.width * height / sourceSize.height
                        fillMode: Image.PreserveAspectFit
                        cache: false
                        Rectangle {
                            anchors.centerIn: parent
                            width: parent.paintedWidth+2
                            height: parent.paintedHeight+2
                            color: "transparent"
                            border.width: 1
                            radius: 4
                            border.color: "black"
                            //layer.
                        }

                        Rectangle {
                            visible: cb_页眉.checked
                            height: float_topMargin.y + float_topMargin.height/2
                            width: parent.width
                            color: "#60BD6D26"
                            clip: true
                            Text {
                                text: "页眉区域"
                                color: "white"
                                anchors.verticalCenter: parent.verticalCenter
                                x: 10
                                font.pixelSize: 12
                            }
                        }

                        Triangle {
                            id: float_topMargin //页眉
                            visible: cb_页眉.checked
                            width: 18
                            height: 18
                            dir: Triangle.Dir.Left
                            fillColor: "#BD6D26"
                            x: parent.width+6
                            y: 40

                            onYChanged: {
                                spinBox_页眉.value =
                                        Helper.pixel2cm(
                                        (y+height/2) * pdf_image.sourceSize.height / pdf_image.paintedHeight
                                        )
                            }

                            MouseArea {
                                anchors.fill: parent
                                drag.target: float_topMargin
                                drag.threshold: 0
                                drag.axis: Drag.YAxis
                                drag.maximumY: pdf_image.height-height/2
                                drag.minimumY: -height/2
                            }
                        }

                        Rectangle {
                            visible: cb_页脚.checked
                            y: float_bottomMargin.y + float_bottomMargin.height/2
                            height: parent.height - y
                            width: parent.width
                            color: "#60BD6D26"
                            clip: true
                            Text {
                                text: "页脚区域"
                                color: "white"
                                anchors.verticalCenter: parent.verticalCenter
                                font.pixelSize: 12
                                x: 10
                            }
                        }

                        Triangle {
                            id: float_bottomMargin //页脚
                            visible: cb_页脚.checked
                            width: 18
                            height: 18
                            dir: Triangle.Dir.Left
                            fillColor: "#BD6D26"
                            x: parent.width+6
                            y: parent.height - 40
                            onYChanged: {
                                spinBox_页脚.value = Helper.pixel2cm(
                                            (pdf_image.height-y-height/2) * pdf_image.sourceSize.height / pdf_image.paintedHeight
                                        )
                            }
                            MouseArea {
                                anchors.fill: parent
                                drag.target: float_bottomMargin
                                drag.threshold: 0
                                drag.axis: Drag.YAxis
                                drag.maximumY: pdf_image.height-height/2
                                drag.minimumY: -height/2
                            }
                        }

                    }

                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter
                        Text {
                            text: "页码"
                        }
                        SpinBox {
                            from: 1; to: 10000
                            //height: 30
                            //width: 100
                            Layout.preferredHeight: 32
                            Layout.preferredWidth: 88
                            //Material.containerStyle: Material.
                            onValueChanged: {
                                Helper.request_renderPdf_as_image(pdfFile,value-1,"#pdfrender11",
                                        function(result,tip=""){
                                            console.log("&&&& ",result,tip);
                                            if(result == true) {
                                                pdf_image.source = ""
                                                pdf_image.source = "image://provider/#pdfrender11"
                                            }
                                            else {
                                                console.log(tip)
                                            }
                                        })
                            }
                        }
                        // Text {
                        //     text: "缩放"
                        // }
                        // SpinBox {
                        //     from: ; to: 10000
                        //     onValueChanged: {

                        //     }
                        // }
                    }
                }

                GridLayout {
                    columns: window.width > window.height ? 2 : 1
                    //Layout.alignment: Qt.AlignBottom
                    Layout.bottomMargin: 34
                    rowSpacing: 12
                    CheckBox {
                        id: cb_页眉
                        text: "启用页眉"
                        Material.accent: "#BD6D26"
                    }
                    MyDoubleSpinBox {
                        id: spinBox_页眉
                        visible: cb_页眉.checked
                    }
                    CheckBox {
                        id: cb_页脚
                        text: "启用页脚"
                        Material.accent: "#BD6D26"
                    }
                    MyDoubleSpinBox {
                        id: spinBox_页脚
                        visible: cb_页脚.checked
                    }
                }

            }
        }

        Row {
            Layout.alignment: Qt.AlignRight
            Layout.rightMargin: 10
            ToolButton {
                text: "上一步"
                enabled: stackLayout.currentIndex > 0
                font.pixelSize: 12
                onClicked: stackLayout.currentIndex--
            }
            ToolButton {
                enabled: stackLayout.currentIndex != steps.length - 1
                text: "下一步"
                font.pixelSize: 12
                onClicked: stackLayout.currentIndex++
            }
            ToolButton {
                visible: stackLayout.currentIndex == steps.length - 1
                text: "开始读取"
                font.pixelSize: 12

                Material.foreground: "#E08F0F"

                onClicked: {
                    main_titleBar.addDocument(
                        {
                            s: -1,
                            tabName: Helper.extractFileNameFromPath(pdfFile),
                            documentType: "souple",
                            icon_src: "qrc:/image/icon_doc.png"
                        }
                    )
                    Helper.requestLoadPdf(pdfFile,{uniHeader:cb_页眉.checked,uniFooter:cb_页脚.checked,
                                        headerMargin:Helper.cm2pixel(spinBox_页眉.value),
                                        footerMargin:Helper.cm2pixel(spinBox_页脚.value)})
                    window.close()
                }

            }
        }
    }
}
