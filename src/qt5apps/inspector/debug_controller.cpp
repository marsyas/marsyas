#include "debug_controller.h"

#include <QDebug>

using namespace Marsyas;

DebugController::DebugController( QObject * parent ):
  QObject(parent),
  m_system(0),
  m_reader(0),
  m_recorder(0)
{}

void DebugController::setSystem( Marsyas::MarSystem * system )
{
  if (m_system == system)
    return;

  delete m_recorder;
  m_recorder = 0;

  m_system = system;

  if (m_system)
    m_recorder = new Debug::Recorder(m_system);
}

bool DebugController::setRecording(const QString &filename)
{
  if (filename.isEmpty())
    return false;

  Debug::FileReader *reader = new Debug::FileReader(filename.toStdString());
  if (!reader->isOpen()) {
    qWarning() << "*** Recording file could not be opened:" << filename;
    delete reader;
    return false;
  }

  delete m_reader;
  m_reader = reader;

  emit recordingChanged(filename);

  return true;
}

void DebugController::tick()
{
  m_report.clear();

  if (m_system)
  {
    Q_ASSERT(m_recorder);
    m_recorder->clear();

    m_system->tick();

    if (m_reader && !m_reader->eof())
    {
      Marsyas::Debug::Record file_record;
      bool ok = m_reader->read( file_record );
      if (ok)
        Debug::compare(m_recorder->record(), file_record, m_report);
    }
  }
  emit ticked();
}

void DebugController::rewind()
{
  if (m_reader)
    m_reader->rewind();
  if (m_recorder)
    m_recorder->clear();
  m_report.clear();
}

const realvec *DebugController::currentValue( const QString & path ) const
{
  if (!m_recorder)
    return 0;

  const auto & entry = m_recorder->record().entries().find(path.toStdString());
  if (entry == m_recorder->record().entries().end())
    return 0;

  return &entry->second.output;
}
