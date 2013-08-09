#include "Logger.h"

int Logger::_level = Logger::WARN;

void Logger::debug(char *msg, ...) {
  if ( DEBUG <= _level ) {
    va_list args;
    va_start( args, msg );
    log(Logger::DEBUG, msg, args);
    va_end( args );
  }
}

void Logger::info(char *msg, ...) {
  if ( INFO <= _level ) {
    va_list args;
    va_start( args, msg );
    log(Logger::INFO, msg, args);
    va_end( args );
  }
}

void Logger::warn(char *msg, ...) {
  if ( WARN <= _level ) {
    va_list args;
    va_start( args, msg );
    log(Logger::WARN, msg, args);
    va_end( args );
  }
}

void Logger::error(char *msg, ...) {
  if ( ERROR <= _level ) {
    va_list args;
    va_start( args, msg );
    log(Logger::ERROR, msg, args);
    va_end( args );
  }
}

void Logger::fatal(char *msg, ...) {
  if ( FATAL <= _level ) {
    va_list args;
    va_start( args, msg );
    log(Logger::FATAL, msg, args);
    va_end( args );
  }
}

void Logger::log(const int level, char *msg, va_list args) {
  char **result = NULL;
  //vasprintf(result, msg, args);
  if ( result ) {
    std::cout << LOGGING_HEADER;
    switch( level ) {
    case Logger::DEBUG:
      std::cout << "DEBUG ";
      break;
    case Logger::INFO:
      std::cout << "INFO  ";
      break;
    case Logger::WARN:
      std::cout << "WARN  ";
      break;
    case Logger::ERROR:
      std::cout << "ERROR ";
      break;
    case Logger::FATAL:
      std::cout << "FATAL ";
      break;
    default:
      std::cout << "NONE  ";
      break;
    }
    std::cout << *result << std::endl;
    free(result);
  } else {
    std::cout << LOGGING_HEADER << " !ERROR LOGGING! " << msg << std::endl;
  }
}
