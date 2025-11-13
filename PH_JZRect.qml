import QtQuick

//居中占位矩形
//与PH_Rect类似，但它可以自动调整宽度，使得右边的所有(solid)obj自动居中
Canvas {
    id: obj
    height: 16
    property int data_id

    function selectw() {}

    function selectp(x1,x2) {}

    function selectcl() {}

    function die() {
        obj.destroy()
    }

    //绘制双向箭头
    onPaint: {
        var ctx = getContext("2d")
        ctx.beginPath()
        ctx.strokeStyle = "#F72056"
        ctx.lineWidth = 1.5
        const aw = Math.min(width*0.15,12)
        ctx.moveTo(width*0.4,height/2)
        ctx.lineTo(1.5,height/2)
        ctx.lineTo(aw,height*0.2)
        ctx.moveTo(1.5,height/2)
        ctx.lineTo(aw,height*0.8)
        ctx.stroke()

        ctx.beginPath()
        ctx.moveTo(width*0.6,height/2)
        ctx.lineTo(width-1.5,height/2)
        ctx.lineTo(width - aw,height*0.2)
        ctx.moveTo(width-1.5,height/2)
        ctx.lineTo(width - aw,height*0.8)
        ctx.stroke()
    }
}
