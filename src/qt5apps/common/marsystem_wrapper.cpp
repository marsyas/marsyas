#include "marsystem_wrapper.h"

namespace MarsyasQt {

Control * System::control( const QString & path )
{
  Control *control = m_controls.value(path);
  if (!control)
    control = createControl(path);
  return control;
}

Control * System::createControl( const QString & path )
{
  Marsyas::RealTime::Control *thread_control = m_runner.control( path.toStdString() );
  if (!thread_control)
    return 0;

  Control *control;

  if ( thread_control->isValueType<bool>() )
    control = new ControlBool(thread_control, &m_controller, this);
  else if ( thread_control->isValueType<mrs_real>() )
    control = new ControlReal(thread_control, &m_controller, this);
  else if ( thread_control->isValueType<mrs_natural>() )
    control = new ControlNatural(thread_control, &m_controller, this);
  else if ( thread_control->isValueType<mrs_string>() )
    control = new ControlString(thread_control, &m_controller, this);
  else if ( thread_control->isValueType<mrs_realvec>() )
    control = new ControlRealvec(thread_control, &m_controller, this);
  else {
    MRSERR( "QtMarSystem: Can not access control - unsupported type." );
    return 0;
  }

  m_controls.insert(path, control);

  QObject::connect(
    control, SIGNAL(destroyed(QObject*)),
    this, SLOT(removeControl(QObject*))
  );

  return control;
}

void System::removeControl( QObject * object )
{
  Control *control = qobject_cast<Control*>(object);
  if (control) {
    QMap<QString, Control*>::iterator it = m_controls.find( control->path() );
    if (it != m_controls.end())
      m_controls.erase(it);
  }
}

#if 0
void QtMarSystem::set_control_value( MarControlPtr & control, const QVariant & value )
{
  switch(value.type())
  {
  case QVariant::Bool:
    control->setValue(value.toBool());
    break;
  case QVariant::Int:
    control->setValue((mrs_natural) value.toInt());
    break;
  case QVariant::Double:
    control->setValue((mrs_real) value.toDouble());
    break;
  case QVariant::String:
    control->setValue( value.toString().toStdString() );
    break;
  default:
    qWarning("QtMarSystem: can not set control value - QVariant of invalid type.");
  }
}
#endif

} // namespace MarsyasQt
