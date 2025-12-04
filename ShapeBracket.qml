import QtQuick

//大括号canvas
Canvas {
    id: canvas
    property color color: "black"
    enum Dir {
        Left,Right,Up,Down
    }
    property int dir: ShapeBracket.Dir.Left
    onPaint: {
        var ctx = canvas.getContext('2d')
        ctx.strokeStyle = color
        ctx.lineWidth = 1.0
        ctx.beginPath()
        const aw = Math.min(8,height*0.1)
        switch(dir) {
        case ShapeBracket.Dir.Left:
            ctx.moveTo(width,0)
            ctx.lineTo(width*0.4,aw)
            ctx.lineTo(width*0.4,height/2-aw*0.6)
            ctx.lineTo(0,height/2)
            ctx.lineTo(width*0.4,height/2+aw*0.6)
            ctx.lineTo(width*0.4,height-aw)
            ctx.lineTo(width,height)
            break
        }
        ctx.stroke()
    }
}
