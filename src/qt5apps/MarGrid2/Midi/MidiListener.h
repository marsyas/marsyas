#ifndef MIDILISTENER_H
#define MIDILISTENER_H

#include <QObject>
#include <QThread>
#include <RtMidi.h>

class MidiEvent;


class MidiListener : public QObject
{
  Q_OBJECT
public:
  static const unsigned char MODULATION_DEPTH1	= 1;
  static const unsigned char MODULATION_DEPTH2	= 2;
  static const unsigned char X_AXIS		= 12;
  static const unsigned char Y_AXIS		= 13;
  static const unsigned char TOUCH_PAD		= 92;
  static const unsigned char UNKNOWN		= 93;
  static const unsigned char FX_DEPTH		= 94;
  static const unsigned char MUTE			= 95;

  MidiListener();
  ~MidiListener();

signals:
  void kaossXYEvent(unsigned char operation, unsigned char value);
  void kaossMuteEvent(bool onOff);

private:
  static void kaossCallback(double deltatime, std::vector<unsigned char> *message, void *data);

  RtMidiIn *_in;

  static bool _touchPadOn;
  static bool _seenXAxis;
  static bool _seenYAxis;
  static unsigned char _xValue;
  static unsigned char _yValue;

};

#endif /* MIDILISTENER_H */

