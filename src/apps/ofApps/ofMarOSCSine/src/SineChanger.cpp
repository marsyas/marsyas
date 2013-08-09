#include "SineChanger.h"

// #define FIRST_BLOOM_COLUMN_IDX	6
// #define FULL_BLOOM_COLUMN_IDX	10
//--------------------------------------------------------------
SineChanger::SineChanger()
{
  counter = 200;

  // listen on the given port
  cout << "listening for osc messages on port " << PORT << "\n";
  receiver.setup( PORT );

  current_msg_string = 0;
  counter = 0;

  ofBackground( 30, 30, 130 );

}

//--------------------------------------------------------------
SineChanger::~SineChanger()
{
  destroy();
}

//--------------------------------------------------------------
void
SineChanger::setup()
{
  startThread(true, false); // blocking, non-verbose
  // startThread(true, true); // blocking, non-verbose
}

//--------------------------------------------------------------
void
SineChanger::destroy()
{
  if (isThreadRunning())
    stopThread();
}


//--------------------------------------------------------------
void
SineChanger::threadedFunction()
{
  cout << "threadedFunction" << endl;

  for (;;) {
    if( lock() ) {

      // hide old messages
      for ( int i=0; i<NUM_MSG_STRINGS; i++ )
      {
        if ( timers[i] < ofGetElapsedTimef() )
          msg_strings[i] = "";
      }

      // check for waiting messages
      while( receiver.hasWaitingMessages() )
      {
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage( &m );

        // check for mouse moved message
        if ( m.getAddress() == "/counter" )
        {
          // both the arguments are int32's
          counter = m.getArgAsInt32( 0 );
        }
      }

      // count++;
      // if(count > 500) count = 200;
      unlock();
      ofSleepMillis(20);
    }
    // cout << "in thread.  count=" << count << endl;
  }
}

void SineChanger::update() {
  cout << "update" << endl;
}

void SineChanger::draw() {
  cout << "draw" << endl;
}

double SineChanger::getCount() {
  return (double)counter;
}
