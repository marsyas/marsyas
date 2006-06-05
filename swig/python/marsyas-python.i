%{

#include <vector>
#include <string>
#include <map>
#include <marsyas/MarControlValue.h>
#include <Python.h>

using namespace std;

static PyObject *STR2PY (string s) { return PyString_FromStringAndSize(s.c_str(),s.size()); }

static PyObject *MCV2PY (MarControlValue mcv) {
        switch(mcv.getType()) {
        case mar_real: return PyFloat_FromDouble(mcv.toReal());
        case mar_bool: if ( mcv.toBool() ) { Py_RETURN_TRUE; } else { Py_RETURN_FALSE; }
        case mar_natural: return PyLong_FromLong(mcv.toNatural());
        case mar_string: return STR2PY(mcv.toString());
        case mar_vec:
        default:
        case mar_null: Py_RETURN_NONE;
        }
}

static string PY2STR ( PyObject *pyo ) {
        return string(PyString_AsString(pyo));
}

static MarControlValue PY2MCV ( PyObject *pyo ) {
        if (PyFloat_Check(pyo)) return MarControlValue(PyFloat_AsDouble(pyo));
        if (pyo == Py_True) return MarControlValue(true);
        if (pyo == Py_False) return MarControlValue(false);
        if (PyInt_Check(pyo)) return MarControlValue(PyInt_AsLong(pyo));
        if (PyString_Check(pyo)) return MarControlValue(PY2STR(pyo));
        return MarControlValue();
}

%}

%typemap(out) string {
        $result = STR2PY($1);
}

%typemap(out) vector<string> {
        $result = PyList_New($1.size());
        for (int i=0; i<$1.size(); i++)
                PyList_SetItem($result,i,STR2PY($1.at(i)));
}

%typemap(out) MarControlValue {
        $result = MCV2PY($1);
}

%typemap(out) map<string,MarControlValue> {
        $result = PyDict_New();
        PyObject *key, *value;
        for (map<string,MarControlValue>::iterator i = $1.begin(); i != $1.end(); i++ )
        {
                key = STR2PY((*i).first);
                value = MCV2PY(i->second);
                PyDict_SetItem($result,key,value);
        }
}

%typemap(in) MarControlValue {
        $1 = PY2MCV($input);
}

%module marsyas_python
%include "../common/marsyas.i"
