import QtQuick
import QtQuick.Shapes 1.5

//锚定对象，用于描述pdf可视资源，如文本、图像，以及一些起辅助布局作用的对象
Item
{
    id: control
    //property bool showBorder: true       //是否显示边框
    property var hLine                   //水平基线
    property int vAlign: Qt.AlignVCenter //垂直对齐方式
    property int hAlign: Qt.AlignLeft    //水平对齐方式, Left: 对齐leftObj；HCenter: 对齐hLine中心
    property real vAlignMargin: 0             //相对于hLine的偏移量 >0为向下 <0为向上
    property var leftObj                 //左对齐对象，可以是VerBaseline或者AnchorObj
    property var rightObj                //右边对象，仅用于记录右边的对象或vLine
    //property real hAlignMargin: 0      //水平对齐的偏移量
    property real rightX: x+width        //右边x
    property real rightX_for_shrink: x+width //[收缩]rightX
    property real leftX: x

    property bool isSolid: true //是否为实体，即真正占据可视空间，这将影响居中等占位符的处理

    onRightXChanged: hLine.dealLayout()//通知dealLayout

    //property real scaleFactor: 1.0       //水平缩放因子，用于挤满HLine

    property bool banSignal_leftObjChanged: false

    property bool isSelected: false      //是否被选中

    property bool protectedMode: false //保护模式：当自身在进行影响rigthX的合并时，启用保护模式

    onIsSelectedChanged: {
        if(isSelected) selectedObj = control
        border.requestPaint()
    }


    function dealLayout() { //提供dealLayout方法
        //do nothing
    }


    //onHLineChanged: hLine.dealLayout() //调整布局

    //property bool isReserveRightX: false
    //anchors.verticalCenter: vAlign === Qt.AlignVCenter ? hLine.verticalCenter : undefined
    anchors.top: vAlign === Qt.AlignTop ? hLine.verticalCenter : undefined
    anchors.bottom: vAlign === Qt.AlignBottom ? hLine.verticalCenter : undefined
    Binding on anchors.verticalCenter {
        when: vAlign == Qt.AlignVCenter
        value: hLine.verticalCenter
    }
    Binding on anchors.left {
        when: hAlign === Qt.AlignLeft
        value: leftObj.right
        //value: (leftObj instanceof VerBaseline ? leftObj.horizontalCenter : leftObj.right)
    }
    //x: hAlign === Qt.AlignHCenter ? (leftObj.x + hLine.rightLine.x)/2-width/2 : null
    Binding on x {
        when: hAlign === Qt.AlignHCenter
        value: (leftObj.x + hLine.rightLine.x)/2-width/2
    }
    anchors.leftMargin: hAlignMargin
    //anchors.horizontalCenterOffset: hAlignMargin
    anchors.verticalCenterOffset: vAlignMargin
    anchors.topMargin: vAlignMargin
    anchors.bottomMargin: -vAlignMargin


    function whenInsertedOnRight() { //当右边被插入时
        /**do nothing**/
    }

    function whenInsertedOnLeft() { //当左边被插入时
        /**do nothing**/
    }

    function insertOnRight(new_obj) { //在右侧插入
        console.log("Obj.insertOnRight()")
        new_obj.leftObj = control
        new_obj.rightObj = rightObj
        rightObj.leftObj = new_obj
        rightObj = new_obj
        new_obj.hLine = hLine

        // new_obj.rightObj.whenInsertedOnLeft() //通知左侧
        // whenInsertedOnRight()
        // new_obj.whenInsertedOnRight() //通知右侧

        tryMergeRight() //尝试合并新obj

        hLine.dealLayout() //通知hLine更新布局

    }

    function runLayoutAction() {}  //布局行为

    //onHLineChanged: hLine.setSignUpdateLayout() //请求更新布局

    function insertOnLeft(new_obj) { //在左侧插入
        console.log("Obj.insertOnLeft()")
        new_obj.rightObj = control
        new_obj.leftObj = leftObj
        leftObj.rightObj = new_obj
        leftObj = new_obj
        new_obj.hLine = hLine

        // new_obj.leftObj.whenInsertedOnRight() //通知
        // whenInsertedOnLeft()

        new_obj.tryMergeRight() //尝试合并

        hLine.dealLayout() //通知hLine更新布局
    }

    function tryMergeRight() { return false }

    function removeSelf(doDestroy = true) { //安全删除
        if(leftObj != null) leftObj.rightObj = rightObj
        if(rightObj != null) rightObj.leftObj = leftObj
        if(doDestroy) {
            parent = null
            control.destroy(10)
            console.log("destroy,rest: ",soupleEdit.children.length)
        }
    }

    function dropRight(dropWidth) { //丢掉右边dropWidth的内容（返回丢掉的obj)
        console.log("ERROR: call empty dropRight()")
    }

    function dropLeft(dropWidth) { //丢掉左边dropWidth的内容（返回丢掉的obj)
        console.log("ERROR: call empty dropLeft()")
    }

    // function removeSelf(doDestroy = true) {
    //     if(leftObj instanceof AnchorObj) {
    //         leftObj.rightObj = rightObj
    //     }
    //     if(rightObj instanceof AnchorObj) {
    //         rightObj.leftObj = leftObj
    //     }
    //     if(doDestroy) {
    //         destroy()
    //     }
    // }

    // function elf(doDestroy = true) { //安全删除自身, doDestroy: 是否删除
    //     if(rightObj instanceof AnchorObj) {
    //         rightObj.banSignal_leftObjChanged = true
    //         rightObj.leftObj = control.leftObj
    //         rightObj.banSignal_leftObjChanged = false
    //         console.log("rightObj.leftObj = control.leftObj,text=",leftObj.textEdit.text)
    //     } else if(rightObj instanceof VerBaseline) {
    //         if(leftObj instanceof AnchorObj) {
    //             hLine.rightObj = leftObj
    //             leftObj.rightObj = rightObj
    //         } else {
    //             hLine.rightObj = undefined
    //         }
    //     }

    //     if(leftObj instanceof AnchorObj) {
    //         leftObj.rightObj = rightObj
    //     } else if(leftObj instanceof VerBaseline) {
    //         hLine.leftObj = (rightObj instanceof AnchorObj) ?
    //                     rightObj : undefined
    //     }

    //     //leftObj = empty_obj
    //     //rightObj = empty_obj
    //     //hLine = undefined

    //     if(doDestroy) {
    //         parent = null
    //         control.destroy()
    //     }
    //     //destroy() //删除
    // }


    //Component.onDestruction: console.log("onDestruction")

    // onLeftObjChanged: {
    //     if(banSignal_leftObjChanged) return

    //     if(leftObj instanceof VerBaseline) {
    //         if(hLine.leftObj !== undefined) { //leftObj已经有关联对象了
    //             hLine.leftObj.banSignal_leftObjChanged = true //禁用信号
    //             hLine.leftObj.leftObj = control
    //             hLine.leftObj.banSignal_leftObjChanged = false //恢复信号
    //             rightObj = hLine.leftObj
    //         }
    //         hLine.leftObj = control   //修改vLine的关联对象
    //         if(hLine.rightObj === undefined) {
    //             hLine.rightObj = control
    //             rightObj = hLine.rightLine
    //         }
    //     } else if(leftObj instanceof AnchorObj) {
    //         rightObj = leftObj.rightObj
    //         leftObj.rightObj = control //记录
    //     }
    // }


    Rectangle {
        id: lefter  //左边锚定点
        visible: isSelected
        width: 5
        height: control.height
        color: "#ED2F56"
        border.width: 1
        x: leftObj.x+leftObj.width-parent.x
        anchors.verticalCenter: parent.verticalCenter
    }

    Rectangle {
        id: righter  //右边锚定点
        visible: isSelected
        width: 5
        height: control.height
        color: "#E0A0FA"
        border.width: 1
        x: rightObj.x-parent.x-width
        anchors.verticalCenter: parent.verticalCenter
    }


    //     !!!!!! Qt官方类 Shape存在内存泄漏，不得使用！！！

    Canvas {
        id: border
        anchors.fill: parent
        visible: showHelpInf
        onPaint: {
            var ctx = getContext("2d")
            ctx.setLineDash([3,3])
            ctx.strokeStyle = isSelected ? "#1CAB10" : "#ED486E"
            //ctx.lineWidth =
            ctx.strokeRect(0,0,width,height)
        }
    }

    // Shape {
    //     //id: border
    //     //边框
    //     //rendererType: Shape.GeometryRenderer
    //     visible: showBorder
    //     anchors.fill: parent
    //     //anchors.margins: -5
    //     ShapePath {
    //         startX: 0; startY: 0
    //         strokeColor: isSelected ? "#1CAB10" : "#ED486E"
    //         Behavior on strokeColor { ColorAnimation { duration: 600 } }
    //         strokeStyle: ShapePath.DashLine
    //         fillColor: "transparent"
    //         dashPattern: [3,3]
    //         PathLine { x: border.width; y: 0 }
    //         PathLine { x: border.width; y: border.height }
    //         PathLine { x: 0; y: border.height }
    //         PathLine { x: 0; y: 0 }
    //     }
    // }
}
