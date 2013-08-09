#include "PlaylistModel.h"

PlaylistModel::PlaylistModel(QObject *parent)
  : QAbstractItemModel(parent)
{
  _library = MusicCollection::getInstance();
  initializeModel();
}

PlaylistMode::~PlaylistModel() {
  delete _root;
}


void initializeModel() {
  QList<QVarient> rootTitles;
  rootTitles << "Track" << "Artist";
  _root = new PlaylistItem(rootData);

  QList<PlayListItem*> parents;
  parents << rootTitles;

  MusicPlaylistIterator ip = _library.getPlaylists();
  while ( ip.hasNext() ) {
    MusicPlaylist *playlist = ip.next().value();

    MusicTrackIterator it = playlist->getTracks();
    while ( it.hasNext() ) {
      MusicTrack *track = it.next();




    }
    parents.pop_back();
  }
}

QModelIndex PlaylistModel::index(int row, int column, const QModelIndex &parent) const
{
  PlaylistItem *parentItem;

  if ( !parent.isValid() ) {
    parentItem = _root;
  } else {
    parentItem = static_cast<PlaylistItem*>(parent.internalPointer());
  }

  PlaylistItem *childItem = parentItem->child(row);
  if ( childItem ) {
    return createIndex(row, column, childItem);
  } else {
    return QModelIndex();
  }
}

QModelIndex PlaylistModel::parent(const QModelIndex &index) const {
  if ( !index.isValid() ) {
    return QModelIndex();
  }

  PlaylistItem *childItem = static_cast<PlaylistItem*>(index.internalPointer());
  PlaylistItem *parentItem = childItem->parent();

  if ( parentItem == _root ) {
    return QModelIndex();
  }

  return createIndex(parentItem->row(), 0, parentItem);
}

int PlaylistModel::rowCount(const QModelIndex &parent) const {

  PlaylistItem *parentItem;

  if ( !parent.isValid() ) {
    parentItem = _root;
  } else {
    parentItem = static_cast<PlaylistItem*>(parent.internalPointer());
  }

  return parentItem->childCount();
}

int PlaylistModel::columnCount(const QModelIndex &parent) const {
  if ( parent.isValid() ) {
    return static_cast<PlaylistItem*>(parent.internalPointer())->columnCount();
  } else {
    return _root->columnCount();
  }
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const {
  if ( !index.isValid() ) {
    return QVariant();
  }

  if ( role != Qt::DisplayRole ) {
    return QVariant();
  }

  PlaylistItem *item = static_cast<PlaylistItem*>(index.internalPointer());

  return item->data(index.column());
}

Qt::ItemFlags PlaylistModel::flags(const QModelIndex &index) const {

  if ( !index.isValid() ) {
    return Qt::ItemIsEnabled;
  }
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant PlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if ( orientation == Qt::Horizontal && role == Qt::DisplayRole ) {
    return _root->data(section);
  }

  return QVariant();
}

/*
 * =======================================================
 * PlaylistItem
 * =======================================================
 */
PlaylistItem::PlaylistItem(const QList<QVariant> &data, PlaylistItem *parent) {
  parentItem = parent;
  temData = data;
}

PlaylistItem::~PlaylistItem() {
  qDeleteAll(childItems);
}

void PlaylistItem::appendChild(PlaylistItem *item) {
  childItems.append(item);
}

PlaylistItem *PlaylistItem::child(int row) {
  return childItems.value(row);
}

int PlaylistItem::childCount() const {
  return childItems.count();
}

int PlaylistItem::row() const {
  if ( parentItem ) {
    return parentItem->childItems.indexOf(const_cast<PlaylistItem*>(this));
  }
  return 0;
}

int PlaylistItem::columnCount() const {
  return itemData.count();
}

QVariant PlaylistItem::data(int column) const {
  return itemData.value(column);
}

PlaylistItem *PlaylistItem::parent() {
  return parentItem;
}
