import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import com.custom 1.0

// latex公式组件
Image {
    id: obj
    property int data_id
    property alias cp_propertyBar: cp_propertyBar
    property bool isSelected
    source: "qrc:/image/icon_Image.png"
    cache: false
    Component {
        id: cp_propertyBar //属性栏
        GridLayout {
            columns: 2
            Layout.fillWidth: true
            rowSpacing: 8
            Component.onCompleted: {
                latex_edit.text = SoupleManager.qmlGetData(data_id,Helper_Type.TEXT);
                tf_fontSize.text = ""+SoupleManager.qmlGetData(data_id,Helper_Type.FONT_SIZE);
            }
            Text {
                color: theme.fg
                text: "元素类别"; font.pixelSize: 16
                Layout.alignment: Qt.AlignLeft
            }
            Text {
                color: theme.fg
                text: "Latex公式";
                font.pixelSize: 16
                Layout.alignment: Qt.AlignRight
            }
            LineText {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                text: "Latex Code"
            }
            Text {
                color: theme.fg
                text: "Latex代码"; font.pixelSize: 16;
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            }
            TextArea {
                id: latex_edit
                leftPadding: 5; rightPadding: 5
                topPadding: 5; bottomPadding: 5
                Layout.alignment: Qt.AlignTop | Qt.AlignRight
                Layout.minimumHeight: 150
                Layout.fillWidth: true
                placeholderText: "在这里输入Latex代码"
                font.pixelSize: 14
                onFocusChanged: {
                    SoupleManager.sendCommandToData(data_id,Helper_Type.TEXT_UP,latex_edit.text)
                }
            }
            Text {
                color: theme.fg
                text: "字体大小"; font.pixelSize: 16;
                Layout.alignment: Qt.AlignLeft
            }
            TextField {
                id: tf_fontSize
                leftPadding: 5; rightPadding: 5
                topPadding: 5; bottomPadding: 5
                Layout.preferredHeight: 35
                horizontalAlignment: Text.AlignHCenter
                placeholderText: "输入公式字体大小(像素)"
                Layout.alignment: Qt.AlignRight
                Material.accent: theme.accent_light
                font.pixelSize: 16;
                validator: DoubleValidator {bottom: 2; top: 256}
                onTextChanged: {
                    SoupleManager.sendCommandToData(data_id,
                                                    Helper_Type.FONT_SIZE_UP,
                                                    parseFloat(text))
                }
            }
            RectButton {
                Layout.alignment: Qt.AlignHCenter
                Layout.columnSpan: 2
                Layout.preferredWidth: 120
                Layout.preferredHeight: 30
                accent: "#6B342E"
                text: "➤编译并更新"
                onClicked: {
                    SoupleManager.sendCommandToData(data_id,Helper_Type.Request_Sync,latex_edit.text)
                    obj.source = ""
                    obj.source = SoupleManager.qmlGetData(data_id,Helper_Type.SOURCE);
                }
            }
        }
    }
    Rectangle {
        id: border
        visible: marea.containsMouse || obj.isSelected
        anchors.margins: -3
        anchors.fill: parent
        color: "transparent"
        radius: 3
        border.width: 1
        border.color: theme.accent_light
    }
    MouseArea {
        id: marea
        anchors.fill: parent
        hoverEnabled: true
        onPressed: {
            obj.focus = true
            selectedObj = obj
        }
    }

}
