import QtQuick

//水平标尺
Canvas {
    id: canvas
    property real float_x
    property bool show_float: false
    property real xScale: 1.0
    onFloat_xChanged: requestPaint()
    onShow_floatChanged: requestPaint()
    onXScaleChanged: requestPaint()
    property color lineColor: show_float ? '#2C2C70' : '#919191'
    Behavior on lineColor {
        ColorAnimation { duration: 300 }
    }
    onLineColorChanged: requestPaint()
    onPaint: {
        const mm_width = Helper.cm2pixel(1.0)*0.1*xScale //1毫米 = mm_width像素
        var ctx = getContext('2d')

        ctx.fillStyle = '#ffffff'
        ctx.beginPath()
        ctx.roundedRect(0,0,width,height,4,4)
        ctx.fill()

        ctx.beginPath()
        ctx.strokeStyle = lineColor
        ctx.fillStyle = lineColor
        ctx.lineWidth = 0.8
        ctx.font = '8px Arial'
        for(var i = 1; i < 9999; ++i) {
            const xi = i * mm_width
            if(xi+0.5 > width) break
            ctx.moveTo(xi,0)
            if(i%5 == 0) {
                ctx.lineTo(xi,height * 0.4)
                if(i%10 == 0){
                    ctx.fillText(`${i/10}`,xi-3,height*0.4+6)
                }
            } else ctx.lineTo(xi,height * 0.25)
        }

        ctx.stroke()

        if(show_float) { //显示浮标
            ctx.beginPath()
            ctx.lineWidth = 2.0
            ctx.strokeStyle = '#BD494B'
            ctx.moveTo(float_x*xScale,0)
            ctx.lineTo(float_x*xScale,height)
            ctx.stroke()
        }
    }
}
