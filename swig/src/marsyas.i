/* Define the default name for the marsyas package,
 * can be overridden in language-specific file
 * 
 * Only contains classes which stay arround for a 
 * significant ammount of time:
 *	- MarSystemManager
 *	- MarSystem
 *
 * Other classes, which are meant realy to hold a
 * value should be defined in a target language
 * specific manner. Currently these classes are:
 *	- string 			[I/O]
 *	- MarControlValue		[I/O]
 *	- vector<string>		[O]
 *	- map<string,MarControlValue>	[O]
 *	- Repeat			[I]
 *
 * These will probably be defined using SWIG typemaps.
 */

/* Headers for classes used here */
%{
#undef HAVE_CONFIG_H
#include <marsyas/MarSystemManager.h>
#include <marsyas/MarControl.h>
#include <marsyas/MarSystem.h>
#include <marsyas/Repeat.h>
#include <vector>
#include <string>
#include <map>

using namespace Marsyas;
using namespace std;

%}

#ifdef SWIGRUBY
%include "ruby.i"
#endif
#ifdef SWIGPYTHON
%include "python.i"
#endif

/* Class description for MarSystems */
class MarSystem {
        protected:
                MarSystem(); // Hide constructor
        public:
                void tick (); // Perform one processing step
                void update (); // Update system w.r.t controls
                
                void addMarSystem (MarSystem *msys); // attach a dependant MarSystem

                /* Querying methods */
                string getType ();
                string getName ();
                string getPrefix ();

                /* Methods for dealing with controls */
                void            setControl(string,MarControlPtr);
                void            updControl(string,MarControlPtr);
                
                /* MarControlPtr   getControl(string); */
                bool            hasControl(string);
                void            linkControl(string,string);

                /* Listing of all controls & current values */
                map<string,MarControlPtr> getControls();
};

%extend MarSystem {
	MarControlPtr getControl(string cname)
	{
		if (self->hasControl(cname))
			return self->getControl(cname);
		else
			return MarControlPtr("Invalid Control");
	}
}

class MarSystemManager {
        public:
                MarSystemManager(); // Enable Constructor

                /* Create a MarSystem of a given <type> having the given <name>. */
                MarSystem *create(string,string);

                /* Get a list of all prototypes in this MarSystemManager */
                vector<string> registeredPrototypes();
};


