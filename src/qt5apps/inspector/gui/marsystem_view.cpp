#include "marsystem_view.h"

#include <QDebug>
#include <iostream>

using namespace Marsyas;
using namespace std;

typedef std::map<std::string, MarControlPtr> control_map_t;
typedef std::pair<std::string, MarControlPtr> control_mapping_t;

void MarSystemItem::setSystem( MarSystem * system )
{
  m_system = system;

  try_recreate();
}

void MarSystemItem::setDelegate( QQmlComponent * delegate )
{
  if (m_delegate)
  {
    m_delegate->disconnect(this);
  }

  m_delegate = delegate;

  connect(m_delegate, SIGNAL(statusChanged(QQmlComponent::Status)),
          this, SLOT(delegateStatusChanged(QQmlComponent::Status)));

  try_recreate();
}

void MarSystemItem::delegateStatusChanged(QQmlComponent::Status status)
{
  if (status == QQmlComponent::Ready)
    try_recreate();
}

void MarSystemItem::try_recreate()
{
  if (!m_system || !m_delegate || m_delegate->status() != QQmlComponent::Ready)
    return;

  recreate(m_system, m_delegate);
}

//////////////////////////

static QVariant variantFromControl ( const MarControlPtr & control )
{
  QVariant value;

  std::string type = control->getType();
  if (type == "mrs_real")
    value = QString::number( control->to<mrs_real>() );
  else if (type == "mrs_natural")
    value = QString::number( control->to<mrs_natural>() );
  else if (type == "mrs_bool")
    value = QVariant( control->to<mrs_bool>() ).convert(QVariant::String);
  else if (type == "mrs_string")
    value = QString::fromStdString(control->to<mrs_string>());
  else if (type == "mrs_realvec")
    value = QString("[...]");
  else
    value = QString("<unknown>");

  return value;
}

void MarSystemView::recreate(Marsyas::MarSystem * system, QQmlComponent * delegate)
{
  clear();

  createItem( system, delegate, this );
}

void MarSystemView::createItem( MarSystem *system, QQmlComponent * delegate, QQuickItem * parent )
{

  int level = -1;
  MarSystem *s = system;
  while(s) {
    s = s->getParent();
    ++level;
  }

  QQmlPropertyMap *data = new QQmlPropertyMap;
  data->insert("level",  level);

  // Get controls

  QQmlPropertyMap *defaultControls = new QQmlPropertyMap;
  defaultControls->insert("inSamples", variantFromControl(system->getControl("mrs_natural/inSamples")));
  defaultControls->insert("onSamples", variantFromControl(system->getControl("mrs_natural/onSamples")));
  defaultControls->insert("inObservations", variantFromControl(system->getControl("mrs_natural/inObservations")));
  defaultControls->insert("onObservations", variantFromControl(system->getControl("mrs_natural/onObservations")));
  defaultControls->insert("israte", variantFromControl(system->getControl("mrs_real/israte")));
  defaultControls->insert("osrate", variantFromControl(system->getControl("mrs_real/osrate")));
  defaultControls->insert("inStabilizingDelay", variantFromControl(system->getControl("mrs_natural/inStabilizingDelay")));
  defaultControls->insert("onStabilizingDelay", variantFromControl(system->getControl("mrs_natural/onStabilizingDelay")));
  defaultControls->insert("inObsNames", variantFromControl(system->getControl("mrs_string/inObsNames")));
  defaultControls->insert("onObsNames", variantFromControl(system->getControl("mrs_string/onObsNames")));
  defaultControls->insert("active", variantFromControl(system->getControl("mrs_bool/active")));
  defaultControls->insert("mute", variantFromControl(system->getControl("mrs_bool/mute")));
  defaultControls->insert("debug", variantFromControl(system->getControl("mrs_bool/debug")));
  defaultControls->insert("verbose", variantFromControl(system->getControl("mrs_bool/verbose")));
  defaultControls->insert("processedData", variantFromControl(system->getControl("mrs_realvec/processedData")));

  QQmlPropertyMap *customControls = new QQmlPropertyMap;

  control_map_t controls = system->controls();
  control_map_t::iterator it;
  for (it = controls.begin(); it != controls.end(); ++it)
  {
    const MarControlPtr & control = it->second;

    QString name = QString::fromStdString( control->getName() );
    name = name.split('/').last();

    if (defaultControls->contains(name))
      continue;

    customControls->insert(name, variantFromControl(control));
  }

  data->insert("defaultControls", QVariant::fromValue<QObject*>(defaultControls));
  data->insert("controls", QVariant::fromValue<QObject*>(customControls));
  data->insert("absolutePath", QString::fromStdString(system->getAbsPath()));
  data->insert("path", QString::fromStdString(system->getPrefix()));

  // Set context

  QQmlContext *context = new QQmlContext( delegate->creationContext() );
  context->setContextProperty("system", data);

  QObject *object = delegate->create(context);
  object->setParent(parent);
  m_items.append(object);

  QQuickItem *item = qobject_cast<QQuickItem*>(object);
  if (!item)
    return;

  QString path = QString::fromStdString(system->getPrefix());
  std::vector<MarSystem*> children = system->getChildren();
  int children_count = children.size();
  bool has_children = children_count > 0;

  bool create_if_not_existent = true;
  MarSystemViewAttached *attached =
      qobject_cast<MarSystemViewAttached*>(qmlAttachedPropertiesObject<MarSystemView>(item, create_if_not_existent) );

  Q_ASSERT(attached);

  attached->setPath(path);
  attached->setHasChildren(has_children);
  attached->setSystem(system);

  QQuickItem *children_area = attached->childrenArea();
  if (!children_area)
    children_area = item;

  for (int child_idx = 0; child_idx < children_count; ++child_idx)
  {
    MarSystem *child_system = children[child_idx];
    createItem(child_system, delegate, children_area);
  }

  item->setParentItem(parent);
}

void MarSystemView::clear()
{
  foreach(QObject * item, m_items)
    delete item;
  m_items.clear();
}

MarSystemViewAttached *MarSystemView::qmlAttachedProperties(QObject *object)
{
  //qDebug("creating attached property");
  return new MarSystemViewAttached(object);
}


void MarSystemViewAttached::setSystem( MarSystem * system )
{
  m_system = system;

  if (m_controls) {
    delete m_controls;
    m_controls = new QQmlPropertyMap(this);
  }

  control_map_t controls = system->controls();
  control_map_t::iterator it;
  for (it = controls.begin(); it != controls.end(); ++it)
  {
    const MarControlPtr & control = it->second;

    QString name = QString::fromStdString( control->getName() );
    name = name.split('/').last();

    QVariant value;
    std::string type = control->getType();
    if (type == "mrs_real")
      value = QString::number( control->to<mrs_real>() );
    else if (type == "mrs_natural")
      value = QString::number( control->to<mrs_natural>() );
    else if (type == "mrs_bool")
      value = QVariant( control->to<mrs_bool>() ).convert(QVariant::String);
    else if (type == "mrs_string")
      value = QString::fromStdString(control->to<mrs_string>());
    else
      value = QString("<undefined>");

    m_controls->insert(name, value);
  }

  emit systemChanged();
  emit controlsChanged();
}

////////////////////////////////

void MarSystemControlView::recreate(Marsyas::MarSystem * system, QQmlComponent * delegate)
{
  foreach(QObject * item, m_items)
    delete item;
  m_items.clear();

  control_map_t controls = system->controls();
  control_map_t::iterator it;
  for (it = controls.begin(); it != controls.end(); ++it)
  {
    cout << "a control: " << it->first << endl;

    const MarControlPtr & control = it->second;

    QString name = QString::fromStdString( control->getName() );
    QVariant value;

    std::string type = control->getType();
    if (type == "mrs_real")
      value = QString::number( control->to<mrs_real>() );
    else if (type == "mrs_natural")
      value = QString::number( control->to<mrs_natural>() );
    else if (type == "mrs_bool")
      value = QVariant( control->to<mrs_bool>() ).convert(QVariant::String);
    else if (type == "mrs_string")
      value = QString::fromStdString(control->to<mrs_string>());
    else
      value = QString("<undefined>");

    //value.convert(QVariant::String);

    QQmlPropertyMap *data = new QQmlPropertyMap;
    data->insert("name",  name);
    data->insert("value", value);

    QQmlContext *context = new QQmlContext( delegate->creationContext() );
    context->setContextProperty("control", data);

    QObject * object = delegate->create(context);
    context->setParent(object);
    m_items.append(object);

    QQuickItem *item = qobject_cast<QQuickItem*>(object);
    if (!item)
      return;

    item->setParentItem( this->parentItem() );
  }
}

