#ifndef MARSYAS_JSON_IO_INCLUDED
#define MARSYAS_JSON_IO_INCLUDED

#include <marsyas/system/MarSystemManager.h>
#include <marsyas/system/MarSystem.h>

namespace Marsyas
{

MarSystem *system_from_json_string( const std::string & json_string );
MarSystem *system_from_json_file( const std::string & json_file_name );

}

#endif // MARSYAS_JSON_IO_INCLUDED
