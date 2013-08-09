#include "MusicCollection.h"

MusicCollection* MusicCollection::_instance = NULL;

std::ostream &operator<<(std::ostream& out, const QString &str) {
  out << str.toStdString();
  return out;
}

MusicCollection* MusicCollection::getInstance() {
  if ( !_instance ) {
    _instance = new MusicCollection();
  }
  return _instance;
}

MusicCollection::MusicCollection() {
  _artists = new MusicArtistMap();
  _genres = new MusicGenreMap();
  _playlists = new MusicPlaylistMap();
  _trackhash = new MusicTrackHash();
  _trackvec = new MusicTrackVector();
}

MusicCollection::~MusicCollection() {
  empty();
  delete _artists;
  delete _genres;
  delete _trackhash;
  delete _trackvec;
  delete _playlists;
}

void MusicCollection::linkArtist(MusicTrack *track) {
  if ( track ) {
    MusicArtist *artist;
    MusicArtistMap::iterator rs = _artists->find(track->getArtist());
    if ( rs != _artists->end() ) {
      artist = *rs;
    } else {
      artist = new MusicArtist(track->getArtist());
      _artists->insert(track->getArtist(), artist);
    }
    artist->linkTrack( track );
    artist->linkAlbum( track );
  }
}

void MusicCollection::linkGenre(MusicTrack *track) {
  if ( track ) {
    MusicGenre *genre;
    MusicGenreMap::iterator rs = _genres->find(track->getGenre());
    if ( rs != _genres->end() ) {
      genre = *rs;
    } else {
      genre = new MusicGenre(track->getGenre());
      _genres->insert(track->getGenre(), genre);
    }
    genre->linkTrack( track );
  }
}

void MusicCollection::addTrack(MusicTrack *track) {
  if ( track ) {
    linkGenre(track);
    linkArtist(track);
    _trackhash->insert(track->getTrackId(), track);
    _trackvec->push_back(track);
  }
}

void MusicCollection::addTrackToPlaylist(QString text, MusicTrack *track) {
  if ( track ) {
    MusicPlaylist *pl;
    MusicPlaylistMap::iterator rs = _playlists->find(text);
    if ( rs != _playlists->end() ) {
      pl = *rs;
    } else {
      pl = new MusicPlaylist(text);
      _playlists->insert(text, pl);
    }
    pl->linkTrack(track);
  }
}

void MusicCollection::addPlaylist(MusicPlaylist *pl) {
  if ( pl ) {
    MusicPlaylistMap::iterator rs = _playlists->find(pl->getName());
    if ( rs != _playlists->end() ) {
      //found remove and delete
      MusicPlaylist *d = *rs;
      delete d;
    }
    _playlists->insert(pl->getName(), pl);
  }
}

void MusicCollection::empty() {
  emptyArtists();
  emptyGenres();
  emptyTracks();
  emptyPlaylists();
}

void MusicCollection::emptyArtists() {
  MusicArtistMap::iterator i = _artists->begin();
  while ( i != _artists->end() ) {
    delete *i;
    i++;
  }
  _artists->clear();
}

void MusicCollection::emptyGenres() {
  MusicGenreMap::iterator i = _genres->begin();
  while ( i != _genres->end() ) {
    delete *i;
    i++;
  }
  _genres->clear();
}

void MusicCollection::emptyPlaylists() {
  MusicPlaylistMap::iterator i = _playlists->begin();
  while ( i != _playlists->end() ) {
    delete *i;
    i++;
  }
  _playlists->clear();
}

void MusicCollection::emptyTracks() {
  MusicTrackVector::iterator b = _trackvec->begin(),
                             e = _trackvec->end();
  while ( b != e ) {
    delete *b;
    b++;
  }
  _trackvec->clear();
  _trackhash->clear();
}

void MusicCollection::display() {
  displayTracks();
  displayArtists();
  displayGenres();
  displayPlaylists();
}

void MusicCollection::displayTracks() {
  std::cout << "\nTracks\n-----------------------------\n";
  MusicTrackIterator it = getTracks();
  while( it.hasNext() ) {
    MusicTrack *track = it.next();
    std::cout << track->getTitle() << std::endl;
  }
}

void MusicCollection::displayArtists() {
  std::cout << "\nArtists\n-----------------------------\n";
  MusicArtistIterator ia = getArtists();
  while( ia.hasNext() ) {
    MusicArtist *artist = ia.next().value();
    std::cout << artist->getName() << std::endl;

    MusicAlbumIterator il = artist->getAlbums();
    while( il.hasNext() ) {
      MusicAlbum *album = il.next().value();
      std::cout << "\t#" << album->getName() << std::endl;

      MusicTrackIterator it = album->getTracks();
      while( it.hasNext() ) {
        MusicTrack *track = it.next();
        std::cout << "\t\t-" << track->getTitle() << std::endl;
      }
    }

    MusicTrackIterator it = artist->getTracks();
    while( it.hasNext() ) {
      MusicTrack *track = it.next();
      std::cout << "\t$" << track->getTitle() << std::endl;
    }
  }
}

void MusicCollection::displayGenres() {
  std::cout << "\nGenres\n-----------------------------\n";
  MusicGenreIterator ig = getGenres();
  while( ig.hasNext() ) {
    MusicGenre *genre = ig.next().value();
    std::cout << genre->getName() << std::endl;

    MusicTrackIterator it = genre->getTracks();
    while( it.hasNext() ) {
      MusicTrack *track = it.next();
      std::cout << "\t-" << track->getTitle()
                << "(" << track->getArtist() << ")" << std::endl;
    }
  }
}

void MusicCollection::displayPlaylists() {
  std::cout << "\nPLaylists\n-----------------------------\n";
  MusicPlaylistIterator ip = getPlaylists();
  while( ip.hasNext() ) {
    MusicPlaylist *playlist = ip.next().value();
    std::cout << playlist->getName() << std::endl;

    MusicTrackIterator it = playlist->getTracks();
    while( it.hasNext() ) {
      MusicTrack *track = it.next();
      std::cout << "\t-" << track->getTitle()
                << "(" << track->getArtist() << ")" << std::endl;
    }
  }
}


int MusicCollection::generateTrackList(std::ostream& out) {
  int count = 0;
  MusicTrackIterator iter = getTracks();
  while( iter.hasNext() )
  {
    MusicTrack *track = iter.next();
    out << track->getLocation().toStdString().c_str() << std::endl;
    count++;
  }
  return count;
}
int MusicCollection::generatePlayList(std::ostream& out, QString playList)
{
  int count = 0;
  MusicPlaylist *pl;
  MusicPlaylistMap::iterator rs = _playlists->find(playList);
  if ( rs != _playlists->end() )
  {
    pl = *rs;
  }
  MusicTrackIterator mti = pl->getTracks();
  while(mti.hasNext())
  {
    MusicTrack *track = mti.next();
    out << track->getLocation() << std::endl;
    count++;
  }
  return count;

}

MusicArtistIterator MusicCollection::getArtists() const {
  return MusicArtistIterator(*_artists);
}

MusicArtist* MusicCollection::getArtistByName(QString artist) const {
  MusicArtistMap::iterator rs = _artists->find(artist);
  if ( rs != _artists->end() ) {
    return *rs;
  }
  return NULL;
}

MusicGenreIterator MusicCollection::getGenres() const {
  return MusicGenreIterator(*_genres);
}

MusicGenre* MusicCollection::getGenreByName(QString genre) const {
  MusicGenreMap::iterator rs = _genres->find(genre);
  if ( rs != _genres->end() ) {
    return *rs;
  }
  return NULL;
}

MusicPlaylistIterator MusicCollection::getPlaylists() const {
  return MusicPlaylistIterator(*_playlists);
}

MusicPlaylist* MusicCollection::getPlaylistByName(QString name) const {
  MusicPlaylistMap::iterator rs = _playlists->find(name);
  if ( rs != _playlists->end() ) {
    return *rs;
  }
  return NULL;
}

int MusicCollection::getNumTracks() const {
  return _trackvec->size();
}

MusicTrackIterator MusicCollection::getTracks() const {
  return MusicTrackIterator(*_trackvec);
}

MusicTrack* MusicCollection::getTrackByIndex(int index) const {
  if ( index < _trackvec->size() ) {
    return _trackvec->value(index);
  }
  return NULL;
}

MusicTrack* MusicCollection::getTrackById(int id) const {
  MusicTrack *track = _trackhash->value(id);
  return track;
}

