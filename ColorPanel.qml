// ColorPicker.qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Rectangle {
    id: colorPicker
    implicitWidth: 216
    implicitHeight: 290
    color: "#f0f0f0"
    radius: 8
    border.color: "#cccccc"
    border.width: 1

    // 属性定义
    property int rows: 6
    property int columns: 8
    property int colorItemSize: 20
    property int spacing: 4
    property list<color> recentColors: []
    property list<color> presetColors: [
        "#FFFFFF", "#808080", "#CCD4FF", "#DFFFF7", "#C9FFC7", "#FFF9D5", "#FFE0E0", "#FFDBB0",
        "#EDEDED", "#696969", "#8CA6FF", "#B8FFE5", "#97F090", "#FFF4AE", "#FF9A9A", "#FFBF7A",
        "#D4D4D4", "#525252", "#5379FF", "#40E0D0", "#7DF06D", "#FFEC82", "#FF5C5C", "#FFAB27",
        "#BFBFBF", "#383838", "#2666FF", "#48B089", "#3EDE3D", "#FFE854", "#FF2626", "#805204",
        "#ABABAB", "#262626", "#0A35FF", "#207556", "#188C14", "#A39523", "#DE0000", "#61370E",
        "#949494", "#000000", "#000482", "#07362C", "#004000", "#615503", "#8C0000", "#331D08"
    ]

    // 信号
    signal colorSelected(color selectedColor)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        // 最近使用的颜色标题
        Text {
            text: qsTr("最近使用")
            font.bold: true
            font.pixelSize: 14
            color: "#333333"
            Layout.alignment: Qt.AlignLeft
        }

        // 最近使用的颜色网格
        GridLayout {
            id: recentColorsGrid
            Layout.fillWidth: true
            rows: 1
            columns: colorPicker.columns
            rowSpacing: colorPicker.spacing
            columnSpacing: colorPicker.spacing

            Repeater {
                id: repeater_recentColor
                model: colorPicker.columns
                Rectangle {
                    width: colorPicker.colorItemSize
                    height: colorPicker.colorItemSize
                    radius: 4
                    border.color: "#cccccc"
                    border.width: 1
                    color: index < colorPicker.recentColors.length ? colorPicker.recentColors[index] : "transparent"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (index < colorPicker.recentColors.length) {
                                colorPicker.colorSelected(colorPicker.recentColors[index])
                            }
                        }
                    }

                    // 空状态的提示文字
                    Text {
                        anchors.centerIn: parent
                        text: "+"
                        font.pixelSize: 20
                        color: "#999999"
                        visible: index >= colorPicker.recentColors.length
                    }
                }
            }
        }

        // 常用颜色标题
        Text {
            text: qsTr("常用颜色")
            font.bold: true
            font.pixelSize: 14
            color: "#333333"
            Layout.alignment: Qt.AlignLeft
            Layout.topMargin: 5
        }

        // 常用颜色网格
        GridLayout {
            id: presetColorsGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            rows: colorPicker.rows
            columns: colorPicker.columns
            rowSpacing: colorPicker.spacing
            columnSpacing: colorPicker.spacing

            Repeater {
                model: colorPicker.rows * colorPicker.columns
                Rectangle {
                    id: colorItem
                    width: colorPicker.colorItemSize
                    height: colorPicker.colorItemSize
                    radius: 4
                    border.color: "#cccccc"
                    border.width: 1
                    color: index < colorPicker.presetColors.length ? colorPicker.presetColors[index] : "transparent"

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            if (index < colorPicker.presetColors.length) {
                                const selectedColor = colorPicker.presetColors[index]
                                colorPicker.colorSelected(selectedColor)
                                addToRecentColors(selectedColor)
                            }
                        }
                        onEntered: {
                            if (index < colorPicker.presetColors.length) {
                                tooltip.text = colorPicker.presetColors[index]
                                tooltip.visible = true
                            }
                        }
                        onExited: {
                            tooltip.visible = false
                        }
                    }

                    // 颜色提示工具
                    ToolTip {
                        id: tooltip
                        delay: 500
                        timeout: 3000
                        visible: false
                    }
                }
            }
        }

        // 底部按钮
        Button {
            id: moreColorsButton
            text: qsTr("选取更多颜色...")
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 10
            flat: true

            contentItem: Text {
                text: moreColorsButton.text
                font.pixelSize: 14
                color: "#0066cc"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
                color: "transparent"
                radius: 4
                border.width: 1
                border.color: moreColorsButton.hovered ? "#0066cc" : "transparent"
            }

            onClicked: {
                colorDialog.open()
            }
        }
    }

    // 颜色选择对话框
    ColorDialog {
        id: colorDialog
        title: qsTr("选择颜色")

        onAccepted: {
            const selectedColor = colorDialog.selectedColor
            colorPicker.colorSelected(selectedColor)
            addToRecentColors(selectedColor)
        }
    }

    // 添加到最近使用颜色列表
    function addToRecentColors(color) {
        // 如果颜色已经在列表中，先移除
        var index = recentColors.indexOf(color)
        if (index !== -1) {
            recentColors.splice(index, 1)
        }

        // 添加到列表开头
        recentColors.unshift(color)

        // 限制最近使用颜色数量
        if (recentColors.length > columns) {
            recentColors = recentColors.slice(0, columns)
        }

        // 更新UI
        repeater_recentColor.model--
        repeater_recentColor.model++
    }

    // 初始化函数
    function initialize() {
        // 可以在这里添加初始化逻辑
        // 例如从设置中加载最近使用的颜色
    }

    Component.onCompleted: {
        initialize()
    }
}
