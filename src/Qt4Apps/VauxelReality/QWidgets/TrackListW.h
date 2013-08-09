#ifndef TRACKLISTW_H
#define TRACKLISTW_H

#include <QListWidget>

#include "MusicCollection.h"

class TrackListW : public QListWidget
{
public:
  TrackListW(QWidget *parent=0);
  ~TrackListW();

  void listTracks(MusicPlaylist *playlist);
  void listTracks(MusicTrackIterator *tracks);

protected:
  void mousePressEvent(QMouseEvent *event);

private:

};

#endif /* TRACKLISTW_H */
