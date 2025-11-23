
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Windows
import QtQuick.Window 2.15
import com.custom 1.0

// Path对象
Canvas {
    id: obj
    property int data_id
    property var path: []    // 路径数组
    property color fillStyle: "transparent"
    property color strokeStyle: "black"
    property bool fill: false
    property bool stroke: true
    property real lineWidth: 2
    property bool isSelected

    property alias cp_propertyBar: cp_propertyBar

    onHeightChanged: {
        if (height <= 0) height = 1;
    }

    function die() {
        obj.destroy()
    }

    Component {
        id: cp_propertyBar
        GridLayout {
            columns: 2
            columnSpacing: parent.width * 0.1
            rowSpacing: 8

            Text { text: "元素类别"; font.pixelSize: 16 }
            Text { text: "路径"; font.pixelSize: 16 }

            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "锚定信息"
            }

            Text { text: "垂直对齐"; font.pixelSize: 16 }
            ComboBox {
                model: ["中心对齐","下对齐","上对齐"]
                Layout.preferredWidth: 120
            }

            Text { text: "垂直偏移"; font.pixelSize: 16 }
            MyDoubleSpinBox {
                step: 0.1
                precision: 1
                initialValue: Helper.pixel2cm(SoupleManager.qmlGetData(data_id,Helper_Type.VALIGNOFFSET))
                suffix: "cm"
                onValueChanged: {
                    SoupleManager.sendCommandToData(data_id,Helper.VALIGNOFFSET_UP,Helper.cm2pixel(value))
                }
                Layout.preferredWidth: 120
            }

            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "路径信息"
            }

            Text { text: "水平缩放"; font.pixelSize: 16 }
            MyDoubleSpinBox {
                id: spin_1
                step: 0.1
                precision: 2
                initialValue: Helper.pixel2cm(SoupleManager.qmlGetData(data_id,Helper_Type.XSCALE))
                onValueChanged: {
                    if (value <= 0) value = 0.1;
                    if (value > 10) value = 10;
                }
                Layout.preferredWidth: 120
            }

            Text { text: "垂直缩放"; font.pixelSize: 16 }
            MyDoubleSpinBox {
                id: spin_2
                step: 0.1
                precision: 2
                initialValue: Helper.pixel2cm(obj.height)
                Binding on value {
                    when: obj.heightChanged
                    value: Helper.cm2pixel(obj.height)
                }
                onValueChanged: {
                    if (value <= 0) value = 0.1;
                    if (value > 10) value = 10;
                    obj.height = Helper.cm2pixel(value)
                }
                Layout.preferredWidth: 120
            }

            Button {
                text: "编辑"
                font.pointSize: 18
                padding: 8
                Layout.columnSpan: 2
                Layout.alignment: Qt.AlignLeft
                onClicked: {
                    try {
                        if (!Array.isArray(obj.path)) {
                            obj.path = [];
                        }
                        if (!pathedit) {
                            pathedit = patheditComponent.createObject(obj);
                        } else if (pathedit.status === Component.Destroyed) {
                            pathedit = patheditComponent.createObject(obj);
                        }
                        pathedit.loadPath();
                        pathedit.visible = true;
                    } catch (e) {
                        console.error("编辑按钮点击出错:", e);
                    }
                }
            }
        }
    }

    Menu {
        id: menu
        MenuItem {
            text: "删除"
            onTriggered: {
                SoupleManager.requestDeleteObj(obj.data_id)
                parent.focus = false
            }
        }
    }

    onFocusChanged: {
        if(focus) selectedObj = obj
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onPressed: function(mouse){
            parent.focus = true
        }
        onClicked: function(mouse){
            if(mouse.button == Qt.RightButton) {
                menu.x = mouse.x
                menu.y = mouse.y
                menu.open()
            }
        }
    }

    onPathChanged: {
        //console.log("路径已更新，新路径长度:", path.length);
        requestPaint();
    }

    onPaint: {
        var ctx = getContext('2d');
        ctx.clearRect(0, 0, width, height);

        // 同时设置线条颜色和填充颜色
        ctx.fillStyle = fill ? fillStyle : "transparent";
        ctx.strokeStyle = strokeStyle;
        ctx.lineWidth = lineWidth;
        ctx.lineCap = "round";
        ctx.lineJoin = "round";

        ctx.beginPath();

        if (path.length > 0) {
            for(let i = 0; i < path.length; ++i) {
                let action = path[i];
                if (!action || typeof action.t === "undefined") {
                    console.warn("无效的路径动作，跳过");
                    continue;
                }

                switch(action.t) {
                    case 77: // 'M' - 移动到
                        ctx.moveTo(action.x || 0, action.y || 0);
                        break;
                    case 76: // 'L' - 直线到
                        ctx.lineTo(action.x || 0, action.y || 0);
                        break;
                    case 66: // 'B' - 贝塞尔曲线
                        if (i + 2 < path.length) {
                            const cp1 = action;
                            const cp2 = path[++i];
                            const end = path[++i];
                            ctx.bezierCurveTo(
                                cp1.x || 0, cp1.y || 0,
                                cp2.x || 0, cp2.y || 0,
                                end.x || 0, end.y || 0
                            );
                        } else {
                            console.error("贝塞尔曲线数据不完整，无法绘制");
                        }
                        break;
                    default:
                        console.warn("未知路径类型:", action.t);
                }
            }

            if(fill) ctx.fill();
            if(stroke) ctx.stroke();
        }
    }
    Component {
        id: patheditComponent
        Window {
            id: pathedit
            visible: false
            width: 800
            height: 600
            title: "Path Editor"
            //transientParent: obj.window
            flags: Qt.Window | Qt.WindowTitleHint | Qt.WindowMinimizeButtonHint |
                   Qt.WindowMaximizeButtonHint | Qt.WindowCloseButtonHint

            ListModel {
                id: pathModel
            }

            // 存储填充颜色，与线条颜色保持关联
            property color strokeColor: obj.strokeStyle || "black"
            property color fillColor: obj.fill ? obj.fillStyle || obj.strokeStyle : "transparent"
            property int lineWidth: obj.lineWidth || 2
            property bool fillPath: obj.fill || false
            property bool showControlPoints: true
            property bool showControlLines: true

            property int selectedPointIndex: -1
            property int dragPointType: 0 // 0:主点, 1:控制点1, 2:控制点2
            property bool isDragging: false
            property int dragPointIndex: -1
            property point dragStartPos: Qt.point(0, 0)
            property int colorIndex: obj.strokeStyle ? colorList.indexOf(obj.strokeStyle) : 0

            property list<string> colorList: [
                "black", "red", "green", "blue",
                "yellow", "purple", "orange", "pink",
                "cyan", "magenta", "gray", "brown"
            ]

            // 颜色变化时同步更新填充颜色
            onStrokeColorChanged: {
                if (fillPath) {
                    fillColor = strokeColor;
                }
                pathCanvas.requestPaint();
            }

            // 填充选项变化时更新填充颜色
            onFillPathChanged: {
                fillColor = fillPath ? strokeColor : "transparent";
                pathCanvas.requestPaint();
            }

            function findClosestPoint(x, y, maxDistance = 20) {
                let closestIndex = -1;
                let closestType = 0;
                let minDistance = maxDistance * maxDistance;

                for (let i = 0; i < pathModel.count; i++) {
                    const p = pathModel.get(i);

                    let dx = x - p.x;
                    let dy = y - p.y;
                    let dist = dx*dx + dy*dy;
                    if (dist < minDistance) {
                        minDistance = dist;
                        closestIndex = i;
                        closestType = 0;
                    }

                    if (i < pathModel.count - 1 && p.isBezier) {
                        dx = x - p.cp1x;
                        dy = y - p.cp1y;
                        dist = dx*dx + dy*dy;
                        if (dist < minDistance) {
                            minDistance = dist;
                            closestIndex = i;
                            closestType = 1;
                        }

                        const nextPoint = pathModel.get(i+1);
                        dx = x - nextPoint.cp2x;
                        dy = y - nextPoint.cp2y;
                        dist = dx*dx + dy*dy;
                        if (dist < minDistance) {
                            minDistance = dist;
                            closestIndex = i+1;
                            closestType = 2;
                        }
                    }
                }

                return {index: closestIndex, type: closestType};
            }

            function createPoint(x, y) {
                if (pathModel.count === 0) {
                    pathModel.append({
                        type: 77,
                        x: x, y: y,
                        cp1x: x + 50, cp1y: y - 50,
                        cp2x: x + 100, cp2y: y - 50,
                        isBezier: true
                    });
                } else {
                    const lastPoint = pathModel.get(pathModel.count - 1);
                    const dx = x - lastPoint.x;
                    const dy = y - lastPoint.y;
                    const len = Math.sqrt(dx*dx + dy*dy) || 1;
                    const unitX = dx / len;
                    const unitY = dy / len;

                    pathModel.setProperty(pathModel.count - 1, "cp1x", lastPoint.x + unitX * len * 0.3);
                    pathModel.setProperty(pathModel.count - 1, "cp1y", lastPoint.y + unitY * len * 0.3 - 50);

                    const cp2x = x - unitX * len * 0.3;
                    const cp2y = y - unitY * len * 0.3 - 50;

                    pathModel.append({
                        type: 76,
                        x: x, y: y,
                        cp1x: x + unitX * len * 0.3,
                        cp1y: y + unitY * len * 0.3 + 50,
                        cp2x: cp2x,
                        cp2y: cp2y,
                        isBezier: true
                    });
                }

                selectedPointIndex = pathModel.count - 1;
                pathCanvas.requestPaint();
            }

            function updatePoint(index, type, x, y) {
                if (index < 0 || index >= pathModel.count) return;

                if (type === 0) {
                    pathModel.setProperty(index, "x", x);
                    pathModel.setProperty(index, "y", y);
                } else if (type === 1) {
                    pathModel.setProperty(index, "cp1x", x);
                    pathModel.setProperty(index, "cp1y", y);
                } else if (type === 2) {
                    pathModel.setProperty(index, "cp2x", x);
                    pathModel.setProperty(index, "cp2y", y);
                }

                pathCanvas.requestPaint();
            }

            function deletePoint(index) {
                if (index < 0 || index >= pathModel.count) return;

                pathModel.remove(index);
                selectedPointIndex = -1;
                pathCanvas.requestPaint();
            }

            function loadPath() {
                try {
                    pathModel.clear();

                    // 初始化颜色
                    strokeColor = obj.strokeStyle || "black";
                    colorIndex = colorList.indexOf(strokeColor);
                            if (colorIndex === -1) {
                                colorIndex = 0;
                            }
                            fillPath = obj.fill || false;
                            fillColor = fillPath ? strokeColor : "transparent";


                    if (!obj.path || !Array.isArray(obj.path) || obj.path.length === 0) {
                        const centerX = pathCanvas.width / 2;
                        const centerY = pathCanvas.height / 2;
                        pathModel.append({
                            type: 77,
                            x: centerX, y: centerY,
                            cp1x: centerX + 50, cp1y: centerY - 50,
                            cp2x: centerX + 100, cp2y: centerY - 50,
                            isBezier: false
                        });
                        return;
                    }

                    let i = 0;
                    while (i < obj.path.length) {
                        const action = obj.path[i];
                        if (typeof action !== "object" || action.t === undefined) {
                            console.warn("无效的路径动作，跳过索引:", i);
                            i++;
                            continue;
                        }

                        switch (action.t) {
                            case 77:
                                pathModel.append({
                                    type: 77,
                                    x: action.x || 0,
                                    y: action.y || 0,
                                    cp1x: (action.x || 0) + 50,
                                    cp1y: (action.y || 0) - 50,
                                    cp2x: (action.x || 0) + 100,
                                    cp2y: (action.y || 0) - 50,
                                    isBezier: false
                                });
                                i++;
                                break;

                            case 76:
                                pathModel.append({
                                    type: 76,
                                    x: action.x || 0,
                                    y: action.y || 0,
                                    cp1x: (action.x || 0) + 50,
                                    cp1y: (action.y || 0) - 50,
                                    cp2x: (action.x || 0) + 100,
                                    cp2y: (action.y || 0) - 50,
                                    isBezier: false
                                });
                                i++;
                                break;

                            case 66:
                                if (i + 2 < obj.path.length) {
                                    const cp1 = action;
                                    const cp2 = obj.path[i + 1];
                                    const endPoint = obj.path[i + 2];

                                    pathModel.append({
                                        type: 76,
                                        x: endPoint.x || 0,
                                        y: endPoint.y || 0,
                                        cp1x: cp1.x || 0,
                                        cp1y: cp1.y || 0,
                                        cp2x: cp2.x || 0,
                                        cp2y: cp2.y || 0,
                                        isBezier: true
                                    });
                                    i += 3;
                                } else {
                                    console.warn("贝塞尔曲线数据不完整，跳过索引:", i);
                                    i++;
                                }
                                break;

                            default:
                                console.log("未知路径类型:", action.t, "，跳过索引:", i);
                                i++;
                        }
                    }

                    if (pathModel.count === 0) {
                        const centerX = pathCanvas.width / 2;
                        const centerY = pathCanvas.height / 2;
                        pathModel.append({
                            type: 77,
                            x: centerX, y: centerY,
                            cp1x: centerX + 50, cp1y: centerY - 50,
                            cp2x: centerX + 100, cp2y: centerY - 50,
                            isBezier: false
                        });
                    }
                } catch (e) {
                    console.error("加载路径时出错:", e);
                    pathModel.clear();
                    const centerX = pathCanvas.width / 2;
                    const centerY = pathCanvas.height / 2;
                    pathModel.append({
                        type: 77,
                        x: centerX, y: centerY,
                        cp1x: centerX + 50, cp1y: centerY - 50,
                        cp2x: centerX + 100, cp2y: centerY - 50,
                        isBezier: false
                    });
                }
                pathCanvas.requestPaint();
            }

            function savePath() {
                try {
                    const newPath = [];

                    for (let i = 0; i < pathModel.count; i++) {
                        const p = pathModel.get(i);
                        if (i === 0) {
                            newPath.push({ t: 77, x: p.x, y: p.y });
                        }
                        if (i > 0) {
                            const prevPoint = pathModel.get(i-1);
                            if (prevPoint.isBezier) {
                                newPath.push({ t: 66, x: prevPoint.cp1x, y: prevPoint.cp1y });
                                newPath.push({ t: 66, x: p.cp2x, y: p.cp2y });
                                newPath.push({ t: 66, x: p.x, y: p.y });
                            } else {
                                newPath.push({ t: 76, x: p.x, y: p.y });
                            }
                        }
                    }

                    obj.path.length = 0;
                    for (let i = 0; i < newPath.length; i++) {
                        obj.path.push(newPath[i]);
                    }

                    // 同时保存线条颜色和填充颜色
                    obj.strokeStyle = strokeColor;
                    obj.fillStyle = fillColor;
                    obj.lineWidth = lineWidth;
                    obj.fill = fillPath;

                    obj.pathChanged();
                    obj.requestPaint();

                    console.log("路径已保存，新路径点数:", newPath.length);
                    pathedit.visible = false;
                } catch (e) {
                    console.error("保存路径时出错:", e);
                }
            }

            RowLayout {
                anchors.fill: parent
                spacing: 0

                Rectangle {
                    Layout.preferredWidth: 250
                    Layout.fillHeight: true
                    color: "#f0f0f0"

                    ColumnLayout {
                        anchors.fill: parent
                        Layout.margins: 5
                        spacing: 5

                        GroupBox {
                            title: "Path Properties"
                            Layout.fillWidth: true
                            Layout.margins: 2

                            ColumnLayout {
                                Layout.margins: 3
                                spacing: 3

                                RowLayout {
                                    spacing: 2
                                    Layout.fillWidth: true

                                    Label {
                                        text: "Color:"
                                        font.pixelSize: 12
                                        Layout.alignment: Qt.AlignVCenter
                                    }

                                    Rectangle {
                                        id: colorPicker
                                        width: 20
                                        height: 20
                                        radius: 3
                                        color: strokeColor
                                        border.color: "darkgray"
                                        border.width: 1
                                        Layout.alignment: Qt.AlignVCenter


                                        MouseArea {
                                                anchors.fill: parent
                                                onClicked: {
                                                    // 直接计算下一个颜色，不依赖colorIndex的递增
                                                    const nextIndex = (colorIndex + 1) % colorList.length;
                                                    const nextColor = colorList[nextIndex];
                                                    colorPicker.color = nextColor;
                                                    strokeColor = nextColor;
                                                    colorIndex = nextIndex; // 直接设置索引值
                                                    if (fillPath) {
                                                        fillColor = nextColor;
                                                    }
                                                    pathCanvas.requestPaint();
                                                }
                                        }
                                    }
                                }

                                RowLayout {
                                    spacing: 2
                                    Layout.fillWidth: true

                                    Label {
                                        text: "Width:"
                                        font.pixelSize: 12
                                        Layout.alignment: Qt.AlignVCenter
                                        Layout.preferredWidth: 40
                                    }
                                    Slider {
                                        id: lineWidthSlider
                                        from: 1; to: 10
                                        value: lineWidth
                                        Layout.fillWidth: true
                                        onValueChanged: {
                                            lineWidth = Math.round(value);
                                            lineWidthLabel.text = lineWidth;
                                            pathCanvas.requestPaint();
                                        }
                                    }
                                    Label {
                                        id: lineWidthLabel
                                        text: lineWidth
                                        font.pixelSize: 12
                                        Layout.preferredWidth: 20
                                    }
                                }

                                CheckBox {
                                    text: "Fill Path"
                                    checked: fillPath
                                    font.pixelSize: 12
                                    onCheckedChanged: {
                                        fillPath = checked;
                                        // 填充选项变化时更新填充颜色
                                        fillColor = fillPath ? strokeColor : "transparent";
                                        pathCanvas.requestPaint();
                                    }
                                }

                                CheckBox {
                                    text: "Show Control Points"
                                    checked: showControlPoints
                                    font.pixelSize: 12
                                    onCheckedChanged: {
                                        showControlPoints = checked;
                                        pathCanvas.requestPaint();
                                    }
                                }

                                CheckBox {
                                    text: "Show Control Lines"
                                    checked: showControlLines
                                    font.pixelSize: 12
                                    onCheckedChanged: {
                                        showControlLines = checked;
                                        pathCanvas.requestPaint();
                                    }
                                }
                            }
                        }

                        GroupBox {
                            title: "Path Points"
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            Rectangle {
                                anchors.fill: parent
                                color: "lightgray"
                                radius: 4
                                border.color: "darkgray"

                                ColumnLayout {
                                    anchors.fill: parent
                                    Layout.margins: 8
                                    spacing: 4

                                    ListView {
                                        id: pointListView
                                        model: pathModel
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true
                                        highlightFollowsCurrentItem: false
                                        cacheBuffer: 200

                                        delegate: RowLayout {
                                            Layout.fillWidth: true
                                            height: 30
                                            spacing: 6

                                            Button {
                                                text: model.isBezier ? " Bezier " : "  Line  "
                                                Layout.preferredWidth: 50
                                                onClicked: {
                                                    model.isBezier = !model.isBezier;
                                                    pathCanvas.requestPaint();
                                                }
                                            }

                                            Label {
                                                text: model.type === 77 ? "Move to" : "Line to"
                                                Layout.preferredWidth: 50
                                            }

                                            Label {
                                                text: "(" + (model.x).toFixed(2) + ", " + (model.y).toFixed(2) + ")"
                                                Layout.fillWidth: true
                                            }

                                            Button {
                                                text: "X"
                                                Layout.preferredWidth: 20
                                                onClicked: deletePoint(index)
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#ffffff"

                    Canvas {
                        id: pathCanvas
                        anchors.fill: parent

                        onPaint: {
                            const ctx = getContext('2d');
                            ctx.clearRect(0, 0, width, height);

                            if (pathModel.count > 0) {
                                ctx.beginPath();
                                for (let i = 0; i < pathModel.count; i++) {
                                    const p = pathModel.get(i);
                                    if (p.type === 77) {
                                        ctx.moveTo(p.x, p.y);
                                    } else if (i > 0) {
                                        const prevPoint = pathModel.get(i-1);
                                        if (prevPoint.isBezier) {
                                            ctx.bezierCurveTo(
                                                prevPoint.cp1x, prevPoint.cp1y,
                                                p.cp2x, p.cp2y,
                                                p.x, p.y
                                            );
                                        } else {
                                            ctx.lineTo(p.x, p.y);
                                        }
                                    }
                                }

                                // 使用同步后的颜色
                                ctx.strokeStyle = strokeColor;
                                ctx.fillStyle = fillColor;
                                ctx.lineWidth = lineWidth;
                                if (fillPath) ctx.fill();
                                ctx.stroke();

                                if (showControlPoints || showControlLines) {
                                    ctx.lineWidth = 1;
                                    ctx.strokeStyle = "gray";

                                    for (let i = 0; i < pathModel.count; i++) {
                                        const p = pathModel.get(i);

                                        if (i < pathModel.count - 1 && p.isBezier) {
                                            const nextPoint = pathModel.get(i+1);

                                            if (showControlLines) {
                                                ctx.beginPath();
                                                ctx.moveTo(p.x, p.y);
                                                ctx.lineTo(p.cp1x, p.cp1y);
                                                ctx.stroke();

                                                ctx.beginPath();
                                                ctx.moveTo(nextPoint.x, nextPoint.y);
                                                ctx.lineTo(nextPoint.cp2x, nextPoint.cp2y);
                                                ctx.stroke();
                                            }

                                            if (showControlPoints) {
                                                ctx.beginPath();
                                                ctx.arc(p.cp1x, p.cp1y, 4, 0, Math.PI * 2);
                                                ctx.fillStyle = "green";
                                                ctx.fill();
                                                ctx.stroke();

                                                ctx.beginPath();
                                                ctx.arc(nextPoint.cp2x, nextPoint.cp2y, 4, 0, Math.PI * 2);
                                                ctx.fillStyle = "blue";
                                                ctx.fill();
                                                ctx.stroke();
                                            }
                                        }

                                        ctx.beginPath();
                                        ctx.arc(p.x, p.y, 5, 0, Math.PI * 2);
                                        ctx.fillStyle = (i === selectedPointIndex && dragPointType === 0) ? "yellow" : "red";
                                        ctx.fill();
                                        ctx.stroke();
                                    }
                                }
                            } else {
                                ctx.fillStyle = "gray";
                                ctx.font = "14px Arial";
                                ctx.textAlign = "center";
                                ctx.fillText("点击空白区域添加路径点", width / 2, height / 2);
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: pathedit.isDragging ? Qt.ClosedHandCursor : Qt.ArrowCursor

                            onPressed: function(mouse){
                                const closest = pathedit.findClosestPoint(mouse.x, mouse.y);
                                pathedit.selectPoint(closest.index, closest.type);

                                if (closest.index !== -1) {
                                    pathedit.dragPointIndex = closest.index;
                                    pathedit.dragPointType = closest.type;
                                    pathedit.dragStartPos = Qt.point(mouse.x, mouse.y);
                                    pathedit.isDragging = true;
                                } else {
                                    pathedit.isDragging = false;
                                    pathedit.selectedPointIndex = -1;
                                    pathedit.dragPointType = 0;
                                }
                            }

                            onPositionChanged: function(mouse) {
                                if (pathedit.isDragging && pathedit.dragPointIndex >= 0) {
                                    const dx = mouse.x - pathedit.dragStartPos.x;
                                    const dy = mouse.y - pathedit.dragStartPos.y;

                                    const p = pathModel.get(pathedit.dragPointIndex);
                                    let newX, newY;

                                    if (pathedit.dragPointType === 0) {
                                        newX = p.x + dx;
                                        newY = p.y + dy;

                                        if (pathedit.dragPointIndex > 0) {
                                            const prevPoint = pathModel.get(pathedit.dragPointIndex - 1);
                                            pathModel.setProperty(pathedit.dragPointIndex - 1, "cp1x", prevPoint.cp1x + dx);
                                            pathModel.setProperty(pathedit.dragPointIndex - 1, "cp1y", prevPoint.cp1y + dy);
                                        }

                                        if (pathedit.dragPointIndex < pathModel.count - 1) {
                                            pathModel.setProperty(pathedit.dragPointIndex, "cp2x", p.cp2x + dx);
                                            pathModel.setProperty(pathedit.dragPointIndex, "cp2y", p.cp2y + dy);
                                        }
                                    } else if (pathedit.dragPointType === 1) {
                                        newX = p.cp1x + dx;
                                        newY = p.cp1y + dy;
                                    } else if (pathedit.dragPointType === 2) {
                                        newX = p.cp2x + dx;
                                        newY = p.cp2y + dy;
                                    }

                                    pathedit.updatePoint(pathedit.dragPointIndex, pathedit.dragPointType, newX, newY);
                                    pathedit.dragStartPos = Qt.point(mouse.x, mouse.y);
                                }
                            }

                            onReleased: {
                                pathedit.isDragging = false;
                                pathedit.dragPointIndex = -1;
                                pathedit.dragPointType = 0;
                            }

                            onClicked: function(mouse) {
                                if (!pathedit.isDragging && pathedit.selectedPointIndex === -1) {
                                    pathedit.createPoint(mouse.x, mouse.y);
                                }
                            }

                            onDoubleClicked: function() {
                                if (pathedit.selectedPointIndex >= 0 && pathedit.dragPointType === 0) {
                                    pathedit.deletePoint(pathedit.selectedPointIndex);
                                }
                            }
                        }
                    }
                }
            }

            RowLayout {
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.margins: 10
                spacing: 10

                Button {
                    text: "取消"
                    onClicked: {
                        pathedit.visible = false;
                    }
                }
                Button {
                    text: "保存"
                    onClicked: savePath();
                }
            }

            function selectPoint(index, type) {
                if (index >= 0 && index < pathModel.count) {
                    selectedPointIndex = index;
                    dragPointType = type;
                } else {
                    selectedPointIndex = -1;
                    dragPointType = 0;
                }
                pathCanvas.requestPaint();
            }

            onClosing: {
                this.destroy();
            }
        }
    }
    property var pathedit: null
}







// import QtQuick
// import QtQuick.Layouts
// import QtQuick.Controls.Material
// import com.custom 1.0
// //Path对象
// Canvas {
//     id: obj
//     property int data_id
//     property var path    //路径
//     property color fillStyle //填充模式
//     property color strokeStyle //描边模式
//     property bool fill //是否填充
//     property bool stroke //是否描边
//     property real lineWidth  //描边宽度

//     property alias cp_propertyBar: cp_propertyBar

//     function die() {
//         obj.destroy()
//     }

//     Component {
//         id: cp_propertyBar //属性栏
//         GridLayout {
//             columns: 2
//             columnSpacing: parent.width * 0.18
//             Text { text: "元素类别"; font.pixelSize: 16 }
//             Text {
//                 text: "路径";
//                 font.pixelSize: 16
//             }
//             LineText {
//                 Layout.columnSpan: 2
//                 Layout.fillWidth: true
//                 text: "锚定信息"
//             }
//             Text { text: "垂直对齐"; font.pixelSize: 16 }
//             ComboBox {
//                 model: ["中心对齐","下对齐","上对齐"]
//             }

//             Text { text: "垂直偏移"; font.pixelSize: 16 }
//             MyDoubleSpinBox {
//                 step: 0.1
//                 precision: 1
//                 initialValue: Helper.pixel2cm(SoupleManager.qmlGetData(data_id,Helper_Type.VALIGNOFFSET))
//                 suffix: "cm"
//                 onValueChanged: {
//                     SoupleManager.sendCommandToData(data_id,Helper.VALIGNOFFSET_UP,Helper.cm2pixel(value))
//                 }
//             }

//             LineText {
//                 Layout.columnSpan: 2
//                 Layout.fillWidth: true
//                 text: "路径信息"
//             }
//             Text { text: "水平缩放"; font.pixelSize: 16 }
//             MyDoubleSpinBox {
//                 id: spin_1
//                 step: 0.1
//                 precision: 2
//                 initialValue: Helper.pixel2cm(SoupleManager.qmlGetData(data_id,Helper_Type.XSCALE))
//                 onValueChanged: {
//                     //obj.width = Helper.cm2pixel(value)
//                 }
//             }
//             Text { text: "垂直缩放"; font.pixelSize: 16 }
//             MyDoubleSpinBox {
//                 id: spin_2
//                 step: 0.1
//                 precision: 2
//                 initialValue: Helper.pixel2cm(obj.height)
//                 Binding on value {
//                     when: obj.heightChanged
//                     value: Helper.pixel2cm(obj.height)
//                 }
//                 onValueChanged: {
//                     obj.height = Helper.cm2pixel(value)
//                 }
//             }
//         }
//     }

//     Menu {
//         id: menu
//         MenuItem {
//             text: "删除"
//             onTriggered: {
//                 SoupleManager.requestDeleteObj(obj.data_id)
//                 parent.focus = false
//             }
//         }
//     }

//     onFocusChanged: {
//         if(focus) selectedObj = obj
//     }

//     MouseArea {
//         anchors.fill: parent
//         acceptedButtons: Qt.LeftButton | Qt.RightButton
//         onPressed: function(mouse){
//             parent.focus = true
//         }
//         onClicked: function(mouse){
//             if(mouse.button == Qt.RightButton) {
//                 menu.x = mouse.x
//                 menu.y = mouse.y
//                 menu.open()
//             }
//         }
//     }


//     onPathChanged: {
//         requestPaint()
//     }
//     onPaint: {
//         var ctx = getContext('2d')
//         ctx.fillStyle = fillStyle
//         ctx.strokeStyle = strokeStyle
//         ctx.lineWidth = lineWidth
//         ctx.beginPath()
//         for(let i = 0; i < path.length; ++i)
//         {
//             let action = path[i]
//             if(action.t == 77) { //'M'
//                 ctx.moveTo(action.x,action.y)
//             } else if(action.t == 76) { //'L'
//                 ctx.lineTo(action.x,action.y)
//             } else if(action.t == 66) { //'B' 贝塞尔曲线
//                 //console.log("贝塞尔")
//                 const cp1 = action
//                 const cp2 = path[++i]
//                 const s = path[++i]
//                 ctx.bezierCurveTo(cp1.x,cp1.y,cp2.x,cp2.y,s.x,s.y)
//             }
//         }
//         if(fill) ctx.fill()
//         if(stroke) ctx.stroke()
//     }
// }
