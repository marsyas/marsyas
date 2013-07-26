#include "controls_widget.h"

#include <MarControl.h>

#include <QDebug>
#include <QVBoxLayout>

using namespace Marsyas;

typedef std::map<std::string, MarControlPtr> control_map_t;

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

ControlsWidget::ControlsWidget( QWidget * parent):
  QWidget(parent),
  m_system(0)
{
  m_tree = new QTreeWidget;
  m_tree->setHeaderLabels( QStringList() << "Name" << "Type" << "Value" );

  m_label = new QLineEdit;
  //m_label->setFrameStyle( Qt::NoFrame );
  m_label->setReadOnly(true);
  {
    QFont f = m_label->font();
    f.setPointSize( f.pointSize() + 1 );
    m_label->setFont(f);
  }

  QVBoxLayout *column = new QVBoxLayout;
  column->addWidget(m_label);
  column->addWidget(m_tree);

  setLayout(column);

  connect(m_tree, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
          this, SLOT(onItemClicked(QTreeWidgetItem*,int)));
}

void ControlsWidget::setSystem( Marsyas::MarSystem * system )
{
  if (m_system == system)
    return;

  m_system = system;

  if (m_system)
    m_label->setText( QString::fromStdString(m_system->getAbsPath()) );
  else
    m_label->clear();

  rebuild();
}

void ControlsWidget::rebuild()
{
  m_tree->clear();

  if (!m_system)
    return;

  control_map_t controls = m_system->controls();
  control_map_t::iterator it;
  for (it = controls.begin(); it != controls.end(); ++it)
  {
    QString path = QString::fromStdString( it->first );
    QStringList path_components = path.split('/');
    if (path_components.size() < 2) {
      qWarning() << "Anomalous control path:" << path;
      continue;
    }
    QString type = path_components[0];
    QString name = path_components[1];

    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setData(NameColumn, Qt::DisplayRole, name);
    item->setData(TypeColumn, Qt::DisplayRole, type);
    item->setData(PathColumn, Qt::DisplayRole, path);

    m_tree->addTopLevelItem(item);
  }

  refresh();
}

void ControlsWidget::refresh()
{
  if (!m_system)
    return;

  int count = m_tree->topLevelItemCount();
  for (int item_idx = 0; item_idx < count; ++item_idx)
  {
    QTreeWidgetItem *item = m_tree->topLevelItem(item_idx);
    QString path = item->data( PathColumn, Qt::DisplayRole ).toString();
    MarControlPtr control = m_system->getControl( path.toStdString() );
    if (control.isInvalid()) {
      qWarning() << "Control path invalid:" << path;
      continue;
    }
    item->setData(ValueColumn, Qt::DisplayRole, variantFromControl(control));
  }
}

void ControlsWidget::onItemClicked( QTreeWidgetItem *item, int )
{
  QString path = item->data( PathColumn, Qt::DisplayRole ).toString();
  emit controlClicked(path);
}
