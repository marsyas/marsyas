#include "controls_widget.h"
#include "../../common/control_model.h"
#include <marsyas/system/MarControl.h>

#include <QDebug>
#include <QVBoxLayout>

using namespace Marsyas;
using namespace MarsyasQt;

ControlsWidget::ControlsWidget( QWidget * parent):
  QWidget(parent),
  m_system(0)
{
  m_model = new ControlModel(this);

  m_tree = new QTreeView;
  m_tree->setRootIsDecorated(false);
  m_tree->setModel(m_model);

  QVBoxLayout *column = new QVBoxLayout;
  column->setContentsMargins(0,0,0,0);
  column->setSpacing(0);
  column->addWidget(m_tree);

  setLayout(column);

  connect(m_tree, SIGNAL(clicked(const QModelIndex &)),
          this, SLOT(onItemClicked(const QModelIndex &)));
}

void ControlsWidget::setSystem( Marsyas::MarSystem * system )
{
  if (m_system == system)
    return;

  m_system = system;

  m_model->setSystem(system);

  if (m_system)
    emit pathChanged( QString::fromStdString(m_system->path()) );
  else
    emit pathChanged( QString() );
}

void ControlsWidget::refresh()
{
  m_model->refresh();
}

void ControlsWidget::onItemClicked(const QModelIndex & index)
{
  MarControlPtr control = m_model->controlAt(index);
  if (!control.isInvalid())
  {
    QString path = QString::fromStdString( control->getName() );
    emit controlClicked(path);
  }
}
