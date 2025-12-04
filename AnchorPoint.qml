import QtQuick

//AnchorPoint:锚定点
//作为HLine和VLine的交点
//虽然它没有继承AnchorObj，但要保持与AnchorObj的接口一致性
Item {
    id: obj
    property bool isLeft  //true:左锚定点 false:右锚定点
    property var hLine
    property var vLine: isLeft ? hLine.leftLine : hLine.rightLine
    property var leftObj
    property var rightObj

    property real rightX: x
    property real rightX_for_shrink: x
    property real leftX: x

    x: vLine.centerX
    y: hLine.centerY
    width: 0; height: 0

    function whenInsertedOnRight() { //当右边被插入时
        /**do nothing**/
    }

    function whenInsertedOnLeft() { //当左边被插入时
        /**do nothing**/
    }

    function insertOnRight(new_obj) { //在右侧插入
        console.log("Point.insertOnRight()")
        new_obj.leftObj = obj
        new_obj.rightObj = rightObj
        rightObj.leftObj = new_obj
        rightObj = new_obj
        new_obj.hLine = hLine
        //new_obj.rightObj.whenInsertedOnLeft()
        new_obj.tryMergeRight()
    }
    function insertOnLeft(new_obj) { //在左侧插入
        console.log("Point.insertOnLeft()")
        new_obj.rightObj = obj
        new_obj.leftObj = leftObj
        leftObj.rightObj = new_obj
        leftObj = new_obj
        new_obj.hLine = hLine
        //new_obj.leftObj.whenInsertedOnRight()
        new_obj.leftObj.tryMergeRight()
    }

    function tryMergeRight() { return false } //空函数

    function getCursorFromRight() { //从右方获得光标
        //AnchorPoint只需要转移该指令即可
        if(isLeft) { //转移到上一个hline
            if(hLine.lastHline != undefined) {
                hLine.lastHline.getCursorFromRight()
            }
        } else { //转移到leftObj上即可
            leftObj.getCursorFromRight()
        }
    }

    function getCursorFromLeft() {
        if(isLeft) {
            rightObj.getCursorFromLeft()
        } else {
            if(hLine.lastHline != undefined) {
                hLine.lastHline.getCursorFromLeft()
            }
        }
    }

    function getShrinkObj(dropWidth) { //获取左边shrink obj
        if(isLeft)
            return rightObj.dropLeft(dropWidth) //从右端obj获取
        else return null //右anchorPoint直接返回null即可
    }

    function dropLeft() { return null }
    function dropRight() { return null }


    onLeftObjChanged: {
        if(! isLeft && leftObj instanceof AnchorPoint) hLine.dealLayout() //行为空，需要dealLayout
    }

    //function runLayoutAction() {}


    //property bool isDealingOverflow: false

    // function dealOverflow() {
    //     var hasDrop = false //是否产生了drop
    //     if(isDealingOverflow || leftObj.protectedMode) return
    //     isDealingOverflow = true
    //     if(leftObj.rightX > obj.x) { //溢出
    //         console.log("rightX溢出",leftObj.rightX)
    //         //var rec_leftObj = leftObj
    //         //timer_tasks.push( function() {
    //             const dropObj = leftObj.dropRight(leftObj.rightX - obj.x)
    //             if(dropObj != null) hLine.overflowObj(dropObj) //交给hLine，进行溢出
    //        // })
    //     } else if(leftObj.rightX_for_shrink < obj.x) { //收缩
    //         console.log("rightX_for_shrink收缩",leftObj.rightX_for_shrink)
    //         //var rec_leftObj = leftObj
    //         //timer_tasks.push( function() {
    //             const dropObj = hLine.getShrinkObj(obj.x - leftObj.rightX_for_shrink)
    //             if(dropObj != null) hLine.shrinkObj(dropObj) //交给hLine，进行
    //         //})
    //     }
    //     isDealingOverflow = false
    // }

    //右边的AnchorPoint需要处理leftObj的右溢出和右收缩
    // Connections {
    //     id: conn
    //     enabled: ! isLeft && (leftObj instanceof AnchorObj)
    //     target: leftObj
    //     function onRightXChanged() {
    //         conn.enabled = false
    //         //console.log("rightXChanged")
    //         dealOverflow()

    //         conn.enabled = true
    //     }
    // }

    // Rectangle {
    //     color: isLeft ? "#FF405A" : "#AC5AB8"
    //     width: 6
    //     height: 6
    //     radius: 3
    //     x: -3; y: -3
    // }
}
