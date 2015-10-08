#include "LibsndfileSource.h"

namespace Marsyas {

LibsndfileSource::LibsndfileSource(const string & name):
  MarSystem("LibsndfileSource", name),
  m_file(0)
{
  addControl("mrs_string/filename", string(), m_filenameControl);
  m_filenameControl->setState(true);

  addControl("mrs_bool/hasData", false, m_hasDataControl);
}

LibsndfileSource::LibsndfileSource(const LibsndfileSource & other):
  MarSystem(other),
  m_file(0)
{
  m_filenameControl = control("filename");
  m_hasDataControl = control("hasData");
}

void LibsndfileSource::myUpdate(MarControlPtr)
{
  {
    string filename = m_filenameControl->to<string>();
    if (filename != m_filename)
    {
      close();
      bool has_data = false;
      if (!filename.empty())
      {
        has_data = open(filename);
      }
      m_hasDataControl->setValue(has_data);
    }
  }

  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  if (m_file)
  {
    ctrl_onObservations_->setValue(m_fileInfo.channels, NOUPDATE);
    ctrl_osrate_->setValue((mrs_real) m_fileInfo.samplerate, NOUPDATE);
  }
  else
  {
    ctrl_onObservations_->setValue(0, NOUPDATE);
    ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  }
}

void LibsndfileSource::myProcess(realvec&, realvec& out)
{
  if (!m_file || !m_hasDataControl->to<bool>())
  {
    out.setval(0);
    return;
  }

  assert(out.getRows() == m_fileInfo.channels);

  int outputFrameCount = 0;

  if (sizeof(mrs_real) == 8)
  {
    outputFrameCount = sf_readf_double(m_file, out.getData(), out.getCols());
  }
  // FIXME: This should be conditionally compiled
  // based on how mrs_real is defined
#if 0
  else if (sizeof(mrs_real) == 4)
  {
    outputFrameCount = sf_readf_float(m_file, out.getData(), out.getCols());
  }
#endif

  if (outputFrameCount < out.getCols())
  {
    for (int r = 0; r < out.getRows(); ++r)
    {
      for (int c = outputFrameCount; c < out.getCols(); ++c)
      {
        out(r,c) = 0;
      }
    }

    m_hasDataControl->setValue(false);
  }
}

void LibsndfileSource::close()
{
  if (m_file)
    sf_close(m_file);
  m_file = 0;

  m_filename.clear();
}

bool LibsndfileSource::open(const string & filename)
{
  assert(!m_file);

  m_filename = filename;

  m_fileInfo.format = 0;
  m_file = sf_open(filename.c_str(), SFM_READ, &m_fileInfo);

  if (!m_file)
  {
    MRSERR("Error opening file: " << filename);
    return false;
  }

  return true;
}

}
