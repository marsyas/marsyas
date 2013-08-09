
#include "MusicLink.h"

MusicLink::MusicLink(QString name)
  : _name(name)
{
  _tracks = new MusicTrackVector();
}

MusicLink::~MusicLink() {
  delete _tracks;
}

void MusicLink::linkTrack(MusicTrack *track) {
  if ( track ) {
    _tracks->push_back(track);
  }
}

MusicTrackIterator MusicLink::getTracks() const {
  return MusicTrackIterator(*_tracks);
}

MusicArtist::MusicArtist(QString name)
  : MusicLink(name)
{
  _albums = new MusicAlbumMap();
}

MusicArtist::~MusicArtist() {
  MusicAlbumIterator album = getAlbums();
  while ( album.hasNext() ) {
    MusicAlbum *a = album.next().value();
    delete a;
  }
  _albums->clear();
  delete _albums;
}

void MusicArtist::linkAlbum(MusicTrack *track) {
  if ( track ) {
    MusicAlbum *album;
    MusicAlbumMap::iterator rs = _albums->find(track->getAlbum());
    if ( rs != _albums->end() ) {
      album = *rs;
    } else {
      album = new MusicAlbum(track->getAlbum());
      _albums->insert(track->getAlbum(), album);
    }
    album->linkTrack( track );
  }
}

MusicAlbumIterator MusicArtist::getAlbums() const {
  return MusicAlbumIterator(*_albums);
}

MusicPlaylist::MusicPlaylist(QString name)
  : MusicLink(name)
{
}

void MusicPlaylist::setPersistentId(QString id) {
  _persistentId = id;
}

void MusicPlaylist::setPlaylistId(int id) {
  _playlistId = id;
}


