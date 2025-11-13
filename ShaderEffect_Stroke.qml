import QtQuick

// 描边着色器效果
ShaderEffect {
    property int strokeWidth: 1            //描边宽度
    property color strokeColor: "red"     //描边颜色
    fragmentShader: "qrc:/shader/stroke.frag"
}
