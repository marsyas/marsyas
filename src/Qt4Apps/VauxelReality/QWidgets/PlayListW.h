#ifndef PLAYLISTW_H
#define PLAYLISTW_H

#include <QListWidget>

#include "TrackListW.h"
#include "MusicCollection.h"

class PlayListW : public QListWidget
{
  Q_OBJECT
public:
  PlayListW(TrackListW *tracklist, QWidget *parent=0);
  ~PlayListW();

public slots:
  void playlistClicked(int currentRow);
  void updatePlaylist();

protected:
  void mousePressEvent(QMouseEvent *event);

private:
  TrackListW *_tracklist;
  MusicCollection *_library;
};

#endif /* PLAYLISTW_H */
