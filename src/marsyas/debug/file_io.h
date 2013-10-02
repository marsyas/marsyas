/*
** Copyright (C) 1998-2013 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef MARSYAS_DEBUG_FILE_IO_INCLUDED
#define MARSYAS_DEBUG_FILE_IO_INCLUDED

#include <marsyas/debug/record.h>
#include <marsyas/system/MarSystem.h>
#include <marsyas/export.h>

#include <fstream>

namespace Marsyas { namespace Debug {

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


class marsyas_EXPORT FileWriter
{
  struct SystemDescriptor
  {
    std::string path;
    int out_columns;
    int out_rows;
  };

  std::ofstream m_file;
  std::vector<SystemDescriptor> m_descriptors;

public:
  FileWriter( const std::string & filename, MarSystem * system );
  ~FileWriter();
  bool isOpen() { return m_file.is_open(); }
  bool write( const Record & record )
  {
    return write_record(record);
  }

private:
  void recursive_store_descriptor( MarSystem * system );
  void write_magic();
  void write_header();
  bool write_record( const Record & record );
};


class marsyas_EXPORT FileReader
{
  struct SystemDescriptor
  {
    std::string path;
    int out_columns;
    int out_rows;
  };

  typedef std::ifstream::pos_type pos_t;

  std::ifstream m_file;
  std::vector<SystemDescriptor> m_descriptors;
  size_t m_record_size;
  pos_t m_start_pos;

public:
  FileReader( const std::string & filename );
  ~FileReader();
  bool isOpen() { return m_file.is_open(); }
  bool eof() { return m_file.eof(); }
  bool read( Record & record )
  {
    return read_record(record);
  }
  void rewind();

private:
  bool read_magic();
  bool read_header();
  bool read_record( Record & record );
};

}} // namespace Marsyas::Debug

#endif // MARSYAS_DEBUG_FILE_IO_INCLUDED
