%{
#include <marsyas/script/script.h>

using namespace Marsyas;

%}


MarSystem *system_from_script(std::istream & script_stream,
                              const std::string & working_directory = std::string(),
                              MarSystemManager *mng = 0);

MarSystem *system_from_script(const std::string & script,
                              const std::string & working_directory = std::string(),
                              MarSystemManager *mng = 0);

MarSystem *system_from_script_file(const std::string & filename, MarSystemManager *mng = 0);






