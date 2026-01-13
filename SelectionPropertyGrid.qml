import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material
import com.custom 1.0

//选择属性栏
GridLayout {
    id: grid
    columns: 2
    columnSpacing: parent.width * 0.06

    property bool isSelecting

    Component.onCompleted: {
        isSelecting = SelectMgr.isSelecting()
        //spp.eenablefill = spp.eenablestroke = spp.efamily = spp.efillcolor
        //= spp.efontsize = spp.estrokecolor = spp.estrokewidth = spp.evtextalignmode
        //= spp.evtextalignoffset = false
    }

    Connections {
        target: Helper
        onSelectionStopButKeep: {
            //为了节约计算，当选择内容结束时，进行第一次内容计算。
            SelectMgr.staticProperty(spp) //统计属性
            isSelecting = false //选择结束
            console.log(spp.mfontsize,spp.efontsize,spp.fontsize)
        }
    }

    Text {
        text: "■ 选中内容 <font color='#B4E0BA'>■</font> <font color='#DBE0A2'>■</font> <font color='#E0D1C3'>■</font>";
        font.pixelSize: 16;
        Layout.bottomMargin: 10
        color: "#366080"
        Layout.columnSpan: 2
        Layout.alignment: Qt.AlignLeft
    }

    Flow {
        Layout.columnSpan: 2
        Layout.fillWidth: true
        Layout.bottomMargin: 15
        spacing: 12
        SquareButton {
            text: "复制"
            width: 40; height: 20
        }
        SquareButton {
            text: "剪贴"
            width: 40; height: 20
        }
        SquareButton {
            text: "删除"
            width: 40; height: 20
        }
        SquareButton {
            text: "粘贴"
            width: 40; height: 20
        }
    }

    LineText {
        Layout.columnSpan: 2
        Layout.fillWidth: true
        text: "字体信息"
    }

    Text { text: "字体"; font.pixelSize: 16 }
    RowLayout {
        Layout.preferredHeight: implicitHeight
        TextField {
            id: tf_font
            leftPadding: 5; rightPadding: 5
            topPadding: 5; bottomPadding: 5
            Layout.preferredWidth: grid.width * 0.35
            Layout.preferredHeight: 35
            font.pixelSize: 12
            Binding on text {
                when: spp.familyChanged || spp.mfamilyChanged || spp.efamilyChanged
                value: spp.mfamily ? "[多种]" : (spp.efamily ? spp.family : "[无]")
            }
            onTextChanged: {
                if(spp.efamily == false) { text = "[无]"; return }
                if(spp.mfamily) spp.mfamily = false
                spp.family = text
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


    Text { text: "字体大小"; font.pixelSize: 16 }
    MyDoubleSpinBox {
        id: spinbox_fontsize
        visible: spp.efontsize && ! spp.mfontsize
        step: 0.5
        precision: 1
        initialValue: spp.fontsize
        suffix: "pt"
        Binding on value {
            when: spp.fontsizeChanged || spp.efontsizeChanged || spp.mfontsizeChanged
            value: spp.fontsize
        }
        onValueChanged: {
            spp.fontsize = value
            SelectMgr.sendCommands(Helper_Type.SP_FontSize_Set,value)
        }
    }

    RowLayout {
        visible: ! spinbox_fontsize.visible
        SquareButton {
            visible: spp.mfontsize
            text: "+"
            onClicked: {
                SelectMgr.sendCommands(Helper_Type.SP_FontSize_Add,0.5)
            }
        }
        Text {
            font.pixelSize: 13
            text: spp.mfontsize ? "[多种]" : "[无]"
            color: spp.mfontsize ? "black" : "grey"
        }
        SquareButton {
            visible: spp.mfontsize
            text: "-"
            onClicked: {
                SelectMgr.sendCommands(Helper_Type.SP_FontSize_Add,-0.5)
            }
        }
    }

    LineText {
        Layout.columnSpan: 2
        Layout.fillWidth: true
        text: "文本属性"
    }

    CheckBox {
        id: cb_enable_stroke
        text: "开启描边"
        Layout.columnSpan: 2
        Component.onCompleted: checked = spp.menablestroke || spp.enablestroke && spp.eenablestroke
        Binding on checked {
            when: spp.enablestrokeChanged || spp.menablestrokeChanged || spp.eenablestrokeChanged
            value: spp.menablestroke || spp.enablestroke && spp.eenablestroke
        }
        enabled: spp.eenablestroke && spp.enablestroke || spp.menablestroke
        onCheckedChanged: {

        }
        Text {
            text: "[多种]"
            color: "#CF834C"
            x: parent.width
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 13
            visible: spp.menablestroke
        }
    }

    Text { visible: cb_enable_stroke.checked
        text: "描边宽度"; font.pixelSize: 16 }
    MyDoubleSpinBox {
        id: spinbox_strokeWidth
        visible: ! spp.mstrokewidth && spp.estrokewidth && cb_enable_stroke.checked
        step: 0.1
        precision: 3
        initialValue: 0
        suffix: "mm"
        Binding on value {
            when: spp.strokewidthChanged
            value: spp.strokewidth
        }
        onValueChanged: {

        }
    }

    RowLayout {
        visible: ! spinbox_strokeWidth.visible && cb_enable_stroke.checked
        SquareButton {
            visible: spp.mstrokewidth
            text: "+"
        }
        Text {
            font.pixelSize: 13
            text: spp.mstrokewidth ? "[多种]" : "[无]"
            color: spp.mstrokewidth ? "#CF834C" : "grey"
        }
        SquareButton {
            visible: spp.mstrokewidth
            text: "-"
        }
    }


    Text { visible: cb_enable_stroke.checked
        text: "描边颜色"; font.pixelSize: 16 }

    ColorSelector {
        visible: cb_enable_stroke.checked
        enabled: spp.estrokecolor || spp.mstrokecolor
        Binding on color {
            when: spp.estrokecolorChanged || spp.mstrokecolorChanged || spp.strokecolorChanged
            value: spp.strokecolor
        }
        onColorChanged: {

        }
        Text {
            text: "[多种]"
            color: "#CF834C"
            x: parent.width
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 13
            visible: spp.mstrokecolor
        }
    }

    CheckBox {
        id: cb_enable_fill
        text: "开启填充"
        Layout.columnSpan: 2
        onCheckedChanged: {

        }
    }

    Text { visible: cb_enable_fill.checked
        text: "填充颜色"; font.pixelSize: 16 }

    ColorSelector {
        visible: cb_enable_fill.checked
        onColorChanged: {

        }
    }


    CheckBox {
        text: "粗体"
        onCheckedChanged: {

        }
    }
    CheckBox {
        text: "斜体"
        onCheckedChanged: {

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

    LineText {
        Layout.columnSpan: 2
        Layout.fillWidth: true
        text: "内容统计"
    }

    LineText {
        Layout.columnSpan: 2
        Layout.fillWidth: true
        text: "文本替换"
    }

}
