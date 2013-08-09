#ifndef PLAYLISTITEM_H
#define PLAYLISTITEM_H

#include <iostream>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "../Music/MusicCollection.h"

class PlaylistItem : public QTreeWidgetItem
{
public:
  PlaylistItem(MusicPlaylist *playlist, QTreeWidget *parent=0);
  QVariant data(int column, int role) const;

private:
  MusicPlaylist *_playlist;
};

class PlaylistTrackItem : public QTreeWidgetItem
{
public:
  PlaylistTrackItem(MusicTrack *track, QTreeWidgetItem *parent=0);
  QVariant data(int column, int role) const;

private:
  MusicTrack *_track;
};

#endif /* PLAYLISTITEM_H */

