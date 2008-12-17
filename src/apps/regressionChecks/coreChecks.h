#ifndef REGCHECK_CORE_H
#define REGCHECK_CORE_H

// core/fundamental  Marsyas checks
#include "common-reg.h"
#include <cstdlib>
#ifdef MARSYAS_AUDIOIO
#include "RtAudio3.h"
#endif
//#include "RtMidi.h"


// possibly useful as a basis for new tests
void core_null();
void core_isClose(string infile1, string infile2);
void core_audiodevices();
void core_realvec();

#endif

