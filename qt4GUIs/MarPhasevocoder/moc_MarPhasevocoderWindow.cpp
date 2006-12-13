/****************************************************************************
** Meta object code from reading C++ file 'MarPhasevocoderWindow.h'
**
** Created: Tue Dec 12 04:02:26 2006
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MarPhasevocoderWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MarPhasevocoderWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_MarPhasevocoderWindow[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x0a,
      37,   31,   22,   22, 0x0a,
      54,   31,   22,   22, 0x0a,
      73,   31,   22,   22, 0x0a,
      90,   31,   22,   22, 0x0a,
     112,   22,   22,   22, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MarPhasevocoderWindow[] = {
    "MarPhasevocoderWindow\0\0about()\0value\0timeChanged(int)\0"
    "volumeChanged(int)\0freqChanged(int)\0sinusoidsChanged(int)\0open()\0"
};

const QMetaObject MarPhasevocoderWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MarPhasevocoderWindow,
      qt_meta_data_MarPhasevocoderWindow, 0 }
};

const QMetaObject *MarPhasevocoderWindow::metaObject() const
{
    return &staticMetaObject;
}

void *MarPhasevocoderWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MarPhasevocoderWindow))
	return static_cast<void*>(const_cast<MarPhasevocoderWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MarPhasevocoderWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: about(); break;
        case 1: timeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: volumeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: freqChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: sinusoidsChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: open(); break;
        }
        _id -= 6;
    }
    return _id;
}
