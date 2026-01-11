import QtQuick
import QtQuick.Controls
//鉴于qml自带的Shape存在内存泄漏，这里实现一个虚线
Canvas {
    id: canvas
    property color color
    property real lineWidth: 1.0
    property bool isHorizontal: true
    onColorChanged: canvas.requestPaint()
    onPaint: {
        var ctx = getContext("2d")
        ctx.lineWidth = lineWidth
        ctx.beginPath()
        if(isHorizontal) {
            ctx.moveTo(0,height/2)
            ctx.lineTo(width,height/2)
        } else {
            ctx.moveTo(width/2,0)
            ctx.lineTo(width/2,height)
        }

        ctx.strokeStyle = color
        ctx.setLineDash([4,4])
        ctx.stroke()
    }
}
