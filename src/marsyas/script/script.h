#ifndef MARSYAS_SCRIPT_INCLUDED
#define MARSYAS_SCRIPT_INCLUDED

#include <marsyas/system/MarSystem.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/export.h>
#include <istream>

namespace Marsyas {

marsyas_EXPORT
MarSystem *system_from_script(const std::string & filename, MarSystemManager *mng = 0);

marsyas_EXPORT
MarSystem *system_from_script(std::istream & script_stream,
                              const std::string & working_directory = std::string(),
                              MarSystemManager *mng = 0);

} // namespace Marsyas

#endif // MARSYAS_SCRIPT_INCLUDED
