#ifndef PLAYGROUPBOXW_H
#define PLAYGROUPBOXW_H

#include <QLabel>
#include <QGroupBox>

#include "MusicTrack.h"

class PlayGroupBoxW : public QGroupBox
{
public:
  PlayGroupBoxW(QWidget *parent=0);

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

#endif /* PLAYGROUPBOXW_H */
