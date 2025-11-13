import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
RectButton {
    Layout.alignment: Qt.AlignVCenter
    text: "?"
    accent: "#C9A350"
    property string tipText: "对此有疑问?"
    ToolTip {
        visible: parent.hovered
        text: parent.tipText
    }
}
