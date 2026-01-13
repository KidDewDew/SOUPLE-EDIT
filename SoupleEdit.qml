import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Shapes
import com.custom 1.0

//SoupleEdit: "灵活"编辑器
//就像开火车一样排列各个AnchorObj
Item {
    id: soupleEdit

    //width: page_layout.width
    //height: page_layout.height

    width: SoupleManager.documentWidth
    height: SoupleManager.documentHeight

    property bool showHelpInf: true //显示辅助信息

    property alias chooseBoard: chooseBoard

    // topPadding: 0; leftPadding: 0;
    // bottomPadding: 0; rightPadding: 0

    // background: Rectangle {
    //     color: "#E0E0E0"
    // }

    //property var hLineList: [] //hLine的记录列表

    onCurrentPageIndexChanged: {
        //页面改变，通知改变属性
        var inf = SoupleManager.getCurrentPageInfo()
        tf_page_width.text = ""+Helper.pixel2cm(inf.width).toFixed(2)
        tf_page_height.text = ""+Helper.pixel2cm(inf.height).toFixed(2)
        tf_page_topMargin.text = ""+Helper.pixel2cm(inf.topMargin).toFixed(2)
        tf_page_bottomMargin.text = ""+Helper.pixel2cm(inf.bottomMargin).toFixed(2)
        if(inf.style == Helper_Type.NoFormat_Page) combobox_pageStyle.currentIndex = 0
        else if(inf.style == Helper_Type.Word_Page) combobox_pageStyle.currentIndex = 1
        tf_columnNum.text = ""+inf.column_num
    }

    property int autoNamefor_HLine: 1
    property int autoNamefor_VLine: 1
    //在selectedObj“附近”新建obj

    property var firstHLine
    property real scroll_y

    property var objHash: ({})

    //property var hLineRec: ({})     //存储hLine的情况: {HLine:HLineInf}

    //Component.onCompleted: {
        // for(var i = 0; i < 1000; ++i) {
        //     const obj = cp_obj_textedit.createObject(soupleEdit,{x:50,y:200+i*10})
        //     obj.textEdit.text = "helhello,worldasjdnasdniashello,worldasjdnasdniashello,worldasjdnasdniaslo,worldasjdnasdnias"
        // }
    //}


    Component.onCompleted: {
        //安装事件过滤器
        //SoupleManager.installEventFilterOnSoupleEdit(this)
        console.log("事件过滤器安装好了")
    }


    ChooseBoard {
        id: chooseBoard
        z: Helper_Type.Top
        visible: false
    }


    // Component {
    //     id: cp_page
    //     QtObject {
    //         id: page
    //         property real width //页宽(像素)
    //         property real height //页高(像素)
    //         property int pageNumber //页码
    //         property real top_margin
    //         property real bottom_margin
    //         //property type name: value
    //     }
    // }

    property var pages: []
    property bool pages_UPV

    property int currentPageIndex: -1 //当前页面索引
    property int pageCount: SoupleManager.pageCount

    property var old_selected_obj

    onSelectedObjChanged: {
        if(old_selected_obj == eb_bound.sync_obj) {
            eb_bound.visible = false
            eb_bound.sync_obj = null
        }
        updateObjPropertyPane(selectedObj)
        if(old_selected_obj != null && "isSelected" in old_selected_obj) {
            old_selected_obj.isSelected = false
        }
        if("isSelected" in selectedObj)
            selectedObj.isSelected = true
        old_selected_obj = selectedObj

        //通知SoupleManager
        spMgr.changeSelectedObj(selectedObj.data_id)
    }

    //variantmap to obj
    function vm2obj(vm) {
        var obj
        if(vm.type == "TextEdit")
            obj = cp_obj_textedit.createObject(soupleEdit)
        else if(vm.type == "PH_Rect")
            obj = cp_phrect.createObject(soupleEdit)
        obj.fromVM(vm)
        return obj
    }

    readonly property var type2cp: {
        "HLine":cp_hLine,"VLine":cp_vLine,
        "FPath":cp_FPath,"FImage":cp_FImage,
        "FText":cp_FText,"FlowText":cp_obj_textedit,
        "PH_Right":cp_phright,"PH_Rect":cp_phrect,
        "Image":cp_obj_image,"JZRect":cp_jzrect,
        "Path":cp_path,"PH_Left":cp_PHLeft,
        "BLine":cp_bline,"ILine":cp_iline,
        "TLine":cp_tline,"FUnit":cp_funit,
        "FRich":cp_FRich,"Page":cp_page,
        "GlueL":cp_glue,"GlueR":cp_glue,
        "PH_Glue":cp_glue,"Latex":cp_latex,
        "Frame":cp_frame,"Nav":cp_nav,
        "Spring":cp_spring,"StartSign":cp_startsign,
        "SepLine":cp_sepline
    }

    //创建并返回类型为type的Souple前端对象
    function newObj(type) {
        //console.log("newObj(",type)
        return type2cp[type].createObject(soupleEdit)
        // if(type == "HLine")
        //     return cp_hLine.createObject(soupleEdit)
        // else if(type == "VLine")
        //     return cp_vLine.createObject(soupleEdit)
        // else {
        //     if(type.length > 2 && type[0] == 'F' && type[1] != 'l') { //自由元素
        //         if(type == "FPath")
        //             return cp_FPath.createObject(soupleEdit)
        //         if(type == "FImage")
        //             return cp_FImage.createObject(soupleEdit)
        //         if(type == "FText")
        //             return cp_FText.createObject(soupleEdit)
        //         if(type == "FRich")
        //             return cp_FRich.createObject(soupleEdit)
        //     } else {  //锚定元素
        //         if(type == "FlowText") //流文本
        //             return cp_obj_textedit.createObject(soupleEdit)
        //         if(type == "PH_Right")
        //             return cp_phright.createObject(soupleEdit)
        //         if(type == "PH_Rect")
        //             return cp_phrect.createObject(soupleEdit)
        //         if(type == "Image")
        //             return cp_obj_image.createObject(soupleEdit)
        //         if(type == "JZRect")
        //             return cp_jzrect.createObject(soupleEdit)
        //         if(type == "Path")
        //             return cp_path.createObject(soupleEdit)
        //         if(type == "PH_Left")
        //             return cp_PHLeft.createObject(soupleEdit)
        //         if(type == "BLine")
        //             return cp_bline.createObject(soupleEdit)
        //         if(type == "ILine")
        //             return cp_iline.createObject(soupleEdit)
        //     }
        // }
    }

    Connections {
        target: SoupleManager
        function onNewPageAdded(width,height,top_margin,bottom_margin) {
            // var new_page = cp_page.createObject(soupleEdit,{width:width,height:height,
            //                                         top_margin:top_margin,bottom_margin:bottom_margin})
            // pages.push(new_page)
            // pageCount ++
            // pages_UPV = pages_UPV ^ 1
        }
        function onPageIndexChanged(page_index) {

        }
    }

    Connections {
        target: Helper
        function onBeginLoadDocument() {
            //timer_doTask.stop() //停止处理溢出等任务
        }
        function onFileLoaded() {
            //timer_doTask.start()
        }
        function onAddVLine(vLineName,x_offset)
        {  //c++要求添加vLine
            const vline = cp_vLine.createObject(soupleEdit)
            vline.objectName = vLineName
            vline.x = x_offset - vline.width/2
            console.log("addVLine(",vLineName,x_offset)
        }
        function onAddHLine(hLineName,y_offset,leftLine,rightLine,lastLine,lastAnchorLine,hLine_objs) //添加一个hLine
        {  //c++要求添加hLine以及其附属obj到编辑器

            console.log("addHLine(",hLineName,y_offset,leftLine,rightLine,lastLine,lastAnchorLine)

            const hline = cp_hLine.createObject(soupleEdit)
            hline.objectName = hLineName
            hline.leftLine = findItem(leftLine)
            hline.rightLine = findItem(rightLine)
            hline.lastHline = findItem(lastLine)
            hline.anchor_lastHline = findItem(lastAnchorLine)
            if(lastAnchorLine.length == 0) hline.y = y_offset - hline.height / 2  //绝对坐标
            else hline.lastHlineMargin = y_offset - hline.anchor_lastHline.centerY //相对坐标

            //插入hline_objs
            for(var obj_vm of hLine_objs) {
                var obj = vm2obj(obj_vm)
                hline.insertOnRight(obj)
                console.log("VM: ",obj_vm)
            }

            //console.log("addHLine END END END ------------- END")
        }
    }

    // //从hLine往下调整布局
    // function dealLayout(hLine) {
    //     while(hLine != null) {
    //         const obj = hLine.leftPoint.rightObj
    //         const overflow_obj = null
    //         while(obj != null && obj != hLine.rightPoint) {
    //             obj.dealLayout() //调用obj自身处理布局
    //             obj = obj.rightObj
    //         }
    //         hLine = hLine.nextHline
    //     }
    // }


    // Component {
    //     id: cp_ssss
    //     Shape {
    //         id: line //右边的虚线
    //         width: 200
    //         height: 20
    //         x: 40
    //         y: 100
    //         implicitHeight: 4
    //         ShapePath {
    //             startX: 0; startY: line.height/2
    //            strokeColor: "green"
    //             strokeStyle: ShapePath.DashLine
    //             dashPattern: [4,4]
    //             PathLine { x: line.width; y: line.height/2 }
    //         }
    //     }
    // }


    //插入新的自由对象，类型为type
    function insertFreeObj(type) {
        if(pageCount < 1) {
            messageBox("错误","请创建至少一个页面。")
            return
        }
        //SoupleManager.createSoupleObj_Free(type,)
        grey_cover.show()
        marea_cover.callback_clicked = function(p) { //回调
            if(type[0] != 'F') type = 'F' + type
            SoupleManager.createSoupleObj_Free(type,p.x,p.y)
        }
    }


    //尝试插入锚定对象,类型为objName
    function tryInsertAnchorObj(objName) {
        if(pageCount < 1) {
            messageBox("错误","请创建至少一个页面。")
            return
        }

        // if( ! SoupleManager.hasCreatedFirstHLine()) {
        //     messageBox("错误","请创建[起始]水平标线。")
        //     return
        // }


        if(selectedObj == null || selectedObj instanceof VerBaseline)
        { //如果用户未选择对象，则使用对话框询问从哪儿插入
            dialog_insertAnchorObj.anchor_obj = objName
            dialog_insertAnchorObj.open()
        } else {
            if(selectedObj instanceof VerBaseline) {
                messageBox("错误","无法在该位置插入。")
                return
            }
            if(selectedObj instanceof HorBaseline || selectedObj instanceof BaseHorLine
                    || selectedObj instanceof InnerHorLine) {
                dialog_chooseLeftOrRight.open()
                dialog_chooseLeftOrRight.accept_callback = function(){
                    SoupleManager.createSoupleObj_onHline(objName,selectedObj.data_id,dialog_chooseLeftOrRight.isFromLeft)
                }
                dialog_chooseLeftOrRight.reject_callback = function(){
                    //插入失败... //
                }
            } else if("data_id" in selectedObj) {
                if("insertNewObj" in selectedObj) {
                    //交给selectedObj处理
                    if(! selectedObj.insertNewObj(objName)) {
                        messageBox("错误","无法在该位置插入。")
                    }
                } else {
                    //selectedObj.insertOnRight(obj) //否则默认在右边插入
                    SoupleManager.createSoupleObj_byObj(objName,selectedObj.data_id,false)
                }
            }
            else{
                messageBox("错误","无法在该位置插入。")
                obj.destroy()
            }
        }
    }


    Dialog {
        id: dialog_chooseLeftOrRight
        title: "从水平标线插入"
        property bool isFromLeft: radio_333.checked
        property var accept_callback
        property var reject_callback
        onOpened: radio_333.checked = true
        closePolicy: Dialog.NoAutoClose
        parent: Overlay.overlay
        anchors.centerIn: parent
        onAccepted: accept_callback()
        onRejected: reject_callback()
        ColumnLayout {
            RowLayout {
                RadioButton {
                    id: radio_333
                    text: "从左边插入"
                    checked: true
                }
                RadioButton {
                    id: radio_444
                    text: "从右边插入"
                }
            }
            RowLayout {
                Button {
                    text: "取消"
                    onClicked: dialog_chooseLeftOrRight.reject()
                }
                Button {
                    focus: true
                    text: "确认插入"
                    onClicked: dialog_chooseLeftOrRight.accept()
                }
            }
        }
    }


    Dialog {
        id: dialog_insertAnchorObj
        title: "插入锚定对象"
        property int aim_hLine: -1
        property var anchor_obj
        closePolicy: Dialog.NoAutoClose
        parent: Overlay.overlay
        anchors.centerIn: parent
        contentItem: ColumnLayout {
            Text {
                text: "你没有选择一个已有的锚定对象，\n所以需要手动定位该对象。"
            }
            RowLayout {
                Text { text: "插入的水平标线:" }
                TextField {
                    leftPadding: 10; rightPadding: 5
                    topPadding: 5; bottomPadding: 5
                    Layout.preferredHeight: 35
                    onTextChanged: {
                        dialog_insertAnchorObj.aim_hLine = SoupleManager.getHLineIdByName(text)
                    }
                    RectButton {
                        text: dialog_insertAnchorObj.aim_hLine == -1 ? "无效" : "有效"
                        width: 25
                        accent: dialog_insertAnchorObj.aim_hLine == -1 ? "red" : "green"
                        Behavior on accent { ColorAnimation { duration: 600 } }
                        label.font.pixelSize: 10
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                    }
                }
            }
            RowLayout {
                RadioButton {
                    id: radio_111
                    text: "从左端插入"
                }
                RadioButton {
                    id: radio_222
                    text: "从右端插入"
                }
            }
            RowLayout {
                Button {
                    text: "取消"
                    onClicked: {
                        dialog_insertAnchorObj.anchor_obj.destroy() //记住删除
                        dialog_insertAnchorObj.close()
                    }
                }
                Button {
                    text: "确认插入"
                    onClicked: {
                        if(dialog_insertAnchorObj.aim_hLine == -1) {
                            messageBox("错误","请输入有效的水平标线。")
                            return
                        }
                        if(radio_111.checked) {
                            SoupleManager.createSoupleObj_onHline(dialog_insertAnchorObj.anchor_obj,
                                                                  dialog_insertAnchorObj.aim_hLine,
                                                                  true)
                        } else if(radio_222.checked) {
                            SoupleManager.createSoupleObj_onHline(dialog_insertAnchorObj.anchor_obj,
                                                                  dialog_insertAnchorObj.aim_hLine,
                                                                  false)
                        }
                        dialog_insertAnchorObj.close()
                    }
                }
            }
        }
    }

    onScroll_yChanged: {
        //currentPageIndex = getPageOfY(scroll_y)

        currentPageIndex = SoupleManager.getPageIndexOfY(scroll_y) //2055/8/5 修改

        // var scan_from = y_id_map[Math.floor(scroll_y/200)*200]
        // timer_doTask.scan_id = Math.max(0,scan_from - 50)
        //SoupleManager.updateViewSize(scroll_y,scroll_y + parent.height)
    }

    function getPageOfY(y) { //根据obj的y坐标计算其所属的页码(从 0 计算)
        if(pages.length == 0) return -1
        if(y < 0) return 0
        var py = 0
        for(var i = 0; i < pages.length; ++i) {
            py += pages[i].height
            if(py >= y) return i
        }
        return pages.length - 1
    }

    property var timer_tasks: [] //定时器任务


    property var selectedObj //当前选中的对象
    property var contObj    //当前操作的容器对象

    onContObjChanged: {
        contObj.iscont = true
    }

    /*
        容器对象和选择对象有什么区别？
        容器对象指一个被选择的状态，但不一定针对它进行修改。
        而选择对象则一定既被选择又被修改。
    */

    // AnchorObj {
    //     id: empty_obj
    // }


    property var y_id_map: ({})

    Timer {
        id: timer_doTask //处理堆积任务
        interval: 20
        property int pile_time_1: 0
        property int pile_time_2: 0
        property int scan_id: 0
        repeat: true
        running: false
        onTriggered: {
            return
            var task_num = 0
            //var beInterrupted = false //被中断，表明未处理完所有任务

            while(timer_tasks.length > 0 && task_num < 15) {
                try {
                    timer_tasks.shift()()
                }catch(e) {}
                ++task_num
            }

            // pile_time_2 += interval

            // if(pile_time_2 >= 5000) {
            //     pile_time_2 = 0
            //     //Helper.trimCache()
            // }

            pile_time_1 += interval

            if(pile_time_1 >= 200) {
                pile_time_1 = 0
                var scan_n = 0
                const N = Math.min(200,soupleEdit.children.length)
                var y_rec = 0
                while(scan_n < N) {  //扫描obj
                    var obj = soupleEdit.children[scan_id]
                    if(obj.y < scroll_y - obj.height*3
                            || obj.y > scroll_y + window.height + 50) {
                        obj.visible = false
                    } else {
                        obj.visible = true
                    }
                    if(obj.y >= y_rec) {
                        y_id_map[Math.ceil(obj.y/200)*200] = scan_id
                        y_rec = Math.ceil(obj.y/200+1)*200
                    }

                    ++scan_id
                    if(scan_id >= soupleEdit.children.length) scan_id = 0
                    ++scan_n
                }
            }

        }
    }

    function findItem(objName) {
        return objHash[objName]
    }

    Component {
        id: cp_phrect
        PH_Rect {}
    }

    Component {
        id: cp_obj_image
        Obj_Image {}
    }

    Component {
        id: cp_jzrect
        PH_JZRect {}
    }

    Component {
        id: cp_phright
        PH_Right {}
    }

    Component {
        id: cp_obj_textedit
        Obj_TextEdit { y: -10 }
    }

    Component {
        id: cp_anchorPoint
        AnchorPoint {}
    }

    Component {
        id: cp_hLine
        HorBaseline {  }
    }

    Component {
        id: cp_vLine
        VerBaseline { }
    }
    Component {
        id: cp_path
        Obj_Path {}
    }

    Component {
        id: cp_FText
        FreeObj_Text {}
    }

    Component {
        id: cp_FPath
        FreeObj_Path {}
    }

    Component {
        id: cp_FImage
        FreeObj_Image {}
    }

    Component {
        id: cp_FRich
        FreeObj_Rich {}
    }

    Component {
        id: cp_PHLeft
        PH_Left {}
    }

    Component {
        id: cp_bline
        BaseHorLine {}
    }

    Component {
        id: cp_iline
        InnerHorLine {}
    }

    Component {
        id: cp_tline
        Table_HorLine {}
    }

    Component {
        id: cp_funit
        FreeObj_Unit {}
    }

    Component {
        id: cp_page
        Obj_Page {}
    }

    Component {
        id: cp_glue
        PH_Glue {}
    }

    Component {
        id: cp_latex
        Obj_Latex_Formula {}
    }

    Component {
        id: cp_frame
        Obj_Frame {}
    }

    Component {
        id: cp_nav
        Obj_Nav {}
    }

    Component {
        id: cp_spring
        Obj_Spring {}
    }

    Component {
        id: cp_startsign
        Obj_StartSign {}
    }

    Component {
        id: cp_sepline
        SepLine {}
    }

    Editable_Bound {
        id: eb_bound
        visible: false
        z: 1e8
    }

    // 光标对象
    Rectangle {
        id: uniCursor
        objectName: "uniformCursor"
        width: 2
        height: 16
        color: "black"
        z: Helper_Type.Widget_Top
        onVisibleChanged: {
            console.log("UniCursor.visible -> ",visible)
        }

        Timer {
            interval: 500
            repeat: true
            running: uniCursor.visible
            onTriggered: uniCursor.opacity = 1.0 - uniCursor.opacity
        }
    }

    // ColumnLayout { //页面
    //     id: page_layout
    //     spacing: -0.4
    //     z: -10000
    //     Repeater {
    //         model: pages_UPV, pages
    //         delegate:
    //     }
    // }

}
