%module marsyas


%{
#include <cstddef>

%}
%include "marsystem.i"
%include "marsystemmanager.i"
%include "marcontrolptr.i"
%include "realvec.i"
%include "Collection.i"

/* Exception handling: translate C++ exceptions to Python Exceptions.
 * see http://www.swig.org/Doc1.3/Python.html#Python_nn44 .
 */
%exception {
   try {
      $action
   } catch (std::out_of_range &e) {
      PyErr_SetString(PyExc_IndexError, const_cast<char*>(e.what()));
      return NULL;
   }
}

// Some Python specific extensions for realvec objects.
%extend realvec {

    // Simple string representation method.
    std::string __str__() {
        std::ostringstream oss;
        self->dumpDataOnly(oss);
        return oss.str();
    }

    // Print method for standard Marsyas representation.
    void print_() {
        std::cout << *self;
    }

    // Length method.
    mrs_natural __len__() {
        return self->getSize();
    }

    // __getitem__ method to get value from realvec.
    mrs_real __getitem__(mrs_natural i) {
        return self->getValueFenced(i);
    }

    // __setitem__ method to set value of realvec.
    void __setitem__(mrs_natural i, mrs_real x) {
        self->getValueFenced(i) = x;
    }

};

// Some Python specific extensions for MarSystem objects.
%extend MarSystem {

    // Unfortunately, bool overloading does not work (yet?). TODO
    // see https://sourceforge.net/tracker/?func=detail&aid=2801824&group_id=1645&atid=101645
    /*
    void updControl(std::string s, mrs_bool x) {
        self->updControl(s, MarControlPtr(x));
    }
    */
    void updControl(std::string s, mrs_natural x) {
        self->updControl(s, MarControlPtr(x));
    }
    void updControl(std::string s, mrs_real x) {
        self->updControl(s, MarControlPtr(x));
    }
    void updControl(std::string s, mrs_string x) {
        self->updControl(s, MarControlPtr(x));
    }

    // HTML representation method.
    std::string toHtml() {
        std::ostringstream oss;
        $self->put_html(oss);
        return oss.str();
    }
}

