#ifndef MARSYASQT_REALVEC_ITEM_MODEL_INCLUDED
#define MARSYASQT_REALVEC_ITEM_MODEL_INCLUDED

#include "marsyasqt_common.h"
#include <marsyas/realvec.h>
#include <QAbstractTableModel>

namespace MarsyasQt {

class RealvecModel : public QAbstractTableModel
{
public:
  RealvecModel( QObject * parent = 0 ): QAbstractTableModel(parent) {}
  void setData( const Marsyas::realvec & data );
  const Marsyas::realvec & data() const { return m_data; }
  void setEditable(bool editable);

  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  Qt::ItemFlags flags(const QModelIndex & index) const;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
  QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

private:
  Marsyas::realvec m_data;
  bool m_editable;
};

} // namespace MarsyasQt

#endif // MARSYASQT_REALVEC_ITEM_MODEL_INCLUDED
