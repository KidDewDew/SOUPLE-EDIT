import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects

Rectangle {
    id: titleBar
    //radius: 6
    //color: "#C7C7C7"
    color: theme.bg
    implicitHeight: 36

    property int selectedId: -1
    property var selectedTab: null
    property bool isSoupleDocument: false

    property int s_count: 0

    // Rectangle {
    //     width: parent.width
    //     height: 8
    //     color: parent.color
    //     anchors.bottom: parent.bottom
    // }

    function addDocument(tabInf) {
        tabInf.s = s_count++
        model_tabs.append(tabInf)
        if(tabInf.documentType == "souple") {
            //通知SoupleManager
            SoupleManager.addSoupleDocument(tabInf.s)
            SoupleManager.switchSoupleDocument(tabInf.s)
        }
        selectedId = tabInf.s
    }

    function getDocumentList(filter) {
        let result = []
        for(let i = 0; i < model_tabs.count; ++i) {
            const tab = model_tabs.get(i)
            if(filter==null || filter(tab) == true) {
                result.push(tab)
            }
        }
        return result
    }

    onSelectedIdChanged: {
        for(var i = 0; i < model_tabs.count; ++i) {
            var tab = model_tabs.get(i)
            if(tab.s == selectedId) {
                selectedTab = tab
                break
            }
        }
        if(selectedTab.documentType == "souple") { //souple文档
            isSoupleDocument = true
            loader_otherDocument.source = ""
            SoupleManager.switchSoupleDocument(selectedTab.s)
        } else if(selectedTab.documentType == "special") {
            isSoupleDocument = false
            loader_otherDocument.source = selectedTab.qmlSource //使用指定的qml文件来加载页面
            SoupleManager.switchSoupleDocument(-1) //无souple文档
        }
    }

    ListModel {
        id: model_tabs
        // ListElement {
        //     s: 0
        //     tabName: "文档1.pdf"
        //     documentType: "souple"
        //     icon_src: "qrc:/image/icon_doc.png"
        // }
        // ListElement {
        //     s: 1
        //     tabName: "开始页-欢迎"
        //     documentType: "special"
        //     icon_src: "qrc:/image/嗖谷熊_icon.png"
        //     qmlSource: ""
        // }
        // ListElement {
        //     s: 2
        //     tabName: "文档2.pdf"
        //     documentType: "souple"
        //     icon_src: "qrc:/image/icon_doc.png"
        // }
        // ListElement {
        //     s: 3
        //     tabName: "文档3.pdf"
        //     documentType: "souple"
        //     icon_src: "qrc:/image/icon_doc.png"
        // }
    }

    MouseArea {
        anchors.fill: parent
        onPressed: window.startSystemMove()
        onDoubleClicked: {
            if(window.visibility == Window.Maximized) Helper.platform_restoreWindow()
            else Helper.platform_maximizeWindow()
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.topMargin: 2
        spacing: 0
        Image {
            Layout.leftMargin: 8
            source: "qrc:/image/main_icon.png"
            Layout.preferredHeight: 18
            Layout.preferredWidth: 18
            antialiasing: true
        }
        Text {
            Layout.leftMargin: 6
            text: "嗖谷文档编辑软件"
            font.pixelSize: 13
            Layout.rightMargin: 10
            color: theme.fg
        }

        Repeater {
            id: tab_rep
            model: model_tabs
            delegate: RectangularGlow {
                //radius: 4
                id: glow
                property real initialX
                glowRadius: 3
                spread: 0.02
                cornerRadius: 7
                Layout.preferredHeight: 28
                Layout.fillWidth: true
                Layout.maximumWidth: 220
                Layout.leftMargin: 4
                Layout.topMargin: 6
                color: glow.selected ? "#40000000" : "#20000000"
                property bool selected: selectedId == s
                Behavior on x {
                    enabled: marea.pressed == false
                    NumberAnimation { duration: 500 }
                }
                onXChanged: {
                    if(marea.pressed) {
                        //拖动
                        var moveX = glow.x - glow.initialX
                        if(moveX > glow.width * 0.5) {
                            if(index+1 < model_tabs.count) {
                                model_tabs.move(index,index+1,1)
                                glow.initialX += glow.width+4
                            }
                        } else if(moveX < -glow.width * 0.5) {
                            if(index > 0) {
                                model_tabs.move(index,index-1,1)
                                glow.initialX -= glow.width+4
                            }
                        }
                    }
                }
                //Behavior on color { ColorAnimation { duration: 250 } }
                Rectangle {
                    color: glow.selected ? theme.bar :
                                (marea2.hovered ? theme.hover : theme.press)
                    clip: true
                    //anchors.fill: parent
                    width: parent.width
                    height: parent.height
                    radius: 4
                    border.width: 1
                    border.color: marea2.hovered ? "#969696" : "#00000000"
                    MouseArea {
                        id: marea
                        anchors.fill: parent
                        onClicked: selectedId = s
                        drag.target: glow
                        drag.axis: Drag.XAxis
                        onPressed: {
                            glow.initialX = glow.x
                        }
                        onReleased: {
                            glow.x = glow.initialX
                        }
                    }
                    Image {
                        source: icon_src
                        width: 18; height: 18
                        anchors.verticalCenter: parent.verticalCenter
                        fillMode: Image.PreserveAspectFit
                        x:4
                    }
                    Text {
                        text: tabName
                        font.pixelSize: 13
                        anchors.verticalCenter: parent.verticalCenter
                        elide: Text.ElideRight
                        x: 32
                        color: theme.fg
                    }
                    Rectangle {
                        visible: glow.selected || marea2.hovered
                        anchors.fill: rightBar
                        anchors.rightMargin: -4
                        color: parent.color
                    }
                    RowLayout {
                        id: rightBar
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 5
                        spacing: 2
                        visible: glow.selected || marea2.hovered
                        CustomToolButton {
                            Layout.preferredWidth: 18
                            Layout.preferredHeight: 18
                            contentItem: Item {
                                width: 12
                                height: 12
                                Rectangle {
                                    width: 12
                                    height: 1.4
                                    color: "grey"
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                                Rectangle {
                                    width: 12
                                    height: 1.4
                                    color: "grey"
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: -5
                                }
                                Rectangle {
                                    width: 12
                                    height: 1.4
                                    color: "grey"
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: 5
                                }
                            }
                        }

                        CustomToolButton {
                            Layout.preferredWidth: 18
                            Layout.preferredHeight: 18
                            contentItem: Text {
                                text: "×"
                                font.pixelSize: 14
                            }
                        }

                    }
                    HoverHandler {
                        id: marea2
                    }
                }
            }
        }

        // SquareButton {
        //     text: "+"
        // }

        // Item {
        //     Layout.fillWidth: true
        // }
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip:true
            Canvas {
                visible: parent.width > ttt.width + 40
                width: parent.width - 40
                height: 3
                anchors.bottom: parent.bottom
                x: 30
                onPaint: {
                    var ctx = getContext('2d')
                    ctx.beginPath()
                    ctx.moveTo(0,1)
                    ctx.lineTo(width,1)
                    ctx.lineWidth = 0.4
                    ctx.strokeStyle = "#999999"
                    ctx.stroke()
                }
            }

            TextInput {
                id: ttt
                visible: parent.width > ttt.width + 40
                text: "欢迎使用嗖谷编辑软件！"
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 5
                anchors.right: parent.right
                anchors.rightMargin: 10
                font.pixelSize: 13
                color: "#999999"
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.IBeamCursor
                    acceptedButtons: Qt.NoButton
                }
            }
        }

        ImageButton {
            Layout.preferredWidth: 24
            Layout.preferredHeight: 24
            source: "qrc:/image/icon_chajian.png"
            Layout.rightMargin: 14
        }
        ImageButton {
            Layout.preferredWidth: 24
            Layout.preferredHeight: 24
            source: "qrc:/image/icon_user.png"
            Layout.rightMargin: 14
        }
        ImageButton {
            Layout.preferredWidth: 24
            Layout.preferredHeight: 24
            source: "qrc:/image/嗖谷熊_icon.png"
            Layout.rightMargin: 25
        }
        CustomToolButton {
            Layout.preferredWidth: 40
            Layout.fillHeight: true
            Behavior on color { ColorAnimation { duration: 300 } }
            contentItem: Rectangle {
                anchors.centerIn: parent
                width: 9
                height: 1
                color: (darkMode ? "white" : "#212121")
            }
            onClicked: {
                Helper.platform_minimizeWindow()
            }
        }
        CustomToolButton {
            Layout.preferredWidth: 40
            Layout.fillHeight: true
            Behavior on color { ColorAnimation { duration: 300 } }
            contentItem: Rectangle {
                anchors.centerIn: parent
                width: 9
                height: 9
                color: "#00000000"
                border.width: 1
                border.color: (darkMode ? "white" : "#212121")
                radius: 2
                Rectangle {
                    visible: window.visibility == Window.Maximized
                    width: 9
                    height: 9
                    radius: 1
                    x: -2; y: 2
                    color: theme.bg
                    border.width: 1
                    border.color: (darkMode ? "white" : "#212121")
                }
            }
            onClicked: {
                if(window.visibility == Window.Maximized)
                    Helper.platform_restoreWindow()
                else Helper.platform_maximizeWindow()
            }
        }
        CustomToolButton {
            id: ttb1
            Layout.preferredWidth: 40
            Layout.fillHeight: true
            color: down ? "#DE5B61" : (hovered ? "#D44C4E" : "#00D44C4E")
            Behavior on color { ColorAnimation { duration: 300 } }
            property color tcolor: (down || hovered) ? "white" : (darkMode ? "white" : "#212121")
            Behavior on tcolor { ColorAnimation { duration: 300 } }
            onClicked: {
                window.doQuit()
            }
            contentItem: Canvas {
                id: canvas_x
                width: 9; height: 9
                Connections {
                    target: ttb1
                    function onTcolorChanged(){
                        //console.log("tcolor changed")
                        canvas_x.requestPaint()
                    }
                }
                onPaint: {
                    var ctx = getContext('2d')
                    ctx.clearRect(0,0,9,9)
                    ctx.beginPath()
                    ctx.moveTo(0,0)
                    ctx.lineTo(9,9)
                    ctx.moveTo(9,0)
                    ctx.lineTo(0,9)
                    ctx.strokeStyle = ttb1.tcolor
                    ctx.stroke()
                }
            }
        }

    }
}
