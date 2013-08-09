#ifndef MUSICTRACK_H
#define MUSICTRACK_H

#include <QSet>
#include <QHash>
#include <QVector>
#include <QVectorIterator>

class MusicTrack;

typedef QSet<MusicTrack*>  MusicTrackSet;
typedef QVector<MusicTrack*> MusicTrackVector;
typedef QHash<int, MusicTrack*> MusicTrackHash;
typedef QVectorIterator<MusicTrack*> MusicTrackIterator;

std::ostream &operator<<(std::ostream &out, const MusicTrack &track);

class MusicTrack
{
public:
  friend std::ostream &operator<<(std::ostream &out, const MusicTrack &track);

  MusicTrack();

  void setTrackId(int id);
  void setSize(int size);
  void setLength(int length);
  void setNumber(int number);
  void setTitle(QString name);
  void setArtist(QString artist);
  void setAlbum(QString album);
  void setGenre(QString genre);
  void setLocation(QString location);
  void setYear(QString year);
  void setType(QString type);
  void setKind(QString type);
  void setX(int x);
  void setY(int y);
  void setPersistentId(QString id);

  int getTrackId() const { return _trackId; }
  int getSize() const { return _size; }
  int getLength() const { return _length; }
  int getNumber() const { return _number; }
  int getX() const { return _x; }
  int getY() const { return _y; }

  QString getTitle() const { return _title; }
  QString getArtist() const { return _artist; }
  QString getAlbum() const { return _album; }
  QString getGenre() const { return _genre; }
  QString getLocation() const { return _location; }
  QString getYear() const { return _year; }
  QString getType() const { return _type; }
  QString getKind() const { return _kind; }
  QString getPersistentId() const { return _persistentId; }

  QString toString() const;
  std::string toStdString() const;

private:
  int _trackId;
  int _size;
  int _length;
  int _number;
  int _x;
  int _y;

  QString _title;
  QString _artist;
  QString _album;
  QString _genre;
  QString _location;
  QString _year;
  QString _type;
  QString _kind;
  QString _persistentId;
};

#endif /* MUSICTRACK_H */
