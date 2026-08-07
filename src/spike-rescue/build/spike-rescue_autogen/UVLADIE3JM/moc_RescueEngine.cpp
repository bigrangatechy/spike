/****************************************************************************
** Meta object code from reading C++ file 'RescueEngine.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/RescueEngine.hpp"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RescueEngine.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.2. It"
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
struct qt_meta_tag_ZN5spike12RescueEngineE_t {};
} // unnamed namespace

template <> constexpr inline auto spike::RescueEngine::qt_create_metaobjectdata<qt_meta_tag_ZN5spike12RescueEngineE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "spike::RescueEngine",
        "scanProgress",
        "",
        "message",
        "percent",
        "scanFinished",
        "ok",
        "inventoryFinished",
        "destinationsChanged",
        "QList<DestVolume>",
        "vols",
        "copyProgress",
        "currentFile",
        "doneFiles",
        "totalFiles",
        "doneBytes",
        "totalBytes",
        "copyFinished",
        "scanSystems",
        "inventorySystem",
        "systemIndex",
        "refreshDestinations",
        "startCopy",
        "destMount",
        "requestCancel",
        "cleanupMountsSlot",
        "cleanupMounts"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'scanProgress'
        QtMocHelpers::SignalData<void(const QString &, int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::Int, 4 },
        }}),
        // Signal 'scanFinished'
        QtMocHelpers::SignalData<void(bool)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 6 },
        }}),
        // Signal 'inventoryFinished'
        QtMocHelpers::SignalData<void(bool)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 6 },
        }}),
        // Signal 'destinationsChanged'
        QtMocHelpers::SignalData<void(const QVector<DestVolume> &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 10 },
        }}),
        // Signal 'copyProgress'
        QtMocHelpers::SignalData<void(const QString &, qint64, qint64, qint64, qint64)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 12 }, { QMetaType::LongLong, 13 }, { QMetaType::LongLong, 14 }, { QMetaType::LongLong, 15 },
            { QMetaType::LongLong, 16 },
        }}),
        // Signal 'copyFinished'
        QtMocHelpers::SignalData<void(bool)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 6 },
        }}),
        // Slot 'scanSystems'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'inventorySystem'
        QtMocHelpers::SlotData<void(int)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 20 },
        }}),
        // Slot 'refreshDestinations'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'startCopy'
        QtMocHelpers::SlotData<void(int, const QString &)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 20 }, { QMetaType::QString, 23 },
        }}),
        // Slot 'requestCancel'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'cleanupMountsSlot'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'cleanupMounts'
        QtMocHelpers::MethodData<void()>(26, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<RescueEngine, qt_meta_tag_ZN5spike12RescueEngineE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject spike::RescueEngine::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5spike12RescueEngineE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5spike12RescueEngineE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN5spike12RescueEngineE_t>.metaTypes,
    nullptr
} };

void spike::RescueEngine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<RescueEngine *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->scanProgress((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 1: _t->scanFinished((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->inventoryFinished((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->destinationsChanged((*reinterpret_cast<std::add_pointer_t<QList<DestVolume>>>(_a[1]))); break;
        case 4: _t->copyProgress((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<qint64>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<qint64>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<qint64>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<qint64>>(_a[5]))); break;
        case 5: _t->copyFinished((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 6: _t->scanSystems(); break;
        case 7: _t->inventorySystem((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->refreshDestinations(); break;
        case 9: _t->startCopy((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 10: _t->requestCancel(); break;
        case 11: _t->cleanupMountsSlot(); break;
        case 12: _t->cleanupMounts(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (RescueEngine::*)(const QString & , int )>(_a, &RescueEngine::scanProgress, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (RescueEngine::*)(bool )>(_a, &RescueEngine::scanFinished, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (RescueEngine::*)(bool )>(_a, &RescueEngine::inventoryFinished, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (RescueEngine::*)(const QVector<DestVolume> & )>(_a, &RescueEngine::destinationsChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (RescueEngine::*)(const QString & , qint64 , qint64 , qint64 , qint64 )>(_a, &RescueEngine::copyProgress, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (RescueEngine::*)(bool )>(_a, &RescueEngine::copyFinished, 5))
            return;
    }
}

const QMetaObject *spike::RescueEngine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *spike::RescueEngine::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5spike12RescueEngineE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int spike::RescueEngine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void spike::RescueEngine::scanProgress(const QString & _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void spike::RescueEngine::scanFinished(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void spike::RescueEngine::inventoryFinished(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void spike::RescueEngine::destinationsChanged(const QVector<DestVolume> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void spike::RescueEngine::copyProgress(const QString & _t1, qint64 _t2, qint64 _t3, qint64 _t4, qint64 _t5)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2, _t3, _t4, _t5);
}

// SIGNAL 5
void spike::RescueEngine::copyFinished(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}
QT_WARNING_POP
