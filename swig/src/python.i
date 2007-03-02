%module marsyas_python

%{
#include <Python.h>

static PyObject *STR2PY (string s) { return PyString_FromStringAndSize(s.c_str(),s.size()); }

static PyObject *MCP2PY (MarControlPtr mcp) {
	if (mcp.isInvalid()) { Py_RETURN_NONE; }
	if (mcp->getType() == "mrs_natural") return PyLong_FromLong(mcp->toNatural());
	if (mcp->getType() == "mrs_real" ) return PyFloat_FromDouble(mcp->toReal());
	if (mcp->getType() == "mrs_string" ) return STR2PY(mcp->toString());
	if (mcp->getType() == "mrs_bool" ) if(mcp->toBool()) { Py_RETURN_TRUE; } else { Py_RETURN_FALSE; }
	Py_RETURN_NONE;
}

static string PY2STR ( PyObject *pyo ) {
        return string(PyString_AsString(pyo));
}

static MarControlPtr PY2MCP ( PyObject *pyo ) {
        if (PyFloat_Check(pyo)) return MarControlPtr(PyFloat_AsDouble(pyo));
        if (pyo == Py_True) return MarControlPtr(true);
        if (pyo == Py_False) return MarControlPtr(false);
        if (PyInt_Check(pyo)) return MarControlPtr(PyInt_AsLong(pyo));
        if (PyString_Check(pyo)) return MarControlPtr(PY2STR(pyo));
        return MarControlPtr();
}
%}

%typemap(out) vector<string> {
        $result = PyList_New($1.size());
        for (int i=0; i<$1.size(); i++)
                PyList_SetItem($result,i,STR2PY($1.at(i)));
}

%typemap(out) map<string,MarControlPtr> {
        $result = PyDict_New();
        PyObject *key, *value;
        for (map<string,MarControlPtr>::iterator i = $1.begin(); i != $1.end(); i++ )
        {
                key = STR2PY((*i).first);
                value = MCP2PY(i->second);
                PyDict_SetItem($result,key,value);
        }
}

%typemap(in) MarControlPtr { $1 = PY2MCP($input); }
%typemap(out) MarControlPtr { $result = MCP2PY($1); }

%typemap(in) string = char*;
%typemap(typecheck) string = char *;
%typemap(out) string { $result = STR2PY($1); };