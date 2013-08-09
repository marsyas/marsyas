#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QWidget>
#include <QAbstractItemModel>

#include "../Music/MusicCollection.h"

class PlaylistItem;

class PlaylistModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  PlaylistModel(QObject *parent=0);
  ~PlaylistModel();

  QVariant data(const QModelIndex &index, int role) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const;
  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex &index) const;
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;

private:
  void setupModelData(const QStringList &lines, TreeItem *parent);
  PlaylistItem *_root;
  MusicCollection *_library;
};

class PlaylistItem
{
public:
  PlaylistItem(const QList<QVariant> &data, PlaylistItem *parent = 0);
  ~PlaylistItem();

  void appendChild(PlaylistItem *child);

  PlaylistItem *child(int row);
  int childCount() const;
  int columnCount() const;
  QVariant data(int column) const;
  int row() const;
  PlaylistItem *parent();

private:
  QList<PlaylistItem*> childItems;
  QList<QVariant> itemData;
  PlaylistItem *parentItem;
};

#endif /* PLAYLISTMODEL_H */

