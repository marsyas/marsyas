#include "MusicTrack.h"

MusicTrack::MusicTrack() {
  _x = -1;
  _y = -1;
  _persistentId = "";
}
MusicTrack::MusicTrack(QString location)
{
  _location = location;
}

void MusicTrack::setTrackId(int id) {
  _trackId = id;
}

void MusicTrack::setSize(int size) {
  _size = size;
}

void MusicTrack::setLength(int length) {
  _length = length;
}

void MusicTrack::setNumber(int number) {
  _number = number;
}

void MusicTrack::setTitle(QString name) {
  _title = name;
}

void MusicTrack::setArtist(QString artist) {
  _artist = artist;
}

void MusicTrack::setAlbum(QString album) {
  _album = album;
}

void MusicTrack::setGenre(QString genre) {
  _genre = genre;
}

void MusicTrack::setLocation(QString location) {
  _location = location;
}

void MusicTrack::setYear(QString year) {
  _year = year;
}

void MusicTrack::setType(QString type) {
  _type = type;
}

void MusicTrack::setKind(QString kind) {
  _kind = kind;
}

void MusicTrack::setX(int x) {
  _x = x;
}

void MusicTrack::setY(int y) {
  _y = y;
}

QString MusicTrack::toString() const {
  //QString x = "Track:    " + _title + "(" + _number + ")\n";
  QString x = "Track:    " + _title + "\n";
  //QString x = "Track:    " + _title + "(" + _trackId + ")\n";
  x += "Artist:   " + _artist + "\n";
  x += "Album:    " + _album + "(" + _year + ")\n";
  x += "Genre:    " + _genre + "\n";
  x += "Location: " + _location + "\n";
  //x += "Info:     " + _kind + " " + _size + " " + _length + "\n";
  return x;
}

std::string MusicTrack::toStdString() const {
  return toString().toStdString();
}

std::ostream &operator<<(std::ostream &out, const MusicTrack &track) {
  out << track.toStdString();
  return out;
}

