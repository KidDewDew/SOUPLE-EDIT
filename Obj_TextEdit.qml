import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import com.custom 1.0

S_TextInput {
    id: obj
    property bool bFill: true
    property bool bStroke: false
    property real sWidth: 0.0 //描边宽度
    property color sColor //描边颜色
    property bool isSelected

    Behavior on y {
        NumberAnimation {
            duration: 400;
            easing.type: Easing.InOutQuad
        }
    }

    function die() {
        obj.destroy()
    }

    function setLetterSpacing(spacing) {
        font.letterSpacing = spacing
    }

    //color: 填充颜色
    // Binding on color {
    //     when: !bFill
    //     value: "#30000000"
    // }

    //style: Text.Outline

    // [2026/1/12 删除“流文本”的属性框]
    // Timer {
    //     id: timer_setSelected
    //     interval: 200
    //     onTriggered: if(focus) selectedObj = obj
    // }

    onFocusChanged: {
        if(focus) {
            selectedObj = obj
            SoupleManager.sendCommandToData(data_id,Helper_Type.CURSOR_CHANGE,cursorPosition)
        }
    }

    // 通知光标发生改变
    onCursorPositionChanged: {
        if(focus) SoupleManager.sendCommandToData(data_id,Helper_Type.CURSOR_CHANGE,cursorPosition)
    }


    function cpp_updateText(new_text) {
        const old_cursor = cursorPosition
        text = new_text
        if(focus) {
            cursorPosition = old_cursor
        }
    }

    //cpp_xxx为c++端通知ui的自定义函数
    function cpp_overflow(new_text,overflow_textEdit) { //溢出
        const old_cursor = cursorPosition
        text = new_text
        if(focus) { //如果有焦点
            if(old_cursor <= text.length) cursorPosition = old_cursor
            else { //光标需要转移到overflow_textEdit
                console.log("**********")
                if(overflow_textEdit != null) { //ui对象被创建了
                    overflow_textEdit.forceActiveFocus()
                    overflow_textEdit.cursorPosition = old_cursor - text.length
                }
            }
        }
    }

    //自定义insertNewObj，当插入anchor_obj时被调用
    function insertNewObj(objName) {
        return 0 === SoupleManager.sendCommandToData(data_id,Helper_Type.INSERTOBJ,objName)
    }


    onTextChanged: {
        SoupleManager.sendCommandToData(data_id,Helper_Type.TEXT_UP,text)
    }

    Keys.onPressed: function(e){
                if(e.key == Qt.Key_Backspace) { //退格键
                    if(cursorPosition == 0) { //光标位置为 0
                        SoupleManager.sendCommandToData(data_id,Helper_Type.GOTOLEFT,null)
                        if(length == 0) {
                            //请求删除对象
                            SoupleManager.requestRemoveObj(data_id)
                        }
                    }
                } else if(e.key == Qt.Key_Enter || e.key == Qt.Key_Return) { //回车键
                    //请求后端处理回车
                    SoupleManager.sendCommandToData(data_id,Helper_Type.KEY_RETURN,null)
                    //创建右占位符
                    //insertOnRight(cp_phright.createObject(soupleEdit))
                    //创建接续文本框
                    //rightObj.insertOnRight(cp_obj_textedit.createObject(soupleEdit))
                    //rightObj.getCursorFromLeft() //转移光标
                }
                e.accepted = false //不过滤
            }


    //property alias cp_propertyBar: cp_propertyBar
    property var cp_propertyBar: null

    Component {
        id: cp_propertyBar //属性栏
        GridLayout {
            id: propertyBar
            columns: 2
            columnSpacing: 2
            TText {
                text: "元素类别";
            }
            RowLayout {
                Text {
                    text: "流文本";
                    font.pixelSize: 16
                    color: theme.fg
                }
                RectButton {
                    Layout.alignment: Qt.AlignVCenter
                    text: "?"
                    accent: "#C9A350"
                    ToolTip {
                        visible: parent.hovered
                        text: "流文本是什么?"
                    }
                }
            }
            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "锚定信息"
            }
            Text { text: "垂直对齐"; font.pixelSize: 16; color: theme.fg }
            ComboBox {
                model: ["中心对齐","基线对齐","下对齐","上对齐"]
            }

            Text { text: "垂直偏移"; font.pixelSize: 16; color: theme.fg }
            MyDoubleSpinBox {
                step: 0.1
                precision: 1
                initialValue: Helper.pixel2cm(SoupleManager.qmlGetData(data_id,Helper_Type.VALIGNOFFSET))
                suffix: "cm"
                onValueChanged: {
                    SoupleManager.sendCommandToData(data_id,Helper_Type.VALIGNOFFSET_UP,Helper.cm2pixel(value))
                }
            }

            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "文字信息"
            }
            Text { text: "文本"; font.pixelSize: 16; color: theme.fg }
            TextField {
                leftPadding: 5; rightPadding: 5
                topPadding: 5; bottomPadding: 5
                Layout.preferredHeight: 35
                Component.onCompleted: text = obj.text
                font.pixelSize: 10
                Binding on text {
                    when: obj.textChanged
                    value: obj.text
                }
                onTextChanged: {
                    SoupleManager.sendCommandToData(data_id,Helper_Type.TEXT_UP,text)
                    //console.log("Command = ",Helper.TEXT_UP)
                    obj.text = text
                }
            }


            Text { text: "字体"; font.pixelSize: 16;
                Layout.alignment: Qt.AlignVCenter;
                color: theme.fg
            }
            RowLayout {
                Layout.alignment: Qt.AlignVCenter
                Layout.preferredHeight: implicitHeight
                TextField {
                    id: tf_font
                    leftPadding: 5; rightPadding: 5
                    topPadding: 5; bottomPadding: 5
                    Layout.preferredHeight: 35
                    font.pixelSize: 12
                    Component.onCompleted: text = obj.font.family
                    onTextChanged: {
                        //obj.font.family = text
                        SoupleManager.sendCommandToData(data_id,Helper_Type.FONT_FAMILY_UP,text)
                    }
                }
                RectButton {
                    Layout.preferredWidth: 40
                    Layout.preferredHeight: tf_font.height - 6
                    text: "选择"
                    accent: "#26A16D"
                    onClicked: {
                        font_dialog.open()
                        font_dialog.callback = function() {
                            tf_font.text = font_dialog.selectedFont.family
                        }
                    }
                }
            }


            Text {
                text: "字体大小"; font.pixelSize: 16
                color: theme.fg
            }
            MyDoubleSpinBox {
                step: 0.5
                precision: 1
                initialValue: obj.font.pointSize
                suffix: "pt"
                Binding on value {
                    when: obj.font.pointSizeChanged
                    value: obj.font.pointSize
                }
                onValueChanged: {
                    obj.font.pointSize = value
                    SoupleManager.sendCommandToData(data_id,Helper_Type.FONT_SIZE_UP,value)
                }
            }

            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "文字属性"
            }

            CheckBox {
                id: cb_enable_stroke
                text: "开启描边"
                Component.onCompleted: checked = obj.bStroke
                Layout.columnSpan: 2
                onCheckedChanged: {
                    obj.bStroke = checked
                    SoupleManager.sendCommandToData(data_id,Helper_Type.BOOL_STROKE_UP,checked)
                }
                Binding on checked {
                    when: obj.bStrokeChanged
                    value: obj.bStroke
                }
            }

            Text { visible: cb_enable_stroke.checked
                text: "描边宽度"; font.pixelSize: 16 }
            MyDoubleSpinBox {
                 visible: cb_enable_stroke.checked
                step: 0.1
                precision: 3
                initialValue: Helper.pixel2cm(obj.sWidth) * 10
                suffix: "mm"
                Binding on value {
                    when: obj.sWidthChanged
                    value: Helper.pixel2cm(obj.sWidth) * 10
                }
                onValueChanged: {
                    obj.sWidth = Helper.cm2pixel(value*0.1)
                    SoupleManager.sendCommandToData(data_id,Helper_Type.STROKE_WIDTH_UP,obj.sWidth)
                }
            }

            TText {
                visible: cb_enable_stroke.checked
                text: "描边颜色";
            }

            ColorSelector {
                visible: cb_enable_stroke.checked
                Component.onCompleted: color = obj.sColor
                Binding on color {
                    when: obj.sColorChanged
                    value: obj.sColor
                }
                onColorChanged: {
                    obj.sColor = color
                    SoupleManager.sendCommandToData(data_id,Helper_Type.STROKE_COLOR_UP,color)
                }
            }

            CheckBox {
                id: cb_enable_fill
                text: "开启填充"
                Component.onCompleted: checked = obj.bFill
                Layout.columnSpan: 2
                onCheckedChanged: {
                    obj.bFill = checked
                    SoupleManager.sendCommandToData(data_id,Helper_Type.BOOL_FILL_UP,checked)
                }
                Binding on checked {
                    when: obj.bFillChanged
                    value: obj.bFill
                }
            }

            TText {
                visible: cb_enable_fill.checked
                text: "填充颜色";
            }

            ColorSelector {
                visible: cb_enable_fill.checked
                Component.onCompleted: color = obj.color
                Binding on color {
                    when: obj.colorChanged
                    value: obj.color
                }
                onColorChanged: {
                    obj.color = color
                    SoupleManager.sendCommandToData(data_id,Helper_Type.STROKE_COLOR_UP,color)
                }
            }


            CheckBox {
                text: "粗体"
                Component.onCompleted: checked = obj.font.bold
                onCheckedChanged: {
                    obj.font.bold = checked
                    SoupleManager.sendCommandToData(data_id,Helper_Type.FONT_BOLD_UP,checked)
                }
                Binding on checked {
                    when: obj.font.boldChanged
                    value: obj.font.bold
                }
            }
            CheckBox {
                text: "斜体"
                Component.onCompleted: checked = obj.font.italic
                onCheckedChanged: {
                    obj.font.italic = checked
                    SoupleManager.sendCommandToData(data_id,Helper_Type.FONT_ITALIC_UP,checked)
                }
                Binding on checked {
                    when: obj.font.italicChanged
                    value: obj.font.italic
                }
            }
            CheckBox {
                text: "下划线"
                Component.onCompleted: checked = obj.font.underline
                onCheckedChanged: obj.font.underline = checked
                Binding on checked {
                    when: obj.font.underlineChanged
                    value: obj.font.underline
                }
            }
            CheckBox {
                text: "上划线"
                Component.onCompleted: checked = obj.font.overline
                onCheckedChanged: obj.font.overline = checked
                Binding on checked {
                    when: obj.font.overlineChanged
                    value: obj.font.overline
                }
            }
            CheckBox {
                text: "删除线"
                Component.onCompleted: checked = obj.font.strikeout
                onCheckedChanged: obj.font.strikeout = checked
                Binding on checked {
                    when: obj.font.strikeoutChanged
                    value: obj.font.strikeout
                }
            }
        }
    }

    function getCursorFromRight() { //从右边获取光标
        textEdit.forceActiveFocus()
        textEdit.cursorPosition = textEdit.length
    }

    function getCursorFromLeft() { //从左边获取光标
        textEdit.forceActiveFocus()
        textEdit.cursorPosition = 0
    }
    // Rectangle {
    //     visible: width > 0.1
    //     color: "#66DE87"
    //     height: 0.8
    //     anchors.horizontalCenter: parent.horizontalCenter
    //     anchors.bottom: parent.bottom
    //     anchors.bottomMargin: -2
    //     width: parent.focus ? parent.width : 0
    //     Behavior on width { NumberAnimation { duration: 500; easing.type: Easing.InOutQuad } }
    // }

    //使用Glow发光效果来实现描边
    //注：glow底层采用着色器实现，即gpu方式实现，效率很高
    //layer.enabled: bStroke && bFill
    // layer.effect: Glow {
    //     cached: false
    //     radius: sWidth*0.2 //
    //     samples: 2 //
    //     color: sColor
    //     spread: sWidth
    // }
    //layer.effect: bStroke ? cp_glow : null

    //layer.enabled: bStroke && bFill
    //layer.effect: bStroke ? cp_glow : null

    // Component {
    //     id: cp_stroke
    //     ShaderEffect_Stroke {
    //         strokeColor: obj.sColor
    //         strokeWidth: obj.sWidth
    //     }
    // }

    Component {
        id: cp_glow
        Glow {
            cached: false
            radius: sWidth*0.2 //
            samples: 2 //
            color: sColor
            spread: sWidth
        }
    }

}
