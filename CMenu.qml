import QtQuick
import QtQuick.Controls.Material

//自定义Menu

Menu {
    id: menu
    topPadding: 2
    bottomPadding: 2
    leftPadding: 2
    property color accent: "#204D87"
    delegate: MenuItem {
        id: menuItem
        onHighlightedChanged: {
            arrow_canvas.requestPaint()
        }
        arrow: Canvas {
            id: arrow_canvas
            x: parent.width - width
            implicitWidth: 40
            implicitHeight: 30
            visible: menuItem.subMenu
            onPaint: {
                var ctx = getContext("2d")
                ctx.fillStyle = menuItem.highlighted ? "#ffffff" : accent
                ctx.moveTo(18, 10)
                ctx.lineTo(width - 15, height / 2)
                ctx.lineTo(18, height - 10)
                ctx.closePath()
                ctx.fill()
            }
        }
        contentItem: Row {
            Image {
                width: 16
                height: 16
                source: menuItem.icon.source
                fillMode: Image.PreserveAspectFit
                visible: menuItem.icon.source != null && menuItem.icon.source != ""
            }
            Text {
                leftPadding: menuItem.indicator.width
                rightPadding: menuItem.arrow.width
                text: menuItem.text
                font: menuItem.font
                opacity: enabled ? 1.0 : 0.3
                color: menuItem.highlighted ? "#ffffff" : accent
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
        }
        background: Rectangle {
            implicitWidth: 200
            implicitHeight: 30
            opacity: enabled ? 1 : 0.3
            color: menuItem.highlighted ? accent : "transparent"
        }
    }
    background: Rectangle {
        implicitWidth: 160
        implicitHeight: 30
        color: "#ffffff"
        border.color: accent
        radius: 2
    }
}
