#ifndef _MARMAX_H
#define _MARMAX_H

#include "IBT~.h" //header + implementation
#include "ext.h"							// standard Max include, always required (except in Jitter)
#include "ext_obex.h"						// required for new style objects
#include "z_dsp.h"						// required for MSP objects
#include <iostream>

using namespace std;

/*
// a macro to mark exported symbols in the code without requiring an external file to define them
#ifdef WIN_VERSION
	// note that this the required syntax on windows regardless of whether the compiler is msvc or gcc
	#define T_EXPORT __declspec(dllexport)
#else // MAC_VERSION
	// the mac uses the standard gcc syntax, you should also set the -fvisibility=hidden flag to hide the non-marked symbols
	#define T_EXPORT __attribute__((visibility("default")))
#endif
*/

////////////////////////// object struct
typedef struct _MarMax
{
  t_pxobject					ob;			// the object itself (t_pxobject in MSP)
  float						offset;
  double	d_SR;
  void* outletBeat;
  void* outletTempo;
  MarMaxIBT *ibt;
  //Ctestdll testdll;
  MarSystem* m_MarsyasNetwork;
  t_int pos;                    /*frames%dspblocksize */
  t_int bufsize;
  t_int hopsize;
  t_float inductionTime;
  t_int minBPM;
  t_int maxBPM;
  bool stateRecovery;
  char *outPathName;
  //unused for now
  t_int onlineFlag;
  t_int metricalChangesFlag;
} t_MarMax;

///////////////////////// function prototypes
//// standard set
void *MarMax_new(t_symbol *s, long argc, t_atom *argv);
void MarMax_free(t_MarMax *x);
void MarMax_assist(t_MarMax *x, void *b, long m, long a, char *s);
void MarMax_setIBTDefaultParams(t_MarMax *x);
void MarMax_float(t_MarMax *x, double f);

void MarMax_dsp(t_MarMax *x, t_signal **sp, short *count);
t_int *MarMax_perform(t_int *w);
//////////////////////// global class pointer variable
void *MarMax_class;

realvec r; //hopsize = 512

#endif