#ifndef MUSICCOLLECTION_H
#define MUSICCOLLECTION_H

#include "MusicLink.h"
#include "MusicTrack.h"

std::ostream &operator<<(std::ostream& out, const QString &str);

class MusicCollection
{
protected:
  MusicCollection();

public:
  static MusicCollection* getInstance();

  ~MusicCollection();

  void addTrack(MusicTrack *track);
  void addPlaylist(MusicPlaylist *pl);
  void addTrackToPlaylist(QString text, MusicTrack *track);

  MusicArtistIterator getArtists() const;
  MusicArtist* getArtistByName(QString artist) const;

  MusicGenreIterator getGenres() const;
  MusicGenre* getGenreByName(QString genre) const;

  MusicPlaylistIterator getPlaylists() const;
  MusicPlaylist* getPlaylistByName(QString name) const;

  MusicTrackIterator getTracks() const;
  int getNumTracks() const;
  MusicTrack* getTrackByIndex(int index) const;
  MusicTrack* getTrackById(int id) const;

  void display();
  void displayArtists();
  void displayGenres();
  void displayTracks();
  void displayPlaylists();
  void empty();

  int generateTrainingList(std::ostream& out);
  int generateTrackList(std::ostream& out);

private:
  void linkGenre(MusicTrack *genre);
  void linkArtist(MusicTrack *artist);

  void emptyArtists();
  void emptyGenres();
  void emptyPlaylists();
  void emptyTracks();

  MusicArtistMap *_artists;
  MusicGenreMap *_genres;
  MusicTrackHash *_trackhash;
  MusicTrackVector *_trackvec;
  MusicPlaylistMap *_playlists;

  static MusicCollection *_instance;
};

#endif /* MUSICCOLLECTION_H */

