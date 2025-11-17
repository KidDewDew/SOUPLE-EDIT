import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
//窗口：文档目录工具
//提供两个创建(替换)目录方法：1.从段落大纲设置 2.自动识别大纲并设置目录
ApplicationWindow {
    id: view
    color: theme.bg
    width:500
    height:345
    title: "嗖谷文档目录实用工具 v1.0"
    Material.foreground: theme.fg
    Material.background: theme.bg
    Material.primary: theme.bg
    Material.theme: isProtectEyeMode ? Material.Dark : Material.Light
    // properties...
    property var docs: []
    property int doc_index
    property string hline_name

    function loadPDF() {
        Qt.createQmlObject(
            ` Dialog_FileSelect {
                    title: "选择一个PDF文件"
                    onAccepted: {
                        //先打开小助手窗口，并阻塞
                        var helper_window = cp_pdfLoadHelper_window.createObject(window,
                                                {pdfFile:currentFile})
                        helper_window.show()
                        //Helper.requestLoadPdf(Helper.qurl2localfile(currentFile))
                        preloadPDF(helper_window.pdfFile)
                    }
                }
            `,view).open()
    }

    Component.onCompleted: {
        Helper.temp_Set("DirTool_opened",true)
    }

    onClosing: {
        Helper.temp_Set("DirTool_opened",false)
        view.destroy()
    }

    TStackView {
        id: stack
        anchors.fill: parent
        initialItem: cp_page0
    }

    // 目录提取页面
    Component {
        id: cp_page2
        Item {
            anchors.fill: parent
            Component.onCompleted: {

            }
            GridLayout {
                id: wait_row
                anchors.centerIn: parent
                TText {
                    text: "正在解析中..."
                }
                BusyIndicator {
                    running: true
                    Material.accent: theme.accent_light
                    Layout.preferredWidth: 40
                    Layout.preferredHeight: 40
                }
            }
        }
    }


    // 目录样式设置页面
    Component {
        id: cp_page1
        Item {
            id: page1
            anchors.fill: parent
            Column {
                z:1
                x:6;y:12
                spacing: 10
                FlatButton {
                    text: "返回"
                    padding_vertical: 6
                    onClicked: stack.pop()
                    foldV:0.2
                    accent: theme.accent_light
                }
                FlatButton {
                    text: "确认"
                    padding_vertical: 6
                    onClicked: stack.pop()
                    foldV:0.2
                    accent: "#C7B977"
                }
            }
            Component.onCompleted: {

            }
            Flickable {
                contentHeight: grid.height
                width:parent.width
                height:parent.height-16
                ScrollBar.vertical: ScrollBar {}
                y:8
                Control {
                    id: grid
                    anchors.horizontalCenter: parent.horizontalCenter
                    padding: 8
                    background: Rectangle {
                        color: Qt.lighter(theme.bg,1.3)
                    }
                    contentItem: GridLayout {
                        columns: width<500?2:4
                        implicitWidth: page1.width*0.8
                        Repeater {
                            model: 4
                            Item {
                                Layout.fillWidth: true;
                                Layout.horizontalStretchFactor: 1.0
                            }
                        }
                        TText {
                            text: "目录级数"
                        }
                        MyDoubleSpinBox {
                            step: 1
                            suffix: "级"
                            precision: 0
                        }
                        TText {
                            text: "级别缩进"
                        }
                        MyDoubleSpinBox {
                            step: 0.1
                            suffix: "cm"
                            precision: 2
                        }
                        TText {
                            text: "字体"
                            Layout.alignment: Qt.AlignTop
                            Layout.topMargin: 5
                        }
                        FontEdit {
                            Layout.alignment: Qt.AlignTop
                        }
                        TText {
                            text: "目录线型"
                        }
                        TCombo {
                            id: cbox_type
                            model: ["无","点线","虚线","实线"]
                        }
                        TText {
                            visible: cbox_type.currentIndex == 1 ||
                                     cbox_type.currentIndex == 2
                            text: "绘制间距(pt)"
                        }
                        Slider_and_ASB {
                            id: slider_spacing
                            visible: cbox_type.currentIndex == 1 ||
                                     cbox_type.currentIndex == 2
                            precision:1
                            slider.from:0
                            slider.to:10.0
                            slider.stepSize:0.1
                            value:1.0
                            Layout.maximumWidth:230
                        }
                        TText {
                            visible: cbox_type.currentIndex == 1
                            text: "圆点半径(pt)"
                        }
                        Slider_and_ASB {
                            id: slider_radius
                            visible: cbox_type.currentIndex == 1
                            precision:1
                            slider.from:0.0
                            slider.to:6.0
                            slider.stepSize:0.1
                            value:1.0
                            Layout.maximumWidth:230
                        }
                        TText {
                            visible: cbox_type.currentIndex == 2 ||
                                     cbox_type.currentIndex == 3
                            text: "线宽(pt)"
                        }
                        Slider_and_ASB {
                            id: slider_lineWidth
                            visible: cbox_type.currentIndex == 2 ||
                                     cbox_type.currentIndex == 3
                            precision:1
                            slider.from:0
                            slider.to:10.0
                            slider.stepSize:0.1
                            value:1.0
                            Layout.maximumWidth:230
                        }

                        LineText {
                            text: "预览"
                            Layout.columnSpan:parent.columns
                        }

                        NavDirPreview {
                            Layout.columnSpan:parent.columns
                            Layout.fillWidth: true
                        }
                    }
                }
            }
        }
    }


    // 初始页面
    Component {
        id: cp_page0
        GridLayout {
            //anchors.fill: parent
            x:0;y:0
            width:parent.width
            height:parent.height
            columns: 2
            columnSpacing: 10
            anchors.topMargin: 9
            anchors.bottomMargin: 6

            Component.onCompleted: {
                updateDocList(true)
                updateHLineFromCursor()
            }

            function updateDocList(firstTime=false) {
                docs = main_titleBar.getDocumentList(
                            (tab)=>{
                                 return tab.documentType=="souple"
                             });

                if(docs.length == 0) {
                    combo_docs.model = []
                    if(firstTime)loadPDF()
                } else {
                    let doc_title_list = []
                    for(const doc of docs) {
                        doc_title_list.push(doc.tabName)
                    }
                    combo_docs.model = doc_title_list
                }
            }

            function updateHLineFromCursor() {
                if(alias_soupleEdit.selectedObj == null) return
                const id = alias_soupleEdit.selectedObj.data_id //获取选中对象的id
                hline_selector.setHLineName(spMgr.getObjHLineName(id))
            }

            Timer {
                interval: 500 //每隔500ms更新一下当前的文档列表和光标
                running: true
                repeat: true
                onTriggered: {
                    updateDocList() //更新文档列表
                    if(cb_autoHLine.checked) {
                        updateHLineFromCursor()
                    }
                }
            }

            TText {
                text: "- 欢迎使用嗖谷文档目录实用工具 -"
                Layout.columnSpan: 2
                Layout.alignment: Qt.AlignHCenter
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
                TipButton {
                    x: -40
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
            TText {
                text: "添加目录的文档："
                Layout.alignment: Qt.AlignHCenter
            }
            TCombo {
                id: combo_docs
                Layout.alignment: Qt.AlignLeft
                Layout.preferredWidth: r_01.width-20
                RectButton {
                    x: parent.width+5
                    anchors.verticalCenter: parent.verticalCenter
                    text: "+"
                    accent: "#A34441"
                    onClicked: {
                        loadPDF()
                    }
                }
            }
            TText {
                text: "目录创建的位置："
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 5
            }
            RowLayout {
                id: r_01
                Layout.alignment: Qt.AlignLeft
                //Layout.leftMargin: 9
                Layout.topMargin: 5
                spacing: 8
                HLineSelector {
                    id: hline_selector
                    sync: false
                }
                CheckBox {
                    id: cb_autoHLine
                    Material.accent: "#A34441"
                    checked:true
                    text: "由光标设置"
                }
            }
            TText {
                text: "请选择(在当前该行下方)创建目录的途径:"
                Layout.columnSpan: 2
                Layout.alignment: Qt.AlignHCenter
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
                Layout.topMargin: 20
            }
            FlatButton {
                text: "从段落大纲创建目录"
                Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
                padding_vertical: 18
                onClicked: {
                    stack.push(cp_page1)
                }
                Layout.topMargin: 12
            }
            FlatButton {
                text: "从文档自动识别目录"
                Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
                padding_vertical: 18
                onClicked: {
                    stack.push(cp_page1)
                }
                Layout.topMargin: 12
            }
            TText {
                text: "如果已经为文档设置了段落大纲，优先使用该选项。"
                wrapMode: Text.WordWrap
                Layout.fillWidth:true
                Layout.alignment: Qt.AlignHCenter
                horizontalAlignment: Text.AlignHCenter
                color: "gray"
            }
            TText {
                text: `该选项将忽略已经设置的段落大纲，自动解析文档的结构并生成目录。`
                wrapMode: Text.WordWrap
                Layout.fillWidth:true
                horizontalAlignment: Text.AlignHCenter
                color: "gray"
            }
        }
    }
}
