import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Dialogs as Dialogs
import com.custom 1.0

//右边的工具栏
Item {
    id: view

    clip: true

    Rectangle {
        height: 1
        width: parent.width
        color: "#B8991E"
    }

    Rectangle {
        width: 1
        height: parent.height
        color: "#B8991E"
    }

    ListModel {
        id: model_tabs
    }

    Component.onCompleted: {
        model_tabs.append({tabName:"附加工具",tabCp:cp_additional_tools})
        model_tabs.append({tabName:"脚本",tabCp:cp_additional_tools})
        setTab("附加工具")
    }

    function addTab(tabName,tabCp,atFront = true) {
        removeTab(tabName)
        if(atFront)
            model_tabs.insert(0,{tabName:tabName,tabCp:tabCp})
        else model_tabs.append({tabName:tabName,tabCp:tabCp})
    }

    function removeTab(tabName) {
        console.log("removeTab(",tabName)
        for(let i = 0; i < model_tabs.count; ++i)
            if(model_tabs.get(i).tabName === tabName) {
                if(i+1 == model_tabs.count) {
                    tabIndex = 0
                }
                model_tabs.remove(i,1)
                upV ^= 1
                break;
            }
    }

    function setTab(tabName) {
        for(let i = 0; i < model_tabs.count; ++i)
            if(model_tabs.get(i).tabName === tabName) {
                if(tabIndex == i) {
                    //需要手动更新
                    upV ^= 1
                }
                else {
                    tabIndex = i
                    img_cover.visible = visible
                    ani_1.restart()
                    ani_2.restart()
                }
                break;
            }
    }

    property int tabIndex: 0
    property bool upV

    Flickable {
        id: content
        height: parent.height - tabs.height - 5
        y: 5
        x: 4
        width: parent.width-4
        contentWidth: loader.width
        clip: true
        Loader {
            id: loader
            sourceComponent: upV,model_tabs.get(tabIndex).tabCp
            height: parent.height
            PropertyAnimation {
                id: ani_2
                target: loader.item
                property: "y"
                from: content.height
                to: 0.0
                duration: 300
                easing.type: Easing.InOutQuad
            }
        }
    }

    Item {
        anchors.fill: content
        clip: true
        Image {
            id: img_cover
            width: content.width
            height: content.height
            PropertyAnimation {
                id: ani_1
                property: "y"
                target: img_cover
                from: 0.0; to: -content.height
                duration: 300
                easing.type: Easing.InOutQuad
                onFinished: {
                    img_cover.visible = false
                    content.grabToImage(function(result){
                        img_cover.source = result.url
                    })
                }
            }
        }
    }

    ListView {
        id: tabs
        anchors.bottom: parent.bottom
        orientation: ListView.Horizontal
        model: model_tabs
        //width: parent.width
        clip: true
        anchors.left: parent.left
        anchors.right: search_wrapper.left
        anchors.rightMargin: 5
        height: 25
        delegate: ToolButton {
            text: tabName
            height: 25
            topPadding: 2; bottomPadding: 2
            leftPadding: 4; rightPadding: 4
            font.pixelSize: 12
            Material.foreground: fg
            property color fg: tabIndex == index ? "#B8991E" : "#737373"
            Behavior on fg {
                ColorAnimation {
                    duration: 200
                }
            }
            onClicked: {
                if(tabIndex == index) return
                tabIndex = index
                img_cover.visible = visible
                ani_1.restart()
                ani_2.restart()
            }
            Rectangle {
                visible: tabIndex == index
                anchors.bottom: parent.bottom
                //anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                height: 3
                color: "#B8991E"
            }
        }
    }

    Rectangle {
        id: search_wrapper
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 2
        width: 25 + search_input.width
        height: 18
        radius: 6
        border.color: "#6E6E6E"
        Item {
            width: 25
            height: 18
            clip: true
            Rectangle {
                width: 29
                height: 18
                radius: 6
                color: "#6E6E6E"
                Text {
                    text: "搜索"
                    font.pixelSize: 11
                    anchors.centerIn: parent
                    anchors.horizontalCenterOffset: -3
                    color: "white"
                }
            }
        }
        TextInput {
            id: search_input
            x: 25
            //width: 40
            rightPadding: 5
            FontMetrics {
                id: fm
                font: search_input.font
            }
            width: Math.max(fm.advanceWidth(search_input.text),30)
            anchors.verticalCenter: parent.verticalCenter
            clip: true
        }
    }

    // 附加功能：页眉页脚工具 PDF拆分/合并 路径编辑 简笔绘图 流程图绘制 特殊符号表 计算器
    // 脚本：查看脚本 编写脚本 按钮 单行输入框 多行输入框 单选框 复选框 滑动条 数字选取器 帮助

    Component {
        id: cp_additional_tools
        ToolBar_01 {
            items: [
                {
                    name: "页眉页脚",
                    img_src: "qrc:/image/icon_Image.png"
                },
                {
                    name: "字体管理",
                    img_src: "qrc:/image/icon_fontEdit.png",
                    onClicked: function() {

                    }
                },
                {
                    name: "目录识别",
                    img_src: "qrc:/image/icon_fontEdit.png",
                    onClicked: function() {
                        if(Helper.temp_Get("DirTool_opened",false) == false) {
                            //未创建目录工具窗口
                            Qt.createQmlObject(`
                                import QtQuick
                                Dialog_DocumentDirTool {
                                    visible:true;
                                }
                            `,window);
                        } else {
                            messageBox("Info","PDF目录实用工具已处于打开状态")
                        }
                    }
                }
            ]
        }
    }

}
