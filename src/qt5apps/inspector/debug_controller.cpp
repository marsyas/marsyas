#include "debug_controller.h"

#include <apps/debugger/file_io.hpp>

#include <QDebug>

using namespace Marsyas;

DebugController::DebugController( QObject * parent ):
  QObject(parent),
  m_system(0),
  m_recorder(0),
  m_debugger(0),
  m_recording(0),
  m_bug_report(0)
{}

void DebugController::setSystem( Marsyas::MarSystem * system )
{
  if (m_system == system)
    return;

  if (m_system)
  {
    delete m_recorder;
    delete m_debugger;
    m_recorder = 0;
    m_debugger = 0;
  }

  m_system = system;

  if (m_system)
  {
    m_recorder = new recorder(m_system);
    if (m_recording)
      m_debugger = new debugger(m_system, m_recording);
  }
}

bool DebugController::setRecording(const QString &fileName)
{
  if (fileName.isEmpty())
    return false;

  recording_reader reader;
  recording *new_recording = reader.read(fileName.toStdString());

  if (!new_recording) {
    qWarning() << "*** Recording file could not be opened:" << fileName;
    return false;
  }

  if (m_debugger) {
    delete m_debugger;
    m_debugger = 0;
  }

  delete m_recording;
  m_recording = new_recording;

  if (m_system)
    m_debugger = new debugger(m_system, m_recording);

  emit recordingChanged(fileName);

  return true;
}

void DebugController::tick()
{
  delete m_bug_report;
  m_bug_report = 0;
  if (m_system)
  {
    Q_ASSERT(m_recorder);
    m_recorder->clear_record();
    m_system->tick();
    if (m_debugger)
    {
      record *state = m_recorder->current_record();
      m_bug_report = m_debugger->evaluate(state);
      delete state;
      m_debugger->advance();
    }
  }
  emit ticked();
}

void DebugController::rewind()
{
  if (m_debugger)
    m_debugger->rewind();
}

const realvec *DebugController::currentValue( const QString & path ) const
{
  return m_recorder ? m_recorder->current_value(path.toStdString()) : 0;
}
