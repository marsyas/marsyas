#ifndef MUSICLINK_H
#define MUSICLINK_H

#include <QSet>
#include <QSetIterator>
#include <QMap>
#include <QMapIterator>
#include <QString>
#include "MusicTrack.h"

class MusicLink;
class MusicArtist;
class MusicPlaylist;

typedef MusicLink MusicAlbum;
typedef QMap<QString, MusicAlbum*>  MusicAlbumMap;
typedef QMapIterator<QString, MusicAlbum*>  MusicAlbumIterator;

typedef MusicLink MusicGenre;
typedef QSet<MusicGenre*>  MusicGenreSet;
typedef QMap<QString, MusicGenre*> MusicGenreMap;
typedef QMapIterator<QString, MusicGenre*> MusicGenreIterator;

typedef QSet<MusicArtist*>  MusicArtistSet;
typedef QMap<QString, MusicArtist*> MusicArtistMap;
typedef QMapIterator<QString, MusicArtist*> MusicArtistIterator;

typedef QMap<QString, MusicPlaylist*>  MusicPlaylistMap;
typedef QMapIterator<QString, MusicPlaylist*>  MusicPlaylistIterator;

class MusicLink
{
public:
  MusicLink(QString name);
  virtual ~MusicLink();

  QString getName() const { return _name; }
  MusicTrackIterator getTracks() const;

  void linkTrack(MusicTrack *track);

private:
  QString _name;
  MusicTrackVector *_tracks;
};

class MusicArtist : public MusicLink
{
public:
  MusicArtist(QString name);
  ~MusicArtist();

  void linkAlbum(MusicTrack *track);
  MusicAlbumIterator getAlbums() const;

private:
  MusicAlbumMap *_albums;
};

class MusicPlaylist : public MusicLink
{
public:
  MusicPlaylist(QString name);
  ~MusicPlaylist() {}

  void setPersistentId(QString persistentId);
  void setPlaylistId(int id);

  QString getPersistentId() const { return _persistentId; }
  int getPlaylistId() const { return _playlistId; }

private:
  QString _persistentId;
  int _playlistId;
};

#endif /* MUSICLINK_H */
