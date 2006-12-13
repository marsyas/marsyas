/****************************************************************************
** Meta object code from reading C++ file 'MarSystemWrapper.h'
**
** Created: Tue Dec 12 04:02:25 2006
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MarSystemWrapper.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MarSystemWrapper.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_MarSystemWrapper[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      31,   18,   17,   17, 0x05,
      69,   65,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      85,   18,   17,   17, 0x0a,
     135,  129,  115,   17, 0x0a,
     151,   17,   17,   17, 0x0a,
     158,   17,   17,   17, 0x0a,
     166,   17,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MarSystemWrapper[] = {
    "MarSystemWrapper\0\0cname,cvalue\0ctrlChanged(string,MarControlPtr)\0"
    "val\0posChanged(int)\0updctrl(string,MarControlPtr)\0MarControlPtr\0"
    "cname\0getctrl(string)\0play()\0pause()\0run()\0"
};

const QMetaObject MarSystemWrapper::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_MarSystemWrapper,
      qt_meta_data_MarSystemWrapper, 0 }
};

const QMetaObject *MarSystemWrapper::metaObject() const
{
    return &staticMetaObject;
}

void *MarSystemWrapper::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MarSystemWrapper))
	return static_cast<void*>(const_cast<MarSystemWrapper*>(this));
    return QThread::qt_metacast(_clname);
}

int MarSystemWrapper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: ctrlChanged((*reinterpret_cast< string(*)>(_a[1])),(*reinterpret_cast< MarControlPtr(*)>(_a[2]))); break;
        case 1: posChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: updctrl((*reinterpret_cast< string(*)>(_a[1])),(*reinterpret_cast< MarControlPtr(*)>(_a[2]))); break;
        case 3: { MarControlPtr _r = getctrl((*reinterpret_cast< string(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< MarControlPtr*>(_a[0]) = _r; }  break;
        case 4: play(); break;
        case 5: pause(); break;
        case 6: run(); break;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void MarSystemWrapper::ctrlChanged(string _t1, MarControlPtr _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MarSystemWrapper::posChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
