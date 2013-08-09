#include "PlaylistItem.h"

PlaylistItem::PlaylistItem(MusicPlaylist *playlist, QTreeWidget *parent)
  : QTreeWidgetItem(parent), _playlist(playlist)
{
  std::cout << "Playlist: " << playlist->getName() << std::endl;

  MusicTrackIterator it = _playlist->getTracks();
  while ( it.hasNext() ) {
    MusicTrack *track = it.next();
    addChild( new PlaylistTrackItem(track, this) );
  }
}

QVariant PlaylistItem::data(int column, int role) const {
  if ( 0 == column ) {
    return QVariant(_playlist->getName());
  } else {
    return QVariant();
  }
}

PlaylistTrackItem::PlaylistTrackItem(MusicTrack *track, QTreeWidgetItem *parent)
  : QTreeWidgetItem(parent), _track(track)
{
  std::cout << "Track: " << track->getTitle() << std::endl;
}

QVariant PlaylistTrackItem::data(int column, int role) const {
  switch ( column ) {
  case 0:
    return QVariant(_track->getTitle());
  case 1:
    return QVariant(_track->getArtist());
  case 2:
    return QVariant(_track->getGenre());
  default:
    return QVariant();
  }
}

