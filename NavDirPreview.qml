import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import com.custom 1.0

// 目录样式预览
Control {
    id: view
    property int showType   //0:无 1:点线 2:虚线 3:实线
    property real lineWidth
    property real radius
    //property real spacing
    property real levelTab
    property int levels
    property real dashWidth
    //property font font
    QtObject {
        id: d
        property list<int> level_list
    }

    function calcLevelList(level) {
        if(level > levels) return
        for(let i = 0; i < 2; ++i) {
            d.level_list.push(level)
            calcLevelList(level+1)
        }
    }

    onLevelsChanged: {
        //重新计算d.level_list
        repeater.model = null
        d.level_list.splice(0,d.level_list.length)
        calcLevelList(1)
        repeater.model = d.level_list
    }

    /* N = 2^1+2^2+...+2^levels = 2^(levels+1)-2
      一级标题.................1
         二级标题..............2
           三级标题............2
           三级标题............2
         二级标题..............2
           三级标题............2
           三级标题............2
      一级标题.................3
         二级标题..............2
           三级标题............2
           三级标题............2
         二级标题..............2
           三级标题............2
           三级标题............2
    */

    padding: 6
    background: Rectangle {
        color: theme.bg
    }

    contentItem: ColumnLayout {
        Repeater {
            id: repeater
            model: d.level_list
            delegate: RowLayout {
                Layout.fillWidth:true
                Text {
                    color:theme.fg
                    text: `${Helper.number2chinese(modelData)}级标题`
                    font: view.font
                    Layout.alignment: Qt.AlignBaseline
                    Layout.leftMargin: (modelData-1)*view.levelTab
                }
                Canvas {
                    id: canvas
                    Layout.fillWidth:true
                    Layout.alignment: Qt.AlignVCenter
                    implicitHeight: Math.max(lineWidth,radius*2)+8
                    renderStrategy: Canvas.Cooperative
                    Connections {
                        target: view
                        function onSpacingChanged() {
                            canvas.requestPaint()
                            console.log("spces:",view.spacing)
                        }
                        function onRadiusChanged() {
                            canvas.requestPaint()
                        }
                        function onShowTypeChanged() {
                            canvas.requestPaint()
                        }
                        function onDashWidthChanged() {
                            canvas.requestPaint()
                        }
                        function onLineWidthChanged() {
                            canvas.requestPaint()
                        }
                    }
                    onPaint: {
                        var ctx = getContext('2d')
                        let draw_x = 0.0
                        ctx.clearRect(0,0,width,height)
                        ctx.beginPath()
                        ctx.fillStyle = theme.fg
                        switch(view.showType) {
                        case Helper_Type.Dot_LinePattern: //点填充
                            draw_x = view.spacing * 0.5
                            while(draw_x+view.radius*2 <= width) {
                                ctx.ellipse(draw_x+view.radius,height*0.5,
                                            view.radius*2,view.radius*2)
                                draw_x += view.radius*2+view.spacing
                            }
                            ctx.fill()
                            break;
                        case Helper_Type.Dash_LinePattern: //虚线填充
                            ctx.setLineDash([view.dashWidth,view.spacing])
                            ctx.lineWidth = view.lineWidth
                            ctx.moveTo(0,height*0.5)
                            ctx.lineTo(width,height*0.5)
                            ctx.stroke()
                            break;
                        case Helper_Type.Solid_LinePattern: //solid填充
                            ctx.setLineDash([])
                            ctx.lineWidth = view.lineWidth
                            ctx.moveTo(0,height*0.5)
                            ctx.lineTo(width,height*0.5)
                            ctx.stroke()
                            break;
                        }

                    }
                }
                Text {
                    color:theme.fg
                    text: `${index}`
                    font: view.font
                    Layout.alignment: Qt.AlignBaseline
                }
            }
        }
    }
}
