#ifndef MARSYAS_DEBUGGER_FILE_IO_INCLUDED
#define MARSYAS_DEBUGGER_FILE_IO_INCLUDED

#include "recording.hpp"

#include <MarSystem.h>
#include <MarControl.h>

#include <fstream>
#include <iostream>
#include <cassert>

const char magic_id[] = "#marsystemrecording";

using namespace std;
using namespace Marsyas;

namespace Marsyas {

void copy_swap_bytes(char *destination, const char * source, int size );

template <typename T>
void to_bytes( const T & src, char dst_bytes[sizeof(T)] )
{
  const char *src_bytes = reinterpret_cast<const char*>(&src);
  copy_swap_bytes(dst_bytes, src_bytes, sizeof(T));
}

template <typename T>
void from_bytes( T & dst, const char src_bytes[sizeof(T)] )
{
  char *dst_bytes = reinterpret_cast<char*>(&dst);
  copy_swap_bytes(dst_bytes, src_bytes, sizeof(T));
}

}

class recording_writer
{
  MarSystem * m_system;
  const recording * m_recording;
  std::ofstream m_file;

public:
  recording_writer( MarSystem * system, const recording *rec ):
    m_system(system),
    m_recording(rec)
  {}

  void write( const std::string & filename )
  {
    m_file.open(filename.c_str(),
                std::ios_base::out | std::ios_base::binary);

    write_magic();
    write_header();
    write_recording();

    m_file.close();
  }

private:

  void write_magic()
  {
    m_file << magic_id << std::endl;
  }

  void write_header()
  {
    std::ostringstream header;
    for (const std::string & path : m_recording->paths)
    {
      std::string output_path = path;
      output_path += "mrs_realvec/processedData";
      MarControlPtr output_control = m_system->getControl(output_path);
      assert(!output_control.isInvalid());
      const realvec & data = output_control->to<mrs_realvec>();
      header << path << ' ' << data.getCols() << ' ' << data.getRows() << std::endl;
    }

    m_file << header.str() << "." << endl;
  }

  void write_recording()
  {
    for (const record *rec : m_recording->records)
      write_record(rec);
  }

  void write_record( const record * rec )
  {
    for (const realvec & data : rec->entries)
    {
      for (int i = 0; i < data.getSize(); ++i)
      {
        char bytes[sizeof(mrs_real)];
        to_bytes<mrs_real>( data(i), bytes );
        m_file.write( bytes, sizeof(mrs_real) );
      }
    }
  }
};

class recording_reader
{
  recording * m_recording;
  std::ifstream m_file;
  struct realvec_format
  {
    realvec_format( int columns = 0, int rows = 0 ):
      columns(columns), rows(rows)
    {}
    int columns;
    int rows;
  };
  std::vector<realvec_format> m_formats;

public:
  recording_reader():
    m_recording(0)
  {}

  recording * read( const std::string & filename )
  {
    m_recording = new recording;

    m_file.open(filename,
                std::ios_base::in | std::ios_base::binary);

    if (!read_magic()) {
      cerr << "Wrong file type!" << endl;
      m_file.close();
      delete m_recording;
      return 0;
    }

    if (!read_header()) {
      cerr << "Error reading header." << endl;
      m_file.close();
      delete m_recording;
      return 0;
    }

    if (!read_data()) {
      cerr << "Error reading data." << endl;
      m_file.close();
      delete m_recording;
      return 0;
    }

    m_file.close();

    return m_recording;
  }

private:

  bool read_magic()
  {
    char *id = strdup(magic_id);
    m_file.read(id, strlen(id));
    if (m_file.fail())
      return false;
    id[m_file.gcount()] = 0;
    bool ok = strcmp(id, magic_id) == 0;
    free(id);
    return ok;
  }

  bool read_header()
  {
    while (!m_file.eof())
    {
      std::string path;
      int columns;
      int rows;
      m_file >> path;
      if (path == ".")
        break;

      m_file >> columns;
      if (m_file.fail()) {
        return false;
      }

      m_file >> rows;
      if (m_file.fail()) {
        return false;
      }

      //cout << "reader: got header entry: " << path << " " << columns << " " << rows << endl;

      m_recording->paths.push_back(path);
      m_formats.push_back( realvec_format(columns, rows) );
    }

    if (!m_formats.size())
    {
      cerr << "No header entries!" << endl;
      return false;
    }

    m_file.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );

    return true;
  }

  bool read_data()
  {
    int record_size = 0;
    for ( const realvec_format & format : m_formats )
      record_size += (format.columns * format.rows);

    if (!record_size) {
      cerr << "Formats amount to 0 total record size!" << endl;
      return false;
    }

    while (!m_file.eof())
    {
      record *rec = new record;
      for ( const realvec_format & format : m_formats )
      {
        realvec data(format.rows, format.columns);
        for (int i = 0; i < data.getSize(); ++i)
        {
          char bytes[sizeof(mrs_real)];
          m_file.read( bytes, sizeof(mrs_real) );
          from_bytes<mrs_real>( data.getData()[i], bytes );
        }
        if (m_file.fail())
          break;
        rec->entries.push_back(data);
      }
      if (rec->entries.size() == m_formats.size())
        m_recording->records.push_back(rec);
      else if (rec->entries.size()) {
        delete rec;
        return false;
      }
      else
        delete rec;
    }
    return true;
  }
};

#endif // MARSYAS_DEBUGGER_FILE_IO_INCLUDED
