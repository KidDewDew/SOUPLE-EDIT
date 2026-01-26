import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "etc_helper.js" as Etc
// 颜色下拉框
AbstractButton {
    id: control
    padding: 3
    property color color: "white"
    states: [
        State {
            when: control.hovered && !control.pressed
            PropertyChanges {
                target: bg; color: "#dddddd"
            }
        },
        State {
            when: control.pressed
            PropertyChanges {
                target: bg; color: "#c6c6c6"
            }
        }
    ]
    background: Rectangle {
        id: bg
        implicitWidth: 90
        implicitHeight: 26
        color: theme.bg
        border.width: 1
        border.color: theme.splitLine
        radius: 6
    }
    contentItem: FlexboxLayout {
        justifyContent: FlexboxLayout.JustifyCenter
        alignItems: FlexboxLayout.AlignCenter
        gap: 5
        Rectangle {
            id: color_rect
            color: control.color
            height: parent.height - 2
            width: parent.height - 2
            radius: 4
        }
        Text {
            //visible: parent.width > implicitWidth+color_rect.width+8
            text: ""+control.color
        }
    }

    onPressed: {
        Etc.movePopupAt(popup_for_color,control,0,height+1)
        popup_for_color.open()
        Etc.adjustPopupInWindow(popup_for_color,window)
        popup_for_color.callback = function(selected_color) {
            control.color = selected_color
        }
    }


}
