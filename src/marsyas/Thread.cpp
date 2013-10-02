/***************************************************/
/*! \class Thread
    \brief STK thread class.

    This class provides a uniform interface for
    cross-platform thread use.  On Linux and IRIX
    systems, the pthread library is used.  Under Windows,
    the Windows thread library is used.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


#include <marsyas/common_source.h>

#if (defined(MARSYAS_LINUX) || defined(MARSYAS_CYGWIN) || defined(MARSYAS_MACOSX))
#include <pthread.h>
#define THREAD_TYPE
typedef pthread_t THREAD_HANDLE;
typedef void * THREAD_RETURN;
typedef void * (*THREAD_FUNCTION)(void *);
typedef pthread_mutex_t MUTEX;
#endif



#if defined(MARSYAS_WIN32)

#define _WINSOCKAPI_

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <process.h>
#define THREAD_TYPE __stdcall
typedef unsigned long THREAD_HANDLE;
typedef unsigned THREAD_RETURN;
typedef unsigned (__stdcall *THREAD_FUNCTION)(void *);
typedef CRITICAL_SECTION MUTEX;

#endif


#if defined(MARSYAS_MINGW)

#define _WINSOCKAPI_

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <process.h>
#define THREAD_TYPE __stdcall
typedef unsigned long THREAD_HANDLE;
typedef unsigned THREAD_RETURN;
typedef unsigned (__stdcall *THREAD_FUNCTION)(void *);
typedef CRITICAL_SECTION MUTEX;

#endif



#include "Thread.h"

using namespace Marsyas;


Thread :: Thread()
{
  thread = 0;
}

Thread :: ~Thread()
{
#if (defined(MARSYAS_CYGWIN) || defined(MARSYAS_CYGWIN) || defined(MARSYAS_MACOSX))

  pthread_cancel(thread);
  pthread_join(thread, NULL);

#elif defined(MARSYAS_WIN32)

  if ( thread )
    TerminateThread((HANDLE)thread, 0);

#endif
}

bool Thread :: start( THREAD_FUNCTION routine, void * ptr )
{
  bool result = false;
#if (defined(MARSYAS_CYGWIN) || defined(MARSYAS_LINUX) || defined(MARSYAS_MACOSX))

  if ( pthread_create(&thread, NULL, *routine, ptr) == 0 )
    result = true;

#elif defined(MARSYAS_WIN32)
  unsigned thread_id;
  thread = _beginthreadex(NULL, 0, routine, ptr, 0, &thread_id);
  if ( thread ) result = true;

#else
  (void) routine;
  (void) ptr;

#endif
  return result;
}

bool Thread :: wait( long milliseconds )
{
  bool result = false;
#if (defined(MARSYAS_CYGWIN) || defined(MARSYAS_LINUX) || defined(MARSYAS_MACOSX))

  (void) milliseconds;
  pthread_cancel(thread);
  pthread_join(thread, NULL);

#elif defined(MARSYAS_WIN32)

  DWORD timeout, retval;
  if ( milliseconds < 0 ) timeout = INFINITE;
  else timeout = milliseconds;
  retval = WaitForSingleObject( (HANDLE)thread, timeout );
  if ( retval == WAIT_OBJECT_0 ) {
    CloseHandle( (HANDLE)thread );
    thread = 0;
    result = true;
  }

#endif
  return result;
}

void Thread :: test(void)
{
#if (defined(MARSYAS_CYGWIN) || defined(MARSYAS_CYGWIN) || defined(MARSYAS_MACOSX))

  pthread_testcancel();

#endif
}


Mutex :: Mutex()
{

#if (defined(MARSYAS_CYGWIN) || defined(MARSYAS_CYGWIN) || defined(MARSYAS_MACOSX))

  pthread_mutex_init(&mutex, NULL);

#elif defined(MARSYAS_WIN32)

  InitializeCriticalSection(&mutex);

#endif
}

Mutex :: ~Mutex()
{
#if (defined(MARSYAS_CYGWIN) || defined(MARSYAS_CYGWIN) || defined(MARSYAS_MACOSX))

  pthread_mutex_destroy(&mutex);

#elif defined(MARSYAS_WIN32)

  DeleteCriticalSection(&mutex);

#endif
}

void Mutex :: lock()
{
#if (defined(MARSYAS_CYGWIN) || defined(MARSYAS_CYGWIN) || defined(MARSYAS_MACOSX))

  pthread_mutex_lock(&mutex);

#elif defined(MARSYAS_WIN32)

  EnterCriticalSection(&mutex);

#endif
}

void Mutex :: unlock()
{
#if (defined(MARSYAS_CYGWIN) || defined(MARSYAS_CYGWIN) || defined(MARSYAS_MACOSX))

  pthread_mutex_unlock(&mutex);

#elif defined(MARSYAS_WIN32)

  LeaveCriticalSection(&mutex);

#endif
}
