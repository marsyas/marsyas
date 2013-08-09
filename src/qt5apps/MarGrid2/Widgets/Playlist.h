#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QDebug>
#include <QWidget>
#include <QListWidget>

#include "Tracklist.h"
#include "../Music/MusicCollection.h"

class Playlist : public QListWidget
{
  Q_OBJECT
public:
  Playlist(Tracklist *tracklist, QWidget *parent=0);
  ~Playlist();

signals:
  void SelectedPlaylist(QString);

public slots:
  void playlistClicked(int currentRow);
  void updatePlaylist();

protected:
  void mousePressEvent(QMouseEvent *event);

private:
  Tracklist *_tracklist;
  MusicCollection *_library;
};

#endif /* PLAYLIST_H */
