import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Controls.Windows as Windows
//import Qt.labs.platform as Platform
import Qt5Compat.GraphicalEffects
import QtQuick.Dialogs as Dialogs
import QtQuick.Effects
import com.custom 1.0
//直接在这里实现总体 ui

Window {
    id: window
    width: 1160
    height: 680
    visible: true
    title: "嗖谷文档编辑软件"

    property real cycaption          //windows下顶部小标题栏高度

    readonly property real windowShadowBW:
        window.visibility == Window.Maximized ? cycaption+1 : 0
    property alias horRuler: horRuler

    //property var theme: checkbox_protect_eye.checked ? theme_dark : theme_light

    property alias theme: theme
    property alias alias_soupleEdit: soupleEdit

    QtObject {
        id: theme
        property color fg: "black"
        property color bg: "#EBEBEB"
        property color bar: "white"
        property color splitLine: "#A8A8A8"
        property color page_bg: "white"
        property color accent_dark: "#1820A8"
        property color accent_light: "#3C8761"
        property color hover: "#EDEDED"
        property color press: "#C7C7C7"
        property color bottom_pane: "#C7C7C7"
        Behavior on fg {
            ColorAnimation { duration: 800 }
        }
        Behavior on bg {
            ColorAnimation { duration: 800 }
        }
        Behavior on bar {
            ColorAnimation { duration: 800 }
        }
        Behavior on splitLine {
            ColorAnimation { duration: 500 }
        }
        Behavior on page_bg {
            ColorAnimation { duration: 800 }
        }
        Behavior on accent_light {
            ColorAnimation { duration: 500 }
        }
        Behavior on accent_dark {
            ColorAnimation { duration: 500 }
        }
        Behavior on press {
            ColorAnimation { duration: 500 }
        }
        Behavior on hover {
            ColorAnimation { duration: 500 }
        }
        Behavior on bottom_pane {
            ColorAnimation { duration: 500 }
        }
    }

    QtObject {
        id: theme_light
        property color fg: "black"
        property color bg: "#EBEBEB"
        property color bar: "white"
        property color splitLine: "#A8A8A8"
        property color page_bg: "white"
        property color accent_dark: "#1820A8"
        property color accent_light: "#3C8761"
        property color hover: "#EDEDED"
        property color press: "#C7C7C7"
        property color bottom_pane: "#C7C7C7"
    }

    QtObject {
        id: theme_dark
        property color fg: "white"
        property color bg: "#333333"
        property color bar: "#141414"
        property color splitLine: "#666666"
        property color page_bg: "#9FB09D"
        property color accent_dark: "#C29F6E"
        property color accent_light: "#AB6340"
        property color hover: "#262626"
        property color press: "#383838"
        property color bottom_pane: "#000000"
    }

    Material.foreground: theme.fg
    Material.background: theme.bg
    Material.primary: theme.bg
    Material.theme: isProtectEyeMode ? Material.Dark : Material.Light

    Material.accent: theme.accent_light

    //Material.Variant:

    property bool isProtectEyeMode: false//: checkbox_protect_eye.checked

    property bool darkMode: isProtectEyeMode

    onIsProtectEyeModeChanged: {
        var theme2 = isProtectEyeMode ? theme_dark : theme_light
        theme.fg = theme2.fg
        theme.bg = theme2.bg
        theme.bar = theme2.bar
        theme.splitLine = theme2.splitLine
        theme.page_bg = theme2.page_bg
        theme.accent_dark = theme2.accent_dark
        theme.accent_light = theme2.accent_light
        theme.hover = theme2.hover
        theme.press = theme2.press
        theme.bottom_pane = theme2.bottom_pane
        Helper.DB_Set("Protect_Eye",isProtectEyeMode)
    }


    Component.onCompleted: {
        //window.showMaximized()
        main_titleBar.addDocument(
                    {
                        tabName: "欢迎 -开始页",
                        documentType: "special",
                        qmlSource: "DocumentSpec_StartPage.qml",
                        icon_src: "qrc:/image/嗖谷熊_icon.png"
                    }
        )
        //checkbox_protect_eye.checked = Helper.DB_Get("Protect_Eye",false)
    }

    SelectionObj {
        id: spp
    }

    flags: Qt.Window | Qt.FramelessWindowHint | Qt.WindowMinimizeButtonHint

    // MouseArea {
    //     y: parent.height - windowShadowBW - 2
    //     height: 6
    //     width: parent.width - windowShadowBW * 2
    //     x: windowShadowBW
    //     cursorShape: Qt.SizeVerCursor
    //     onPressed: {
    //         window.startSystemResize(Qt.BottomEdge)
    //     }
    // }

    // MouseArea {
    //     y: windowShadowBW - 4
    //     height: 6
    //     width: parent.width - windowShadowBW * 2
    //     x: windowShadowBW
    //     cursorShape: Qt.SizeVerCursor
    //     onPressed: {
    //         window.startSystemResize(Qt.TopEdge)
    //     }
    // }

    // MouseArea {
    //     y: windowShadowBW
    //     height: parent.height - 2*windowShadowBW
    //     width: 6
    //     x: windowShadowBW - 4
    //     cursorShape: Qt.SizeHorCursor
    //     onPressed: {
    //         window.startSystemResize(Qt.LeftEdge)
    //     }
    // }

    // MouseArea {
    //     y: windowShadowBW
    //     height: parent.height - 2*windowShadowBW
    //     width: 6
    //     x: parent.width - windowShadowBW - 2
    //     cursorShape: Qt.SizeHorCursor
    //     onPressed: {
    //         window.startSystemResize(Qt.RightEdge)
    //     }
    // }

    // MouseArea {
    //     y: windowShadowBW
    //     height: parent.height - 2*windowShadowBW
    //     width: 6
    //     x: parent.width - windowShadowBW - 2
    //     cursorShape: Qt.SizeHorCursor
    //     onPressed: {
    //         window.startSystemResize(Qt.RightEdge)
    //     }
    // }

    // MouseArea {
    //     x: windowShadowBW - 4
    //     y: windowShadowBW - 4
    //     height: 8
    //     width: 8
    //     cursorShape: Qt.SizeFDiagCursor
    //     onPressed: {
    //         window.startSystemResize(Qt.LeftEdge | Qt.TopEdge)
    //     }
    // }

    // MouseArea {
    //     x: window.width - windowShadowBW - 2
    //     y: windowShadowBW - 4
    //     height: 8
    //     width: 8
    //     cursorShape: Qt.SizeBDiagCursor
    //     onPressed: {
    //         window.startSystemResize(Qt.RightEdge | Qt.TopEdge)
    //     }
    // }

    // MouseArea {
    //     x: window.width - windowShadowBW - 2
    //     y: window.height - windowShadowBW - 2
    //     height: 8
    //     width: 8
    //     cursorShape: Qt.SizeFDiagCursor
    //     onPressed: {
    //         window.startSystemResize(Qt.RightEdge | Qt.BottomEdge)
    //     }
    // }

    // MouseArea {
    //     x: windowShadowBW - 2
    //     y: window.height - windowShadowBW - 2
    //     height: 8
    //     width: 8
    //     cursorShape: Qt.SizeBDiagCursor
    //     onPressed: {
    //         window.startSystemResize(Qt.LeftEdge | Qt.BottomEdge)
    //     }
    // }


    // property var theme: theme_0
    // property var theme_list: [["浅色",theme_0],["深色",theme_1]]
    // QtObject { //主题配色: 浅色
    //     id: theme_0
    //     property color bg: "#D9D9D9" //背景
    //     property color fg: "black"  //前景
    //     property color bd: "black" //边框
    //     property color bbg: "#FFFFFF" //按钮背景
    //     property color bfg: "black"   //按钮前景
    // }
    // QtObject { //主题配色: 深色
    //     id: theme_1
    //     property color bg: "#D9D9D9"
    // }

    // Dialogs.ColorDialog {
    //     id: color_dialog
    //     onAccepted: control.color = selectedColor
    // }

    //onClosing: {
    //    SoupleManager.pauseDeal() //暂停处理
    //}

    function doQuit() {
        Qt.quit()
    }

    function open_dialog_setJAVA_HOME() {
        console.log("open_dialog_setJAVA_HOME()")
        const d = Qt.createQmlObject(`import QtQuick
                            Dialog_setJAVA_HOME{}`,window);
        d.open()
        d.closed.connect(function(){ d.destroy() })
    }

    //显示【选择】板(锚定坐标，锚定方向(TOP|BOTTOM|LEFT|RIGHT)，问题，选项，回调函数)
    function showChooseBoard(parent_id,anchor_position,anchor_dir,question,options,callback) {
        soupleEdit.chooseBoard.show(parent_id,anchor_position,anchor_dir,question,options,callback)
    }

    function hideChooseBoard(parent_id) {
        if(soupleEdit.chooseBoard.parent_id == parent_id) {
            soupleEdit.chooseBoard.hide()
        }
    }

    function changeViewTop(view_top) {
        scv11.contentY = view_top
    }

    //创建qml-soupleObj
    function generateObj(type) {
        console.log("generateObj(",type)
        return soupleEdit.newObj(type)
    }

    function appendPage(width,height) {
        console.log("appendPage")
        soupleEdit.appendPage(width,height)
    }

    //获取目前被选中的obj-id
    function getSelectedID() {
        if(soupleEdit.selectedObj == null) return -1
        return soupleEdit.selectedObj.data_id
    }

    function preloadPDF(filename) {
        console.log("qml preloadPDF("+filename)
        var task = task_row.addTask("读取PDF字体")
        Helper.requestPreLoadPdf(filename,function(arg) {
            task.setProgress(arg.progress)
        })
    }

    // function getSoupleEdit() {
    //     return soupleEdit;
    // }

    Dialog {
        id: dialog_loading
        anchors.centerIn: Overlay.overlay
        title: "加载进度"
        closePolicy: Popup.NoAutoClose
        background: Rectangle {
            radius: 0
        }
        ColumnLayout {
            RectsProgressBar {
                id: loading_bar
                Layout.preferredHeight: 24
                Layout.preferredWidth: 290
                numRect: 20
                accent: "#196CD9"
                accent2: "#D0FFEB"
                //control_text.text: `${(value*100).toFixed(1)}%`
                // Behavior on value {
                //     NumberAnimation { duration: 500 }
                // }
            }
        }
    }

    Dialogs.FileDialog {
        id: dialog_savePdf
        title: "保存PDF"
        fileMode: Dialogs.FileDialog.SaveFile
        nameFilters: ["PDF Document (*.pdf)"]
        onAccepted: {
            PdfSaver.beginSaveAs(Helper.qurl2localfile(selectedFile))
        }
    }

    Connections {
        target: Helper
        function onBeginLoadDocument() {
            dialog_loading.open()
        }
        function onFileLoadProgress(pg) {
            loading_bar.value = pg
        }
        function onFileLoaded() {
            dialog_loading.close()
        }
        function onBeginSelection() { //开始选择
            if(soupleEdit.selectedObj) {

            }
            setPropertyPane(cp_selection_pane)
        }
        function onErrorMsg(title,msg) {
            messageBox(title,msg)
        }
        //function onClearSelection() { //清除选择内容
            //setPropertyPane(null)
        //}
    }

    Component {
        id: cp_selection_pane
        SelectionPropertyGrid {}
    }


    MyFunctions {
        soupleEdit: soupleEdit
    }

    Component {
        id: cp_menu
        Menu {

        }
    }

    // Component.onCompleted: {
    //     //soupleEdit.newPage(900,1200)
    //     // soupleEdit.newAnchorObj("VLine")
    //     // soupleEdit.newAnchorObj("VLine")
    //     // soupleEdit.newAnchorObj("HLine")
    //     // soupleEdit.findItem("V1").x = 40
    //     // soupleEdit.findItem("V2").x = 500
    //     // const h1 = soupleEdit.findItem("H1")
    //     // h1.leftLine = soupleEdit.findItem("V1")
    //     // h1.rightLine = soupleEdit.findItem("V2")
    //     // for(var i = 0; i < 1000; ++i) {
    //     //     so
    //     // }
    // }

    Dialogs.ColorDialog {
        id: color_dialog
        title: "选择颜色"
        property var callback
        onAccepted: function() {
            callback(selectedColor)
        }
    }

    Dialogs.FontDialog {
        id: font_dialog
        title: "选择字体"
        property var callback
        onAccepted: callback(selectedFont)
    }


    Dialogs.FileDialog {
        id: file_dialog
        property var callback
        onAccepted: callback(file_dialog.currentFile)
    }

    Dialog {
        id: dialog_objContainer
        parent: Overlay.overlay
        anchors.centerIn: parent
        title: "插入锚定元素"
        ObjContainer {
            width: 400
            height: 360
        }
    }

    Dialogs.MessageDialog {
        id: msgDialog
    }

    function messageBox(title,text) { //弹出消息对话框
        msgDialog.title = title
        msgDialog.text = text
        msgDialog.open()
    }

    function updateObjPropertyPane(obj) { //更新属性面板为obj
        // if(pane_property.item != null) {

        //     pane_property.item.destroy() //释放上一个对象
        // }
        if(property_window.visible)
            pane_property2.sourceComponent = obj.cp_propertyBar
        else pane_property.sourceComponent = obj.cp_propertyBar //更新source
    }

    function setPropertyPane(sourceComponent) { //更新属性面板
        if(property_window.visible)
            pane_property2.sourceComponent = sourceComponent
        else pane_property.sourceComponent = sourceComponent //更新source
    }

    // Rectangle {
    //     anchors.fill: parent
    //     color: "#EBEBEB"
    //     radius: 8
    // }

    Rectangle {
        id: fill_rect
        anchors.fill: parent
        anchors.leftMargin: windowShadowBW
        anchors.rightMargin: windowShadowBW
        anchors.topMargin: windowShadowBW
        anchors.bottomMargin: windowShadowBW
        radius: 6
        visible: false
    }

    // DropShadow {
    //     anchors.fill: fill_rect
    //     radius: 14.0
    //     color: "#60000000"
    //     source: fill_rect
    // }

    Rectangle { //背景色
        anchors.fill: parent
        anchors.bottomMargin: bottom_pane.height + windowShadowBW
        anchors.topMargin: main_titleBar.height + windowShadowBW
        anchors.leftMargin:  windowShadowBW
        anchors.rightMargin: windowShadowBW
        color: theme.bg
    }


    ColumnLayout {
        id: view
        y: -2
        anchors.fill: parent
        anchors.topMargin: main_titleBar.height + windowShadowBW

        anchors.leftMargin:  windowShadowBW
        anchors.rightMargin: windowShadowBW
        anchors.bottomMargin: windowShadowBW

        spacing: 0


        SplitView { //总菜单栏
            id: total_menuBar
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            spacing: 0
            ColumnLayout { //左
                id: leftRect
                SplitView.fillWidth: true
                SplitView.fillHeight: true
                Connections {
                    target: window
                    onWidthChanged: {
                        rightRect.SplitView.preferredWidth =
                                Math.max(120+window.width*0.042,window.width - 1200)
                    }
                }
                spacing: 0
                RowLayout { //左上
                    Layout.fillWidth: true
                    spacing: 0
                    MenuBar {  Material.background: theme.bg;id: menuBar; //顶层菜单
                        Menu {
                            title: "文件(&F)"
                            Action {
                                text: "新建(&N)         Ctrl+N"
                                shortcut: "Ctrl+N"
                            }
                            Action {
                                text: "打开(&O)         Ctrl+O"
                                shortcut: "Ctrl+O"
                                onTriggered: {
                                    //console.log("platform.os = ",Qt.platform.os)
                                    file_dialog.nameFilters =
                                            ["Souple File (*.soup *.souple)","PDF Document (*.pdf)"]
                                    file_dialog.open()
                                    file_dialog.callback = function() {
                                        if(file_dialog.selectedNameFilter.index == 0) { //.soup文件
                                            Helper.requestLoadSoup(Helper.qurl2localfile(file_dialog.currentFile))
                                        } else if(file_dialog.selectedNameFilter.index == 1) { //.pdf
                                            //先打开小助手窗口，并阻塞
                                            var helper_window = cp_pdfLoadHelper_window.createObject(window,
                                                                    {pdfFile:Helper.qurl2localfile(file_dialog.currentFile)})
                                            helper_window.show()
                                            //Helper.requestLoadPdf(Helper.qurl2localfile(file_dialog.currentFile))
                                            preloadPDF(helper_window.pdfFile)
                                        }
                                    }
                                    //}
                                }
                            }
                            Action {
                                text: "保存(&S)          Ctrl+S"
                                shortcut: "Ctrl+S"
                                onTriggered: {
                                    dialog_savePdf.open()
                                }
                            }
                            Action {
                                text: "输出为PDF(&T)"
                            }
                            Action {
                                text: "首选项...(&P)"
                            }
                            MenuSeparator {}
                            Action {
                                text: "退出(&E)         Ctrl+ESC"
                                shortcut: "Ctrl+Esc"
                            }
                        }
                        Menu {
                            title: "编辑(&E)"
                            //icon: ""
                            Action {
                                text: "撤销(&Z)            Ctrl+Z"
                                shortcut: "Ctrl+Z"
                            }
                            Action {
                                text: "重做(&Y)            Ctrl+Y"
                                shortcut: "Ctrl+Y"
                                //onTriggered: console.log("Ctrl+Y")
                            }
                            Action {
                                text: "查找/替换(&F)    Ctrl+F"
                                shortcut: "Ctrl+F"
                            }
                            MenuSeparator {}
                            Action {
                                text: "编辑器设置...(&S)"
                            }
                        }
                        Menu {
                            title: "插入(&I)"
                            //icon: ""
                            Menu {
                                title: "锚定元素(&AnchorElement)"
                                MenuItem {
                                    text: "流文本(&TextEdit)"
                                    onTriggered: soupleEdit.tryInsertAnchorObj("FlowText")
                                }
                                MenuItem {
                                    text: "占位文本(PH-TextEdit)"
                                }
                                MenuItem {
                                    text: "html框(&Html-Edit)"
                                }
                                MenuItem {
                                    text: "流矩形(&FlowRect)"
                                }
                                MenuItem {
                                    text: "占位矩形(PH-Rect)"
                                    onTriggered: soupleEdit.tryInsertAnchorObj("PH_Rect")
                                }
                                MenuItem {
                                    text: "占位换行(&Break)"
                                    onTriggered: soupleEdit.tryInsertAnchorObj("PH_Right")
                                }
                                Menu {
                                    title: "形状(&Shape)"
                                    MenuItem {
                                        text: "矩形(&Rect)"
                                    }
                                    MenuItem {
                                        text: "圆形(&Circle)"
                                    }
                                    MenuItem {
                                        text: "三角形(&Triangle)"
                                    }
                                }
                                MenuItem {
                                    text: "图片(&Image)"
                                    onTriggered: soupleEdit.tryInsertAnchorObj("Image")
                                }
                                MenuSeparator {}
                                MenuItem {
                                    text: "垂直标线(&VerLine)"
                                   // onTriggered: soupleEdit.newAnchorObj("VLine")
                                    onTriggered: SoupleManager.createSoupleObj_Free("VLine",200,100)
                                }
                                MenuItem {
                                    text: "水平标线(HorLine)"
                                }
                            }
                            Menu {
                                title: "自由元素(&FreeElement)"
                                MenuItem {
                                    text: "文本框(&TextEdit)"
                                }
                                MenuItem {
                                    text: "矩形(&Rect)"
                                }
                                MenuItem {
                                    text: "圆形(&Circle)"
                                }
                                MenuItem {
                                    text: "三角形(&Triangle)"
                                }
                                MenuItem {
                                    text: "图片(&Image)"
                                }
                                MenuItem {
                                    text: "html框(&Html-Edit)"
                                }
                            }
                        }
                        Menu {
                            title: "帮助(&H)"
                            MenuItem {
                                text: "关于...(&A)"
                            }
                        }
                    } //菜单Menu END
                    Rectangle {
                        Layout.preferredWidth: 0.8
                        Layout.preferredHeight: 26
                        Layout.alignment: Qt.AlignVCenter
                        color: theme.splitLine
                        Layout.leftMargin: 4
                        Layout.rightMargin: 4
                    }
                    ToolButton {
                        icon.source: "qrc:/image/icon_save.png"
                        icon.color: "transparent"
                        icon.width: 25; icon.height: 25
                    }
                    ToolButton {
                        icon.source: "qrc:/image/icon_undo.png"
                        icon.color: darkMode ? "white" : "transparent"
                        icon.width: 20; icon.height: 20
                        onClicked: {
                            SoupleManager.request_undo();
                        }
                    }
                    ToolButton {
                        icon.source: "qrc:/image/icon_redo.png"
                        icon.color: darkMode ? "white" : "transparent"
                        icon.width: 20; icon.height: 20
                        onClicked: {
                            SoupleManager.request_redo();
                        }
                    }
                    Rectangle {
                        Layout.preferredWidth: 0.8
                        Layout.preferredHeight: 26
                        Layout.alignment: Qt.AlignVCenter
                        color: theme.splitLine
                        Layout.leftMargin: 4
                        Layout.rightMargin: 4
                    }
                    TabBar { //左上右
                        id: tabBar
                        //height: menuBar.height
                        //width: parent.width - menuBar.width
                        Layout.fillWidth: true
                        Layout.maximumWidth: 420
                        Layout.alignment: Qt.AlignBottom
                        //Layout.fillHeight: true
                        Material.accent: theme.accent_dark
                        //Material.background: "#EBEBEB"
                        TabButton {
                            text: "开始"
                            height: 40
                            leftPadding: 0
                            rightPadding: 0
                        }
                        TabButton {
                            text: "插入"
                            height: 40
                            leftPadding: 0
                            rightPadding: 0
                        }
                        TabButton {
                            text: "页面"
                            height: 40
                            leftPadding: 0
                            rightPadding: 0
                        }
                        TabButton {
                            text: "布局"
                            height: 40
                            leftPadding: 0
                            rightPadding: 0
                        }
                        TabButton {
                            text: "视图"
                            height: 40
                            leftPadding: 0
                            rightPadding: 0
                        }
                        TabButton {
                            text: "工具"
                            height: 40
                            leftPadding: 0
                            rightPadding: 0
                        }
                    } //左上右 END
                } //左上 END
                Rectangle { //左下
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: theme.bar
                    layer.enabled: true
                    layer.effect: DropShadow {
                        radius: 5.0
                        color: "#50000000"
                    }

                    StackLayout { //二级工具栏
                        id: toolBar_2l
                        anchors.fill: parent
                        currentIndex: tabBar.currentIndex
                        property real dynamic_spacing: Math.min(width * 0.008,8)
                        NumberAnimation on dynamic_spacing {
                            id: ani_0001
                            from: 0.0; to: Math.min(toolBar_2l.width * 0.01,15)
                            duration: 400; easing.type: Easing.InOutQuad
                            onFinished: {
                                toolBar_2l.dynamic_spacing =
                                        Qt.binding(function(){ return Math.min(toolBar_2l.width * 0.01,15) })
                            }
                        }
                        onCurrentIndexChanged: ani_0001.start()

                        TToolBar_Start {
                            spacing: toolBar_2l.dynamic_spacing
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        RowLayout { //“插入”工具栏
                            spacing: toolBar_2l.dynamic_spacing
                            anchors.verticalCenter: parent.verticalCenter
                            Text {
                                text: "Anchor"; font.pixelSize: 11
                                color: "#949494"
                                Layout.alignment: Qt.AlignVCenter
                            }

                            AddToolButton {
                                text: "标线"
                                enableList: true
                                listText: ["水平标线","垂直标线"]
                                onClicked: {
                                    if(index == 0)
                                        SoupleManager.createSoupleObj_Free("HLine",50,soupleEdit.scroll_y+50)
                                    else SoupleManager.createSoupleObj_Free("VLine",200,soupleEdit.scroll_y+40)
                                }
                            }
                            // AddToolButton {
                            //     text: "垂直标线"
                            //     onClicked: SoupleManager.createSoupleObj_Free("VLine",200,100)
                            // }
                            Rectangle {
                                Layout.preferredWidth: 0.8
                                Layout.preferredHeight: 19
                                Layout.leftMargin: 4
                                Layout.rightMargin: 4
                                color: "#A8A8A8"
                            }
                            AddToolButton {
                                text: "文本"
                                onClicked: soupleEdit.tryInsertAnchorObj("FlowText")
                            }
                            // AddToolButton {
                            //     text: "换行"
                            //     onClicked: soupleEdit.tryInsertAnchorObj("PH_Right")
                            // }
                            AddToolButton {
                                text: "段落"
                                onClicked: soupleEdit.tryInsertAnchorObj("PH_Left")
                            }
                            AddToolButton {
                                text: "占位符"
                                enableList: true
                                listText: ["占位矩形","段落","换行","居中占位","左占位","右占位","整行占位"]
                                onSelected: {
                                    switch(index) {
                                      case 0: soupleEdit.tryInsertAnchorObj("PH_Rect"); break
                                      case 1: soupleEdit.tryInsertAnchorObj("PH_Left"); break
                                      case 2: soupleEdit.tryInsertAnchorObj("PH_Right"); break
                                      case 3: soupleEdit.tryInsertAnchorObj("JZRect"); break
                                      case 4: soupleEdit.tryInsertAnchorObj("GlueL"); break
                                      case 5: soupleEdit.tryInsertAnchorObj("GlueR"); break
                                      case 6: soupleEdit.tryInsertAnchorObj("PH_Line"); break
                                    }
                                }
                            }
                            AddToolButton {
                                text: "图片"
                                onClicked: soupleEdit.tryInsertAnchorObj("Image")
                            }
                            AddToolButton {
                                text: "形状"
                                enableList: true
                                listText: ["矩形","圆形","三角形"]
                            }
                            AddToolButton {
                                text: "表格"
                                onClicked: SoupleManager.createTable(4,4)
                            }
                            AddToolButton {
                                text: "装饰框"
                                onClicked: SoupleManager.tryCreateDecorationFrame()
                            }
                            AddToolButton {
                                text: "Latex公式"
                                onClicked: soupleEdit.tryInsertAnchorObj("Latex")
                            }
                            AddToolButton {
                                text: "更多.."
                                enableList: true
                                listText: ["换行","弹簧","导航符"]
                                onSelected: {
                                    switch(index) {
                                      case 0: soupleEdit.tryInsertAnchorObj("PH_Right"); break
                                      case 1: soupleEdit.tryInsertAnchorObj("Spring"); break
                                      case 2: soupleEdit.tryInsertAnchorObj("Nav"); break
                                    }
                                }
                            }
                            Text {
                                text: "Free"; font.pixelSize: 11
                                color: "#949494"
                                Layout.alignment: Qt.AlignVCenter
                            }
                            AddToolButton {
                                text: "自由标线"
                                onClicked: soupleEdit.insertFreeObj("BLine")
                            }
                            AddToolButton {
                                text: "文本"
                                onClicked: soupleEdit.insertFreeObj("FText")
                            }
                            AddToolButton {
                                text: "形状"
                                enableList: true
                                listText: ["矩形","圆形","三角形"]
                                onClicked: soupleEdit.insertFreeObj("FPath",str)
                            }
                            AddToolButton {
                                text: "图片"
                                onClicked: soupleEdit.insertFreeObj("FImage")
                            }
                            AddToolButton {
                                text: "背景框"
                                onClicked: soupleEdit.insertFreeObj("Frame")
                            }
                            AddToolButton {
                                text: "文档块"
                                onClicked: soupleEdit.insertFreeObj("FRich")
                            }
                        } //“插入”工具栏 END

                        RowLayout { //“页面”工具栏
                            spacing: toolBar_2l.dynamic_spacing
                            anchors.verticalCenter: parent.verticalCenter
                            ToolButton {
                                Layout.leftMargin: 5
                                text: "删除页面"
                            }
                            Rectangle {
                                Layout.preferredWidth: 0.8
                                Layout.preferredHeight: 19
                                Layout.leftMargin: 4
                                Layout.rightMargin: 4
                                color: "#A8A8A8"
                            }
                            Text {
                                text: "新建"; font.pixelSize: 11
                                color: "#949494"
                                Layout.alignment: Qt.AlignVCenter
                            }
                            AddToolButton {
                                text: "新建页面"
                                enableList: true
                                listText: ["▶ 继承格式","▶ A3页面","▶ A4页面","▶ A5页面","▶ A6页面","▶ 标准信纸页面"]
                                onSelected: function(index,str) {
                                    switch(index) {
                                    case 0: break
                                    case 1:
                                        SoupleManager.addPage(Helper.cm2pixel(29.7),Helper.cm2pixel(42)
                                                              ,Helper.cm2pixel(3.6),Helper.cm2pixel(3.2))
                                        break
                                    case 2:
                                        SoupleManager.addPage(Helper.cm2pixel(21),Helper.cm2pixel(29.7)
                                                              ,Helper.cm2pixel(2.8),Helper.cm2pixel(2.4))
                                        break
                                    case 3: //A5
                                        SoupleManager.addPage(Helper.cm2pixel(14.8),Helper.cm2pixel(21)
                                                              ,Helper.cm2pixel(2.2),Helper.cm2pixel(2.0))
                                        break
                                    case 4:
                                        SoupleManager.addPage(Helper.cm2pixel(10.5),Helper.cm2pixel(14.8)
                                                              ,Helper.cm2pixel(1.8),Helper.cm2pixel(1.6))
                                        break
                                    case 5:
                                        SoupleManager.addPage(Helper.cm2pixel(21.6),Helper.cm2pixel(27.9)
                                                              ,Helper.cm2pixel(3.0),Helper.cm2pixel(2.8))
                                        break
                                    }
                                }
                            }
                            Text {
                                text: "布局"; font.pixelSize: 11
                                color: "#949494"
                            }
                            Text {
                                text: "风格"
                            }
                            ComboBox {
                                id: combobox_pageStyle //页面风格选择框
                                Layout.preferredHeight: 36
                                Layout.preferredWidth: 90
                                leftPadding: 0
                                rightPadding: 0
                                Material.accent: "#3C8761"
                                model: ["常规","Word"]
                                onCurrentIndexChanged: {
                                    if(currentIndex == 0)
                                        SoupleManager.requestChangeCurrentPageStyle(Helper_Type.NoFormat_Page)
                                    else SoupleManager.requestChangeCurrentPageStyle(Helper_Type.Word_Page)
                                }
                            }
                            Text {
                                text: "栏数:"
                                visible: combobox_pageStyle.currentIndex === 1
                            }
                            TextField {
                                id: tf_columnNum
                                visible: combobox_pageStyle.currentIndex === 1
                                Layout.preferredHeight: 25
                                Layout.preferredWidth: focus ? 60 : 30
                                topPadding: 0; bottomPadding: 0
                                leftPadding: 4; rightPadding: 4
                                horizontalAlignment: Text.AlignHCenter
                                validator: IntValidator {
                                    bottom: 1; top: 5
                                }
                                onTextChanged: {
                                    SoupleManager.requestChangePageColumnNum(parseInt(text))
                                }
                            }
                            Text {
                                text: "页宽:"
                            }
                            TextField {
                                id: tf_page_width
                                Layout.preferredHeight: 25
                                Layout.preferredWidth: focus ? 80 : 40
                                topPadding: 0; bottomPadding: 0
                                leftPadding: 4; rightPadding: 4
                                font.pixelSize: 11
                                Material.accent: "#3C8761"
                                horizontalAlignment: Text.AlignHCenter
                                validator: DoubleValidator {}
                            }
                            Text {
                                text: "cm"
                            }
                            Text {
                                text: "页高:"
                            }
                            TextField {
                                id: tf_page_height
                                Layout.preferredHeight: 25
                                Layout.preferredWidth:  focus ? 80 : 40
                                topPadding: 0; bottomPadding: 0
                                leftPadding: 4; rightPadding: 4
                                font.pixelSize: 11
                                Material.accent: "#3C8761"
                                horizontalAlignment: Text.AlignHCenter
                                validator: DoubleValidator {}
                            }
                            Text {
                                text: "页上边距:"
                            }
                            TextField {
                                id: tf_page_topMargin
                                Layout.preferredHeight: 25
                                Layout.preferredWidth: 40
                                topPadding: 0; bottomPadding: 0
                                leftPadding: 4; rightPadding: 4
                                font.pixelSize: 11
                                Material.accent: "#3C8761"
                                horizontalAlignment: Text.AlignHCenter
                                validator: DoubleValidator {}
                            }
                            Text {
                                text: "页下边距:"
                            }
                            TextField {
                                id: tf_page_bottomMargin
                                Layout.preferredHeight: 25
                                Layout.preferredWidth: 40
                                topPadding: 0; bottomPadding: 0
                                leftPadding: 4; rightPadding: 4
                                font.pixelSize: 11
                                Material.accent: "#3C8761"
                                horizontalAlignment: Text.AlignHCenter
                                validator: DoubleValidator {}
                            }
                            Text {
                                text: "cm"
                            }
                            Text {
                                text: "页面旋转"; font.pixelSize: 11
                                color: "#949494"
                            }
                            ComboBox {
                                id: combobox_page_rotation
                                Layout.preferredHeight: 36
                                Layout.preferredWidth: 70
                                Material.accent: "#3C8761"
                                model: ["0°","90°","180°","270°"]
                            }
                        }

                        RowLayout { //“布局”工具栏
                            spacing: toolBar_2l.dynamic_spacing
                            anchors.verticalCenter: parent.verticalCenter
                            AddToolButton {
                                Layout.leftMargin: 12
                                text: "区间分栏"
                                enableList: true
                                listText: ["单栏","两栏","三栏","两栏-偏左","两栏-偏右"]
                                onSelected: function(index,str) {
                                    switch(index) {
                                    case 0: SoupleManager.requestSetContentColumns(1); break;
                                    case 1: SoupleManager.requestSetContentColumns(2); break;
                                    case 2: SoupleManager.requestSetContentColumns(3); break;
                                    case 3: SoupleManager.requestSetContentColumns(2,"偏左"); break;
                                    case 4: SoupleManager.requestSetContentColumns(2,"偏右"); break;
                                    }
                                }
                            }
                            Text {
                                text: "栏顶距"
                                color: theme.fg
                            }
                            MyDoubleSpinBox {
                                suffix: "cm"
                            }
                            Text {
                                text: "栏底距"
                                color: theme.fg
                            }
                            MyDoubleSpinBox {
                                suffix: "cm"
                            }
                        }

                        RowLayout { //“视图”工具栏
                            spacing: toolBar_2l.dynamic_spacing
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        RowLayout { //“工具”工具栏
                            spacing: toolBar_2l.dynamic_spacing
                            anchors.verticalCenter: parent.verticalCenter
                            CheckBox {
                                text: "定格"
                                Material.accent: "#0B3363"
                                onCheckedChanged: {
                                    if(checked) SoupleManager.freeze_frame()
                                    else SoupleManager.unfreeze_frame()
                                }
                            }
                            Button {
                                text: "测试-1"
                                onClicked: {
                                    Helper.doTest_1(parseInt(tf_test.text))
                                    d_test.open()
                                    img.source = ""
                                    img.source = "image://provider/test_page"
                                }
                                Dialog {
                                    id: d_test
                                    Image {
                                        id: img
                                        width:400
                                        height: sourceSize.height*width/sourceSize.width
                                    }
                                }
                            }
                            TextField {
                                id: tf_test
                                placeholderText: "测试参数"
                            }
                        }

                    } //二级工具栏 END
                } //左下 END
            }//左1 END
            Item {
                id: rightRect
                SplitView.fillHeight: true
                SplitView.preferredWidth: 160
                Behavior on SplitView.preferredWidth {
                    NumberAnimation { duration: 800; easing.type: Easing.InOutQuad }
                }
                DropShadow {
                    source: rect_right
                    anchors.fill: parent
                    radius: 8.0
                    horizontalOffset: -4
                    color: "#80000000"
                }
                Rectangle { //右
                    id: rect_right
                    color: theme.bar
                    anchors.fill: parent
                    Rectangle {
                        x: 4; y: 5; width: parent.width - 4; height: parent.height - 8
                        //color: "#F2F2F2"
                        color: Qt.darker(theme.bar,1.1)
                        RightToolBar {
                            id: rightToolBar
                            anchors.fill: parent
                            height: parent.height
                        }
                    }
                    // ObjContainer {
                    //     anchors.fill: parent
                    // }


                } //右END

                SquareButton {
                    text: rightRect.width < rightRect.parent.width/2 ? "<" : ">"
                    width: 12; height: 20
                    bg.radius: 3
                    bg.border.width: 0
                    label.font.pixelSize: 11
                    label.color: "black"
                    accent: "white"
                    anchors.verticalCenter: parent.verticalCenter
                    x: -width - 10
                    layer.enabled: true
                    layer.effect: DropShadow {
                        radius: 2.0
                        color: "#80000000"
                    }
                    onClicked: {
                        if(rightRect.width < rightRect.parent.width/2) {
                            rightRect.SplitView.preferredWidth = rightRect.parent.width * 0.7
                        } else {
                            rightRect.SplitView.preferredWidth = rightRect.parent.width * 0.2
                        }
                    }
                }

                // MyToolButton {
                //     text: "展开"
                //     font.pixelSize: 10
                //     color: "#6E6E6E"
                //     leftPadding: 0
                //     bottomPadding: 0
                //     topPadding: 0
                //     anchors.left: parent.left
                //     anchors.bottom: parent.bottom
                //     onClicked: {
                //         dialog_objContainer.open()
                //     }
                // }
            }
        } //总菜单栏 END

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.topMargin: 4
            visible: main_splitView.visible == false
            Loader {
                id: loader_otherDocument //加载其他文档的loader
                anchors.fill: parent
            }
        }

        SplitView { //主区域
            id: main_splitView
            visible: main_titleBar.isSoupleDocument
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.topMargin: 4
            orientation: Qt.Horizontal
            ColumnLayout {
                id: main_page_area
                SplitView.fillHeight: true
                SplitView.fillWidth: true
                Item {
                    id: horRuler_box
                    Layout.topMargin: 5
                    Layout.bottomMargin: 2
                    Layout.fillWidth: true
                    Layout.preferredHeight: 13
                    HorRuler{
                        id: horRuler //水平标尺
                        width: soupleEdit_cover.width
                        xScale: view_scale_slider.value2
                        height: 13
                        states: [
                            State {
                                when: horRuler.width < main_page_area.width
                                AnchorChanges {
                                    target: horRuler
                                    anchors.horizontalCenter: horRuler_box.horizontalCenter
                                }
                            },
                            State {
                                when: horRuler.width >= main_page_area.width
                                PropertyChanges {
                                    target: horRuler
                                    x: -scv11.scroll_x
                                }
                                AnchorChanges {
                                    target: horRuler
                                    anchors.horizontalCenter: undefined
                                }
                            }
                        ]
                    }
                }
                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Rectangle {
                        id: bg_001
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: soupleEdit_cover.width
                        height: parent.height
                        color: "#50ffffff"
                        visible: soupleEdit.pageCount > 0
                    }
                    DropShadow {
                        anchors.fill: bg_001
                        color: "#80202020"
                        source: bg_001
                        radius: 8
                        verticalOffset: 2
                    }

                    Flickable {
                        id: scv11
                        anchors.fill: parent
                        contentHeight: soupleEdit_cover.height
                        clip: true
                        property real scroll_x: contentX
                        property real to_contentY

                        acceptedButtons: Qt.NoButton

                        Behavior on contentY {
                            enabled: ! scrollbar_v1.pressed
                            SmoothedAnimation {
                                duration: 250
                                velocity: 300
                            }
                        }

                        onContentYChanged: {
                            if(scrollbar_v1.pressed) {
                                to_contentY = contentY
                            }
                        }

                        MouseArea {
                            id: marea_scv
                            anchors.fill: parent
                            readonly property real scrollStep: Helper.cm2pixel(1.4)
                            onWheel: function(wheel) {
                                if (wheel.angleDelta.y > 0) {
                                    scv11.to_contentY = Math.max(0,scv11.to_contentY - scrollStep)
                                    scv11.contentY = scv11.to_contentY
                                } else if (wheel.angleDelta.y < 0) {
                                    scv11.to_contentY = Math.min(soupleEdit_cover.height - scv11.height,
                                                              scv11.to_contentY + scrollStep)
                                    scv11.contentY = scv11.to_contentY
                                }
                                wheel.accepted = true
                            }
                            // onPressed: {
                            //     console.log(".......pressed")
                            //     SoupleManager.requestClearSelection()
                            // }
                        }

                        //flickDeceleration: 2000

                        flickableDirection: Flickable.VerticalFlick

                        Binding on contentWidth {
                            when: soupleEdit_cover.width > scv11.width
                            value: soupleEdit_cover.width
                        }

                        ScrollBar.horizontal: ScrollBar {
                            id: scrollbar_h1
                            policy: ScrollBar.AlwaysOn
                            visible: scv11.contentWidth > scv11.width
                        }
                        ScrollBar.vertical: ScrollBar {
                            id: scrollbar_v1
                            policy: ScrollBar.AlwaysOn
                            visible: scv11.contentHeight > scv11.height
                        }
                        //contentWidth: soupleEdit.width
                        //ScrollBar.horizontal.policy: ScrollBar.
                        // onWidthChanged: {
                        //     console.log("width:",width,"contentW:",soupleEdit.width)
                        // }

                        property real scroll_y: ScrollBar.vertical.position
                        onHeightChanged: {
                            SoupleManager.updateViewSize(scroll_y*soupleEdit.height,scroll_y*soupleEdit.height + height)
                        }
                        onScroll_yChanged: {
                            SoupleManager.updateViewSize(scroll_y*soupleEdit.height,scroll_y*soupleEdit.height + height)
                        }
                        Item {
                            id: soupleEdit_cover
                            width: soupleEdit.width * view_scale_slider.value2
                            height: soupleEdit.height * view_scale_slider.value2
                            anchors.horizontalCenter:
                                width < parent.width ? parent.horizontalCenter : undefined
                            // MouseArea {
                            //     id: select_drag_layer
                            //     hoverEnabled: true
                            //     anchors.fill: soupleEdit
                            //     onPositionChanged: function(mouse) {
                            //         console.log("mouse move: ",mouse.x,mouse.y)
                            //     }
                            // }
                            SoupleEdit {
                                id: soupleEdit
                                objectName: "qml_soupleEdit"
                                //scale: view_scale_slider.value
                                scroll_y: scv11.scroll_y * height
                                transform: Scale {
                                    origin.x: 0
                                    origin.y: 0
                                    xScale: view_scale_slider.value2
                                    yScale: view_scale_slider.value2
                                }
                                MouseArea {
                                    z: Helper_Type.Widget_Top
                                    anchors.fill: parent
                                    propagateComposedEvents: true
                                    hoverEnabled: false
                                    Component.onCompleted: {
                                        cursorShape = undefined
                                    }
                                    onPressed: (mouse)=> {
                                        //console.log("pressed....")
                                        SoupleManager.requestClearSelection()
                                        mouse.accepted = false
                                    }
                                }
                            }
                        }
                    }
                    Rectangle { //灰色遮盖层
                        id: grey_cover
                        color: "#50878787"
                        visible: false
                        anchors.fill: parent
                        function show() {
                            opacity = 0.0
                            visible = true
                            ani_004.start()
                        }
                        OpacityAnimator { //灰色遮盖层渐显动画
                            id: ani_004
                            target: grey_cover
                            duration: 300
                            to: 1.0
                        }
                        OpacityAnimator { //灰色遮盖层渐隐动画
                            id: ani_003
                            target: grey_cover
                            duration: 300
                            to: 0.0
                            onFinished: {
                                grey_cover.visible = false
                                grey_cover.opacity = 1.0
                            }
                        }
                        MouseArea {
                            id: marea_cover
                            anchors.fill: parent
                            cursorShape: Qt.CrossCursor   //默认十字光标(如果修改，请随后恢复)
                            property var callback_clicked //点击回调
                            onClicked: function(m) {
                                ani_003.start()
                                var point = Qt.point(m.x,m.y)
                                point = marea_cover.mapToItem(soupleEdit,point) //转换到soupleEdit
                                callback_clicked(point)
                            }
                        }
                        Rectangle {
                            id: calcel_cover_area //取消区域
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            width: 90
                            height: 55
                            scale: marea_002.pressed ? 0.85 : 1.0
                            color: "#BBF04F52"
                            Text {
                                anchors.centerIn: parent
                                text: "取消(Esc)"
                                color: "white"
                                font.bold: true
                                scale: marea_002.containsMouse ? 1.2 : 1.0
                            }
                            MouseArea {
                                id: marea_002
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.CrossCursor
                                onClicked: {
                                    ani_003.start() //隐藏
                                }
                            }
                        }
                    }
                }
            }
            ColumnLayout { //右边or左边属性面板
                id: page_propertyPane
                SplitView.preferredWidth: 280
                SplitView.fillHeight: true

                //窗口模式和面板模式只有一个显示
                visible: ! property_window.visible
                //title: "V1 - 属性"
                Item {
                    id: header_001
                    Layout.fillWidth: true
                    Layout.preferredHeight: 48
                    ToolButton {
                        text: "<"
                        anchors.verticalCenter: parent.verticalCenter
                        font.bold: true
                        ToolTip.visible: hovered
                        ToolTip.text: "移动该面板"
                        ToolTip.delay: 200
                        onClicked: {
                            if(text == "<") {
                                page_propertyPane.parent = null
                                main_page_area.parent = null
                                page_propertyPane.parent = main_splitView
                                main_page_area.parent = main_splitView
                                text = ">"
                            } else {
                                page_propertyPane.parent = null
                                main_page_area.parent = null
                                main_page_area.parent = main_splitView
                                page_propertyPane.parent = main_splitView
                                text = "<"
                            }
                        }
                    }
                    Text {
                        id: text_001
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        text: "❖属性面板"
                        font.pixelSize: 16
                        topPadding: 6
                    }

                    ToolButton {
                        text: "⦿"
                        font.bold: true
                        onClicked: {
                            property_window.visible = true
                            pane_property2.sourceComponent = pane_property.sourceComponent
                            pane_property.sourceComponent = undefined
                        }
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                    }

                    Rectangle {
                        id: rect_001
                        anchors.horizontalCenter: text_001.horizontalCenter
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 4
                        height:2.0
                        width: header_001.width * 0.6
                        color: "#3C8761"
                        // NumberAnimation{
                        //     id: ani_0002
                        //     target: rect_001; property: "width"
                        //     from:2.0; to: header_001.width * 0.6
                        //     duration: 800; easing.type: Easing.InOutQuad
                        // }
                    }
                }
                Rectangle {
                    id: bg_right_pane
                    color: theme.bg
                    border.width: 0.8
                    border.color: "#969696"
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    ScrollView {
                        anchors.fill: parent
                        contentHeight: pane_coulmn_001.height + 75
                        clip: true
                        ScrollBar.vertical.width: 5
                        ScrollBar.horizontal.height: 5
                        ColumnLayout {
                            id: pane_coulmn_001
                            width: bg_right_pane.width - 12
                            x: 6
                            Item { Layout.preferredHeight: 20 }
                            Loader {
                                id: pane_property //属性面板加载器
                                Layout.fillWidth: true
                                onSourceComponentChanged: {
                                    //ani_0002.restart()
                                }
                            }
                        }
                    }
                }
            }
        } //主区域 END

        Pane {
            id: bottom_pane
            Layout.fillWidth: true
            topPadding: 0; bottomPadding: 0
            background: Rectangle {
                color: theme.bottom_pane
                Rectangle {
                    color: "#999999"
                    width: parent.width
                    height: 0.8
                }
            }
            RowLayout {
                width: parent.width
                ToolButton {
                    Layout.preferredHeight: 20
                    text: "页码: " + (soupleEdit.currentPageIndex+1)+'/'+soupleEdit.pageCount
                    font.pixelSize: 12
                }
                Rectangle {
                    implicitHeight: 14
                    implicitWidth: 1
                    color: "#909090"
                }
                Text {
                    id: tip_status
                    Layout.leftMargin: 4
                    text: "Souple编辑器 1.0"
                    font.pixelSize: 12
                }
                Rectangle {
                    implicitHeight: 14
                    implicitWidth: 1
                    color: "#909090"
                }
                ToolButton {
                    id: tip_status2
                    Layout.leftMargin: 4
                    Layout.preferredHeight: 22
                    text: "编辑模式"
                    font.pixelSize: 12
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    RowLayout {  //底部栏任务序列
                        id: task_row
                        anchors.centerIn: parent
                        function addTask(text) {
                            return cp_task.createObject(task_row,{text:text})
                        }
                        Component {
                            id: cp_task
                            RowLayout {
                                Layout.alignment: Qt.AlignVCenter
                                property string text: "任务"
                                function setProgress(progress) {
                                    progress_rect.width = progress * 120
                                    if(progress >= 1.0) {
                                        destroy()
                                    }
                                }
                                Text {
                                    text: parent.text
                                    font.pixelSize: 11
                                }
                                Item {
                                    width: 120
                                    height: 12
                                    Rectangle {
                                        id: progress_rect
                                        width: 0
                                        height: 12
                                        color: "#3FB83C"
                                    }
                                    Rectangle {
                                        width: 120
                                        height: 12
                                        color: "#00000000"
                                        border.color: "#000000"
                                        clip: true
                                    }
                                }
                            }
                        } //cp_task END
                    }
                }
                ToolButton {
                    font.pixelSize: 12
                    text: `${Math.round(100*view_scale_slider.value2)}%   ▾`
                    Layout.preferredHeight: 22
                    onClicked: function(m){
                        var p = mapToItem(Overlay.overlay,width/2,0)
                        popup_selectScale.x = p.x - popup_selectScale.width/2
                        popup_selectScale.y = p.y - popup_selectScale.height - 10
                        //popup_selectScale.y = p.y - popup_selectScale.height - height
                        popup_selectScale.open()
                    }
                }
                Slider {
                    id: view_scale_slider
                    property real value2:
                        value*(7*value-2.5)+0.5
                    //0.5 - 5.0
                    //二次函数过：7x^2-2.5x+0.5
                    //(0.5,1.0)
                    //(0.0,0.5)
                    //(1.0,5.0)
                    Material.accent: "#268F4F"
                    Layout.preferredHeight: 24
                    Layout.preferredWidth: 144
                    from: 5/28.0; to: 1.0
                    value: 0.5
                }
            }
        }
    }


    MainWindowTitleBar {
        id: main_titleBar
        width: parent.width - 2*windowShadowBW
        y: windowShadowBW
        x: windowShadowBW
    }



    Popup {
        id: popup_selectScale
        onOpened: {
            spinbox_001_scale.value = Math.round(view_scale_slider.value2*100)
        }
        ColumnLayout {
            Text {
                text: "显示比例"
            }
            RadioButton {
                text: "200%"
                onCheckedChanged: {
                    if(checked) {
                        spinbox_001_scale.value = 200
                    }
                }
            }
            RadioButton {
                text: "130%"
                onCheckedChanged: {
                    if(checked) {
                        spinbox_001_scale.value = 130
                    }
                }
            }
            RadioButton {
                text: "100%"
                onCheckedChanged: {
                    if(checked) {
                        spinbox_001_scale.value = 100
                    }
                }
            }
            RadioButton {
                text: "75%"
                onCheckedChanged: {
                    if(checked) {
                        spinbox_001_scale.value = 75
                    }
                }
            }
            RadioButton {
                text: "页宽"
            }
            Text {
                text: "自定义"
            }
            MyDoubleSpinBox {
                id: spinbox_001_scale
                from: 30; to: 500
                step: 1
                suffix: "%"
                precision: 0
                onValueChanged:  //求根公式
                    view_scale_slider.value = (2.5+Math.sqrt(0.28*value-7.75))/14.0
            }
        }
    }




    Window {
        id: property_window
        visible: false
        flags: Qt.Window | Qt.FramelessWindowHint | Qt.NoDropShadowWindowHint
        modality: Qt.NonModal
        width: 200
        height: 500

        Rectangle {
            id: title_0002
            width: parent.width
            height: 30
            color: "#163316"
            Text {
                text: "属性面板"
                color: "white"
                anchors.centerIn: parent
            }
            MouseArea {
                anchors.fill: parent
                onPositionChanged: property_window.startSystemMove()
            }
            ToolButton {
                text: "×"
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                Material.foreground: "white"
                onClicked: {
                    property_window.visible = false
                    pane_property.sourceComponent = pane_property2.sourceComponent
                    pane_property2.sourceComponent = undefined
                }
            }
        }


        ScrollView {
            width: parent.width
            y: title_0002.height
            height: parent.height - y
            contentHeight: pane_coulmn_002.height
            clip: true
            ScrollBar.vertical.width: 5
            ColumnLayout {
                id: pane_coulmn_002
                width: parent.width - 6
                x: 6
                Item { Layout.preferredHeight: 20 }
                Loader {
                    id: pane_property2 //属性面板
                    Layout.fillWidth: true
                }
            }
        }


        Canvas {
            x: parent.width - width
            height: 13
            width: 13
            y: parent.height - height
            onPaint: {
                var ctx = getContext('2d')
                ctx.beginPath()
                ctx.moveTo(width,0)
                ctx.lineTo(width,height)
                ctx.lineTo(0,height)
                ctx.lineTo(width,0)
                ctx.fillStyle = "#163316"
                ctx.fill()
            }
            MouseArea {
                id: marea_11handler
                anchors.fill: parent
                onPositionChanged: {
                    property_window.startSystemResize(Qt.RightEdge|Qt.BottomEdge)
                }
            }
        }

        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.width: 0.8
            border.color: "#163316"
        }

    }

    // Rectangle {
    //     id: main_window_border
    //     anchors.fill: parent
    //     radius: 6
    //     color: "transparent"
    //     border.width: 0.5
    //     border.color: "#787878"
    //     anchors.bottomMargin: windowShadowBW
    //     anchors.topMargin: windowShadowBW
    //     anchors.leftMargin:  windowShadowBW
    //     anchors.rightMargin: windowShadowBW
    // }

    // Canvas {

    // }

    property real borderDragInitialX
    property real borderDragInitialY
    property real initialWindowX
    property real initialWindowY
    property real initialWindowWidth
    property real initialWindowHeight

    Component {
        id: cp_pdfLoadHelper_window
        PdfLoadHelper {
            modality: Qt.ApplicationModal
        }
    }
}
