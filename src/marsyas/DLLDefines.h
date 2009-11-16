


#ifndef _marsyas_DLLDEFINES_H 
#define _marsyas_DLLDEFINES_H 

#if defined (_WIN32)
#if defined (marsyas_EXPORTS) 
#define marsyas_EXPORT __declspec(dllexport)
#else 
#define marsyas_EXPORT __declspec(dllimport)
#endif 
#else
#define marsyas_EXPORt
#endif 
#endif 
