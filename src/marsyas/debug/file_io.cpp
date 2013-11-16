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

#include <marsyas/debug/file_io.h>

static const char magic_id[] = "#marsystemrecording";

namespace Marsyas { namespace Debug {

void copy_swap_bytes(char *destination, const char * source, int size )
{
#ifdef MARSYAS_BIGENDIAN
  for (size_t i = 0; i < size; ++i)
    destination[i] = source[size - i - 1];
#else
  std::memcpy(destination, source, size);
#endif
}


FileWriter::FileWriter( const std::string & filename, MarSystem * system )
{
  recursive_store_descriptor(system);

  m_file.open(filename.c_str(),
              std::ios_base::out | std::ios_base::binary);
  // TODO: check for error.

  write_magic();
  write_header();
}

FileWriter::~FileWriter()
{
  m_file.close();
}

void FileWriter::recursive_store_descriptor( MarSystem * system )
{
  MarControlPtr out_samples_ctl = system->getControl("mrs_natural/onSamples");
  MarControlPtr out_observations_ctl = system->getControl("mrs_natural/onObservations");
  assert(!out_samples_ctl.isInvalid());
  assert(!out_observations_ctl.isInvalid());

  SystemDescriptor descriptor;
  descriptor.path = system->getAbsPath();
  descriptor.out_columns = out_samples_ctl->to<mrs_natural>();
  descriptor.out_rows = out_observations_ctl->to<mrs_natural>();

  m_descriptors.push_back(descriptor);

  std::vector<MarSystem*> children = system->getChildren();
  for (MarSystem *child : children)
    recursive_store_descriptor(child);
}

void FileWriter::write_magic()
{
  m_file << magic_id << std::endl;
}

void FileWriter::write_header()
{
  std::ostringstream header;
  for (const auto & descriptor : m_descriptors)
  {
    header << descriptor.path
           << ' ' << descriptor.out_columns
           << ' ' << descriptor.out_rows
           << std::endl;
  }

  m_file << header.str() << "." << std::endl;
}

bool FileWriter::write_record( const Record & record )
{
  std::vector<const realvec*> data;

  for (const auto & descriptor : m_descriptors)
  {
    const realvec * output;

    try
    {
      const Record::Entry & record_entry = record.entries().at(descriptor.path);
      output = &record_entry.output;
    }
    catch (const std::out_of_range &)
    {
      std::cerr << "Marsyas::Debug::FileWriter: Record has no entry for path: "
                << descriptor.path << std::endl;
      return false;
    }

    if (output->getRows() != descriptor.out_rows ||
        output->getCols() != descriptor.out_columns)
    {
      std::cerr << "Marsyas::Debug::FileWriter: Record entry format mismatch for path: "
                << descriptor.path << std::endl;
      return false;
    }

    data.push_back(output);
  }

  for (const realvec * d : data)
  {
    const realvec & vector = *d;
    for (int i = 0; i < vector.getSize(); ++i)
    {
      char bytes[sizeof(mrs_real)];
      to_bytes<mrs_real>( vector(i), bytes );
      m_file.write( bytes, sizeof(mrs_real) );
    }
  }

  return true;
}


FileReader::FileReader( const std::string & filename ):
  m_record_size(0)
{
  m_file.open(filename,
              std::ios_base::in | std::ios_base::binary);

  if (!read_magic()) {
    std::cerr << "Marsyas::Debug::FileReader: Wrong file type!" << std::endl;
    m_file.close();
    return;
  }

  if (!read_header()) {
    std::cerr << "Marsyas::Debug::FileReader: Error reading header!" << std::endl;
    m_file.close();
    return;
  }
}

FileReader::~FileReader()
{
  m_file.close();
}

void FileReader::rewind()
{
  if (m_start_pos == (pos_t)-1)
    return;

  // Before C++11, and always in GCC,
  // seekg() will not clear error flags (e.g. eof) on success.
  m_file.clear();
  m_file.seekg(m_start_pos);

  if (m_file.fail())
    std::cerr << "Marsyas::Debug::FileReader: Error rewinding!" << std::endl;
}


bool FileReader::read_magic()
{
  char id[sizeof magic_id];
  m_file.read(id, sizeof magic_id);
  if (m_file.fail())
    return false;
  id[m_file.gcount()] = 0;
  bool ok = strcmp(id, magic_id) == 0;
  return ok;
}

bool FileReader::read_header()
{
  while (!m_file.eof())
  {
    SystemDescriptor descriptor;

    m_file >> descriptor.path;
    if (descriptor.path == ".")
      break;

    m_file >> descriptor.out_columns;
    if (m_file.fail())
      return false;

    m_file >> descriptor.out_rows;
    if (m_file.fail())
      return false;

    m_descriptors.push_back(descriptor);

    m_record_size += descriptor.out_columns * descriptor.out_rows;
  }

  if (!m_descriptors.size())
  {
    std::cerr << "Marsyas::Debug::FileReader: No header entries!" << std::endl;
    return false;
  }

  m_file.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );

  m_start_pos = m_file.tellg();

  return true;
}

bool FileReader::read_record( Record & record )
{
  if (!m_record_size || m_start_pos == (pos_t)-1 || m_file.eof())
  {
    std::cerr << "Marsyas::Debug::FileReader: Nothing to read." << std::endl;
    return false;
  }

  for (const auto & descriptor : m_descriptors)
  {
    realvec data(descriptor.out_rows, descriptor.out_columns);

    for (int i = 0; i < data.getSize(); ++i)
    {
      char bytes[sizeof(mrs_real)];
      m_file.read( bytes, sizeof(mrs_real) );
      from_bytes<mrs_real>( data.getData()[i], bytes );
    }
    if (m_file.fail())
      return false;

    Record::Entry entry;
    entry.output = data;
    record.insert(descriptor.path, entry);
  }

  return true;
}

}}
