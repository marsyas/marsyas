#include "manager.h"
#include "../common_source.h"

#include <map>
#include <vector>
#include <stack>
#include <string>

using namespace std;

namespace Marsyas {

map<string,string> g_scripts;

string regular_path(const std::string & path)
{
  vector<string> components;

  string::size_type pos = 0;

  while(pos < path.size())
  {
    string::size_type next_pos = path.find('/', pos);

    string component = path.substr(pos, next_pos - pos);
    if (component == "..")
    {
      if (!components.empty())
        components.pop_back();
      else
      {
        MRSWARN("ScriptManager: Invalid path: " << path);
        return string();
      }
    }
    else
    {
      components.push_back(component);
    }

    pos = next_pos;
  }

  string out_path;

  if (!components.empty())
  {
    out_path = components[0];
    for (size_t i = 1; i < components.size(); ++i)
    {
      out_path += '/';
      out_path += components[i];
    }
  }

  return out_path;
}

void ScriptManager::add(const std::string & path, const std::string & script)
{
  string reg_path = regular_path(path);
  g_scripts[reg_path] = script;
}

void ScriptManager::remove(const std::string & path)
{
  string reg_path = regular_path(path);
  g_scripts.erase(reg_path);
}

const std::string & ScriptManager::get(const std::string & path)
{
  string reg_path = regular_path(path);
  return g_scripts.at(reg_path);
}

} // namespace Marsyas
