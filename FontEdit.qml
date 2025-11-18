import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls
// FontEdit 把字体的设置封装到一个块内
Control {
    id: view
    //implicitWidth: 260
    //implicitHeight: 215
    property font currentFont
    horizontalPadding: 8
    verticalPadding: 6
    Component.onCompleted: {
        combo_fonts.model = Helper.getFontFamilyList()
    }

    onCurrentFontChanged: {
        cb_bold.checked = currentFont.bold
        cb_italic.checked = currentFont.italic
        cb_underline.checked = currentFont.underline
        cb_deleteline.checked = currentFont.strikeout
        sb_pointsize.value = currentFont.pointSize
    }


    background: Rectangle {
        color: theme.bg
        border.color: theme.accent_light
        radius: 6
    }
    contentItem: GridLayout {
        id: gridLayout
        //anchors.fill: parent
        columns: 2
        TText {
            text: "“字体&123”"
            font: view.currentFont
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumWidth: gridLayout.width*0.9
            wrapMode: Text.WrapAnywhere
        }
        TText {
           font.pointSize: 11
           text: "字体族"
        }
        TCombo {
            id: combo_fonts
        }
        TText {
           font.pointSize: 11
           text: "字体大小"
        }
        MyDoubleSpinBox {
            id: sb_pointsize
            step: 0.5
            suffix: "pt"
            precision:1
            from:1.0
            to:128.0
            initialValue: currentFont.pointSize
            onValueChanged: currentFont.pointSize = value
        }
        CheckBox {
            id: cb_bold
            text: "粗体"
            onCheckedChanged: currentFont.bold = checked
        }
        CheckBox {
            id: cb_italic
            text: "斜体"
            onCheckedChanged: currentFont.italic = checked
        }
        CheckBox {
            id: cb_underline
            text: "下划线"
            onCheckedChanged: currentFont.underline = checked
        }
        CheckBox {
            id: cb_deleteline
            text: "删除线"
            onCheckedChanged: currentFont.strikeout = checked
        }
    }
}
