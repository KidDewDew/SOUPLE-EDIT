import QtQuick
import QtQuick.Effects

Item {
    id: button
    implicitWidth:ttext.width+padding_horizontal
    implicitHeight:ttext.height+padding_vertical
    property real padding_vertical: 18
    property real padding_horizontal: 16
    property string text
    signal clicked()
    property color accent: theme.accent_light
    property real foldV: 1.0
    Rectangle {
        id: content
        visible: false
        color: Qt.lighter(accent,marea.pressed?1.3:(marea.containsMouse?1.2:1.0))
        anchors.fill:parent
        TText {
            id:ttext
            text: button.text
            color: "white"
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: 4
        }
        radius: 6
        Column {
            x:3
            spacing:4
            anchors.verticalCenter: parent.verticalCenter
            Rectangle {
                color:"white";
                width:6;height:6;radius:3
            }
            Rectangle {
                color:"white";
                width:6;height:6;radius:3
            }
            Rectangle {
                color:"white";
                width:6;height:6;radius:3
            }
        }
    }

    ShaderEffect {
        id: se
        visible: false
        property real w: content.width
        property real h: content.height
        property real d:
            (marea.pressed ? -content.height*0.5 :
                marea.containsMouse?-5.0:4.0)*foldV;
        property real mx: 0.65
        property real darkv: 1.0-Math.abs(d) / content.height
        property vector2d light_pos: Qt.vector2d(lx,height-7.0)
        property real lx: d<0 ? (mx+1.0)*0.5*width*0.75 : -2.0
        property real light_r: marea.pressed ? 8.0 :
                                marea.containsMouse?8.0:16.0;
        Behavior on d {
            NumberAnimation {
                duration: 120
                easing.type: Easing.InOutQuad
            }
        }
        Behavior on lx {
            SmoothedAnimation {
                duration: 320
                easing.type: Easing.InOutQuad
            }
        }
        Behavior on light_r {
            NumberAnimation {
                duration: 480
                easing.type: Easing.InOutQuad
            }
        }
        mesh: GridMesh {
            resolution: Qt.size(100,1)
        }
        property variant src:
            ShaderEffectSource {
                sourceItem: content
                width: content.width
                height: content.height
            }
        vertexShader:"qrc:/shader/folding.vert"
        fragmentShader:"qrc:/shader/folding.frag"
        anchors.fill: content
    }

    MultiEffect {
        id: e1
        visible: true
        source: se
        anchors.fill: se
        shadowEnabled: true
        shadowOpacity: 0.6
        shadowBlur:0.2
        shadowVerticalOffset:2
        shadowHorizontalOffset:2
    }

    // MultiEffect {
    //     source: content
    //     anchors.fill: content
    //     brightness: marea.pressed?0.5:(marea.containsMouse?0.3:0.0)
    //     blurEnabled: true
    //     blur: marea.pressed?0.2:0.0
    //     saturation: marea.pressed?0.5:(marea.containsMouse?0.3:0.0)
    //     //saturation:2.0
    // }
    MouseArea {
        id: marea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: button.clicked()
        onMouseXChanged: {
            se.mx = marea.mouseX/marea.width
        }
    }
}
