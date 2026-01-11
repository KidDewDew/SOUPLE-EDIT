import QtQuick
import QtQuick.Controls
import QtQuick.Shapes
import com.custom 1.0

Item {
    id: root

    property bool enableAdjustW: true
    property bool enableAdjustH: true
    property bool enableRotate: true
    property bool enableDrag: true

    // 核心属性 - 双向绑定
    property real eb_x: 300           // 中心点x坐标
    property real eb_y: 200           // 中心点y坐标
    property real eb_rotation: 0      // 旋转角度(度)
    property real eb_width: 200       // 宽度
    property real eb_height: 150      // 高度

    onEb_xChanged: {
        if(sync_obj) {
            sync_obj.x = eb_x - eb_width*0.5
            SoupleManager.sendCommandToData(sync_obj.data_id,
                        Helper_Type.X_UP,sync_obj.x)
        }
    }
    onEb_yChanged: {
        if(sync_obj) {
            sync_obj.y = eb_y - eb_height*0.5
            SoupleManager.sendCommandToData(sync_obj.data_id,
                        Helper_Type.Y_UP,sync_obj.y)
        }
    }
    onEb_widthChanged: {
        if(sync_obj) {
            sync_obj.width = eb_width
            sync_obj.x = eb_x - eb_width*0.5
            SoupleManager.sendCommandToData(sync_obj.data_id,
                        Helper_Type.WIDTH_UP,eb_width)
        }
    }
    onEb_heightChanged: {
        if(sync_obj) {
            sync_obj.height = eb_height
            sync_obj.y = eb_y - eb_height*0.5
            SoupleManager.sendCommandToData(sync_obj.data_id,
                        Helper_Type.HEIGHT_UP,eb_height)
        }
    }
    onEb_rotationChanged: {
        if(sync_obj) {
            sync_obj.rotation = eb_rotation
            SoupleManager.sendCommandToData(sync_obj.data_id,
                        Helper_Type.ROTATION_UP,eb_rotation)
        }
    }

    // 功能开关
    property var sync_obj: null          // 同步对象
    property bool darkMode: false        // 黑暗主题

    onSync_objChanged: {
        eb_x = sync_obj.x+sync_obj.width*0.5
        eb_y = sync_obj.y+sync_obj.height*0.5
        eb_width = sync_obj.width
        eb_height = sync_obj.height
        eb_rotation = sync_obj.rotation
    }

    // 组件位置和尺寸 - 基于中心点计算
    x: eb_x
    y: eb_y

    width: 0
    height: 0
    rotation: eb_rotation

    property color accent: "#CCA52F"

    property bool show_new: false
    property real new_left
    property real new_right
    property real new_top
    property real new_bottom
    property real new_rotation
    property bool move_left: false
    property bool move_top: false
    property bool move_right: false
    property bool move_bottom: false
    property bool isRotate: false

    Binding on new_left {
        when: !move_left
        value: -eb_width*0.5
    }
    Binding on new_right {
        when: !move_right
        value: eb_width*0.5
    }
    Binding on new_top {
        when: !move_top
        value: -eb_height*0.5
    }
    Binding on new_bottom {
        when: !root.move_bottom
        value: eb_height*0.5
    }

    MouseArea {
        id: marea_l1 //左边 marea
        visible: enableDrag
        hoverEnabled: true
        x:-eb_width*0.5-5
        y:-height*0.5
        width: 10
        height: Math.abs(eb_height)
        cursorShape: Qt.SizeAllCursor
        drag.target: root
        onReleased: {
            eb_x = root.x
            eb_y = root.y
        }
        onCanceled: {
            eb_x = root.x
            eb_y = root.y
        }
    }

    MouseArea {
        id: marea_drag_left //左drag marea
        visible: enableAdjustW
        anchors.fill: drag_left
        anchors.leftMargin: -3
        anchors.rightMargin: -3
        cursorShape: Qt.SizeHorCursor
        onPressed: {
            move_left = true
            marea_001.cursorShape = cursorShape
        }
        onMouseXChanged: function(m){
            new_left = m.x-width*0.5-eb_width*0.5
        }
        onReleased: function(m){
            eb_x += (m.x-width*0.5) * Math.cos(root.rotation/180.0*Math.PI) * 0.5
            eb_y += (m.x-width*0.5) * Math.sin(root.rotation/180.0*Math.PI) * 0.5
            eb_width = new_right - new_left
            move_left = false
        }
    }

    MouseArea {
        id: marea_l2 //右边
        visible: enableDrag
        hoverEnabled: true
        x:eb_width*0.5-5
        y:-height*0.5
        width: 10
        height: Math.abs(eb_height)
        cursorShape: Qt.SizeAllCursor
        drag.target: root
        onReleased: {
            eb_x = root.x
            eb_y = root.y
        }
        onCanceled: {
            eb_x = root.x
            eb_y = root.y
        }
    }

    MouseArea {
        id: marea_drag_right //右drag marea
        anchors.fill: drag_right
        visible: enableAdjustW
        cursorShape: Qt.SizeHorCursor
        onPressed: {
            move_right = true
            marea_001.cursorShape = cursorShape
        }
        onMouseXChanged: function(m){
            new_right = m.x-width*0.5+eb_width*0.5
        }
        onReleased: function(m){
            eb_x += (m.x-width*0.5) * Math.cos(root.rotation/180.0*Math.PI) * 0.5
            eb_y += (m.x-width*0.5) * Math.sin(root.rotation/180.0*Math.PI) * 0.5
            eb_width = new_right - new_left
            move_right = false
        }
    }

    MouseArea {
        id: marea_l3 //上边 marea
        x:-width*0.5
        visible: enableDrag
        y:-eb_height*0.5 - 5
        width: Math.abs(eb_width)
        height: 10
        cursorShape: Qt.SizeAllCursor
        drag.target: root
        onReleased: {
            eb_x = root.x
            eb_y = root.y
        }
        onCanceled: {
            eb_x = root.x
            eb_y = root.y
        }
    }

    MouseArea {
        id: marea_drag_top //上drag marea
        anchors.fill: drag_top
        visible: enableAdjustH
        anchors.topMargin: -4
        anchors.bottomMargin: -4
        cursorShape: Qt.SizeVerCursor
        onPressed: function(m){
            move_top = true
            marea_001.cursorShape = cursorShape
            new_top = m.y-height*0.5-eb_height*0.5
        }
        onPositionChanged: function(m){
            new_top = m.y-height*0.5-eb_height*0.5
        }
        onReleased: function(m){
            eb_x -= (m.y-height*0.5) * Math.sin(root.rotation/180.0*Math.PI) * 0.5
            eb_y += (m.y-height*0.5) * Math.cos(root.rotation/180.0*Math.PI) * 0.5
            eb_height = new_bottom - new_top
            move_top = false
        }
    }

    MouseArea {
        id: marea_l4 //下边 marea
        hoverEnabled: true
        visible: enableDrag
        x:-width*0.5
        y:eb_height*0.5 - 5
        width: Math.abs(eb_width)
        height: 10
        cursorShape: Qt.SizeAllCursor
        drag.target: root
        onReleased: {
            eb_x = root.x
            eb_y = root.y
        }
        onCanceled: {
            eb_x = root.x
            eb_y = root.y
        }
    }

    MouseArea {
        id: marea_drag_bottom //下drag marea
        visible: enableAdjustH
        anchors.fill: drag_bottom
        anchors.topMargin: -4
        anchors.bottomMargin: -4
        cursorShape: Qt.SizeVerCursor
        onPressed: function(m){
            move_bottom = true
            marea_001.cursorShape = cursorShape
            new_bottom = m.y-height*0.5+eb_height*0.5
        }
        onPositionChanged: function(m){
            new_bottom = m.y-height*0.5+eb_height*0.5
        }
        onReleased: function(m){
            eb_x -= (m.y-height*0.5) * Math.sin(root.rotation/180.0*Math.PI) * 0.5
            eb_y += (m.y-height*0.5) * Math.cos(root.rotation/180.0*Math.PI) * 0.5
            eb_height = new_bottom - new_top
            move_bottom = false
        }
    }

    MouseArea {   //左上点 marea
        id: marea_1
        anchors.fill: p1
        hoverEnabled: true
        visible: enableAdjustW && enableAdjustH
        cursorShape: Qt.SizeFDiagCursor
        onPressed: {
            move_top = move_left = true
            marea_001.cursorShape = cursorShape
        }
        onMouseXChanged: function(m){
            new_left = m.x-width*0.5-eb_width*0.5
            new_top = m.y-height*0.5-eb_height*0.5
        }
        onReleased: function(m){
            const a = root.rotation/180.0*Math.PI
            eb_x += (m.x-width*0.5) * Math.cos(a) * 0.5 -
                    (m.y-height*0.5) * Math.sin(a) * 0.5
            eb_y += (m.x-width*0.5) * Math.sin(a) * 0.5 +
                    (m.y-height*0.5) * Math.cos(a) * 0.5
            eb_height = new_bottom - new_top
            eb_width = new_right - new_left
            move_top = move_left = false
        }
    }

    MouseArea {   //右上点 marea
        id: marea_2
        anchors.fill: p2
        visible: enableAdjustW && enableAdjustH
        hoverEnabled: true
        cursorShape: Qt.SizeBDiagCursor
        onPressed: {
            move_top = move_right = true
            marea_001.cursorShape = cursorShape
        }
        onMouseXChanged: function(m){
            new_right = m.x-width*0.5+eb_width*0.5
            new_top = m.y-height*0.5-eb_height*0.5
        }
        onReleased: function(m){
            const a = root.rotation/180.0*Math.PI
            eb_x += (m.x-width*0.5) * Math.cos(a) * 0.5 -
                    (m.y-height*0.5) * Math.sin(a) * 0.5
            eb_y += (m.x-width*0.5) * Math.sin(a) * 0.5 +
                    (m.y-height*0.5) * Math.cos(a) * 0.5
            eb_height = new_bottom - new_top
            eb_width = new_right - new_left
            move_top = move_right = false
        }
    }

    MouseArea {    //左下点 marea
        id: marea_3
        anchors.fill: p3
        visible: enableAdjustW && enableAdjustH
        hoverEnabled: true
        cursorShape: Qt.SizeBDiagCursor
        onPressed: {
            move_bottom = move_left = true
            marea_001.cursorShape = cursorShape
        }
        onMouseXChanged: function(m){
            new_left = m.x-width*0.5-eb_width*0.5
            new_bottom = m.y-height*0.5+eb_height*0.5
        }
        onReleased: function(m){
            const a = root.rotation/180.0*Math.PI
            eb_x += (m.x-width*0.5) * Math.cos(a) * 0.5 -
                    (m.y-height*0.5) * Math.sin(a) * 0.5
            eb_y += (m.x-width*0.5) * Math.sin(a) * 0.5 +
                    (m.y-height*0.5) * Math.cos(a) * 0.5
            eb_height = new_bottom - new_top
            eb_width = new_right - new_left
            move_bottom = move_left = false
        }
    }

    MouseArea {   //右下点 marea
        id: marea_4
        anchors.fill: p4
        visible: enableAdjustW && enableAdjustH
        hoverEnabled: true
        cursorShape: Qt.SizeFDiagCursor
        onPressed: {
            move_right = move_bottom = true
            marea_001.cursorShape = cursorShape
        }
        onMouseXChanged: function(m){
            new_right = m.x-width*0.5+eb_width*0.5
            new_bottom = m.y-height*0.5+eb_height*0.5
        }
        onReleased: function(m){
            const a = root.rotation/180.0*Math.PI
            eb_x += (m.x-width*0.5) * Math.cos(a) * 0.5 -
                    (m.y-height*0.5) * Math.sin(a) * 0.5
            eb_y += (m.x-width*0.5) * Math.sin(a) * 0.5 +
                    (m.y-height*0.5) * Math.cos(a) * 0.5
            eb_height = new_bottom - new_top
            eb_width = new_right - new_left
            move_bottom = move_right = false
        }
    }

    MouseArea {
        id: marea_001
        visible: shape_adjust.visible
        anchors.fill: shape_adjust
        anchors.margins: -10
        cursorShape: Qt.SizeFDiagCursor
    }

    // 下面是绘制内容------

    Shape {
        id: shape_adjust
        visible: move_left || move_right || move_top || move_bottom || isRotate
        width: new_right - new_left
        height: new_bottom - new_top
        x: new_left
        y: new_top
        rotation: new_rotation
        ShapePath {
            strokeWidth: 1
            strokeColor: "#269C1D"
            startX: 0
            startY: 0
            strokeStyle: ShapePath.DashLine
            dashPattern: [3,5]
            fillColor: "#00000000"

            PathLine {
                x: new_right - new_left
                y: 0
            }
            PathLine {
                x: new_right - new_left
                y: new_bottom - new_top
            }
            PathLine {
                x: 0
                y: new_bottom - new_top
            }
            PathLine {
                x: 0
                y: 0
            }
        }
    }

    Rectangle {//左边
        width: 1
        height: Math.abs(eb_height)
        x: -eb_width*0.5 - 1
        y: -height*0.5
        color: accent
    }

    Rectangle {//左drag
        id: drag_left
        width: 6
        height: Math.min(30,Math.abs(eb_height)*0.5)
        x: -eb_width*0.5 - 3
        y: -height*0.5
        color: "white"
        radius: 3
        border.color:"#A3A3A3"
    }

    Rectangle { //右边
        width: 1
        height: Math.abs(eb_height)
        x: eb_width*0.5 - 1
        y: -height*0.5
        color: accent
    }

    Rectangle {//右drag
        id: drag_right
        width: 6
        height: Math.min(30,Math.abs(eb_height)*0.5)
        x: eb_width*0.5 - 3
        y: -height*0.5
        color: "white"
        radius: 3
        border.color:"#A3A3A3"
    }

    Rectangle { //上边
        width: Math.abs(eb_width)
        height: 1
        x: -width*0.5
        y: -eb_height*0.5-1
        color: accent
    }

    Rectangle {//上drag
        id: drag_top
        width: Math.min(30,Math.abs(eb_width)*0.5)
        height: 6
        x: -width*0.5
        y: -eb_height*0.5-3
        color: "white"
        radius: 3
        border.color:"#A3A3A3"
    }

    Rectangle { //下边
        width: Math.abs(eb_width)
        height: 1
        x: -width*0.5
        y: eb_height*0.5-1
        color: accent
    }

    Rectangle {//下drag
        id: drag_bottom
        width: Math.min(30,Math.abs(eb_width)*0.5)
        height: 6
        x: -width*0.5
        y: eb_height*0.5-3
        color: "white"
        radius: 3
        border.color:"#A3A3A3"
    }

    Rectangle {
        id: p1
        radius: 5
        width: 10
        height: 10
        x: -eb_width*0.5 - width*0.5
        y: -eb_height*0.5 - height*0.5
        color: marea_1.containsMouse ? accent : Qt.lighter(accent,1.5)
        border.color: Qt.darker(accent,1.5)
    }

    Rectangle {
        id: p2
        radius: 5
        width: 10
        height: 10
        x:  eb_width*0.5 - width*0.5
        y: -eb_height*0.5 - height*0.5
        color: marea_2.containsMouse ? accent : Qt.lighter(accent,1.5)
        border.color: Qt.darker(accent,1.5)
    }

    Rectangle {
        id: p3
        radius: 5
        width: 10
        height: 10
        x: -eb_width*0.5 - width*0.5
        y: eb_height*0.5 - height*0.5
        color: marea_3.containsMouse ? accent : Qt.lighter(accent,1.5)
        border.color: Qt.darker(accent,1.5)
    }

    Rectangle {
        id: p4
        radius: 5
        width: 10
        height: 10
        x: eb_width*0.5 - width*0.5
        y: eb_height*0.5 - height*0.5
        color: marea_4.containsMouse ? accent : Qt.lighter(accent,1.5)
        border.color: Qt.darker(accent,1.5)
    }

    Rectangle {
        visible: enableRotate
        width: 1
        color: "#888"
        height: 15
        x: -1
        y: -eb_height*0.5 - 18
    }

    Image {
        id: img_rotate
        visible: enableRotate
        width: 20
        height: 20
        source: "qrc:/image/icon_rotate.png"
        y: -eb_height*0.5 - 36
        x: -10
    }

    MouseArea {
        anchors.fill: img_rotate
        visible: enableRotate
        cursorShape: Qt.ClosedHandCursor
        onPressed: {
            isRotate = true
            new_rotation = 0
        }
        onPositionChanged: function(m){
            new_rotation =
                    90+180.0*Math.atan2((m.y-height/2+y),(m.x-width/2+x))/Math.PI
        }
        onReleased: {
            eb_rotation += new_rotation
            isRotate = false
            new_rotation = 0
        }
    }

}
