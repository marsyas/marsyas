#ifndef MEAWS_DEFS_H
#define MEAWS_DEFS_H

#define BACKEND_PLAYBACK 1
#define BACKEND_PITCHES 2
#define BACKEND_AMPLITUDES 3
#define BACKEND_PITCHES_AMPLITUDES 4

#define MEAWS_UPDATE 0
#define MEAWS_READY_NOTHING 1
#define MEAWS_READY_USER 2
#define MEAWS_READY_EXERCISE 3
#define MEAWS_TRY_PAUSED 4
#define MEAWS_TRY_RUNNING 5

#define TRY_NOTHING 0
#define TRY_SELECTED 1
#define TRY_BADNOTES 2


// might work?  might not.  Can't check by myself.
#ifdef WIN32
#define MEAWS_DIR "c:\\temp\\meaws"
#endif

#ifndef MEAWS_DIR
#define MEAWS_DIR QDir::homePath() + "/.meaws/"
#endif


#endif

