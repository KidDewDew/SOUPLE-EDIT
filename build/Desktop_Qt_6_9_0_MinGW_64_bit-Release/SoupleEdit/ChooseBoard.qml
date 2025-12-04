import QtQuick
import QtQuick.Layouts
import QtQuick.Shapes
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import com.custom 1.0

Item {
    id: anchor_point
    property int anchor_dir
    property var callback
    property int parent_id  //隶属于哪个对象？

    NumberAnimation {
        id: ani_appear
        property: "opacity"
        target: anchor_point
        from: 0.0; to: 1.0
        duration: 300
    }

    NumberAnimation {
        id: ani_disappear
        property: "opacity"
        target: anchor_point
        to: 0.0
        duration: 500
        onFinished: {
            anchor_point.visible = false
        }
    }

    function hide() {
        ani_disappear.start()
    }

    function show(id,anchor_position,anchor_dir,question,options,callback) {
        if(anchor_point.visible == false) {
            anchor_point.visible = true
            ani_appear.start()
            if(ani_disappear.running) ani_disappear.stop()
        }
        parent_id = id
        anchor_point.x = anchor_position.x
        anchor_point.y = anchor_position.y
        text_question.text = question
        option_repeater.model = options
        anchor_point.callback = callback
        var cx = 0, cy = 0
        if(anchor_dir&Helper_Type.Dir_LEFT) //左
            cx -= board.width*0.8
        if(anchor_dir&Helper_Type.Dir_RIGHT) //右
            cx += board.width*0.8
        if(anchor_dir&Helper_Type.Dir_TOP) //上
            cy -= board.height
        if(anchor_dir&Helper_Type.Dir_BOTTOM) //下
            cy += board.height
        board.x = cx - board.width/2
        board.y = cy - board.height/2
    }

    DropShadow {
        anchors.fill: board
        source: board
        radius: 4
        color:"#40000000"
    }

    Canvas {
        id: board
        property int centerX: x+width/2
        property int centerY: y+height/2
        width: board_layout.width + 16
        height: board_layout.height + 12
        onPaint: {
            var ctx = getContext('2d')
            ctx.beginPath()
            ctx.moveTo(8,1)
            ctx.lineTo(width-1,1)
            ctx.lineTo(width-1,height-1)
            ctx.lineTo(1,height-1)
            ctx.lineTo(1,8)
            ctx.lineTo(8,1)
            ctx.strokeStyle = "#946A39"
            ctx.fillStyle = "white"
            ctx.fill()
            ctx.stroke()
        }

        MouseArea {
            anchors.fill: parent
            drag.target: board
        }

        SingleCharButton {
            text: "×"
            ori_color: "black"
            hover_color: "grey"
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.topMargin: 1
            anchors.rightMargin: 1
            topPadding: 0; leftPadding: 0
            bottomPadding: 0; rightPadding: 0
            onClicked: {
                hide()
            }
        }

        ColumnLayout {
            id: board_layout
            anchors.centerIn: parent
            Text {
                id: text_question
                Layout.rightMargin: 12
                color: "#CC4564"
                maximumLineCount: 2
                Layout.maximumWidth: 150
                font.pixelSize: 15
                wrapMode: Text.WordWrap
            }
            RowLayout {
                Repeater {
                    id: option_repeater
                    delegate: ToolButton {
                        text: modelData
                        Material.foreground: "#D4B332"
                        Layout.preferredHeight: 28
                        Layout.preferredWidth: modelData.length * 20
                    }
                }
            }
        }
    }

    // onPaint: {
    //     var ctx = getContext('2d')
    //     ctx.beginPath()
    //     ctx.moveTo(0,0)
    //     ctx.lineTo(board_point.centerX,board_point.centerY)
    //     ctx.strokeStyle = "#ABCC90"
    //     ctx.setLineDash([4,4])
    //     ctx.stroke()
    // }

    Shape {
        ShapePath {
            strokeWidth: 1.8
            strokeColor: "#CC4564"
            strokeStyle: ShapePath.DashLine
            fillColor: "transparent"
            dashPattern: [2,4]
            startX: 0; startY: 0
            PathLine { x: board_point.x; y: board_point.y }
            // PathCubic {
            //     x: board_point.x; y: board_point.y
            //     control1X: x/3; control1Y: y
            //     control2X: x*2/3; control2Y: 0
            // }
        }
    }

    Rectangle {
        x: -2; y: -2
        width: 4; height: 4
        radius: 2
        border.color: "black"
    }

    Rectangle {
        id: board_point
        property int centerX: (board.centerY < 0 || board.centerX < 0) ?
                    board.x+board.width-6 : board.x+6
        property int centerY: board.centerY < 0 ?
                    board.y+board.height-6 : board.y+6
        x: centerX-2
        y: centerY-2
        width: 4; height: 4
        radius: 2
        border.color: "black"
    }

}
