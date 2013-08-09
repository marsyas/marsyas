#ifndef PLAYBACK_H
#define PLAYBACK_H

#include <QWidget>

class Playback : public QWidget
{
public slot:
  virtual void playTrack(MusicTrack track*);
};


#endif /* PLAYBACK_H */

