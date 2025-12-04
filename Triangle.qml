import QtQuick

Canvas {
    id: canvas
    enum Dir {
        Left,Right,Up,Down
    }
    property int dir: Dir.Right
    property color fillColor
    onDirChanged: requestPaint()
    onPaint: {
        var ctx = canvas.getContext('2d')
        ctx.fillStyle = fillColor
        ctx.clearRect(0,0,width,height)
        ctx.beginPath()
        switch(dir) {
        case Triangle.Dir.Left: //左三角
            ctx.moveTo(0,height/2)
            ctx.lineTo(width,0)
            ctx.lineTo(width,height)
            ctx.lineTo(0,height/2)
            break
        case Triangle.Dir.Right: //右三角
            ctx.lineTo(width,height/2)
            ctx.lineTo(0,height)
            ctx.lineTo(0,0)
            break
        case Triangle.Dir.Down: //下三角
            ctx.lineTo(width,0)
            ctx.lineTo(width/2,height)
            ctx.lineTo(0,0)
            break
        case Triangle.Dir.Up: //上三角
            ctx.moveTo(width/2,0)
            ctx.lineTo(width,height)
            ctx.lineTo(0,height)
            ctx.lineTo(width/2,0)
            break
        }
        ctx.fill()
    }
}
