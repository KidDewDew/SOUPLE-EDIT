import QtQuick

Rectangle {
    id: bar
    property color accent
    property color accent2: Qt.lighter(accent,1.7)
    property real value
    property int numRect: 20
    property int showNum
    border.color: "#A3A3A3"
    border.width: 1
    onValueChanged: showNum = value * numRect
    function mixColors(color1, color2, ratio) {
        return Qt.rgba(
            color1.r * (1 - ratio) + color2.r * ratio,
            color1.g * (1 - ratio) + color2.g * ratio,
            color1.b * (1 - ratio) + color2.b * ratio,
            color1.a * (1 - ratio) + color2.a * ratio
        )
    }
    // 使用示例
    //mixedColor: mixColors("red", "blue", 0.5)  // 混合红色和蓝色，各占50%
    Repeater {
        model: numRect
        Rectangle {
            //visible: index < showNum
            color: index < showNum ? mixColors(accent,accent2,1.0*index/(numRect-1)) : "#E6E6E6"
            x: index*(bar.width/numRect) +1
            y: 2
            width: bar.width/numRect - 2
            height: bar.height - 4
        }
    }
}
