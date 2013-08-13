#ifndef MARSYASQT_INSPECTOR_DEBUG_CONTROLLER_INCLUDED
#define MARSYASQT_INSPECTOR_DEBUG_CONTROLLER_INCLUDED

#include <QObject>

#include <apps/debugger/debugger.hpp>
#include <apps/debugger/recording.hpp>

class DebugController : public QObject
{
  Q_OBJECT
public:
  DebugController( QObject * parent = 0 );
  void setSystem( Marsyas::MarSystem * system );
  bool setRecording( const QString & fileName );
  const debugger::report *bugReport() const { return m_bug_report; }
  const realvec *currentValue( const QString & path ) const;

public slots:
  void tick();
  void rewind();

signals:
  void ticked();

private:
  Marsyas::MarSystem * m_system;
  recorder * m_recorder;
  debugger * m_debugger;
  recording * m_recording;
  debugger::report *m_bug_report;
};

#endif // MARSYASQT_INSPECTOR_DEBUG_CONTROLLER_INCLUDED
