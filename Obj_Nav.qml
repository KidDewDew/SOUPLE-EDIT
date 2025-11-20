import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Controls.Basic as Basic
import com.custom 1.0
// 导航item..qml
ImageButton {
    id: obj
    width: 22
    height: 18
    source: "qrc:/image/nav_down.svg"
    color: (hovered||down) ? "#eee" : "transparent"
    property int data_id
    property real tab //缩进
    property bool showLevel //是否显示level
    property int level //级别
    property alias cp_propertyBar: cp_propertyBar
    Component {
        id: cp_propertyBar //属性栏
        GridLayout {
            columns: 2
            columnSpacing: parent.width * 0.08
            TText { text: "元素类别"; }
            TText { text: "导航符"; }
        }
    }
    Basic.ToolTip {
        visible: obj.hovered || obj.down
        text: "跳转到导航点..."
    }
}
