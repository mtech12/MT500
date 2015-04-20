/****************************************************************************
** Meta object code from reading C++ file 'mt500.h'
**
** Created: Sun Apr 19 22:57:42 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mt500.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mt500.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MT500[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       7,    6,    6,    6, 0x0a,
      17,    6,    6,    6, 0x0a,
      26,    6,    6,    6, 0x0a,
      36,    6,    6,    6, 0x0a,
      47,    6,    6,    6, 0x0a,
      60,    6,    6,    6, 0x0a,
      73,    6,    6,    6, 0x0a,
      82,    6,    6,    6, 0x08,
     105,    6,    6,    6, 0x08,
     128,    6,    6,    6, 0x08,
     152,    6,    6,    6, 0x08,
     175,    6,    6,    6, 0x08,
     212,  206,    6,    6, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MT500[] = {
    "MT500\0\0getData()\0sendHB()\0getFips()\0"
    "readData()\0ipError(int)\0resetPorts()\0"
    "clrBuf()\0on_addButton_clicked()\0"
    "on_delButton_clicked()\0on_testButton_clicked()\0"
    "on_clrButton_clicked()\0"
    "on_reconfigureButton_clicked()\0index\0"
    "on_runModeComboBox_currentIndexChanged(int)\0"
};

void MT500::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MT500 *_t = static_cast<MT500 *>(_o);
        switch (_id) {
        case 0: _t->getData(); break;
        case 1: _t->sendHB(); break;
        case 2: _t->getFips(); break;
        case 3: _t->readData(); break;
        case 4: _t->ipError((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->resetPorts(); break;
        case 6: _t->clrBuf(); break;
        case 7: _t->on_addButton_clicked(); break;
        case 8: _t->on_delButton_clicked(); break;
        case 9: _t->on_testButton_clicked(); break;
        case 10: _t->on_clrButton_clicked(); break;
        case 11: _t->on_reconfigureButton_clicked(); break;
        case 12: _t->on_runModeComboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MT500::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MT500::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MT500,
      qt_meta_data_MT500, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MT500::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MT500::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MT500::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MT500))
        return static_cast<void*>(const_cast< MT500*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MT500::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
