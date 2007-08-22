/****************************************************************************
** Meta object code from reading C++ file 'MarSystemQtWrapper.h'
**
** Created: Tue Aug 21 19:37:11 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MarSystemQtWrapper.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MarSystemQtWrapper.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_MarSystemQtWrapper[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      26,   20,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
      66,   53,   19,   19, 0x0a,
     120,  103,   19,   19, 0x0a,
     155,  103,   19,   19, 0x0a,
     184,   19,   19,   19, 0x0a,
     220,   20,  206,   19, 0x0a,
     236,   19,   19,   19, 0x0a,
     243,   19,   19,   19, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MarSystemQtWrapper[] = {
    "MarSystemQtWrapper\0\0cname\0"
    "ctrlChanged(MarControlPtr)\0control,cval\0"
    "updctrl(MarControlPtr,MarControlPtr)\0"
    "cname,newcontrol\0updctrl(std::string,MarControlPtr)\0"
    "updctrl(char*,MarControlPtr)\0"
    "emitTrackedControls()\0MarControlPtr\0"
    "getctrl(string)\0play()\0pause()\0"
};

const QMetaObject MarSystemQtWrapper::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_MarSystemQtWrapper,
      qt_meta_data_MarSystemQtWrapper, 0 }
};

const QMetaObject *MarSystemQtWrapper::metaObject() const
{
    return &staticMetaObject;
}

void *MarSystemQtWrapper::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MarSystemQtWrapper))
	return static_cast<void*>(const_cast< MarSystemQtWrapper*>(this));
    return QThread::qt_metacast(_clname);
}

int MarSystemQtWrapper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: ctrlChanged((*reinterpret_cast< MarControlPtr(*)>(_a[1]))); break;
        case 1: updctrl((*reinterpret_cast< MarControlPtr(*)>(_a[1])),(*reinterpret_cast< MarControlPtr(*)>(_a[2]))); break;
        case 2: updctrl((*reinterpret_cast< std::string(*)>(_a[1])),(*reinterpret_cast< MarControlPtr(*)>(_a[2]))); break;
        case 3: updctrl((*reinterpret_cast< char*(*)>(_a[1])),(*reinterpret_cast< MarControlPtr(*)>(_a[2]))); break;
        case 4: emitTrackedControls(); break;
        case 5: { MarControlPtr _r = getctrl((*reinterpret_cast< string(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< MarControlPtr*>(_a[0]) = _r; }  break;
        case 6: play(); break;
        case 7: pause(); break;
        }
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void MarSystemQtWrapper::ctrlChanged(MarControlPtr _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
