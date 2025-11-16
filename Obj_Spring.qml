import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import com.custom 1.0

// 弹簧----
Rectangle {
    id: obj
    property int data_id
    property alias cp_propertyBar: cp_propertyBar
    property real radius
    property real lineWidth
    property real spacing
    property int showType
    Component {
        id: cp_propertyBar
        GridLayout {
            columns: 2
            columnSpacing: parent.width * 0.1
            rowSpacing: 8
            Text { text: "元素类别"; font.pixelSize: 16 }
            Text { text: "弹簧"; font.pixelSize: 16 }
        }
    }

    onRadiusChanged: {
        if(showType == 0) {
            canvas.requestPaint()
        }
    }

    onSpacingChanged: {
        if(showType == 0) {
            canvas.requestPaint()
        }
    }

    Canvas {
        id: canvas
        x:0
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        height: Math.max(lineWidth,radius*2)+8
        renderStrategy: Canvas.Cooperative
        onPaint: {
            var ctx = getContext('2d')
            let draw_x = 0.0
            ctx.clearRect(0,0,width,height)
            ctx.fillStyle = "black"
            //ctx.fillRect(0,0,width,height)
            console.log("radius:",radius)
            switch(obj.showType) {
            case 0: //点填充
                draw_x = spacing * 0.5
                while(draw_x+radius*2 <= width) {
                    ctx.ellipse(draw_x+radius,height*0.5,
                                radius*2,radius*2)
                    draw_x += radius*2+spacing
                }
                break;
            }
            ctx.fill()
        }
    }
}
