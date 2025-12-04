/****************************************************************************
** Meta object code from reading C++ file 'helper.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../helper.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'helper.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN6HelperE_t {};
} // unnamed namespace

template <> constexpr inline auto Helper::qt_create_metaobjectdata<qt_meta_tag_ZN6HelperE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Helper",
        "requestTrimCache",
        "",
        "errorMsg",
        "title",
        "text",
        "beginLoadDocument",
        "fileLoadProgress",
        "pg",
        "fileLoaded",
        "beginSavePdf",
        "pdfSaved",
        "addVLine",
        "vLineName",
        "x_offset",
        "addHLine",
        "hLineName",
        "y_offset",
        "leftLine",
        "rightLine",
        "lastLine",
        "lastAchorLine",
        "QVariant",
        "hLine_objs",
        "beginSelection",
        "clearSelection",
        "selectionStopButKeep",
        "pixel2cm",
        "val",
        "cm2pixel",
        "point2pixel",
        "pixel2point",
        "qurl2localfile",
        "url",
        "requestLoadSoup",
        "filename",
        "requestLoadPdf",
        "QVariantMap",
        "options",
        "requestPreLoadPdf",
        "QJSValue",
        "callback",
        "requestRenderLatexFormula",
        "latex",
        "fontSize",
        "image_save_path",
        "trimCache",
        "isChineseChar",
        "c",
        "mobile_selectFile",
        "mobile_requestPermission",
        "permission",
        "request_renderPdf_as_image",
        "page_index",
        "into_image_id",
        "mainThread_do",
        "std::function<void(void)>",
        "func",
        "temp_Get",
        "key",
        "default_value",
        "temp_Set",
        "value",
        "DB_Get",
        "DB_Read",
        "DB_Set",
        "removeImageFromProvider",
        "id",
        "platform_maximizeWindow",
        "platform_minimizeWindow",
        "platform_restoreWindow",
        "extractFileNameFromPath",
        "createNavLines_fromLevels",
        "args",
        "createNavLines_auto",
        "doTest_1",
        "arg",
        "number2chinese",
        "n",
        "getFontFamilyList",
        "__invokeJSFunc",
        "Error",
        "Error_Invalid_Command",
        "Error_Repeat",
        "Error_Invalid_Data",
        "Error_Failed",
        "Error_Unknown",
        "Error_Other",
        "Layer_Z",
        "Bottom",
        "Text",
        "Cover",
        "Top",
        "Align",
        "AlignVCenter",
        "AlignVBaseLine",
        "AlignTop",
        "AlignBottom",
        "AlignHCenter",
        "AlignLeft",
        "AlignRight",
        "DataName",
        "X",
        "Y",
        "Z",
        "WIDTH",
        "HEIGHT",
        "TEXT",
        "VALIGNOFFSET",
        "NAME",
        "LEFTLINE",
        "RIGHTLINE",
        "DRAGY0",
        "DRAGY1",
        "ANCHOR_LASTHLINE",
        "LOGIC_LASTHLINE",
        "LOGIC_NEXTHLINE",
        "TOP_MARGIN",
        "SOURCE",
        "TOPLINE",
        "BOTTOMLINE",
        "BOTTOM_MARGIN",
        "LEFT_MARGIN",
        "RIGHT_MARGIN",
        "MAX_DRAG_X",
        "NEED_SHOW",
        "TABLE_ALIGNMODE",
        "FONT_SIZE",
        "FONT_COLOR",
        "FILL_MODE",
        "LINE_WIDTH",
        "RADIUS",
        "RADIUS_1",
        "RADIUS_2",
        "RADIUS_3",
        "RADIUS_4",
        "STROKE_COLOR",
        "BG_COLOR",
        "STROKE_WIDTH",
        "LEVEL",
        "SHOW_LEVEL",
        "FIRSTLINE_TAB",
        "ETC_TAB",
        "Command",
        "X_UP",
        "Y_UP",
        "WIDTH_UP",
        "HEIGHT_UP",
        "Z_UP",
        "TEXT_UP",
        "FONT_SIZE_UP",
        "FONT_ITALIC_UP",
        "FONT_BOLD_UP",
        "GOTOLEFT",
        "KEY_RETURN",
        "FONT_FAMILY_UP",
        "COLOR_UP",
        "STROKE_COLOR_UP",
        "VALIGNOFFSET_UP",
        "BOOL_STROKE_UP",
        "INSERTOBJ",
        "STROKE_WIDTH_UP",
        "ANCHOR_LASTHLINE_UP",
        "LOGIC_LASTHLINE_UP",
        "TOP_MARGIN_UP",
        "LEFTLINE_UP",
        "RIGHTLINE_UP",
        "NAME_UP",
        "SOURCE_UP",
        "TOPLINE_UP",
        "BOTTOMLINE_UP",
        "BOTTOM_MARGIN_UP",
        "LEFT_MARGIN_UP",
        "RIGHT_MARGIN_UP",
        "ADDLINE",
        "STHCENTER",
        "STVCENTER",
        "FOCUS_CHANGED",
        "CONT_CHANGED",
        "TABLE_ALIGNMODE_UP",
        "TABLE_MERGE_UNIT",
        "TABLE_DELETE_ROW",
        "TABLE_DELETE_COLUMN",
        "TABLE_SPLIT_UNIT",
        "KEY_PRESSED",
        "KEY_BACKSPACE_PRESSED",
        "Request_Sync",
        "KEY_DOWN",
        "BG_COLOR_UP",
        "FILL_MODE_UP",
        "RADIUS_UP",
        "RADIUS_1_UP",
        "RADIUS_2_UP",
        "RADIUS_3_UP",
        "RADIUS_4_UP",
        "CREATE",
        "SET_FOCUS",
        "LEVEL_UP",
        "SHOW_LEVEL_UP",
        "FIRSTLINE_TAB_UP",
        "ETC_TAB_UP",
        "CLICKED",
        "CURSOR_CHANGE",
        "SelectionCommand",
        "SP_Family_Set",
        "SP_FontSize_Add",
        "SP_FontSize_Set",
        "SP_FontBold_Set",
        "SP_FontItalic_Set",
        "SP_FontEnableStroke_Set",
        "SP_FontStrokeColor_Set",
        "SP_Color_Set",
        "SP_FontStrokeWidth_Add",
        "SP_FontStrokeWidth_Set",
        "SP_UnderLine_Set",
        "SomeMore",
        "Dir_LEFT",
        "Dir_RIGHT",
        "Dir_TOP",
        "Dir_BOTTOM",
        "Dir_SKEW",
        "SelectionProperty",
        "SP_FontSize",
        "SP_FontBold",
        "SP_FontItalic",
        "SP_TextEnableStroke",
        "SP_TextStrokeColor",
        "SP_TextStrokeWidth",
        "SP_UnderLine",
        "SP_TextColor",
        "SP_PathColor",
        "SP_TextEnableFill",
        "SP_vTextAlignMode",
        "SP_vTextAlignOffset",
        "SP_Family",
        "PageType",
        "NoFormat_Page",
        "Word_Page",
        "FillMode",
        "No_Fill",
        "Color_Fill",
        "Gradient_Fill",
        "Image_Fill",
        "Pattern_Fill",
        "Repeat_xy",
        "No_Repeat",
        "LinePattern",
        "None_LinePattern",
        "Solid_LinePattern",
        "Dash_LinePattern",
        "Dot_LinePattern"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'requestTrimCache'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'errorMsg'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 4 }, { QMetaType::QString, 5 },
        }}),
        // Signal 'beginLoadDocument'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'fileLoadProgress'
        QtMocHelpers::SignalData<void(qreal)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QReal, 8 },
        }}),
        // Signal 'fileLoaded'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'beginSavePdf'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'pdfSaved'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'addVLine'
        QtMocHelpers::SignalData<void(const QString &, double)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 }, { QMetaType::Double, 14 },
        }}),
        // Signal 'addHLine'
        QtMocHelpers::SignalData<void(const QString &, double, const QString &, const QString &, const QString &, const QString &, const QVariant &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 16 }, { QMetaType::Double, 17 }, { QMetaType::QString, 18 }, { QMetaType::QString, 19 },
            { QMetaType::QString, 20 }, { QMetaType::QString, 21 }, { 0x80000000 | 22, 23 },
        }}),
        // Signal 'beginSelection'
        QtMocHelpers::SignalData<void()>(24, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'clearSelection'
        QtMocHelpers::SignalData<void()>(25, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'selectionStopButKeep'
        QtMocHelpers::SignalData<void()>(26, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'pixel2cm'
        QtMocHelpers::MethodData<qreal(qreal)>(27, 2, QMC::AccessPublic, QMetaType::QReal, {{
            { QMetaType::QReal, 28 },
        }}),
        // Method 'cm2pixel'
        QtMocHelpers::MethodData<qreal(qreal)>(29, 2, QMC::AccessPublic, QMetaType::QReal, {{
            { QMetaType::QReal, 28 },
        }}),
        // Method 'point2pixel'
        QtMocHelpers::MethodData<qreal(qreal)>(30, 2, QMC::AccessPublic, QMetaType::QReal, {{
            { QMetaType::QReal, 28 },
        }}),
        // Method 'pixel2point'
        QtMocHelpers::MethodData<qreal(qreal)>(31, 2, QMC::AccessPublic, QMetaType::QReal, {{
            { QMetaType::QReal, 28 },
        }}),
        // Method 'qurl2localfile'
        QtMocHelpers::MethodData<QString(QUrl)>(32, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QUrl, 33 },
        }}),
        // Method 'requestLoadSoup'
        QtMocHelpers::MethodData<void(const QString &)>(34, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 35 },
        }}),
        // Method 'requestLoadPdf'
        QtMocHelpers::MethodData<void(const QString &, const QVariantMap &)>(36, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 35 }, { 0x80000000 | 37, 38 },
        }}),
        // Method 'requestLoadPdf'
        QtMocHelpers::MethodData<void(const QString &)>(36, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::QString, 35 },
        }}),
        // Method 'requestPreLoadPdf'
        QtMocHelpers::MethodData<void(const QString &, QJSValue)>(39, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 35 }, { 0x80000000 | 40, 41 },
        }}),
        // Method 'requestRenderLatexFormula'
        QtMocHelpers::MethodData<QString(const QString &, float, const QString &)>(42, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 43 }, { QMetaType::Float, 44 }, { QMetaType::QString, 45 },
        }}),
        // Method 'trimCache'
        QtMocHelpers::MethodData<void()>(46, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'isChineseChar'
        QtMocHelpers::MethodData<bool(QChar)>(47, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QChar, 48 },
        }}),
        // Method 'mobile_selectFile'
        QtMocHelpers::MethodData<void(QJSValue)>(49, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 40, 41 },
        }}),
        // Method 'mobile_requestPermission'
        QtMocHelpers::MethodData<void(const QString &)>(50, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 51 },
        }}),
        // Method 'request_renderPdf_as_image'
        QtMocHelpers::MethodData<void(const QString &, int, const QString &, QJSValue)>(52, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 35 }, { QMetaType::Int, 53 }, { QMetaType::QString, 54 }, { 0x80000000 | 40, 41 },
        }}),
        // Method 'mainThread_do'
        QtMocHelpers::MethodData<void(std::function<void(void)>)>(55, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 56, 57 },
        }}),
        // Method 'temp_Get'
        QtMocHelpers::MethodData<QVariant(const QString &, const QVariant &)>(58, 2, QMC::AccessPublic, 0x80000000 | 22, {{
            { QMetaType::QString, 59 }, { 0x80000000 | 22, 60 },
        }}),
        // Method 'temp_Get'
        QtMocHelpers::MethodData<QVariant(const QString &)>(58, 2, QMC::AccessPublic | QMC::MethodCloned, 0x80000000 | 22, {{
            { QMetaType::QString, 59 },
        }}),
        // Method 'temp_Set'
        QtMocHelpers::MethodData<void(const QString &, const QVariant &)>(61, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 59 }, { 0x80000000 | 22, 62 },
        }}),
        // Method 'DB_Get'
        QtMocHelpers::MethodData<QVariant(const QString &, const QVariant &)>(63, 2, QMC::AccessPublic, 0x80000000 | 22, {{
            { QMetaType::QString, 59 }, { 0x80000000 | 22, 60 },
        }}),
        // Method 'DB_Get'
        QtMocHelpers::MethodData<QVariant(const QString &)>(63, 2, QMC::AccessPublic | QMC::MethodCloned, 0x80000000 | 22, {{
            { QMetaType::QString, 59 },
        }}),
        // Method 'DB_Read'
        QtMocHelpers::MethodData<QVariant(const QString &, const QVariant &)>(64, 2, QMC::AccessPublic, 0x80000000 | 22, {{
            { QMetaType::QString, 59 }, { 0x80000000 | 22, 60 },
        }}),
        // Method 'DB_Read'
        QtMocHelpers::MethodData<QVariant(const QString &)>(64, 2, QMC::AccessPublic | QMC::MethodCloned, 0x80000000 | 22, {{
            { QMetaType::QString, 59 },
        }}),
        // Method 'DB_Set'
        QtMocHelpers::MethodData<void(const QString &, const QVariant &)>(65, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 59 }, { 0x80000000 | 22, 62 },
        }}),
        // Method 'removeImageFromProvider'
        QtMocHelpers::MethodData<void(const QString &)>(66, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 67 },
        }}),
        // Method 'platform_maximizeWindow'
        QtMocHelpers::MethodData<void()>(68, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'platform_minimizeWindow'
        QtMocHelpers::MethodData<void()>(69, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'platform_restoreWindow'
        QtMocHelpers::MethodData<void()>(70, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'extractFileNameFromPath'
        QtMocHelpers::MethodData<QString(QUrl)>(71, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QUrl, 33 },
        }}),
        // Method 'createNavLines_fromLevels'
        QtMocHelpers::MethodData<void(QVariantMap)>(72, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 37, 73 },
        }}),
        // Method 'createNavLines_auto'
        QtMocHelpers::MethodData<void(QVariantMap)>(74, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 37, 73 },
        }}),
        // Method 'doTest_1'
        QtMocHelpers::MethodData<void(QVariant)>(75, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 22, 76 },
        }}),
        // Method 'number2chinese'
        QtMocHelpers::MethodData<QChar(int)>(77, 2, QMC::AccessPublic, QMetaType::QChar, {{
            { QMetaType::Int, 78 },
        }}),
        // Method 'getFontFamilyList'
        QtMocHelpers::MethodData<QStringList()>(79, 2, QMC::AccessPublic, QMetaType::QStringList),
        // Method '__invokeJSFunc'
        QtMocHelpers::MethodData<void(QJSValue, const QVariantMap &)>(80, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 40, 57 }, { 0x80000000 | 37, 76 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'Error'
        QtMocHelpers::EnumData<Error>(81, 81, QMC::EnumFlags{}).add({
            {   82, Error::Error_Invalid_Command },
            {   83, Error::Error_Repeat },
            {   84, Error::Error_Invalid_Data },
            {   85, Error::Error_Failed },
            {   86, Error::Error_Unknown },
            {   87, Error::Error_Other },
        }),
        // enum 'Layer_Z'
        QtMocHelpers::EnumData<Layer_Z>(88, 88, QMC::EnumFlags{}).add({
            {   89, Layer_Z::Bottom },
            {   90, Layer_Z::Text },
            {   91, Layer_Z::Cover },
            {   92, Layer_Z::Top },
        }),
        // enum 'Align'
        QtMocHelpers::EnumData<Align>(93, 93, QMC::EnumFlags{}).add({
            {   94, Align::AlignVCenter },
            {   95, Align::AlignVBaseLine },
            {   96, Align::AlignTop },
            {   97, Align::AlignBottom },
            {   98, Align::AlignHCenter },
            {   99, Align::AlignLeft },
            {  100, Align::AlignRight },
        }),
        // enum 'DataName'
        QtMocHelpers::EnumData<DataName>(101, 101, QMC::EnumFlags{}).add({
            {  102, DataName::X },
            {  103, DataName::Y },
            {  104, DataName::Z },
            {  105, DataName::WIDTH },
            {  106, DataName::HEIGHT },
            {  107, DataName::TEXT },
            {  108, DataName::VALIGNOFFSET },
            {  109, DataName::NAME },
            {  110, DataName::LEFTLINE },
            {  111, DataName::RIGHTLINE },
            {  112, DataName::DRAGY0 },
            {  113, DataName::DRAGY1 },
            {  114, DataName::ANCHOR_LASTHLINE },
            {  115, DataName::LOGIC_LASTHLINE },
            {  116, DataName::LOGIC_NEXTHLINE },
            {  117, DataName::TOP_MARGIN },
            {  118, DataName::SOURCE },
            {  119, DataName::TOPLINE },
            {  120, DataName::BOTTOMLINE },
            {  121, DataName::BOTTOM_MARGIN },
            {  122, DataName::LEFT_MARGIN },
            {  123, DataName::RIGHT_MARGIN },
            {  124, DataName::MAX_DRAG_X },
            {  125, DataName::NEED_SHOW },
            {  126, DataName::TABLE_ALIGNMODE },
            {  127, DataName::FONT_SIZE },
            {  128, DataName::FONT_COLOR },
            {  129, DataName::FILL_MODE },
            {  130, DataName::LINE_WIDTH },
            {  131, DataName::RADIUS },
            {  132, DataName::RADIUS_1 },
            {  133, DataName::RADIUS_2 },
            {  134, DataName::RADIUS_3 },
            {  135, DataName::RADIUS_4 },
            {  136, DataName::STROKE_COLOR },
            {  137, DataName::BG_COLOR },
            {  138, DataName::STROKE_WIDTH },
            {  139, DataName::LEVEL },
            {  140, DataName::SHOW_LEVEL },
            {  141, DataName::FIRSTLINE_TAB },
            {  142, DataName::ETC_TAB },
        }),
        // enum 'Command'
        QtMocHelpers::EnumData<Command>(143, 143, QMC::EnumFlags{}).add({
            {  144, Command::X_UP },
            {  145, Command::Y_UP },
            {  146, Command::WIDTH_UP },
            {  147, Command::HEIGHT_UP },
            {  148, Command::Z_UP },
            {  149, Command::TEXT_UP },
            {  150, Command::FONT_SIZE_UP },
            {  151, Command::FONT_ITALIC_UP },
            {  152, Command::FONT_BOLD_UP },
            {  153, Command::GOTOLEFT },
            {  154, Command::KEY_RETURN },
            {  155, Command::FONT_FAMILY_UP },
            {  156, Command::COLOR_UP },
            {  157, Command::STROKE_COLOR_UP },
            {  158, Command::VALIGNOFFSET_UP },
            {  159, Command::BOOL_STROKE_UP },
            {  160, Command::INSERTOBJ },
            {  161, Command::STROKE_WIDTH_UP },
            {  162, Command::ANCHOR_LASTHLINE_UP },
            {  163, Command::LOGIC_LASTHLINE_UP },
            {  164, Command::TOP_MARGIN_UP },
            {  165, Command::LEFTLINE_UP },
            {  166, Command::RIGHTLINE_UP },
            {  167, Command::NAME_UP },
            {  168, Command::SOURCE_UP },
            {  169, Command::TOPLINE_UP },
            {  170, Command::BOTTOMLINE_UP },
            {  171, Command::BOTTOM_MARGIN_UP },
            {  172, Command::LEFT_MARGIN_UP },
            {  173, Command::RIGHT_MARGIN_UP },
            {  174, Command::ADDLINE },
            {  175, Command::STHCENTER },
            {  176, Command::STVCENTER },
            {  177, Command::FOCUS_CHANGED },
            {  178, Command::CONT_CHANGED },
            {  179, Command::TABLE_ALIGNMODE_UP },
            {  180, Command::TABLE_MERGE_UNIT },
            {  181, Command::TABLE_DELETE_ROW },
            {  182, Command::TABLE_DELETE_COLUMN },
            {  183, Command::TABLE_SPLIT_UNIT },
            {  184, Command::KEY_PRESSED },
            {  185, Command::KEY_BACKSPACE_PRESSED },
            {  186, Command::Request_Sync },
            {  187, Command::KEY_DOWN },
            {  188, Command::BG_COLOR_UP },
            {  189, Command::FILL_MODE_UP },
            {  190, Command::RADIUS_UP },
            {  191, Command::RADIUS_1_UP },
            {  192, Command::RADIUS_2_UP },
            {  193, Command::RADIUS_3_UP },
            {  194, Command::RADIUS_4_UP },
            {  195, Command::CREATE },
            {  196, Command::SET_FOCUS },
            {  197, Command::LEVEL_UP },
            {  198, Command::SHOW_LEVEL_UP },
            {  199, Command::FIRSTLINE_TAB_UP },
            {  200, Command::ETC_TAB_UP },
            {  201, Command::CLICKED },
            {  202, Command::CURSOR_CHANGE },
        }),
        // enum 'SelectionCommand'
        QtMocHelpers::EnumData<SelectionCommand>(203, 203, QMC::EnumFlags{}).add({
            {  204, SelectionCommand::SP_Family_Set },
            {  205, SelectionCommand::SP_FontSize_Add },
            {  206, SelectionCommand::SP_FontSize_Set },
            {  207, SelectionCommand::SP_FontBold_Set },
            {  208, SelectionCommand::SP_FontItalic_Set },
            {  209, SelectionCommand::SP_FontEnableStroke_Set },
            {  210, SelectionCommand::SP_FontStrokeColor_Set },
            {  211, SelectionCommand::SP_Color_Set },
            {  212, SelectionCommand::SP_FontStrokeWidth_Add },
            {  213, SelectionCommand::SP_FontStrokeWidth_Set },
            {  214, SelectionCommand::SP_UnderLine_Set },
        }),
        // enum 'SomeMore'
        QtMocHelpers::EnumData<SomeMore>(215, 215, QMC::EnumFlags{}).add({
            {  216, SomeMore::Dir_LEFT },
            {  217, SomeMore::Dir_RIGHT },
            {  218, SomeMore::Dir_TOP },
            {  219, SomeMore::Dir_BOTTOM },
            {  220, SomeMore::Dir_SKEW },
        }),
        // enum 'SelectionProperty'
        QtMocHelpers::EnumData<SelectionProperty>(221, 221, QMC::EnumFlags{}).add({
            {  222, SelectionProperty::SP_FontSize },
            {  223, SelectionProperty::SP_FontBold },
            {  224, SelectionProperty::SP_FontItalic },
            {  225, SelectionProperty::SP_TextEnableStroke },
            {  226, SelectionProperty::SP_TextStrokeColor },
            {  227, SelectionProperty::SP_TextStrokeWidth },
            {  228, SelectionProperty::SP_UnderLine },
            {  229, SelectionProperty::SP_TextColor },
            {  230, SelectionProperty::SP_PathColor },
            {  231, SelectionProperty::SP_TextEnableFill },
            {  232, SelectionProperty::SP_vTextAlignMode },
            {  233, SelectionProperty::SP_vTextAlignOffset },
            {  234, SelectionProperty::SP_Family },
        }),
        // enum 'PageType'
        QtMocHelpers::EnumData<PageType>(235, 235, QMC::EnumFlags{}).add({
            {  236, PageType::NoFormat_Page },
            {  237, PageType::Word_Page },
        }),
        // enum 'FillMode'
        QtMocHelpers::EnumData<FillMode>(238, 238, QMC::EnumFlags{}).add({
            {  239, FillMode::No_Fill },
            {  240, FillMode::Color_Fill },
            {  241, FillMode::Gradient_Fill },
            {  242, FillMode::Image_Fill },
            {  243, FillMode::Pattern_Fill },
            {  244, FillMode::Repeat_xy },
            {  245, FillMode::No_Repeat },
        }),
        // enum 'LinePattern'
        QtMocHelpers::EnumData<LinePattern>(246, 246, QMC::EnumFlags{}).add({
            {  247, LinePattern::None_LinePattern },
            {  248, LinePattern::Solid_LinePattern },
            {  249, LinePattern::Dash_LinePattern },
            {  250, LinePattern::Dot_LinePattern },
        }),
    };
    return QtMocHelpers::metaObjectData<Helper, qt_meta_tag_ZN6HelperE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Helper::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6HelperE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6HelperE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN6HelperE_t>.metaTypes,
    nullptr
} };

void Helper::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Helper *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->requestTrimCache(); break;
        case 1: _t->errorMsg((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 2: _t->beginLoadDocument(); break;
        case 3: _t->fileLoadProgress((*reinterpret_cast< std::add_pointer_t<qreal>>(_a[1]))); break;
        case 4: _t->fileLoaded(); break;
        case 5: _t->beginSavePdf(); break;
        case 6: _t->pdfSaved(); break;
        case 7: _t->addVLine((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 8: _t->addHLine((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[6])),(*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[7]))); break;
        case 9: _t->beginSelection(); break;
        case 10: _t->clearSelection(); break;
        case 11: _t->selectionStopButKeep(); break;
        case 12: { qreal _r = _t->pixel2cm((*reinterpret_cast< std::add_pointer_t<qreal>>(_a[1])));
            if (_a[0]) *reinterpret_cast< qreal*>(_a[0]) = std::move(_r); }  break;
        case 13: { qreal _r = _t->cm2pixel((*reinterpret_cast< std::add_pointer_t<qreal>>(_a[1])));
            if (_a[0]) *reinterpret_cast< qreal*>(_a[0]) = std::move(_r); }  break;
        case 14: { qreal _r = _t->point2pixel((*reinterpret_cast< std::add_pointer_t<qreal>>(_a[1])));
            if (_a[0]) *reinterpret_cast< qreal*>(_a[0]) = std::move(_r); }  break;
        case 15: { qreal _r = _t->pixel2point((*reinterpret_cast< std::add_pointer_t<qreal>>(_a[1])));
            if (_a[0]) *reinterpret_cast< qreal*>(_a[0]) = std::move(_r); }  break;
        case 16: { QString _r = _t->qurl2localfile((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 17: _t->requestLoadSoup((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 18: _t->requestLoadPdf((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[2]))); break;
        case 19: _t->requestLoadPdf((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 20: _t->requestPreLoadPdf((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QJSValue>>(_a[2]))); break;
        case 21: { QString _r = _t->requestRenderLatexFormula((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 22: _t->trimCache(); break;
        case 23: { bool _r = _t->isChineseChar((*reinterpret_cast< std::add_pointer_t<QChar>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 24: _t->mobile_selectFile((*reinterpret_cast< std::add_pointer_t<QJSValue>>(_a[1]))); break;
        case 25: _t->mobile_requestPermission((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 26: _t->request_renderPdf_as_image((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QJSValue>>(_a[4]))); break;
        case 27: _t->mainThread_do((*reinterpret_cast< std::add_pointer_t<std::function<void(void)>>>(_a[1]))); break;
        case 28: { QVariant _r = _t->temp_Get((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = std::move(_r); }  break;
        case 29: { QVariant _r = _t->temp_Get((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = std::move(_r); }  break;
        case 30: _t->temp_Set((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[2]))); break;
        case 31: { QVariant _r = _t->DB_Get((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = std::move(_r); }  break;
        case 32: { QVariant _r = _t->DB_Get((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = std::move(_r); }  break;
        case 33: { QVariant _r = _t->DB_Read((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = std::move(_r); }  break;
        case 34: { QVariant _r = _t->DB_Read((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = std::move(_r); }  break;
        case 35: _t->DB_Set((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[2]))); break;
        case 36: _t->removeImageFromProvider((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 37: _t->platform_maximizeWindow(); break;
        case 38: _t->platform_minimizeWindow(); break;
        case 39: _t->platform_restoreWindow(); break;
        case 40: { QString _r = _t->extractFileNameFromPath((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 41: _t->createNavLines_fromLevels((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 42: _t->createNavLines_auto((*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 43: _t->doTest_1((*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[1]))); break;
        case 44: { QChar _r = _t->number2chinese((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QChar*>(_a[0]) = std::move(_r); }  break;
        case 45: { QStringList _r = _t->getFontFamilyList();
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = std::move(_r); }  break;
        case 46: _t->__invokeJSFunc((*reinterpret_cast< std::add_pointer_t<QJSValue>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariantMap>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 20:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QJSValue >(); break;
            }
            break;
        case 24:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QJSValue >(); break;
            }
            break;
        case 26:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 3:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QJSValue >(); break;
            }
            break;
        case 46:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QJSValue >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (Helper::*)()>(_a, &Helper::requestTrimCache, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (Helper::*)(const QString & , const QString & )>(_a, &Helper::errorMsg, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (Helper::*)()>(_a, &Helper::beginLoadDocument, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (Helper::*)(qreal )>(_a, &Helper::fileLoadProgress, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (Helper::*)()>(_a, &Helper::fileLoaded, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (Helper::*)()>(_a, &Helper::beginSavePdf, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (Helper::*)()>(_a, &Helper::pdfSaved, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (Helper::*)(const QString & , double )>(_a, &Helper::addVLine, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (Helper::*)(const QString & , double , const QString & , const QString & , const QString & , const QString & , const QVariant & )>(_a, &Helper::addHLine, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (Helper::*)()>(_a, &Helper::beginSelection, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (Helper::*)()>(_a, &Helper::clearSelection, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (Helper::*)()>(_a, &Helper::selectionStopButKeep, 11))
            return;
    }
}

const QMetaObject *Helper::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Helper::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6HelperE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Helper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 47)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 47;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 47)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 47;
    }
    return _id;
}

// SIGNAL 0
void Helper::requestTrimCache()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Helper::errorMsg(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void Helper::beginLoadDocument()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Helper::fileLoadProgress(qreal _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void Helper::fileLoaded()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void Helper::beginSavePdf()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void Helper::pdfSaved()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void Helper::addVLine(const QString & _t1, double _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2);
}

// SIGNAL 8
void Helper::addHLine(const QString & _t1, double _t2, const QString & _t3, const QString & _t4, const QString & _t5, const QString & _t6, const QVariant & _t7)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1, _t2, _t3, _t4, _t5, _t6, _t7);
}

// SIGNAL 9
void Helper::beginSelection()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void Helper::clearSelection()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void Helper::selectionStopButKeep()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}
QT_WARNING_POP
