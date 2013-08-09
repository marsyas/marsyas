#ifndef PLAYBOX_H
#define PLAYBOX_H

#include <iostream>
#include <QLabel>
#include <QFont>
#include <QGroupBox>
#include <QGridLayout>
#include "../Music/MusicTrack.h"

class PlayBox : public QGroupBox
{
public:
  PlayBox(QWidget *parent=0);

  void updateCurrentlyPlaying(MusicTrack *track);

private:
  QLabel *_title;
  QLabel *_artist;
  QLabel *_album;
  QLabel *_location;
  QLabel *_genre;
  QLabel *_kind;
  QLabel *_year;
  QLabel *_length;
};

#endif /* PLAYBOX_H */
