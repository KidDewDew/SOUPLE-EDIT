import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
//对话框：文档目录工具
//提供两个创建(替换)目录方法：1.从段落大纲设置 2.自动识别大纲并设置目录
ApplicationWindow {
    id: view
    color: theme.bg
    width:500
    height:345
    title: "PDF目录实用工具 v1.0"
    Material.foreground: theme.fg
    Material.background: theme.bg
    Material.primary: theme.bg
    Material.theme: isProtectEyeMode ? Material.Dark : Material.Light
    property var docs: []
    function loadPDF() {
        Qt.createQmlObject(
            `
                import QtQuick
                Dialog_FileSelect {
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
        id: cp_page1
        Item {
            anchors.fill: parent
            property int doc_index
            property string hline_name
            Component.onCompleted: {

            }
            RowLayout {
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
                text: "- 欢迎使用PDF目录实用工具 -"
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
