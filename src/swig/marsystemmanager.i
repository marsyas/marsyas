
/* Headers for classes used here */
%{
#include <marsyas/system/MarSystemManager.h>

using namespace Marsyas;

%}

%include "std_vector.i"
%include "std_string.i"

namespace std {
        %template(Vector_String) vector<string>;
};

class MarSystemManager {
        public:
                MarSystemManager(); // Enable Constructor

                /* Create a MarSystem of a given <type> having the given <name>. */
                MarSystem *create(std::string,std::string);
		MarSystem *create(std::string);
		MarSystem *loadFromFile(std::string);
		void registerPrototype(std::string, MarSystem*);

                /* Get a list of all prototypes in this MarSystemManager */
                std::vector<std::string> registeredPrototypes();
                %extend {
                        /* Wrap the read() method, so that we can use it with
                           python string objects */
                        MarSystem* getMarSystem(std::string src, MarSystem *parent=NULL)
                        {
                           std::string skipstr;
                           std::stringstream s(src);
                           MarSystem* ms = self->getMarSystem((std::istream&)s, parent);
                           return ms;
                        }
                };
};


