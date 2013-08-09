#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

//#define LOGGING_HEADER __func__ << "(" << __FILE__ << ":" << __LINE__ << ") "
#define LOGGING_HEADER ""

class Logger {

public:
  static const int DEBUG	= 4;
  static const int INFO	= 3;
  static const int WARN	= 2;
  static const int ERROR	= 1;
  static const int FATAL	= 0;

  /*
  Logger();
  ~Logger();
  */

  static void debug(char *msg, ...);
  static void info(char *msg, ...);
  static void warn(char *msg, ...);
  static void error(char *msg, ...);
  static void fatal(char *msg, ...);

  static void setLogLevel(int level) { _level = level; }
  static int getLogLevel() { return _level; }

private:
  static void log(const int level, char *msg, va_list);
  static int _level;
};

#endif /* LOGGER_H */
