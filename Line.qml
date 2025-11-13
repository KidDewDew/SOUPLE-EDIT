import QtQuick
import QtQuick.Shapes

Shape {
    id: shape
    implicitHeight: Math.abs(y1 - y0)
    implicitWidth: Math.abs(x0 - x1)
    property real x0
    property real y0
    property real x1
    property real y1
    property real strokeWidth: 1
    property color color: "grey"
    ShapePath {
        startX: x0
        startY: y0
        strokeWidth: shape.strokeWidth
        strokeColor: shape.color
        PathLine {
            x: x1; y: y1
        }
    }
}
