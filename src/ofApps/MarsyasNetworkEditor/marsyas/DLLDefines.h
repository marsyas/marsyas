


#ifndef _marsyas_DLLDEFINES_H 
#define _marsyas_DLLDEFINES_H 


#if defined (_WIN32)
#include "config.h" 
#if defined MARSYAS_STATIC 
#define marsyas_EXPORT 
#else 
#if defined (marsyas_EXPORTS)
#pragma warning(disable: 4251)
#define marsyas_EXPORT __declspec(dllexport)
#else 
#pragma warning(disable: 4251)
#define marsyas_EXPORT __declspec(dllimport)
#endif
#endif  
#else
#define marsyas_EXPORT
#endif 
#endif 
