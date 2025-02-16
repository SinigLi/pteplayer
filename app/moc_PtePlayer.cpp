/****************************************************************************
** Meta object code from reading C++ file 'PtePlayer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "./PtePlayer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PtePlayer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PtePlayer_t {
    QByteArrayData data[18];
    char stringdata0[253];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PtePlayer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PtePlayer_t qt_meta_stringdata_PtePlayer = {
    {
QT_MOC_LITERAL(0, 0, 9), // "PtePlayer"
QT_MOC_LITERAL(1, 10, 8), // "openFile"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 8), // "filename"
QT_MOC_LITERAL(4, 29, 17), // "moveCaretToSystem"
QT_MOC_LITERAL(5, 47, 6), // "system"
QT_MOC_LITERAL(6, 54, 19), // "moveCaretToPosition"
QT_MOC_LITERAL(7, 74, 8), // "position"
QT_MOC_LITERAL(8, 83, 17), // "startStopPlayback"
QT_MOC_LITERAL(9, 101, 18), // "from_measure_start"
QT_MOC_LITERAL(10, 120, 18), // "moveCaretToPrevBar"
QT_MOC_LITERAL(11, 139, 18), // "moveCaretToNextBar"
QT_MOC_LITERAL(12, 158, 20), // "moveCaretToPrevStaff"
QT_MOC_LITERAL(13, 179, 13), // "enableEditing"
QT_MOC_LITERAL(14, 193, 6), // "enable"
QT_MOC_LITERAL(15, 200, 11), // "redrawScore"
QT_MOC_LITERAL(16, 212, 23), // "moveCaretToFirstSection"
QT_MOC_LITERAL(17, 236, 16) // "moveCaretToStart"

    },
    "PtePlayer\0openFile\0\0filename\0"
    "moveCaretToSystem\0system\0moveCaretToPosition\0"
    "position\0startStopPlayback\0"
    "from_measure_start\0moveCaretToPrevBar\0"
    "moveCaretToNextBar\0moveCaretToPrevStaff\0"
    "enableEditing\0enable\0redrawScore\0"
    "moveCaretToFirstSection\0moveCaretToStart"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PtePlayer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x08 /* Private */,
       4,    1,   77,    2, 0x08 /* Private */,
       6,    1,   80,    2, 0x08 /* Private */,
       8,    1,   83,    2, 0x08 /* Private */,
       8,    0,   86,    2, 0x28 /* Private | MethodCloned */,
      10,    0,   87,    2, 0x08 /* Private */,
      11,    0,   88,    2, 0x08 /* Private */,
      12,    0,   89,    2, 0x08 /* Private */,
      13,    1,   90,    2, 0x08 /* Private */,
      15,    0,   93,    2, 0x08 /* Private */,
      16,    0,   94,    2, 0x08 /* Private */,
      17,    0,   95,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void PtePlayer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PtePlayer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->openFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->moveCaretToSystem((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->moveCaretToPosition((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->startStopPlayback((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->startStopPlayback(); break;
        case 5: _t->moveCaretToPrevBar(); break;
        case 6: _t->moveCaretToNextBar(); break;
        case 7: _t->moveCaretToPrevStaff(); break;
        case 8: _t->enableEditing((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->redrawScore(); break;
        case 10: _t->moveCaretToFirstSection(); break;
        case 11: _t->moveCaretToStart(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PtePlayer::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_PtePlayer.data,
    qt_meta_data_PtePlayer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PtePlayer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PtePlayer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PtePlayer.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int PtePlayer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
