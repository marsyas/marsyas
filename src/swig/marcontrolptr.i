%{
#include <marsyas/system/MarControl.h>

using namespace Marsyas;

%}

%include "types.i"

/* Class description for MarControlPtrs */
class MarControlPtr {
        public: /* Constructors */
                MarControlPtr();
                MarControlPtr( const MarControlPtr &a );

                ~MarControlPtr();

                bool isInvalid() const ;
};

%extend MarControlPtr {
                /* Extra static "constructors" to match to_* methods */
                static MarControlPtr from_natural ( mrs_natural x ) { return MarControlPtr(x); }
                static MarControlPtr from_real    ( mrs_real    x ) { return MarControlPtr(x); }
                static MarControlPtr from_bool    ( mrs_bool    x ) { return MarControlPtr(x); }
                static MarControlPtr from_string  ( mrs_string  x ) { return MarControlPtr(x); }
				static MarControlPtr from_realvec ( realvec 	x ) { return MarControlPtr(x); }

                /* Querying methods */
                mrs_string getType () { return (*self)->getType(); }
                mrs_string getName () { return (*self)->getName(); }

                /* Assignment methods */
                mrs_bool setValue_natural ( mrs_natural x ) { return (*self)->setValue(x); }
                mrs_bool setValue_real    ( mrs_real    x ) { return (*self)->setValue(x); }
                mrs_bool setValue_bool    ( mrs_bool    x ) { return (*self)->setValue(x); }
                mrs_bool setValue_string  ( mrs_string  x ) { return (*self)->setValue(x); }
				mrs_bool setValue_realvec ( realvec 	x ) { return (*self)->setValue(x); }
				
                mrs_natural   to_natural () { return (*self)->to_natural(); }
                mrs_real         to_real () { return (*self)->to_real(); }
                mrs_string     to_string () { return (*self)->to_string(); }
                mrs_bool         to_bool () { return (*self)->to_bool(); }
                realvec		  to_realvec () { return (*self)->to_realvec(); }
};

