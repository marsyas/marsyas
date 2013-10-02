#ifndef COMMON_REG_H
#define COMMON_REG_H

#include <iostream>
#include <cstdlib>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/Collection.h>
#include <marsyas/FileName.h>

#define CLOSE_ENOUGH 0.0001

using namespace std;
using namespace Marsyas;

static MarSystemManager mng;

// really useful global functions
mrs_real addSource(MarSystem* net, string infile);

void addDest(MarSystem* net, string outfile);

#endif

