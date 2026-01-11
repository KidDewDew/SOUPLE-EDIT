import QtQuick
import QtQuick.Controls.Material
// 专为字体大小设计的下拉框
ComboBox {
    id: combo_fontsize

    property bool isMultiple: false
    property real fontSize: 11

    implicitHeight: 26
    implicitWidth: 70
    model: ["5","5.5","6.5","7.5","8","9","10","10.5","11","12","14","16","18",
    "20","22","24","26","28","36","48","56","72"]
    displayText: isMultiple ? "多种" : `${fontSize}`
    editable: true
    validator: DoubleValidator {}
}
