/****************************************************************************
** Meta object code from reading C++ file 'souplemanager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../souplemanager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'souplemanager.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN13SoupleManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto SoupleManager::qt_create_metaobjectdata<qt_meta_tag_ZN13SoupleManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SoupleManager",
        "documentWidthChanged",
        "",
        "documentHeightChanged",
        "pageCountChanged",
        "newPageAdded",
        "width",
        "height",
        "top_margin",
        "bottom_margin",
        "pageIndexChanged",
        "page_index",
        "imp_updateUI",
        "documentHeight",
        "documentWidth",
        "getPageCount",
        "sendCommandToData",
        "data_id",
        "command",
        "QVariant",
        "arg",
        "qmlGetData",
        "dataName",
        "requestDeleteObj",
        "pauseDeal",
        "continueDeal",
        "freeze_frame",
        "unfreeze_frame",
        "request_undo",
        "request_redo",
        "createSoupleObj_onHline",
        "type",
        "hline_id",
        "isInsertOnLeft",
        "createSoupleObj_byObj",
        "obj_id",
        "createSoupleObj_Free",
        "x",
        "y",
        "createTable",
        "n_row",
        "n_col",
        "updateViewSize",
        "top",
        "bottom",
        "hasCreatedFirstHLine",
        "getHLineIdByName",
        "name",
        "doc_id",
        "requestRemoveObj",
        "id",
        "tryCreateDecorationFrame",
        "addInheritPage",
        "Page*",
        "addPage",
        "updatePageTopMargin",
        "margin",
        "updatePageBottomMargin",
        "checkHLineValid",
        "hline_name",
        "setShowHelpLine",
        "show",
        "request_render_page",
        "document_id",
        "into_image_id",
        "getPageIndexOfY",
        "installEventFilter",
        "qobj",
        "efST",
        "requestChangeCurrentPageStyle",
        "style",
        "getCurrentPageStyle",
        "getCurrentPageInfo",
        "QVariantMap",
        "getCurrentDocumentID",
        "requestChangePageColumnNum",
        "num",
        "addSoupleDocument",
        "switchSoupleDocument",
        "switch_to_id",
        "getObjHLineName",
        "pageCount"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'documentWidthChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'documentHeightChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'pageCountChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'newPageAdded'
        QtMocHelpers::SignalData<void(float, float, float, float)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 6 }, { QMetaType::Float, 7 }, { QMetaType::Float, 8 }, { QMetaType::Float, 9 },
        }}),
        // Signal 'pageIndexChanged'
        QtMocHelpers::SignalData<void(int)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 11 },
        }}),
        // Slot 'imp_updateUI'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Method 'documentHeight'
        QtMocHelpers::MethodData<float() const>(13, 2, QMC::AccessPublic, QMetaType::Float),
        // Method 'documentWidth'
        QtMocHelpers::MethodData<float() const>(14, 2, QMC::AccessPublic, QMetaType::Float),
        // Method 'getPageCount'
        QtMocHelpers::MethodData<int() const>(15, 2, QMC::AccessPublic, QMetaType::Int),
        // Method 'sendCommandToData'
        QtMocHelpers::MethodData<int(qint32, int, QVariant)>(16, 2, QMC::AccessPublic, QMetaType::Int, {{
            { QMetaType::Int, 17 }, { QMetaType::Int, 18 }, { 0x80000000 | 19, 20 },
        }}),
        // Method 'qmlGetData'
        QtMocHelpers::MethodData<QVariant(qint32, int)>(21, 2, QMC::AccessPublic, 0x80000000 | 19, {{
            { QMetaType::Int, 17 }, { QMetaType::Int, 22 },
        }}),
        // Method 'requestDeleteObj'
        QtMocHelpers::MethodData<void(qint32)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 17 },
        }}),
        // Method 'pauseDeal'
        QtMocHelpers::MethodData<void()>(24, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'continueDeal'
        QtMocHelpers::MethodData<void()>(25, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'freeze_frame'
        QtMocHelpers::MethodData<void()>(26, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'unfreeze_frame'
        QtMocHelpers::MethodData<void()>(27, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'request_undo'
        QtMocHelpers::MethodData<void()>(28, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'request_redo'
        QtMocHelpers::MethodData<void()>(29, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'createSoupleObj_onHline'
        QtMocHelpers::MethodData<void(const QString &, qint32, bool)>(30, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 31 }, { QMetaType::Int, 32 }, { QMetaType::Bool, 33 },
        }}),
        // Method 'createSoupleObj_byObj'
        QtMocHelpers::MethodData<void(const QString &, qint32, bool)>(34, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 31 }, { QMetaType::Int, 35 }, { QMetaType::Bool, 33 },
        }}),
        // Method 'createSoupleObj_Free'
        QtMocHelpers::MethodData<void(const QString &, float, float)>(36, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 31 }, { QMetaType::Float, 37 }, { QMetaType::Float, 38 },
        }}),
        // Method 'createTable'
        QtMocHelpers::MethodData<bool(int, int)>(39, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::Int, 40 }, { QMetaType::Int, 41 },
        }}),
        // Method 'updateViewSize'
        QtMocHelpers::MethodData<void(float, float)>(42, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 43 }, { QMetaType::Float, 44 },
        }}),
        // Method 'hasCreatedFirstHLine'
        QtMocHelpers::MethodData<bool()>(45, 2, QMC::AccessPublic, QMetaType::Bool),
        // Method 'getHLineIdByName'
        QtMocHelpers::MethodData<qint32(const QString &)>(46, 2, QMC::AccessPublic, QMetaType::Int, {{
            { QMetaType::QString, 47 },
        }}),
        // Method 'getHLineIdByName'
        QtMocHelpers::MethodData<qint32(int, const QString &)>(46, 2, QMC::AccessPublic, QMetaType::Int, {{
            { QMetaType::Int, 48 }, { QMetaType::QString, 47 },
        }}),
        // Method 'requestRemoveObj'
        QtMocHelpers::MethodData<void(qint32)>(49, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 50 },
        }}),
        // Method 'tryCreateDecorationFrame'
        QtMocHelpers::MethodData<void()>(51, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'addInheritPage'
        QtMocHelpers::MethodData<Page *()>(52, 2, QMC::AccessPublic, 0x80000000 | 53),
        // Method 'addPage'
        QtMocHelpers::MethodData<Page *(float, float, float, float)>(54, 2, QMC::AccessPublic, 0x80000000 | 53, {{
            { QMetaType::Float, 6 }, { QMetaType::Float, 7 }, { QMetaType::Float, 8 }, { QMetaType::Float, 9 },
        }}),
        // Method 'updatePageTopMargin'
        QtMocHelpers::MethodData<void(int, float)>(55, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 11 }, { QMetaType::Float, 56 },
        }}),
        // Method 'updatePageBottomMargin'
        QtMocHelpers::MethodData<void(int, float)>(57, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 11 }, { QMetaType::Float, 56 },
        }}),
        // Method 'checkHLineValid'
        QtMocHelpers::MethodData<bool(const QString &)>(58, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 59 },
        }}),
        // Method 'checkHLineValid'
        QtMocHelpers::MethodData<bool(int, const QString &)>(58, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::Int, 48 }, { QMetaType::QString, 59 },
        }}),
        // Method 'setShowHelpLine'
        QtMocHelpers::MethodData<void(bool)>(60, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 61 },
        }}),
        // Method 'request_render_page'
        QtMocHelpers::MethodData<bool(int, int, const QString &)>(62, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::Int, 63 }, { QMetaType::Int, 11 }, { QMetaType::QString, 64 },
        }}),
        // Method 'getPageIndexOfY'
        QtMocHelpers::MethodData<int(float)>(65, 2, QMC::AccessPublic, QMetaType::Int, {{
            { QMetaType::Float, 38 },
        }}),
        // Method 'installEventFilter'
        QtMocHelpers::MethodData<void(QObject *)>(66, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QObjectStar, 67 },
        }}),
        // Method 'efST'
        QtMocHelpers::MethodData<void(QObject *)>(68, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QObjectStar, 67 },
        }}),
        // Method 'requestChangeCurrentPageStyle'
        QtMocHelpers::MethodData<void(int)>(69, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 70 },
        }}),
        // Method 'getCurrentPageStyle'
        QtMocHelpers::MethodData<int()>(71, 2, QMC::AccessPublic, QMetaType::Int),
        // Method 'getCurrentPageInfo'
        QtMocHelpers::MethodData<QVariantMap()>(72, 2, QMC::AccessPublic, 0x80000000 | 73),
        // Method 'getCurrentDocumentID'
        QtMocHelpers::MethodData<int()>(74, 2, QMC::AccessPublic, QMetaType::Int),
        // Method 'requestChangePageColumnNum'
        QtMocHelpers::MethodData<void(int)>(75, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 76 },
        }}),
        // Method 'addSoupleDocument'
        QtMocHelpers::MethodData<void(int)>(77, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 50 },
        }}),
        // Method 'switchSoupleDocument'
        QtMocHelpers::MethodData<bool(int)>(78, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::Int, 79 },
        }}),
        // Method 'getObjHLineName'
        QtMocHelpers::MethodData<QString(int)>(80, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::Int, 50 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'documentWidth'
        QtMocHelpers::PropertyData<float>(14, QMetaType::Float, QMC::DefaultPropertyFlags | QMC::Final, 0),
        // property 'documentHeight'
        QtMocHelpers::PropertyData<float>(13, QMetaType::Float, QMC::DefaultPropertyFlags | QMC::Final, 1),
        // property 'pageCount'
        QtMocHelpers::PropertyData<int>(81, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Final, 2),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SoupleManager, qt_meta_tag_ZN13SoupleManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SoupleManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13SoupleManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13SoupleManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13SoupleManagerE_t>.metaTypes,
    nullptr
} };

void SoupleManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SoupleManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->documentWidthChanged(); break;
        case 1: _t->documentHeightChanged(); break;
        case 2: _t->pageCountChanged(); break;
        case 3: _t->newPageAdded((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[4]))); break;
        case 4: _t->pageIndexChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->imp_updateUI(); break;
        case 6: { float _r = _t->documentHeight();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 7: { float _r = _t->documentWidth();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 8: { int _r = _t->getPageCount();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 9: { int _r = _t->sendCommandToData((*reinterpret_cast< std::add_pointer_t<qint32>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[3])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 10: { QVariant _r = _t->qmlGetData((*reinterpret_cast< std::add_pointer_t<qint32>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = std::move(_r); }  break;
        case 11: _t->requestDeleteObj((*reinterpret_cast< std::add_pointer_t<qint32>>(_a[1]))); break;
        case 12: _t->pauseDeal(); break;
        case 13: _t->continueDeal(); break;
        case 14: _t->freeze_frame(); break;
        case 15: _t->unfreeze_frame(); break;
        case 16: _t->request_undo(); break;
        case 17: _t->request_redo(); break;
        case 18: _t->createSoupleObj_onHline((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qint32>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 19: _t->createSoupleObj_byObj((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qint32>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 20: _t->createSoupleObj_Free((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[3]))); break;
        case 21: { bool _r = _t->createTable((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 22: _t->updateViewSize((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2]))); break;
        case 23: { bool _r = _t->hasCreatedFirstHLine();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 24: { qint32 _r = _t->getHLineIdByName((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = std::move(_r); }  break;
        case 25: { qint32 _r = _t->getHLineIdByName((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = std::move(_r); }  break;
        case 26: _t->requestRemoveObj((*reinterpret_cast< std::add_pointer_t<qint32>>(_a[1]))); break;
        case 27: _t->tryCreateDecorationFrame(); break;
        case 28: { Page* _r = _t->addInheritPage();
            if (_a[0]) *reinterpret_cast< Page**>(_a[0]) = std::move(_r); }  break;
        case 29: { Page* _r = _t->addPage((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[4])));
            if (_a[0]) *reinterpret_cast< Page**>(_a[0]) = std::move(_r); }  break;
        case 30: _t->updatePageTopMargin((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2]))); break;
        case 31: _t->updatePageBottomMargin((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2]))); break;
        case 32: { bool _r = _t->checkHLineValid((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 33: { bool _r = _t->checkHLineValid((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 34: _t->setShowHelpLine((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 35: { bool _r = _t->request_render_page((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 36: { int _r = _t->getPageIndexOfY((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 37: _t->installEventFilter((*reinterpret_cast< std::add_pointer_t<QObject*>>(_a[1]))); break;
        case 38: _t->efST((*reinterpret_cast< std::add_pointer_t<QObject*>>(_a[1]))); break;
        case 39: _t->requestChangeCurrentPageStyle((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 40: { int _r = _t->getCurrentPageStyle();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 41: { QVariantMap _r = _t->getCurrentPageInfo();
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 42: { int _r = _t->getCurrentDocumentID();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 43: _t->requestChangePageColumnNum((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 44: _t->addSoupleDocument((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 45: { bool _r = _t->switchSoupleDocument((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 46: { QString _r = _t->getObjHLineName((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SoupleManager::*)()>(_a, &SoupleManager::documentWidthChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SoupleManager::*)()>(_a, &SoupleManager::documentHeightChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (SoupleManager::*)()>(_a, &SoupleManager::pageCountChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (SoupleManager::*)(float , float , float , float )>(_a, &SoupleManager::newPageAdded, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (SoupleManager::*)(int )>(_a, &SoupleManager::pageIndexChanged, 4))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<float*>(_v) = _t->documentWidth(); break;
        case 1: *reinterpret_cast<float*>(_v) = _t->documentHeight(); break;
        case 2: *reinterpret_cast<int*>(_v) = _t->getPageCount(); break;
        default: break;
        }
    }
}

const QMetaObject *SoupleManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SoupleManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13SoupleManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SoupleManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 47;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void SoupleManager::documentWidthChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SoupleManager::documentHeightChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void SoupleManager::pageCountChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void SoupleManager::newPageAdded(float _t1, float _t2, float _t3, float _t4)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2, _t3, _t4);
}

// SIGNAL 4
void SoupleManager::pageIndexChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}
QT_WARNING_POP
