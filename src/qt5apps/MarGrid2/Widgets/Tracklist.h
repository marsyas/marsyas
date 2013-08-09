#ifndef TRACKLIST_H
#define TRACKLIST_H

#ifndef TRACK_LOCATION
#define TRACK_LOCATION (Qt::UserRole + 1)
#endif

#include <QDebug>
#include <QListWidget>

#include "../Music/MusicCollection.h"

class Tracklist : public QListWidget
{
public:
  Tracklist(QWidget *parent=0);
  ~Tracklist();

  void listTracks(MusicPlaylist *playlist);
  void listTracks(MusicTrackIterator *tracks);
protected:
  void mousePressEvent(QMouseEvent *event);

private:

};

#endif /* TRACKLIST_H */
