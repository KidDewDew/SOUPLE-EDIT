import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Controls.Windows as Windows
//import Qt.labs.platform as Platform
import Qt5Compat.GraphicalEffects
import QtQuick.Dialogs as Dialogs
import QtQuick.Effects
import com.custom 1.0

RowLayout { //“开始”工具栏

    Item { implicitWidth: 8 }

    Text {
        text: "字体"; font.pixelSize: 11
        //color: combo_font.focus ? "#3BBD4A" : "#949494"
        color: "#949494"
        Layout.alignment: Qt.AlignVCenter
        //Behavior on color { ColorAnimation { duration: 400 } }
    }

    ComboBox {
        id: combo_font
        Layout.alignment: Qt.AlignVCenter
        Layout.preferredHeight: 26
        model: ["宋体","黑体","..."]
    }

    ComboBox_forFontSize {

    }

    ImageButton {
        implicitWidth: 24
        implicitHeight: 24
        source: isProtectEyeMode ? "qrc:/image/A_add_dark.png" : "qrc:/image/A_add_light.png"
    }

    ImageButton {
        implicitWidth: 24
        implicitHeight: 24
        source: isProtectEyeMode ? "qrc:/image/A_sub_dark.png" : "qrc:/image/A_sub_light.png"
    }

    // ColumnLayout {
    //     Layout.fillHeight: true
    //     spacing: 2
    //     Layout.leftMargin: 2-parent.spacing
    //     RectButton {
    //         text: "A<sup>+</sup>"
    //         label.font.pixelSize: 11
    //         label.textFormat: Text.RichText
    //         label.color: "#224726"
    //         label.font.bold: false
    //         bg.radius: 1
    //         bg.border.color: "grey"
    //         Layout.preferredHeight: 11
    //         Layout.preferredWidth: 18
    //         accent: "#F2F2F2"
    //     }
    //     RectButton {
    //         text: "A<sup>-</sup>"
    //         label.font.pixelSize: 11
    //         label.textFormat: Text.RichText
    //         label.color: "#472122"
    //         label.font.bold: false
    //         bg.radius: 1
    //         bg.border.color: "grey"
    //         Layout.preferredHeight: 11
    //         Layout.preferredWidth: 18
    //         accent: "#F2F2F2"
    //     }
    // }
    // Text {
    //     text: "字体属性"; font.pixelSize: 11
    //     color: "#949494"
    //     Layout.alignment: Qt.AlignVCenter
    // }

    Rectangle {
        Layout.preferredWidth: 0.8
        Layout.preferredHeight: 19
        Layout.leftMargin: 6
        Layout.rightMargin: 6
        color: theme.splitLine
    }

    Row {
        spacing: 4
        SelectedButton {
            label.text: "B"
            label.font.family: "Times New Roman"
            label.font.bold: true
            label.font.pixelSize: 14
            Layout.alignment: Qt.AlignVCenter
        }
        SelectedButton {
            label.text: "I"
            label.font.family: "Times New Roman"
            label.font.italic: true
            label.font.pixelSize: 14
            Layout.alignment: Qt.AlignVCenter
        }
        SelectedButton {
            label.text: "U"
            label.font.family: "Times New Roman"
            label.font.underline: true
            label.font.pixelSize: 14
            Layout.alignment: Qt.AlignVCenter
        }
        // SelectedButton {
        //     label.text: "U"
        //     label.font.family: "Times New Roman"
        //     label.font.overline: true
        //     label.font.pixelSize: 14
        //     Layout.alignment: Qt.AlignVCenter
        // }
        SelectedButton {
            label.text: "A"
            label.font.family: "Times New Roman"
            label.font.strikeout: true
            label.font.pixelSize: 14
            Layout.alignment: Qt.AlignVCenter
        }
        SelectedButton {
            label.text: "A"
            label.font.family: "Times New Roman"
            label.style: Text.Outline
            label.color: "white"
            label.font.pixelSize: 14
            Layout.alignment: Qt.AlignVCenter
        }

    }

    Text {
        text: "前景"; font.pixelSize: 11
        color: "#949494"
        Layout.alignment: Qt.AlignVCenter
    }
    ColorSelector {}
    Text {
        text: "背景"; font.pixelSize: 11
        color: "#949494"
        Layout.alignment: Qt.AlignVCenter
    }
    ColorSelector {}
    // ComboBox {
    //     model: 2
    // }
    Rectangle {
        Layout.preferredWidth: 0.8
        Layout.preferredHeight: 19
        Layout.leftMargin: 6
        Layout.rightMargin: 6
        color: theme.splitLine
    }
    MyToolButton {
        text: "查找/替换"
    }
    Rectangle {
        Layout.preferredWidth: 0.8
        Layout.preferredHeight: 19
        Layout.leftMargin: 6
        Layout.rightMargin: 6
        color: theme.splitLine
    }
    CheckBox {
        Material.accent: "#3C8761"
        text: "布局线"
        checked: false
        onCheckedChanged: {
            //soupleEdit.showHelpInf = checked
            SoupleManager.setShowHelpLine(checked)
        }
    }
    CheckBox {
        id: checkbox_protect_eye
        Material.accent: "#3C8761"
        text: "护眼模式"
    }
} //“开始”工具栏 END
