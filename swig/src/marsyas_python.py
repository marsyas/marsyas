# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.

import _marsyas_python
import new
new_instancemethod = new.instancemethod
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


def _swig_setattr_nondynamic_method(set):
    def set_attr(self,name,value):
        if (name == "thisown"): return self.this.own(value)
        if hasattr(self,name) or (name == "this"):
            set(self,name,value)
        else:
            raise AttributeError("You cannot add attributes to %s" % self)
    return set_attr


class MarSystem(object):
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _marsyas_python.delete_MarSystem
MarSystem.tick = new_instancemethod(_marsyas_python.MarSystem_tick,None,MarSystem)
MarSystem.update = new_instancemethod(_marsyas_python.MarSystem_update,None,MarSystem)
MarSystem.addMarSystem = new_instancemethod(_marsyas_python.MarSystem_addMarSystem,None,MarSystem)
MarSystem.getType = new_instancemethod(_marsyas_python.MarSystem_getType,None,MarSystem)
MarSystem.getName = new_instancemethod(_marsyas_python.MarSystem_getName,None,MarSystem)
MarSystem.getPrefix = new_instancemethod(_marsyas_python.MarSystem_getPrefix,None,MarSystem)
MarSystem.setControl = new_instancemethod(_marsyas_python.MarSystem_setControl,None,MarSystem)
MarSystem.updControl = new_instancemethod(_marsyas_python.MarSystem_updControl,None,MarSystem)
MarSystem.hasControl = new_instancemethod(_marsyas_python.MarSystem_hasControl,None,MarSystem)
MarSystem.linkControl = new_instancemethod(_marsyas_python.MarSystem_linkControl,None,MarSystem)
MarSystem.getControls = new_instancemethod(_marsyas_python.MarSystem_getControls,None,MarSystem)
MarSystem.getControl = new_instancemethod(_marsyas_python.MarSystem_getControl,None,MarSystem)
MarSystem_swigregister = _marsyas_python.MarSystem_swigregister
MarSystem_swigregister(MarSystem)

class MarSystemManager(object):
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args): 
        _marsyas_python.MarSystemManager_swiginit(self,_marsyas_python.new_MarSystemManager(*args))
    __swig_destroy__ = _marsyas_python.delete_MarSystemManager
MarSystemManager.create = new_instancemethod(_marsyas_python.MarSystemManager_create,None,MarSystemManager)
MarSystemManager.registeredPrototypes = new_instancemethod(_marsyas_python.MarSystemManager_registeredPrototypes,None,MarSystemManager)
MarSystemManager_swigregister = _marsyas_python.MarSystemManager_swigregister
MarSystemManager_swigregister(MarSystemManager)



