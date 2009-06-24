/************************************************************************/
/*! \class RtError
    \brief Exception handling class for RtAudio3 & RtMidi.

    The RtError class is quite simple but it does allow errors to be
    "caught" by RtError3::Type. See the RtAudio3 and RtMidi
    documentation to know which methods can throw an RtError3.

*/
/************************************************************************/

#ifndef RTERROR3_H
#define RTERROR3_H

#include <iostream>
#include <string>

class RtError3
{
public:
  //! Defined RtError3 types.
  enum Type {
    WARNING,           /*!< A non-critical error. */
    DEBUG_WARNING,     /*!< A non-critical error which might be useful for debugging. */
    UNSPECIFIED,       /*!< The default, unspecified error type. */
    NO_DEVICES_FOUND,  /*!< No devices found on system. */
    INVALID_DEVICE,    /*!< An invalid device ID was specified. */
    INVALID_STREAM,    /*!< An invalid stream ID was specified. */
    MEMORY_ERROR,      /*!< An error occured during memory allocation. */
    INVALID_PARAMETER, /*!< An invalid parameter was specified to a function. */
    DRIVER_ERROR,      /*!< A system driver error occured. */
    SYSTEM_ERROR,      /*!< A system error occured. */
    THREAD_ERROR       /*!< A thread error occured. */
  };

protected:
  std::string message_;
  Type type_;

public:
  //! The constructor.
  RtError3(const std::string& message, Type type = RtError3::UNSPECIFIED) : message_(message), type_(type) {}

  //! The destructor.
  virtual ~RtError3(void) {};

  //! Prints thrown error message to stderr.
  virtual void printMessage(void) { std::cerr << '\n' << message_ << "\n\n"; }

  //! Returns the thrown error message type.
  virtual const Type& getType(void) { return type_; }

  //! Returns the thrown error message string.
  virtual const std::string& getMessage(void) { return message_; }

  //! Returns the thrown error message as a C string.
  virtual const char *getMessageString(void) { return message_.c_str(); }
};

#endif
