#ifndef MARSYAS_CSV_SINK_INCLUDED
#define MARSYAS_CSV_SINK_INCLUDED

#include <marsyas/system/MarSystem.h>
#include <string>
#include <fstream>

namespace Marsyas {
/**
 * @brief Outputs Comma-Separated Values (CSV) file.
 *
 * In the output file, lines represent samples and columns represent observations.
 *
 * \b Controls:
 * - \b mrs_string/filename: File to write to.
 * - \b mrs_string/separator: Value-separator string. Default: a single space.
 */

class CsvSink: public MarSystem
{
public:
  CsvSink(const std::string & name);
  CsvSink(const CsvSink & other);
  ~CsvSink();
  MarSystem *clone() const { return new CsvSink(*this); }

private:
  void myUpdate( MarControlPtr );
  void myProcess( realvec & in, realvec & out );
  std::string m_separator;
  std::string m_filename;
  std::ofstream m_file;
};

} // namespace Marsyas

#endif // MARSYAS_CSV_SINK_INCLUDED
