//
// mirex_extract
//
// Extract all the SVSTFTMFCC features from the files in
// "collection.mf" and output them to a .txt file of format
//
//   feature1.1,feature1.2,feature1.3\t/path/to/filename1.wav
//   feature2.1,feature2.2,feature2.3\t/path/to/filename2.wav
//
// *NB* : \t is the [TAB] character
//

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>

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

// map table with pointers to the functions that create
// each specific and supported feature extraction MarSystem
typedef MarSystem* (*FeatureExtractorFactory)();
map<string,FeatureExtractorFactory >  featureExtractors;
map<string, string> featureExtractorDesc;

MarSystem* createSTFTMFCCextractor()
{
  MarSystemManager mng;

  MarSystem* extractor = mng.create("Series", "STFTMFCCextractor");
  extractor->addMarSystem(mng.create("PowerSpectrumNet","powerSpect"));
  // Spectrum Shape descriptors
  MarSystem* spectrumFeatures = mng.create("Fanout", "spectrumFeatures");
  spectrumFeatures->addMarSystem(mng.create("Centroid", "cntrd"));
  spectrumFeatures->addMarSystem(mng.create("Rolloff", "rlf"));
  spectrumFeatures->addMarSystem(mng.create("Flux", "flux"));
  spectrumFeatures->addMarSystem(mng.create("MFCC", "mfcc"));
  extractor->addMarSystem(spectrumFeatures);
  extractor->linkControl("mrs_natural/winSize", "PowerSpectrumNet/powerSpect/mrs_natural/winSize");

  return extractor;
}

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

////////////////////////////////////////
//
// Extract features from a single file
//
void extract(Collection collection, string outWekaName)
// void bextract_trainAccumulator(vector<Collection> cls, mrs_natural label,
// 							   string pluginName, string classNames,
// 							   string wekafname,
// 							   mrs_natural memSize, string extractorStr,
// 							   bool withBeatFeatures)
{

  // open output file
//   ofstream outstream(outfname.c_str());
//   if (!outstream) {
// 	cerr << "Could not open output file (" << outfname << ")" << endl;
// 	exit(0);
//   }

  string sfName = collection.entry(0);

  ////////////////////////////////////////////////////////////
  // Create a MarSystemManager to run the show
  ////////////////////////////////////////////////////////////
  MarSystemManager mng;

  ////////////////////////////////////////////////////////////
  // create the file source
  ////////////////////////////////////////////////////////////
  MarSystem* src = mng.create("SoundFileSource", "src");

  ////////////////////////////////////////////////////////////
  // Feature Extractor : create the correct feature extractor
  // using the table of known feature extractors.
  ////////////////////////////////////////////////////////////
  string extractorStr = "STFTMFCC";
  MarSystem* featureExtractor = (*featureExtractors[extractorStr])();
  featureExtractor->updControl("mrs_natural/winSize", winSize);

  ////////////////////////////////////////////////////////////
  // Build the overall feature calculation network
  ////////////////////////////////////////////////////////////
  MarSystem* featureNetwork = mng.create("Series", "featureNetwork");
  featureNetwork->addMarSystem(src);

  ////////////////////////////////////////////////////////////
  // convert stereo files to mono
  ////////////////////////////////////////////////////////////
  featureNetwork->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  featureNetwork->addMarSystem(featureExtractor);

  ////////////////////////////////////////////////////////////
  // Texture Window Statistics
  ////////////////////////////////////////////////////////////
  featureNetwork->addMarSystem(mng.create("TextureStats", "tStats"));
  featureNetwork->updControl("TextureStats/tStats/mrs_natural/memSize", memSize);

  ////////////////////////////////////////////////////////////
  // update controls
  ////////////////////////////////////////////////////////////
  featureNetwork->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  featureNetwork->updControl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);

  ////////////////////////////////////////////////////////////
  // accumulate feature vectors over 30 seconds
  ////////////////////////////////////////////////////////////
  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->updControl("mrs_natural/nTimes", accSize_);

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

  ////////////////////////////////////////////////////////////
  // Get parameters
  ////////////////////////////////////////////////////////////
  total->updControl("mrs_natural/inSamples", winSize);

  //////////////////////////////////////////////////////////////////////////
  // Main loop for extracting the features
  //////////////////////////////////////////////////////////////////////////
  //mrs_natural wc = 0;
  //mrs_natural samplesPlayed =0;
  string className = "";
  realvec beatfeatures;
  beatfeatures.create((mrs_natural)8,(mrs_natural)1);
  realvec estimate;
  estimate.create((mrs_natural)8,(mrs_natural)1);
  realvec in;
  realvec timbreres;
  realvec fullres;
  realvec afullres;

  in.create(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
            total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  timbreres.create(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                   total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  fullres.create(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                 total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  afullres.create(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>() + 1,
                  total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  annotator->updControl("mrs_natural/inObservations", total->getctrl("mrs_natural/onObservations")->to<mrs_natural>());

  annotator->updControl("mrs_natural/inSamples", total->getctrl("mrs_natural/onSamples"));
  annotator->updControl("mrs_real/israte", total->getctrl("mrs_real/israte"));

  wsink->updControl("mrs_natural/inSamples", annotator->getctrl("mrs_natural/onSamples"));
  wsink->updControl("mrs_natural/inObservations", annotator->getctrl("mrs_natural/onObservations")->to<mrs_natural>());
  wsink->updControl("mrs_real/israte", annotator->getctrl("mrs_real/israte"));

  mrs_natural timbreSize = total->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
  annotator->updControl("mrs_string/inObsNames", total->getctrl("mrs_string/onObsNames"));

  if (outWekaName != EMPTYSTRING)
    wsink->updControl("mrs_string/inObsNames", annotator->getctrl("mrs_string/onObsNames"));


  realvec iwin;

  ////////////////////////////////////////////////////////////
  // Assign the name of each file as the label of the
  // extracted feature.  This lets us keep track of which
  // feature corresponds to which label
  ////////////////////////////////////////////////////////////
  string all_files_in_collection = "";
  for (mrs_natural i = 0; i < collection.getSize(); ++i) {
    all_files_in_collection += collection.entry(i);
    if (i < collection.getSize() - 1)
      all_files_in_collection += ",";
  }
  cout << "all_files_in_collection=" << all_files_in_collection << endl;
  cout << "collection.getSize()=" << collection.getSize() << endl;

  wsink->updControl("mrs_string/labelNames",all_files_in_collection);
  wsink->updControl("mrs_natural/nLabels", (mrs_natural)collection.getSize());
  wsink->updControl("mrs_string/filename", outWekaName);
  cout << "Writing weka .arff file to :" << outWekaName << endl;

  cout << "------------------------------" << endl;
  cout << "Label names" << endl;
  cout << wsink->getctrl("mrs_string/labelNames")->to<mrs_string>() << endl;
  cout << "------------------------------\n" << endl;

  ////////////////////////////////////////////////////////////
  // Iterate over all files in collection
  ////////////////////////////////////////////////////////////
  for (mrs_natural i=0; i < collection.size(); ++i)
  {
    // Update the featureNetwork to read the current file in the collection
    total->updControl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_string/filename", collection.entry(i));
    //wc = 0;
    //samplesPlayed = 0;
//   	  annotator->updControl("mrs_natural/label", collection.labelNum(collection.labelEntry(i)));
    annotator->updControl("mrs_real/label", (mrs_real)i);

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

  featureExtractors["STFTMFCC"] = &createSTFTMFCCextractor;
  featureExtractorDesc["STFTMFCC"] = "Centroid, Rolloff Flux, ZeroCrossings, Mel-frequency Cepstral Coefficients";

  read_collection(c,inCollectionName);
  extract(c,outfname);

  exit(0);

}
