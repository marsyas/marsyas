#include <marsyas/export.h>
#include <string>

namespace Marsyas {

class marsyas_EXPORT ScriptManager
{
public:
  static void add(const std::string & path, const std::string & script);
  static void remove(const std::string & path);
  static const std::string & get(const std::string & path);
};

} // namespace Marsyas
