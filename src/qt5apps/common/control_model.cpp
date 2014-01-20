#include "control_model.h"

namespace MarsyasQt {

int ControlModel::rowCount(const QModelIndex & parent) const
{
  if (parent == QModelIndex())
    return m_items.count();
  else
    return 0;
}

int ControlModel::columnCount(const QModelIndex & parent) const
{
  if (parent == QModelIndex())
    return 3;
  else
    return 0;
}

QModelIndex ControlModel::index(int row, int column, const QModelIndex & parent) const
{
  if (parent != QModelIndex())
    return QModelIndex();

  return createIndex(row, column);
}

QModelIndex ControlModel::parent(const QModelIndex &) const
{
  return QModelIndex();
}

QVariant ControlModel::data(const QModelIndex & index, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  if (index.column() < 0 || index.column() >= 3 ||
      index.row() < 0 || index.row() >= m_items.size())
    return QVariant();

  const Item & item = m_items[index.row()];

  switch(index.column())
  {
  case 0:
    return QString::fromStdString( item.control->id() );
  case 1:
    if (item.control->hasType<bool>())
    {
      bool value = item.control->to<bool>();
      return value ? QString("true") : QString("false");
    }
    else if (item.control->hasType<mrs_natural>())
      return QString::number( item.control->to<mrs_natural>() );
    else if (item.control->hasType<mrs_real>())
      return QString::number( item.control->to<mrs_real>() );
    else if (item.control->hasType<mrs_string>())
      return QString::fromStdString( item.control->to<mrs_string>() );
    else if (item.control->hasType<mrs_realvec>())
      return QString("[...]");
    else
      return QVariant();
  case 2:
    return QString::fromStdString( item.control->getType() );
  default:
    return QVariant();
  }
}

QVariant ControlModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation != Qt::Horizontal)
    return QVariant();

  if (role != Qt::DisplayRole)
    return QVariant();

  if (section < 0 || section >= columnCount())
    return QVariant();

  switch(section)
  {
  case 0:
    return QString(tr("Name"));
  case 1:
    return QString(tr("Value"));
  case 2:
    return QString(tr("Type"));
  default:
    return QVariant();
  }
}

void ControlModel::rebuild()
{
  beginResetModel();

  m_items.clear();

  if (m_system)
  {
    const std::map<std::string, MarControlPtr> & controls = m_system->controls();
    std::map<std::string, MarControlPtr>::const_iterator it;
    for( it = controls.begin(); it != controls.end(); ++it )
    {
      m_items.append( Item(it->second) );
    }
  }

  endResetModel();
}

void ControlModel::refresh()
{
  if (!rowCount())
    return;

  emit dataChanged( createIndex(0,0),
                    createIndex(rowCount()-1, columnCount()-1),
                    QVector<int>() << Qt::DisplayRole );
}

MarControlPtr ControlModel::controlAt( const QModelIndex & index )
{
  int row = index.row();

  if (row < 0 || row >= m_items.count())
    return MarControlPtr();

  return m_items[row].control;
}


} // namespace MarsyasQt
