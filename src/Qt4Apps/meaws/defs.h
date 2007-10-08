#ifndef MEAWS_DEFS_H
#define MEAWS_DEFS_H

#define TYPE_NONE 0
#define TYPE_PLAYBACK 1
#define TYPE_INTONATION 2
#define TYPE_RHYTHM 3
#define TYPE_CONTROL 4
#define TYPE_SHIFT 5

#define BACKEND_PITCHES 0x0001
#define BACKEND_AMPLITUDES 0x0002

#define MEAWS_READY_NOTHING 1
#define MEAWS_READY_USER 2
#define MEAWS_READY_EXERCISE 3
#define MEAWS_READY_AUDIO 4

// might work?  might not.  Can't check by myself.
#ifdef WIN32
#define MEAWS_DIR "c:\\temp\\meaws"
#endif

#ifndef MEAWS_DIR
#define MEAWS_DIR QDir::homePath() + "/.meaws/"
#endif


#endif

