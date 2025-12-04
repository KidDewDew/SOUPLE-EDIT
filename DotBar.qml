import QtQuick

//显示一堆点的进度条
Item {
    id: view
    property int full_num: 10
    property real show_num: 0
    property real spacing: 2
    property real radius: (width / full_num - spacing) / 2
    property color accent: "blue"
    property bool isStatic: true
    property int changeAnimationDuration: 3000
    property bool showBorder: true
    implicitWidth: 200
    implicitHeight: radius * 2 + spacing * 2
    onShow_numChanged: canvas.requestPaint()
    onFull_numChanged: canvas.requestPaint()
    Behavior on show_num {
        enabled: isStatic //只有静态进度条才允许属性动画
        NumberAnimation { duration: changeAnimationDuration }
    }

    Rectangle {
        id: border
        visible: showBorder
        anchors.fill: parent
        anchors.margins: -2
        border.width: 1
        border.color: accent
        radius: 5
        Rectangle {
            anchors.fill: parent
            anchors.margins: 2
            border.width: 1
            border.color: accent
            radius: 4
            //color: "#F0F0F0"
        }
    }
    Canvas {
        id: canvas
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0,0,width,height)
            ctx.fillStyle = Qt.lighter(accent,1.7)
            ctx.strokeStyle = accent
            var fs_num = Math.floor(show_num)
            var escale = show_num-fs_num
            for(var i = 0; i < full_num; ++i) {
                ctx.beginPath()
                if(i < fs_num)
                    ctx.arc(spacing*0.5+(radius*2+spacing)*i+radius, height/2, radius, 0, Math.PI*2, true)
                else if(i >= show_num)
                    ctx.arc(spacing*0.5+(radius*2+spacing)*i+radius, height/2, radius*0.6, 0, Math.PI*2, true)
                else
                    ctx.arc(spacing*0.5+(radius*2+spacing)*i+radius, height/2, radius*(escale*0.4+0.6), 0, Math.PI*2, true)
                ctx.closePath()
                ctx.stroke()
            }

            ctx.beginPath()

            for(var i = 0; i < show_num; ++i) {
                if(i < fs_num)
                    ctx.arc(spacing*0.5+(radius*2+spacing)*i+radius, height/2, radius*0.6, 0, Math.PI*2, true)
                else
                    ctx.arc(spacing*0.5+(radius*2+spacing)*i+radius, height/2, radius*(0.2+0.4*escale), 0, Math.PI*2, true)
            }
            ctx.closePath()
            ctx.fill()
        }
    }
}
