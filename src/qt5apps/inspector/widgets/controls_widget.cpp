#include "controls_widget.h"
#include "../../common/control_model.h"
#include <marsyas/system/MarControl.h>

#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>

using namespace Marsyas;
using namespace MarsyasQt;

bool ControlFilterModel::filterAcceptsRow(int row,
                                          const QModelIndex & parent) const
{
  if (!QSortFilterProxyModel::filterAcceptsRow(row, parent))
    return false;

  QModelIndex is_public_index = sourceModel()->index(row, ControlModel::Access, parent);
  bool is_public = sourceModel()->data(is_public_index).toBool();
  return is_public || m_show_private;
}

ControlsWidget::ControlsWidget( QWidget * parent):
  QWidget(parent),
  m_system(0)
{
  m_model = new ControlModel(this);

  m_filter_model = new ControlFilterModel(this);
  m_filter_model->setFilterCaseSensitivity(Qt::CaseInsensitive);
  m_filter_model->setSourceModel(m_model);

  m_tree = new QTreeView;
  m_tree->setRootIsDecorated(false);
  m_tree->setEditTriggers( QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed );
  m_tree->setModel(m_filter_model);
  m_tree->setColumnHidden( ControlModel::Access, true );
  // NOTE: don't resize value column, as it may contain
  // ridiculously long strings.
  m_tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

  m_filter_edit = new QLineEdit;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
  m_filter_edit->setClearButtonEnabled(true);
#endif

  m_private_toggle = new QCheckBox("Private");

  QHBoxLayout *filter_layout = new QHBoxLayout;
  filter_layout->setSpacing(10);
  filter_layout->addWidget(new QLabel("Filter:"));
  filter_layout->addWidget(m_filter_edit);
  filter_layout->addWidget(m_private_toggle);

  QVBoxLayout *column = new QVBoxLayout;
  column->setContentsMargins(0,0,0,0);
  column->setSpacing(1);
  column->addLayout(filter_layout);
  column->addWidget(m_tree);

  setLayout(column);

  connect(m_tree, SIGNAL(clicked(const QModelIndex &)),
          this, SLOT(onItemClicked(const QModelIndex &)));
  connect(m_filter_edit, SIGNAL(textChanged(QString)),
          m_filter_model, SLOT(setFilterFixedString(QString)));
  connect(m_private_toggle, SIGNAL(toggled(bool)),
          m_filter_model, SLOT(setShowPrivate(bool)));
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
