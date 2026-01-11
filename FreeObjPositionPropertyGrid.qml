import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material
import com.custom 1.0

//自由对象的定位属性栏(FreeaimObj)
GridLayout {
    id: grid
    columns: 2
    columnSpacing: parent.width * 0.06

    required property int data_id //输入data_id
    required property var aimObj     //输入作用对象

    Component.onCompleted: {
        tf_topline.text = SoupleManager.qmlGetData(data_id,Helper_Type.TOPLINE)
        tf_bottomline.text = SoupleManager.qmlGetData(data_id,Helper_Type.BOTTOMLINE)
        tf_leftline.text = SoupleManager.qmlGetData(data_id,Helper_Type.LEFTLINE)
        tf_rightline.text = SoupleManager.qmlGetData(data_id,Helper_Type.RIGHTLINE)
        checkbox_enable_topline.checked = (tf_topline.text.length > 0)
        checkbox_enable_bottomline.checked = (tf_bottomline.length > 0)
        checkbox_enable_leftline.checked = (tf_leftline.length > 0)
        checkbox_enable_rightline.checked = (tf_rightline.length > 0)
    }

    CheckBox {
        id: checkbox_enable_topline
        text: "上标线定位"
        Layout.columnSpan: checked ? 1 : 2
    }

    HLineSelector {
        visible: checkbox_enable_topline.checked
        data_id: grid.data_id
        hline_property: Helper_Type.TOPLINE
        hline_up_property: Helper_Type.TOPLINE_UP
    }

    Text { visible: checkbox_enable_topline.checked; text: "上边距"; font.pixelSize: 16 }
    MyDoubleSpinBox {
        id: spin_topMargin
        visible: checkbox_enable_topline.checked;
        step: 0.1
        precision: 2
        initialValue: Helper.pixel2cm(SoupleManager.qmlGetData(data_id,Helper_Type.TOP_MARGIN))
        // Binding on value {
        //     when: aimObj.widthChanged
        //     value: Helper.pixel2cm(aimObj.width)
        // }
        onValueChanged: {
            //aimObj.width = Helper.cm2pixel(value)
            SoupleManager.sendCommandToData(data_id,
                        Helper_Type.TOP_MARGIN_UP,Helper.cm2pixel(value))
        }
    }

    CheckBox {
        id: checkbox_enable_bottomline
        text: "下标线定位"
        Layout.columnSpan: checked ? 1 : 2
    }

    HLineSelector {
        visible: checkbox_enable_bottomline.checked
        data_id: grid.data_id
        hline_property: Helper_Type.BOTTOMLINE
        hline_up_property: Helper_Type.BOTTOMLINE_UP
    }

    Text { visible: checkbox_enable_bottomline.checked; text: "下边距"; font.pixelSize: 16 }
    MyDoubleSpinBox {
        id: spin_bottomMargin
        visible: checkbox_enable_bottomline.checked;
        step: 0.1
        precision: 2
        initialValue: Helper.pixel2cm(SoupleManager.qmlGetData(data_id,Helper_Type.BOTTOM_MARGIN))
        // Binding on value {
        //     when: aimObj.widthChanged
        //     value: Helper.pixel2cm(aimObj.width)
        // }
        onValueChanged: {
            SoupleManager.sendCommandToData(data_id,
                        Helper_Type.BOTTOM_MARGIN_UP,Helper.cm2pixel(value))
        }
    }

    CheckBox {
        id: checkbox_enable_leftline
        text: "左标线定位"
        Layout.columnSpan: checked ? 1 : 2
    }

    TextField {
        id: tf_leftline
        visible: checkbox_enable_leftline.checked
        leftPadding: 5; rightPadding: 5
        topPadding: 5; bottomPadding: 5
        Layout.preferredHeight: 35
        placeholderText: "标线名"
        //Component.onCompleted: text = SoupleManager.qmlGetData(data_id,Helper_Type.LEFTLINE)
        onTextChanged: {
            if(text == "") { but_zbx1.text = "无效"; return }
            if(text == SoupleManager.qmlGetData(data_id,Helper_Type.LEFTLINE)) return
            const r = SoupleManager.sendCommandToData(data_id,Helper_Type.LEFTLINE_UP,text)
            but_zbx1.text = r ? "无效" : "有效"
        }
        RectButton {
            id: but_zbx1
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

    Text { visible: checkbox_enable_leftline.checked; text: "左边距"; font.pixelSize: 16 }
    MyDoubleSpinBox {
        id: spin_leftMargin
        visible: checkbox_enable_leftline.checked;
        step: 0.1
        precision: 2
        initialValue: Helper.pixel2cm(SoupleManager.qmlGetData(data_id,Helper_Type.LEFT_MARGIN))
        onValueChanged: {
            SoupleManager.sendCommandToData(data_id,
                        Helper_Type.LEFT_MARGIN_UP,Helper.cm2pixel(value))
        }
    }

    CheckBox {
        id: checkbox_enable_rightline
        text: "右标线定位"
        Layout.columnSpan: checked ? 1 : 2
    }

    TextField {
        id: tf_rightline
        visible: checkbox_enable_rightline.checked
        leftPadding: 5; rightPadding: 5
        topPadding: 5; bottomPadding: 5
        Layout.preferredHeight: 35
        placeholderText: "标线名"
        //Component.onCompleted: text = SoupleManager.qmlGetData(data_id,"rightLine")
        onTextChanged: {
            if(text == "") { but_zbx4.text = "无效"; return }
            if(text == SoupleManager.qmlGetData(data_id,Helper_Type.RIGHTLINE)) return
            const r = SoupleManager.sendCommandToData(data_id,Helper_Type.RIGHTLINE_UP,text)
            but_zbx4.text = r ? "无效" : "有效"
        }
        RectButton {
            id: but_zbx4
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

    Text { visible: checkbox_enable_rightline.checked; text: "右边距"; font.pixelSize: 16 }
    MyDoubleSpinBox {
        id: spin_rightMargin
        visible: checkbox_enable_rightline.checked;
        step: 0.1
        precision: 2
        initialValue: Helper.pixel2cm(SoupleManager.qmlGetData(data_id,Helper_Type.RIGHT_MARGIN))
        onValueChanged: {
            SoupleManager.sendCommandToData(data_id,
                        Helper_Type.RIGHT_MARGIN_UP,Helper.cm2pixel(value))
        }
    }


    Text { text: "页左边距(X)"; font.pixelSize: 16 }
    MyDoubleSpinBox {
        id: spin_pageleftmargin
        step: 0.1
        precision: 2
        //左右标线均无时，才可以修改x坐标
        enabled: ! checkbox_enable_leftline.checked && ! checkbox_enable_rightline.checked
        initialValue: Helper.pixel2cm(SoupleManager.qmlGetData(data_id,Helper_Type.X)) //咱们先不考虑不同页宽的pdf(太少见了)，先默认x坐标就是页左边距
        Binding on value { //双向绑定
            when: aimObj.xChanged
            value: Helper.pixel2cm(aimObj.x)
        }
        onValueChanged: {
            aimObj.x = Helper.cm2pixel(value)
            SoupleManager.sendCommandToData(data_id,
                        Helper_Type.X_UP,aimObj.x)
        }
    }

    Text { text: "页上边距(Y)"; font.pixelSize: 16 }
    MyDoubleSpinBox {
        id: spin_pagetopmargin
        step: 0.1
        precision: 2
        //上下标线均无时，才可以修改y坐标
        enabled: ! checkbox_enable_topline.checked && ! checkbox_enable_bottomline.checked
        initialValue: Helper.pixel2cm(SoupleManager.qmlGetData(data_id,Helper_Type.Y))
        Binding on value {
            when: aimObj.yChanged
            value: Helper.pixel2cm(aimObj.y)
        }
        onValueChanged: {
            aimObj.y = Helper.cm2pixel(value)
            SoupleManager.sendCommandToData(data_id,
                        Helper_Type.Y_UP,aimObj.y)
        }
    }

    RowLayout {
        Text {
            text: "深度";
            font.pixelSize: 16
        }
        RectButton {
            Layout.alignment: Qt.AlignVCenter
            text: "?"
            accent: "#C9A350"
            ToolTip {
                visible: parent.hovered
                text: "什么是深度？"
            }
        }
    }

    GridLayout {
        columnSpacing: 1
        columns: 2
        Text{
            text: "相对"
        }
        ComboBox {
            id: combobox_zlayer
            model: ["底层","文本层","顶层"]
            Layout.preferredWidth: 90
            Layout.preferredHeight: 45
            Component.onCompleted: {
                //根据obj.z确定层级
                //obj.z = SoupleManager.qmlGetData(data_id,"z")
                console.log("zlayer get obj.z = ",obj.z)
                if(obj.z < Helper_Type.Bottom+1001) {
                    const nz = obj.z - Helper_Type.Bottom
                    currentIndex = 0
                    spinBox_layerOffset.value = nz
                } else if(obj.z < Helper_Type.Text+1001) {
                    const nz = obj.z - Helper_Type.Text
                    currentIndex = 1
                    spinBox_layerOffset.value = nz
                } else {
                    const nz = obj.z - Helper_Type.Cover
                    currentIndex = 2
                    spinBox_layerOffset.value = nz
                }
                conn_1.enabled = true //启用conn_1
            }
            onCurrentIndexChanged: {
                if(conn_1.enabled == false) return
                spinBox_layerOffset.value = 0
                obj.z = combobox_zlayer.getBaseZ()
                SoupleManager.sendCommandToData(data_id,
                            Helper_Type.Z_UP,obj.z)
            }
            function getBaseZ() {
                switch(currentIndex) {
                    case 0: return Helper_Type.Bottom
                    case 1: return Helper_Type.Text
                    case 2: return Helper_Type.Cover
                }
            }
        }
        Text{
            text: "上移"
        }
        MyDoubleSpinBox {
            id: spinBox_layerOffset
            from: -1000
            to: 1000
            step: 1
            precision: 0
            suffix: "层"
            textFiled.width: 40
            Connections {
                id: conn_1
                target: spinBox_layerOffset
                enabled: false
                function onValueChanged() {
                    obj.z = combobox_zlayer.getBaseZ()+spinBox_layerOffset.value
                    SoupleManager.sendCommandToData(data_id,
                                Helper_Type.Z_UP,obj.z)
                }
            }
        }
    }
}
