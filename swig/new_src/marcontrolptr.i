%{
#include <marsyas/MarControl.h>

using namespace Marsyas;

%}

%include "types.i"

/* Class description for MarControlPtrs */
class MarControlPtr {
        public: /* Constructors */
                MarControlPtr();
                MarControlPtr( const MarControlPtr &a );
                MarControlPtr( mrs_natural );
                MarControlPtr( mrs_real );
                MarControlPtr( mrs_string );
                MarControlPtr( mrs_bool );

                ~MarControlPtr();

                bool isInvalid() const ;
};

%extend MarControlPtr {
                /* Querying methods */
                mrs_string getType () { return (*self)->getType(); }
                mrs_string getName () { return (*self)->getName(); }

                /* Assignment methods */
                mrs_bool setValue ( mrs_natural x ) { return (*self)->setValue(x); }
                mrs_bool setValue ( mrs_real    x ) { return (*self)->setValue(x); }
                mrs_bool setValue ( mrs_bool    x ) { return (*self)->setValue(x); }
                mrs_bool setValue ( mrs_string  x ) { return (*self)->setValue(x); }

                mrs_natural   to_int () { return (*self)->to<mrs_natural>(); }
                mrs_real     to_real () { return (*self)->to<mrs_real>(); }
                mrs_string to_string () { return (*self)->to<mrs_string>(); }
                mrs_bool     to_bool () { return (*self)->to<mrs_bool>(); }
};

