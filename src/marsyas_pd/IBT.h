#ifndef _MARMAX_IBT_H_
#define _MARMAX_IBT_H_

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iomanip> 

#include "common_source.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"
#include "FileName.h"

#include "EvValUpd.h"
#include "Collection.h"
#include "NumericLib.h"
#include <string.h>

using namespace std;
using namespace Marsyas;

#pragma warning(disable: 4244)  //disable double to float warning
#pragma warning(disable: 4100) //disable argc warning

/**
 * Example plugin that calculates the positions and density of
 * zero-crossing points in an audio waveform.
*/

class MarMaxIBT
{
public:
	MarMaxIBT(mrs_natural winSize, mrs_natural hopSize, mrs_real fs, mrs_real indTime, mrs_natural minBPM, mrs_natural maxBPM, mrs_string outPathName, mrs_bool stateRecovery);
    virtual ~MarMaxIBT();

    MarSystem* createMarsyasNet(void);

protected:
    MarSystemManager mng;
    MarSystem* ibt;
    MarSystem* featureNetwork;
    MarSystem* featExtractor;
	MarSystem* audioflow;
	MarSystem* beattracker;
	mrs_natural inductionTickCount;
};

#endif
