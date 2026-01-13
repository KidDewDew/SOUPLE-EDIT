
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material
//import Qt.labs.platform as Platform
import Qt5Compat.GraphicalEffects
import QtQuick.Dialogs as Dialogs
import com.custom 1.0
//直接在这里实现总体 ui

//Main_mobile.qml
//是专门为移动端设计的适配版界面

Window {
    id: window
    width: 960
    height: 640
    visible: true
    title: "相对布局编辑器"

    property alias horRuler: horRuler

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

    //创建qml-soupleObj
    function generateObj(type) {
        console.log("generateObj(",type)
        return soupleEdit.newObj(type)
    }

    function appendPage(width,height) {
        console.log("appendPage")
        soupleEdit.appendPage(width,height)
    }

    Dialog {
        id: dialog_loading
        anchors.centerIn: Overlay.overlay
        title: "加载进度"
        ColumnLayout {
            MyProgressBar {
                id: loading_bar
                Layout.preferredHeight: 30
                Layout.preferredWidth: 290
                accent: "#196CD9"
                control_text.text: `${(value*100).toFixed(1)}%`
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
    }


    MyFunctions {
        soupleEdit: soupleEdit
    }

    Component {
        id: cp_menu
        Menu {

        }
    }

    Component.onCompleted: {
        //soupleEdit.newPage(900,1200)
        // soupleEdit.newAnchorObj("VLine")
        // soupleEdit.newAnchorObj("VLine")
        // soupleEdit.newAnchorObj("HLine")
        // soupleEdit.findItem("V1").x = 40
        // soupleEdit.findItem("V2").x = 500
        // const h1 = soupleEdit.findItem("H1")
        // h1.leftLine = soupleEdit.findItem("V1")
        // h1.rightLine = soupleEdit.findItem("V2")
        // for(var i = 0; i < 1000; ++i) {
        //     so
        // }
    }

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
        if(obj.cp_propertyBar == null) return //[26/1/12]
        if(property_window.visible)
            pane_property2.sourceComponent = obj.cp_propertyBar
        else pane_property.sourceComponent = obj.cp_propertyBar //更新source
    }

    function setPropertyPane(sourceComponent) { //更新属性面板
        if(property_window.visible)
            pane_property2.sourceComponent = sourceComponent
        else pane_property.sourceComponent = sourceComponent //更新source
    }


    Rectangle { //背景色
        anchors.fill: parent
        color: "#EBEBEB"
    }


    ColumnLayout {
        id: view
        y: -2
        anchors.fill: parent

        spacing: 0

            MenuBar {  //菜单栏
                Material.background: "#EBEBEB";id: menuBar; //顶层菜单
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
                            console.log("platform.os = ",Qt.platform.os)
                            if(Qt.platform.os == "android") { //安卓平台
                                //申请权限
                                //Helper.mobile_requestPermission('android.permission.READ_EXTERNAL_STORAGE')
                                Helper.mobile_requestPermission('android.permission.MANAGE_EXTERNAL_STORAGE')
                                //mobile_fileDialog.open()
                                Helper.mobile_selectFile(function(retCode,file) {
                                    // if(file_dialog.selectedNameFilter.index == 0) { //.soup文件
                                    //     Helper.requestLoadSoup(Helper.qurl2localfile(file_dialog.currentFile))
                                    // } else if(file_dialog.selectedNameFilter.index == 1) { //.pdf
                                    console.log("retCode: ",retCode)
                                    console.log("mobile_selectFile callback: ",file)
                                    Helper.requestLoadPdf(file)
                                    //}
                                })
                            }
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

            // RowLayout { //左中
            //     Layout.fillWidth: true
            //     ToolButton {
            //         icon.source: "qrc:/image/icon_save.png"
            //         icon.color: "transparent"
            //         icon.width: 25; icon.height: 25
            //     }
            //     ToolButton {
            //         icon.source: "qrc:/image/icon_undo.png"
            //         icon.color: "transparent"
            //         icon.width: 20; icon.height: 20
            //     }
            //     ToolButton {
            //         icon.source: "qrc:/image/icon_redo.png"
            //         icon.color: "transparent"
            //         icon.width: 20; icon.height: 20
            //     }
            // } //左中END

            TabBar { //一级工具栏
                id: tabBar
                //height: menuBar.height
                //width: parent.width - menuBar.width
                Layout.fillWidth: true
                //Layout.alignment: Qt.AlignBottom
                //Layout.fillHeight: true
                Material.accent: "#1820A8"
                Material.background: "#EBEBEB"
                TabButton {
                    text: "开始"
                    height: 40
                    leftPadding: 0
                    rightPadding: 0
                    Layout.fillWidth: true
                }
                TabButton {
                    text: "插入"
                    height: 40
                    leftPadding: 0
                    rightPadding: 0
                    Layout.fillWidth: true
                }
                TabButton {
                    text: "页面"
                    height: 40
                    leftPadding: 0
                    rightPadding: 0
                    Layout.fillWidth: true
                }
                TabButton {
                    text: "视图"
                    height: 40
                    leftPadding: 0
                    rightPadding: 0
                    Layout.fillWidth: true
                }
                TabButton {
                    text: "工具"
                    height: 40
                    leftPadding: 0
                    rightPadding: 0
                    Layout.fillWidth: true
                }
            } //一级工具栏 END

            Rectangle { //左下
                Layout.fillWidth: true
                Layout.preferredHeight: 38
                color: "white"
                layer.enabled: true
                layer.effect: DropShadow {
                    radius: 6.0
                    color: "#70000000"
                }
                Flickable { //二级工具栏 Flickable
                    anchors.fill: parent
                    contentWidth: toolBar_2l
                StackLayout { //二级工具栏
                    id: toolBar_2l
                    height: parent.height
                    currentIndex: tabBar.currentIndex
                    property real dynamic_spacing: Math.min(width * 0.01,15)
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
                    RowLayout { //“开始”工具栏
                        spacing: toolBar_2l.dynamic_spacing
                        anchors.verticalCenter: parent.verticalCenter
                        Item { implicitWidth: 8 }
                        Text {
                            text: "字体"; font.pixelSize: 11
                            //color: combo_font.focus ? "#3BBD4A" : "#949494"
                            color: "#949494"
                            Layout.alignment: Qt.AlignVCenter
                            //Behavior on color { ColorAnimation { duration: 400 } }
                        }
                        ComboBox {
                            id: combo_font
                            Layout.alignment: Qt.AlignVCenter
                            model: ["宋体","黑体","..."]
                        }
                        Text {
                            text: "字号"; font.pixelSize: 11
                            color: "#949494"
                            Layout.alignment: Qt.AlignVCenter
                        }
                        TextField {
                            Layout.alignment: Qt.AlignVCenter
                            placeholderText: "字号"
                            Layout.preferredWidth: 45
                            horizontalAlignment: Text.AlignHCenter
                        }
                        Text {
                            text: "字体属性"; font.pixelSize: 11
                            color: "#949494"
                            Layout.alignment: Qt.AlignVCenter
                        }
                        SelectedButton {
                            label.text: "B"
                            label.font.bold: true
                            label.font.pixelSize: 14
                            Layout.alignment: Qt.AlignVCenter
                        }
                        SelectedButton {
                            label.text: "A"
                            label.font.italic: true
                            label.font.pixelSize: 14
                            Layout.alignment: Qt.AlignVCenter
                        }
                        SelectedButton {
                            label.text: "U"
                            label.font.underline: true
                            label.font.pixelSize: 14
                            Layout.alignment: Qt.AlignVCenter
                        }
                        SelectedButton {
                            label.text: "U"
                            label.font.overline: true
                            label.font.pixelSize: 14
                            Layout.alignment: Qt.AlignVCenter
                        }
                        SelectedButton {
                            label.text: "A"
                            label.font.strikeout: true
                            label.font.pixelSize: 14
                            Layout.alignment: Qt.AlignVCenter
                        }
                        SelectedButton {
                            label.text: "A"
                            label.style: Text.Outline
                            label.color: "white"
                            label.font.pixelSize: 14
                            Layout.alignment: Qt.AlignVCenter
                        }
                        Text {
                            text: "前景"; font.pixelSize: 11
                            color: "#949494"
                            Layout.alignment: Qt.AlignVCenter
                        }
                        ColorSelector {}
                        Text {
                            text: "背景"; font.pixelSize: 11
                            color: "#949494"
                            Layout.alignment: Qt.AlignVCenter
                        }
                        ColorSelector {}
                        // ComboBox {
                        //     model: 2
                        // }
                        Rectangle {
                            Layout.preferredWidth: 0.8
                            Layout.preferredHeight: 19
                            Layout.leftMargin: 6
                            Layout.rightMargin: 6
                            color: "#A8A8A8"
                        }
                        MyToolButton {
                            text: "查找/替换"
                        }
                        Rectangle {
                            Layout.preferredWidth: 0.8
                            Layout.preferredHeight: 19
                            Layout.leftMargin: 6
                            Layout.rightMargin: 6
                            color: "#A8A8A8"
                        }
                        CheckBox {
                            Material.accent: "#3C8761"
                            text: "辅助线"
                            onCheckedChanged: {
                                soupleEdit.showHelpInf = checked
                            }
                        }
                        CheckBox {
                            Material.accent: "#3C8761"
                            text: "护眼模式"
                        }
                    } //“开始”工具栏 END

                    RowLayout { //“插入”工具栏
                        spacing: toolBar_2l.dynamic_spacing
                        anchors.verticalCenter: parent.verticalCenter
                        Text {
                            text: "锚定元素"; font.pixelSize: 11
                            color: "#949494"
                            Layout.alignment: Qt.AlignVCenter
                        }
                        AddToolButton {
                            text: "水平标线"
                            onClicked: {
                                //soupleEdit.newAnchorObj("HLine")
                                SoupleManager.createSoupleObj_Free("HLine",50,soupleEdit.scroll_y+50)
                            }
                        }
                        AddToolButton {
                            text: "垂直标线"
                            onClicked: SoupleManager.createSoupleObj_Free("VLine",200,100)
                        }
                        Rectangle {
                            Layout.preferredWidth: 0.8
                            Layout.preferredHeight: 19
                            Layout.leftMargin: 4
                            Layout.rightMargin: 4
                            color: "#A8A8A8"
                        }
                        AddToolButton {
                            text: "流文本"
                            onClicked: soupleEdit.tryInsertAnchorObj("FlowText")
                        }
                        AddToolButton {
                            text: "占位文本"
                            enableList: true
                            listText: ["居中文本","左对齐文本","右对齐文本"]
                        }
                        AddToolButton {
                            text: "换行"
                            onClicked: soupleEdit.tryInsertAnchorObj("PH_Right")
                        }
                        AddToolButton {
                            text: "占位符"
                            enableList: true
                            listText: ["占位矩形","左占位","右占位(换行)","居中占位","整行占位"]
                            onSelected: {
                                switch(index) {
                                  case 0: soupleEdit.tryInsertAnchorObj("PH_Rect"); break
                                  case 1: soupleEdit.tryInsertAnchorObj("PH_Left"); break
                                  case 2: soupleEdit.tryInsertAnchorObj("PH_Right"); break
                                  case 3: soupleEdit.tryInsertAnchorObj("JZRect"); break
                                  case 4: soupleEdit.tryInsertAnchorObj("PH_Line"); break
                                }
                            }
                        }
                        AddToolButton {
                            text: "HTML"
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
                        Text {
                            text: "自由元素"; font.pixelSize: 11
                            color: "#949494"
                            Layout.alignment: Qt.AlignVCenter
                        }
                        AddToolButton {
                            text: "文本"
                        }
                        AddToolButton {
                            text: "形状"
                            enableList: true
                            listText: ["矩形","圆形","三角形"]
                        }
                        AddToolButton {
                            text: "图片"
                        }
                        AddToolButton {
                            text: "html"
                        }
                    } //“插入”工具栏 END

                    RowLayout { //“页面”工具栏
                        spacing: toolBar_2l.dynamic_spacing
                        anchors.verticalCenter: parent.verticalCenter
                        ToolButton {
                            Layout.leftMargin: 5
                            text: "删除当前页面<font color='red'>✘</font>"
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
                            text: "页宽:"
                        }
                        TextField {
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
                            text: "页高:"
                        }
                        TextField {
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
                            Layout.preferredHeight: 36
                            Layout.preferredWidth: 90
                            Material.accent: "#3C8761"
                            model: ["0°","90°","180°","270°"]
                        }
                    }

                } //二级工具栏 END
                } //二级工具栏 Flickable END
            } //左下 END

            ColumnLayout {
                id: main_page_area
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.topMargin: 4
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

                        //flickableDirection: Flickable.VerticalFlick

                        Binding on contentWidth {
                            when: soupleEdit_cover.width > scv11.width
                            value: soupleEdit_cover.width
                        }
                        ScrollBar.horizontal: ScrollBar {}
                        ScrollBar.vertical: ScrollBar {}
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
                            SoupleEdit {
                                id: soupleEdit
                                //scale: view_scale_slider.value
                                scroll_y: scv11.scroll_y * height
                                transform: Scale {
                                    origin.x: 0
                                    origin.y: 0
                                    xScale: view_scale_slider.value2
                                    yScale: view_scale_slider.value2
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 170
                    id: page_propertyPane
                    color: "#EBEBEB"
                    border.width: 0.8
                    border.color: "#969696"
                    ScrollView {
                        anchors.fill: parent
                        contentHeight: pane_coulmn_001.height
                        clip: true
                        ScrollBar.vertical.width: 5
                        ColumnLayout {
                            id: pane_coulmn_001
                            width: parent.width - 6
                            x: 6
                            Item { Layout.preferredHeight: 20 }
                            Loader {
                                id: pane_property //属性面板
                                Layout.fillWidth: true
                                onSourceComponentChanged: {
                                    ani_0002.restart()
                                }
                            }
                        }
                    }
                }

        Pane {
            id: bottom_pane
            Layout.fillWidth: true
            topPadding: 0; bottomPadding: 0
            background: Rectangle {
                color: "#C7C7C7"
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
                Item {
                    Layout.fillWidth: true
                }
                ToolButton {
                    font.pixelSize: 12
                    text: `${Math.round(100*view_scale_slider.value2)}%   ▾`
                    Layout.preferredHeight: 22
                    onClicked: function(m){
                        popup_selectScale.x = x - popup_selectScale.width/2
                        popup_selectScale.y = bottom_pane.y - popup_selectScale.height
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
        flags:  Qt.FramelessWindowHint
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

}
