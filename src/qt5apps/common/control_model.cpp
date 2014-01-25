#include "control_model.h"

#include <QDebug>

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
    return ColumnCount;
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

Qt::ItemFlags ControlModel::flags(const QModelIndex & index) const
{
  if (index == QModelIndex())
    return Qt::NoItemFlags;

  if (index.column() < 0 || index.column() >= ColumnCount ||
      index.row() < 0 || index.row() >= m_items.size())
    return Qt::NoItemFlags;

  Qt::ItemFlags flags;
  flags |= Qt::ItemNeverHasChildren | Qt::ItemIsSelectable | Qt::ItemIsEnabled;

  if (index.column() == 1)
  {
    const MarControlPtr & control = m_items[index.row()].control;
    if (!control->hasType<mrs_realvec>())
    {
      flags |= Qt::ItemIsEditable;
    }
  }

  return flags;
}

QVariant ControlModel::data(const QModelIndex & index, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  if (index.column() < 0 || index.column() >= ColumnCount ||
      index.row() < 0 || index.row() >= m_items.size())
    return QVariant();

  const Item & item = m_items[index.row()];

  switch(index.column())
  {
  case Name:
    return QString::fromStdString( item.control->id() );
  case Value:
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
  case Type:
    return QString::fromStdString( item.control->getType() );
  case Access:
    return item.control->isPublic();
  default:
    return QVariant();
  }
}

bool ControlModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (role != Qt::EditRole)
    return false;

  if (index == QModelIndex())
    return false;

  if (index.column() != 1 ||
      index.row() < 0 || index.row() >= m_items.size())
    return false;

  Item &item = m_items[index.row()];
  MarControlPtr & control = item.control;

  if (control->hasType<bool>())
    control->setValue( value.value<bool>() );
  else if (control->hasType<mrs_natural>())
    control->setValue( value.value<mrs_natural>() );
  else if (control->hasType<mrs_real>())
    control->setValue( value.value<mrs_real>() );
  else if (control->hasType<mrs_string>())
    control->setValue( value.value<QString>().toStdString() );
  else
    return false;

  // For now, refresh all fields;
  // we don't know which related controls might also change.
  refresh();
  //emit dataChanged( index, index, QVector<int>() << Qt::DisplayRole );

  return true;
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
