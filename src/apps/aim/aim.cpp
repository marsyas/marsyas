//
// aim - Implementations of code from the AIM-C library including the
// SAI (Stabilized Auditory Image) and SSI (Shape Size Image)
//

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector> 
#include "Collection.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"

using namespace std;
using namespace Marsyas;

//
// Hard-coded options
//
mrs_natural memSize = 40;
mrs_natural winSize = 512;
mrs_natural hopSize = 512;
mrs_natural accSize_ = 1298;
mrs_real samplingRate_ = 22050.0;
mrs_real start = 0.0;
mrs_real length = -1.0;


////////////////////////////////////////
//
// Usage
//
void usage()
{
  cout << "mirex_extract input.mf output.arff" << endl;
}


////////////////////////////////////////
//
// Read in collection (.mf) file
//
void
read_collection(Collection& c, string inCollectionName)
{
  c.read(inCollectionName);

}

void
aim(Collection collection, string outWekaName)
{
  cout << "aim" << endl;
	
  string sfName = collection.entry(0);

  MarSystemManager mng;

  ////////////////////////////////////////////////////////////     
  // The feature extraction network
  ////////////////////////////////////////////////////////////     
  MarSystem* featureNetwork = mng.create("Series", "net");

  featureNetwork->addMarSystem(mng.create("SoundFileSource", "src"));

  featureNetwork->addMarSystem(mng.create("AimGammatone", "aimgammatone"));

  MarSystem* strobe_fanout = mng.create("Fanout", "fanout");
  // strobe_fanout->addMarSystem(mng.create("AimHCL", "aimhcl"));
  strobe_fanout->addMarSystem(mng.create("Gain", "gain"));
  strobe_fanout->addMarSystem(mng.create("AimLocalMax", "aimlocalmax"));

  featureNetwork->addMarSystem(strobe_fanout);

  featureNetwork->addMarSystem(mng.create("AimSAI", "aimsai"));
  featureNetwork->addMarSystem(mng.create("AimSSI", "aimssi"));


  ////////////////////////////////////////////////////////////
  // update controls
  ////////////////////////////////////////////////////////////
  featureNetwork->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  featureNetwork->updctrl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);
  
  ////////////////////////////////////////////////////////////
  // accumulate feature vectors over 30 seconds 
  ////////////////////////////////////////////////////////////
  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->updctrl("mrs_natural/nTimes", accSize_);

  ////////////////////////////////////////////////////////////
  // add network to accumulator
  ////////////////////////////////////////////////////////////
  acc->addMarSystem(featureNetwork->clone());

  ////////////////////////////////////////////////////////////
  // WEKA output
  ////////////////////////////////////////////////////////////
  MarSystem* wsink = mng.create("WekaSink", "wsink");

  ////////////////////////////////////////////////////////////
  // Annotator
  ////////////////////////////////////////////////////////////
  MarSystem* annotator = mng.create("Annotator", "annotator");

  ////////////////////////////////////////////////////////////
  // Generate 30-second statistics 
  ////////////////////////////////////////////////////////////
  MarSystem* statistics = mng.create("Fanout", "statistics2");
  statistics->addMarSystem(mng.create("Mean", "mn"));
  statistics->addMarSystem(mng.create("StandardDeviation", "std"));
   
  ////////////////////////////////////////////////////////////
  // A final network to compute 30-second statistics 
  ////////////////////////////////////////////////////////////
  MarSystem* total = mng.create("Series", "total");
  total->addMarSystem(acc);
  total->addMarSystem(statistics);

  //////////////////////////////////////////////////////////////////////////
  // Main loop for extracting the features 
  //////////////////////////////////////////////////////////////////////////

  mrs_natural wc = 0;
  mrs_natural samplesPlayed =0;
  realvec in;
  realvec timbreres;
  realvec fullres;
  realvec afullres;

  timbreres.create(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), 
 				   total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  in.create(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), 
 			total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  fullres.create(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), 
 				 total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  afullres.create(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>() + 1,
 				  total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());  

  annotator->updctrl("mrs_natural/inObservations", total->getctrl("mrs_natural/onObservations")->to<mrs_natural>());      

  annotator->updctrl("mrs_natural/inSamples", total->getctrl("mrs_natural/onSamples"));
  annotator->updctrl("mrs_real/israte", total->getctrl("mrs_real/israte"));

  wsink->updctrl("mrs_natural/inSamples", annotator->getctrl("mrs_natural/onSamples"));
  wsink->updctrl("mrs_natural/inObservations", annotator->getctrl("mrs_natural/onObservations")->to<mrs_natural>());
  wsink->updctrl("mrs_real/israte", annotator->getctrl("mrs_real/israte"));

  mrs_natural timbreSize = total->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
  annotator->updctrl("mrs_string/inObsNames", total->getctrl("mrs_string/onObsNames"));  

  if (outWekaName != EMPTYSTRING)
  	wsink->updctrl("mrs_string/inObsNames", annotator->getctrl("mrs_string/onObsNames"));

  ////////////////////////////////////////////////////////////
  // Iterate over all files in collection
  ////////////////////////////////////////////////////////////
  for (size_t i=0; i < collection.size(); ++i)
  {
    // Update the featureNetwork to read the current file in the collection
    total->updctrl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_string/filename", collection.entry(i));
    wc = 0;  	  
    samplesPlayed = 0;
    //   	  annotator->updctrl("mrs_natural/label", collection.labelNum(collection.labelEntry(i)));
    annotator->updctrl("mrs_natural/label", (mrs_natural)i);

    // 	  cout << "collection.labelNum(collection.labelEntry(i))" << collection.labelNum(collection.labelEntry(i)) << endl;

    total->process(in, timbreres);
	  
    // concatenate timbre and beat vectors 
    for (int t=0; t < timbreSize; t++)
      fullres(t,0) = timbreres(t,0);

    annotator->process(fullres, afullres);
    wsink->process(afullres, afullres);

    // 	  cout << *annotator << endl;

    // 	  for (int j=0; j < fullres.getRows(); j++) {
    // 		outstream << fullres(j,0);
    // 		if (j < fullres.getRows() - 1) {
    // 		  outstream << ",";
    // 		}
    // 	  }
    // 	  outstream << "\t" << collection.entry(i) << endl;
		
    cerr << "Processed " << collection.entry(i) << endl;
  }
	
  delete featureNetwork;

  ////////////////////////////////////////////////////////////
  // Get parameters
  ////////////////////////////////////////////////////////////
  // total->updctrl("mrs_natural/inSamples", winSize);


  // cout << "UPDATE" << endl;

  // net->updctrl("SoundFileSource/src/mrs_string/filename", sfName);

  // // cout << *net;

  // while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>()) 
  // {
  //   // cout << "tik tok" << endl;
  //   net->tick();
  //   // cout << "AFTER" << endl;
  //   // cout << *net;

  //   cout << net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
  // }
  // delete net;
}

  

int
main(int argc, const char **argv)
{
  string inCollectionName;
  string outfname;


  if (argc < 3) {
	usage();
	exit(1);
  } else {
	inCollectionName = argv[1];
	outfname = argv[2];
  }

  Collection c;

  read_collection(c,inCollectionName);
  aim(c,outfname);

  exit(0);

}
