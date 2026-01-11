import QtQuick


// 页面obj
Rectangle {
    id: page
    //Layout.alignment: Qt.AlignHCenter
    color: theme.page_bg
    property int data_id
    property real top_margin
    property real bottom_margin
    property int index
    Rectangle { //底部线，用于表达页面结尾
        height: 1
        color: "#A6A6A6"
        width: parent.width
        //anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 1
    }
    Text { //显示页码
        text: "第"+(index+1) + "页"
        font.pixelSize: 9
        color: "grey"
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }
    Triangle {
        id: tc_top
        width: 14
        height: 15
        dir: Triangle.Dir.Right
        fillColor: "#8C6869"
        x: -width
        y: parent.top_margin - height/2
    }
    ShapeBracket {
        visible: tc_top.focus
        anchors.right: parent.left
        width: 12
        color: "#FF4B78"
        height: parent.top_margin
        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.left
            text: `${Helper.pixel2cm(page.top_margin).toFixed(2)}cm`
        }
    }
    MouseArea {
        anchors.fill: tc_top
        drag.target: tc_top
        drag.axis: Drag.YAxis
        drag.threshold: 2
        drag.maximumY: parent.height+height/2
        drag.minimumY: -height/2
        onPressed: tc_top.focus = true
        onMouseYChanged: {
            //console.log(pages[index].top_margin)
            //pages[index].top_margin = parent.top_margin = tc_top.y + height/2
            top_margin = tc_top.y + height/2
            SoupleManager.updatePageTopMargin(index,parent.top_margin)
        }
    }
    Triangle {
        id: tc_bottom
        width: 14
        height: 15
        dir: Triangle.Dir.Right
        fillColor: "#8C6869"
        x: -width
        y: parent.height-parent.bottom_margin-height/2
    }
    ShapeBracket {
        visible: tc_bottom.focus
        anchors.right: parent.left
        width: 12
        color: "#FF4B78"
        height: parent.bottom_margin
        anchors.bottom: parent.bottom
        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.left
            text: `${Helper.pixel2cm(page.bottom_margin).toFixed(2)}cm`
        }
    }
    MouseArea {
        anchors.fill: tc_bottom
        drag.target: tc_bottom
        drag.axis: Drag.YAxis
        drag.threshold: 2
        drag.maximumY: parent.height-height/2
        drag.minimumY: -height/2
        onPressed: tc_bottom.focus = true
        onMouseYChanged: {
            bottom_margin = parent.height -  tc_bottom.y - tc_bottom.height/2
            SoupleManager.updatePageBottomMargin(index,bottom_margin)
        }
    }
    DashLine {
        id: line_top
        y: parent.top_margin-1.5
        width: parent.width
        height: 3
        color: "#C9C9C9"
    }
    DashLine {
        id: line_bottom
        y: parent.height-parent.bottom_margin-1.5
        width: parent.width
        height: 3
        color: "#C9C9C9"
    }
}
