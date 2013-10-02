/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    Vamp

    An API for audio analysis and feature extraction plugins.

    Centre for Digital Music, Queen Mary, University of London.
    Copyright 2006 Chris Cannam.

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

    Except as contained in this notice, the names of the Centre for
    Digital Music; Queen Mary, University of London; and Chris Cannam
    shall not be used in advertising or otherwise to promote the sale,
    use or other dealings in this Software without prior written
    authorization.
*/

#include "vamp/vamp.h"
#include "vamp-sdk/PluginAdapter.h"

#include "ZeroCrossing.h"
#include "MarsyasBExtractZeroCrossings.h"
#include "MarsyasBExtractCentroid.h"
#include "MarsyasBExtractLPCC.h"
#include "MarsyasBExtractLSP.h"
#include "MarsyasBExtractMFCC.h"
#include "MarsyasBExtractRolloff.h"
#include "MarsyasBExtractSCF.h"
#include "MarsyasBExtractSFM.h"
#include "MarsyasIBT.h"

#include <cstdio>
#include "vamp-sdk/Plugin.h"
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
using namespace Marsyas;

static Vamp::PluginAdapter<ZeroCrossing> ZeroCrossingAdapter;
static Vamp::PluginAdapter<MarsyasBExtractZeroCrossings> MarsyasBExtractZeroCrossingsAdapter;
static Vamp::PluginAdapter<MarsyasBExtractCentroid> MarsyasBExtractCentroidAdapter;
static Vamp::PluginAdapter<MarsyasBExtractLPCC> MarsyasBExtractLPCCAdapter;
static Vamp::PluginAdapter<MarsyasBExtractLSP> MarsyasBExtractLSPAdapter;
static Vamp::PluginAdapter<MarsyasBExtractMFCC> MarsyasBExtractMFCCAdapter;
static Vamp::PluginAdapter<MarsyasBExtractRolloff> MarsyasBExtractRolloffAdapter;
static Vamp::PluginAdapter<MarsyasBExtractSCF> MarsyasBExtractSCFAdapter;
static Vamp::PluginAdapter<MarsyasBExtractSFM> MarsyasBExtractSFMAdapter;
static Vamp::PluginAdapter<MarsyasIBT> MarsyasIBT;


const VampPluginDescriptor *vampGetPluginDescriptor(unsigned int version,
    unsigned int index)
{
  if (version < 1) return 0;

  switch (index) {
  case  0: return ZeroCrossingAdapter.getDescriptor();
  case  1: return MarsyasBExtractZeroCrossingsAdapter.getDescriptor();
  case  2: return MarsyasBExtractCentroidAdapter.getDescriptor();
  case  3: return MarsyasBExtractLPCCAdapter.getDescriptor();
  case  4: return MarsyasBExtractLSPAdapter.getDescriptor();
  case  5: return MarsyasBExtractMFCCAdapter.getDescriptor();
  case  6: return MarsyasBExtractRolloffAdapter.getDescriptor();
  case  7: return MarsyasBExtractSCFAdapter.getDescriptor();
  case  8: return MarsyasBExtractSFMAdapter.getDescriptor();
  case  9: return MarsyasIBT.getDescriptor();
  default: return 0;
  }
}

