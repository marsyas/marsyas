#include "MidiListener.h"

bool MidiListener::_touchPadOn = false;
bool MidiListener::_seenXAxis = false;
bool MidiListener::_seenYAxis = false;
unsigned char MidiListener::_xValue = 0;
unsigned char MidiListener::_yValue = 0;

/*
 * ----------------------------------------------------------------------------------
 * Midi Callback Handler
 *
 *  - 92 - 127 on pad
 *  - pairs of 12/13 -> send
 *  - 92 - 0 off pad
 * ----------------------------------------------------------------------------------
 */
void MidiListener::kaossCallback(double stamp, std::vector<unsigned char> *message, void* midi) {
  size_t bytes = message->size();

  if ( 3 == bytes ) {
    unsigned char byte0 = message->at(0);
    unsigned char byte1 = message->at(1);
    unsigned char byte2 = message->at(2);

    if ( 176 == byte0 ) {
      //Control Messages
      switch( byte1 ) {
      case MidiListener::TOUCH_PAD:
        MidiListener::_touchPadOn = (byte2 < 64) ? false : true;
        break;
      case MidiListener::X_AXIS:
        MidiListener::_seenXAxis = true;
        MidiListener::_xValue = byte2;
        break;
      case MidiListener::Y_AXIS:
        MidiListener::_seenYAxis = true;
        MidiListener::_yValue = byte2;
        break;
      case MidiListener::MUTE:
        bool onOff = (byte2 < 64) ? false : true;
        emit ((MidiListener*)midi)->kaossMuteEvent(onOff);
        break;
      }

      if ( MidiListener::_touchPadOn
           && MidiListener::_seenXAxis && MidiListener::_seenYAxis ) {

        emit ((MidiListener*)midi)->kaossXYEvent(MidiListener::_xValue,
            MidiListener::_yValue);
        MidiListener::_seenXAxis = false;
        MidiListener::_seenYAxis = false;
      }

    } else if ( 192 == byte0 ) {
      //Program Messages
    }
  }
}

MidiListener::MidiListener() {
  try {
    unsigned int queue_size_limit = 2048;
    std::string client_name = "MarGrid";

    _in = new RtMidiIn(RtMidi::UNSPECIFIED, client_name, queue_size_limit);

    // Check available ports.
    unsigned int nPorts = _in->getPortCount();
    if ( nPorts > 0 ) {
      _in->openPort( 0 );

      _in->setCallback( this->kaossCallback, this );

      // Don't ignore sysex, timing, or active sensing messages.
      //_in->ignoreTypes( false, false, false );
      _in->ignoreTypes( true, true, true );
    } else {
      std::cout << "No MIDI ports/devices available!\n";
      delete _in;
      _in = NULL;
    }
  } catch ( RtError &error ) {
    error.printMessage();
    delete _in;
    _in = NULL;
  }
}

MidiListener::~MidiListener() {
  if ( _in ) {
    delete _in;
  }
}

