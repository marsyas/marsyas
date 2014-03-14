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

class ScriptTranslator
{
public:
  ScriptTranslator( MarSystemManager * manager = 0 );
  ~ScriptTranslator();
  MarSystem *translateFile(const std::string & filename);
  MarSystem *translateStream(std::istream & script_stream,
                             const std::string & working_directory = std::string());
  MarSystem *translateRegistered(const std::string & path);

private:
  MarSystemManager *m_manager;
  const bool m_own_manager;
};

} // namespace Marsyas

#endif // MARSYAS_SCRIPT_INCLUDED
