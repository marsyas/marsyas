#ifndef MARSYAS_SCRIPT_INCLUDED
#define MARSYAS_SCRIPT_INCLUDED

#include <marsyas/system/MarSystem.h>
#include <marsyas/export.h>
#include <istream>

namespace Marsyas {

marsyas_EXPORT
MarSystem *system_from_script(const std::string & filename);

marsyas_EXPORT
MarSystem *system_from_script(std::istream & script_stream,
                              const std::string & working_directory = std::string());

} // namespace Marsyas

#endif // MARSYAS_SCRIPT_INCLUDED
