%{
#include <marsyas/system/MarSystem.h>

using namespace Marsyas;

%}

%include "std_map.i"
%include "std_string.i"
%include "types.i"

namespace std {
        %template(Map_String_MarControlPtr) map<string,MarControlPtr>;
}

/* Class description for MarSystems */
class MarSystem {
        protected:
                MarSystem(); // Hide constructor
        public:
                void tick (); // Perform one processing step
                void process(realvec& in, realvec& out);
                void update (); // Update system w.r.t controls

                /* attach a dependant MarSystem */
                void addMarSystem (MarSystem *msys);

                /* set the parent MarSystem */
                void setParent(const MarSystem* parent);

                /* fetch a dependant MarSystem, stripping the parent's prefixes */
                MarSystem* getChildMarSystem(std::string childPath);

                /* Querying methods */
                std::string getType ();
                std::string getName ();
                std::string getPrefix ();
                std::string toString ();
				std::string toStringShort ();

                /* Methods for dealing with controls */
                void            setControl(std::string, MarControlPtr);
                /* These methods should be defined in a language-specific way
                void            setControl(std::string,mrs_bool);
                void            setControl(std::string,mrs_natural);
                void            setControl(std::string,mrs_real);
                void            setControl(std::string,mrs_string);
                */

                void            updControl(std::string, MarControlPtr);
                /* These methods should be defined in a language-specific way
                void            updControl(std::string,mrs_bool);
                void            updControl(std::string,mrs_natural);
                void            updControl(std::string,mrs_real);
                void            updControl(std::string,mrs_string);
                */

                MarControlPtr   getControl(std::string);
                bool            hasControl(std::string);
                void            linkControl(std::string, std::string);

                /* Listing of all controls & current values */
                std::map<std::string,MarControlPtr> getControls();
};
