import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts

// 带有加减按钮、数值label的slider
RowLayout {
    id: layout
    property alias slider: slider
    property string suffix
    property int precision:1
    property real value
    onValueChanged: slider.value = value
    spacing:0
    Binding on value {
        when: slider.valueChanged
        value: slider.value
    }
    RoundButton {
        text: "+"
        onClicked: slider.value += slider.stepSize
        font.pointSize:13
        radius: 10
        highlighted: true
        Layout.preferredWidth: 36
        Layout.preferredHeight: 36
        Material.accent: "#BA9D46"
    }
    Slider {
        id: slider
        Layout.fillWidth:true
        Material.accent:"#8F2A2C"
    }
    TText {
        text: `${slider.value.toFixed(precision)}${suffix}`
        font.pointSize: 11
    }
    RoundButton {
        text: "-"
        onClicked: slider.value -= slider.stepSize
        font.pointSize:13
        radius: 10
        highlighted: true
        Layout.preferredWidth: 36
        Layout.preferredHeight: 36
        Material.accent: "#943A3B"
    }
}
