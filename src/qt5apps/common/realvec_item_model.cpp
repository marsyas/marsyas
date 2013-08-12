#include "realvec_item_model.h"

using namespace Marsyas;

namespace MarsyasQt {

void RealvecModel::setData( const Marsyas::realvec & data )
{
  bool same_dimensions = data.getRows() == m_data.getRows() && data.getCols() == m_data.getCols();
  bool is_empty = data.getRows() == 0 || data.getCols() == 0;

  if (same_dimensions) {
    if (!is_empty) {
      m_data = data;
      emit dataChanged( index(0,0), index(m_data.getRows()-1, m_data.getCols()-1) );
    }
  }
  else
  {
    beginResetModel();
    m_data = data;
    endResetModel();
  }
}

void RealvecModel::setEditable(bool editable)
{
  m_editable = editable;
}

int RealvecModel::rowCount(const QModelIndex & parent) const
{
  (void) parent;
  return m_data.getRows();
}

int RealvecModel::columnCount(const QModelIndex & parent) const
{
  (void) parent;
  return m_data.getCols();
}

Qt::ItemFlags RealvecModel::flags(const QModelIndex & index) const
{
  (void) index;
  Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  if (m_editable)
    flags |= Qt::ItemIsEditable;
  return flags;
}

QVariant RealvecModel::data(const QModelIndex & index, int role) const
{
  switch (role)
  {
  case Qt::DisplayRole:
    return QVariant( m_data(index.row(), index.column()) );
  default:
    return QVariant();
  }
}

bool RealvecModel::setData(const QModelIndex & index, const QVariant & value, int role )
{
  int row = index.row();
  int column = index.column();

  switch(role)
  {
  case Qt::EditRole:
  case Qt::DisplayRole:
  {
    if (!value.canConvert<mrs_real>())
      return false;
    m_data(row,column) = value.value<mrs_real>();
  }
  default:
    return false;
  }

  return true;
}

QVariant RealvecModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    switch(role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return QString::number(section);
    default:
        return QAbstractTableModel::headerData(section, orientation, role);
    }
}

} // namespace MarsyasQt
