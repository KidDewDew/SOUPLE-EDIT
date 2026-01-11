import QtQuick

QtObject {
    /* 字体 */
    property bool mfontsize
    property bool efontsize
    property real fontsize

    property bool mfamily
    property bool efamily
    property string family

    property bool menablestroke
    property bool eenablestroke
    property bool enablestroke

    property bool menablefill
    property bool eenablefill
    property bool enablefill

    property bool mstrokewidth
    property bool estrokewidth
    property real strokewidth

    property bool mstrokecolor
    property bool estrokecolor
    property color strokecolor

    property bool mfillcolor
    property bool efillcolor
    property color fillcolor

    property bool mvtextalignmode
    property bool evtextalignmode
    property int vtextalignmode

    property bool mvtextalignoffset
    property bool evtextalignoffset
    property real vtextalignoffset

    /* 统计 */
    property int charNum
    property int punctionNum  //标点符号数
    property int imageNum
    property int lineCount    //行数 ~PH_Right数
    property int phraCount    //段落数 ~PH_Left数
}
