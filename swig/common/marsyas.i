/* Define the default name for the marsyas package,
 * can be overridden in language-specific file
 */
%module marsyas

/* Headers for classes used here */
%{
#include <marsyas/MarSystemManager.h>
#include <marsyas/MarSystem.h>
#include <vector>
#include <map>
%}

/* Basic mapping of input variables, treats all
 * string arguments as char *.
 */
%typemap(in) string = char *;

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
                void            setControl(string cname, MarControlValue value);
                MarControlValue getControl(string cname);
                bool            hasControl(string cname);
                void            updControl(string cname, MarControlValue value);
                
                /* Listing of all controls & current values */
                map<string,MarControlValue> getControls();
};

class MarSystemManager {
        public:
                MarSystemManager(); // Enable Constructor

                /* Create a MarSystem of a given <type> having the given <name>. */
                MarSystem *create(string type,string name);

                /* Get a list of all prototypes in this MarSystemManager */
                vector<string> registeredPrototypes();
};
