#ifndef MYDISPLAY_H
#define MYDISPLAY_H

#include <ostream>

#include <QWidget>

#include <ctime>
#include <cstdlib>

#include "Classifier.h"
#include "Extractor.h"
#include "../Music/MusicTrack.h"
#include "../Widgets/Tracklist.h"

class MyDisplay : public QWidget
{
  Q_OBJECT

public:
  MyDisplay(Tracklist *tracklist, QWidget *parent = 0)
    : QWidget(parent), _tracklist(tracklist) {}

  virtual ~MyDisplay() {};
  virtual void clear() = 0;


public slots:
  virtual void extract() = 0;
  virtual void predict() = 0;
  virtual void train() = 0;
  virtual void midiXYEvent(unsigned char /* xaxis */, unsigned char /* yaxis */) {}
  virtual void midiPlaylistEvent(bool /* next */) {}
  virtual void reload() {}

signals:
  void playingTrack(MusicTrack *track);

protected:
  Tracklist *_tracklist;
};


#endif /* MyDISPLAY_H */
