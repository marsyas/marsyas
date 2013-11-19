#ifndef MARSYAS_SCRIPT_INCLUDED
#define MARSYAS_SCRIPT_INCLUDED

#include <marsyas/system/MarSystem.h>
#include <istream>

namespace Marsyas {

MarSystem *system_from_script(std::istream & script_stream);

} // namespace Marsyas

#endif // MARSYAS_SCRIPT_INCLUDED
