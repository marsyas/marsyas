#include "ofMain.h"
#include "ofMarSimpleSine.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ) {

  ofAppGlutWindow window;
  ofSetupOpenGL(&window, 300,200, OF_WINDOW);			// <-------- setup the GL context

  // this kicks off the running of my app
  // can be OF_WINDOW or OF_FULLSCREEN
  // pass in width and height too:
  ofRunApp( new ofMarSimpleSine());

}
