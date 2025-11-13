import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts

ComboBox {
    id: control
    font.pointSize: 11
    Material.accent: theme.accent_light
    height: 38
    Layout.preferredHeight: 38
    Layout.preferredWidth: 160
    leftPadding: 2
    rightPadding: 12
    topPadding: 3
    bottomPadding: 3
}
