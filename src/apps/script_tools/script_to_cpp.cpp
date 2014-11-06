#include <marsyas/CommandLineOptions.h>
#include <marsyas/FileName.h>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>

using namespace Marsyas;
using namespace std;

void printUsage()
{
  cout << "Usage: marsyas-script-cpp [<option>...] "
       << "<output file> <input file> [<input file>...] "
       << endl;
}

string::size_type length(char) { return 1; }
string::size_type length(const string & s) { return s.length(); }

template <typename Replaced>
void replace( string & s, Replaced in, const string & out)
{
  string::size_type pos = 0;
  while(pos != string::npos)
  {
    pos = s.find(in, pos);
    if (pos != string::npos)
    {
      s.replace(pos, length(in), out);
      pos += out.length();
    }
  }
}

void replace( string & s, char in, char out )
{
  std::replace(s.begin(), s.end(), in, out);
}

struct emitter
{
  typedef vector<string>::const_iterator opt_iter;
  typedef pair<opt_iter, opt_iter> opt_range;
  const CommandLineOptions & opt;
  string register_path;
  string out_source_name;
  string out_header_name;
  string namespace_name;
  opt_range in_file_names;
  ofstream out;

  emitter(const CommandLineOptions & options): opt(options)
  {
    auto & other_opt = opt.getRemaining();

    if (other_opt.size() < 2)
    {
      printUsage();
      throw std::runtime_error("Wrong arguments.");
    }

    const string & out_file_name = other_opt[0];
    out_source_name = out_file_name + ".cpp";
    out_header_name = out_file_name + ".hpp";

    in_file_names.first = other_opt.begin() + 1;
    in_file_names.second = other_opt.end();

    namespace_name = opt.value<string>("namespace");

    register_path = opt.value<string>("path");
  }

  bool run()
  {
    if (!write_source())
      return false;
    if (!write_header())
      return false;
    return true;
  }

  bool write_source()
  {
    out.open(out_source_name.c_str());
    if (!out.is_open()) {
      cerr << "Could not open output file for writing!" << endl;
      return false;
    }

    source_includes();

    out << endl;

    bool ok = source_namespace();

    out.close();

    return ok;
  }

  void source_includes()
  {
    out << "#include \"" << out_header_name << "\"" << endl;
    out << "#include <marsyas/script/manager.h>" << endl;
  }

  bool source_namespace()
  {
    if (!namespace_name.empty())
    {
      out << "namespace " << namespace_name << " {" << endl;
      out << endl;
    }

    if (!source_function())
      return false;

    if (!namespace_name.empty())
    {
      out << endl;
      out << "} // namespace " << namespace_name << endl;
    }

    return true;
  }

  bool source_function()
  {
    out << "void registerScripts()" << endl;
    out << "{" << endl;
    out << endl;

    for (opt_iter it = in_file_names.first; it != in_file_names.second; ++it)
    {
      if (!source_script_def(*it))
        return false;
      out << endl;
    }

    out << "}" << endl;

    return true;
  }

  string make_script_name( const string & script_file_name )
  {
    string name(script_file_name);

    replace(name, '_', "__");
    replace(name, '.', '_');
    replace(name, '/', '_');

    return name;
  }

  string make_script_path(const string & script_file_name)
  {
    string path = register_path;
    if (!path.empty())
      path += '/';

    FileName info(script_file_name);
    path += info.name();

    return path;
  }

  bool source_script_def(const string & script_file_name)
  {
    ifstream in(script_file_name);
    if (!in.is_open())
    {
      cerr << "Failed to open input file for reading: " << script_file_name << endl;
      return false;
    }

    //string script_name = make_script_name(script_file_name);
    string path = make_script_path(script_file_name);

    out << "{" << endl;

    out << "const char * script = " << endl;
    while(in)
    {
      string line;
      getline(in, line);
      if (in)
      {
        replace(line, '\\', "\\\\");
        replace(line, '"', "\\\"");
        out << "\"" << line << "\\n\"" << endl;
      }
    }
    out << ";" << endl;

    out << "Marsyas::ScriptManager::add(\""
        << path << "\", script);" << endl;

    out << "}" << endl;

    in.close();

    return true;
  }

  bool write_header()
  {
    assert(!out.is_open());
    out.open( out_header_name.c_str() );
    if (!out.is_open()) {
      cerr << "Could not open output file for writing!" << endl;
      return false;
    }

    if (!namespace_name.empty())
    {
      out << "namespace " << namespace_name << " {" << endl;
      out << endl;
    }

    out << "void registerScripts();" << endl;

    if (!namespace_name.empty())
    {
      out << endl;
      out << "} // namespace " << namespace_name << endl;
    }

    out.close();

    return true;
  }
};

int main(int argc, const char **argv)
{
  CommandLineOptions opt;
  opt.define<string>("namespace", 'n', "name", "C++ namespace to contain produced code.");
  opt.define<string>("path", 'p', "path", "Register path to store scripts under");

  if (!opt.readOptions(argc, argv))
  {
    return 1;
  }

  bool ok;

  try {
    emitter e(opt);
    ok = e.run();
  }
  catch (...)
  {
    ok = false;
  }

  return ok ? 0 : 1;
}
