#ifndef AUDIOPROPERTIES_H
#define AUDIOPROPERTIES_H

#include <QFrame>
#include "ui_audioproperties.h"
#include "Signal.h"

class AudioProperties : public QFrame, private Ui::AudioPropertiesClass
{
  Q_OBJECT

public:
  AudioProperties(QWidget *parent = 0, Signal* const audio = 0);
  ~AudioProperties();

private:
  Signal* const audio_;

  void populate();
};

#endif // AUDIOPROPERTIES_H
