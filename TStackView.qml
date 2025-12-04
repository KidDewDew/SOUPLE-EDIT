import QtQuick
import QtQuick.Controls
// 带渐变动画的StackView
StackView {
    id: stackview
    anchors.fill: parent
    property int transition_time: 240
    pushEnter: Transition {
        PropertyAnimation {
            property: "opacity"
            from: 0
            to:1
            duration: transition_time
        }
    }
    pushExit: Transition {
        PropertyAnimation {
            property: "opacity"
            from: 1
            to:0
            duration: transition_time
        }
    }
    popEnter: Transition {
        PropertyAnimation {
            property: "opacity"
            from: 0
            to:1
            duration: transition_time
        }
    }
    popExit: Transition {
        PropertyAnimation {
            property: "opacity"
            from: 1
            to:0
            duration: transition_time
        }
    }
}

