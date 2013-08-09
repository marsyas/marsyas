

#ifndef MARPD__H
#define MARPD__H


#include "IBT.h" //header + implementation
#include <iostream>



// define attributes for explicit export of symbols:


#if defined MSW										// when compiling for Windows	
#define EXPORT __declspec(dllexport)
#elif __GNUC__ >= 4										// else, when compiling with GCC 4.0 or higher
#define EXPORT __attribute__((visibility("default")))
#endif
#ifndef EXPORT
#define EXPORT											// empty definition for other cases
#endif


#include <m_pd.h>

// declaration of exported function:

extern "C" EXPORT void ibt_pd_setup(void);

realvec r; //hopsize = 512

#endif