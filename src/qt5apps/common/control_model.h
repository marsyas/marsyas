#ifndef MARSYASQT_CONTROL_MODEL_INCLUDED
#define MARSYASQT_CONTROL_MODEL_INCLUDED

#include <QAbstractItemModel>
#include <QList>

#include <marsyas/system/MarSystem.h>
#include <marsyas/system/MarControl.h>

namespace MarsyasQt {

using namespace Marsyas;

class ControlModel : public QAbstractItemModel
{
  Q_OBJECT

  struct Item
  {
    Item( const MarControlPtr & c ): control(c) {}
    MarControlPtr control;
  };

public:
  enum DataColumn
  {
    Name = 0,
    Value,
    Type,
    Access,

    ColumnCount
  };

  ControlModel( QObject * parent = 0 ):
    QAbstractItemModel(parent)
  {}

  void setSystem( MarSystem * system )
  {
    m_system = system;
    rebuild();
  }

  void refresh();

  MarControlPtr controlAt( const QModelIndex & index );

  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex & index) const;
  Qt::ItemFlags flags(const QModelIndex & index) const;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private:
  void rebuild();

  MarSystem * m_system;
  QList<Item> m_items;
};

} // namespace MarsyasQt

#endif // MARSYASQT_CONTROL_MODEL_INCLUDED
