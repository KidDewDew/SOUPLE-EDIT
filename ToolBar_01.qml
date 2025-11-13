import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Effects
// 单行图标+文字工具栏
// Flickable {
//     id: root
//     contentWidth: content.width

//     anchors.fill: parent

//     property var items

//     ScrollBar.horizontal: ScrollBar {
//         policy: ScrollBar.AlwaysOn
//         visible: root.contentWidth > root.width
//     }

    RowLayout {
        id: root
        height: parent.height
        property var items
        Repeater {
            id: repeater
            model: root.items
            delegate: GlowImageButton {
                text: modelData.name
                img_src: modelData.img_src
                onClicked: modelData.onClicked()
            }
        }
    }
//}
