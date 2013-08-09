#define DEBUG 0

#if DEBUG
#include <android/log.h>
#  define  D(x...)  __android_log_print(ANDROID_LOG_INFO,"hellomarsyas",x)
#else
#  define  D(...)  do {} while (0)
#endif

/* return current time in milliseconds */
static double
now_ms(void)
{
  struct timespec res;
  clock_gettime(CLOCK_REALTIME, &res);
  return 1000.0*res.tv_sec + (double)res.tv_nsec/1e6;
}
