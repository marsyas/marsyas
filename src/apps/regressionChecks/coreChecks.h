#ifndef REGCHECK_CORE
#define REGCHECK_CORE

// core/fundamental  Marsyas checks
#include "common-reg.h"
#include "RtAudio.h"
//#include "RtMidi.h"


// possibly useful as a basis for new tests
void core_null();
void core_isClose(string infile1, string infile2);
void core_audiodevices();
void core_realvec();

#endif

