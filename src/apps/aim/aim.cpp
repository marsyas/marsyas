//
// aim - Implementations of code from the AIM-C library including the
// SAI (Stabilized Auditory Image) and SSI (Shape Size Image)
//

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/common_source.h>

using namespace std;
using namespace Marsyas;

//
// Hard-coded options
//
mrs_natural offset = 0;
mrs_real duration = 30.0f;
mrs_natural memSize = 40;
mrs_natural winSize = 512;
mrs_natural hopSize = 512;
// mrs_natural accSize_ = 1298;
// hmm, bextract must default to stereo now.  changes are fun. :P
mrs_natural accSize_ = 649;
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
aim(string pluginName, string wekafname,
    mrs_natural memSize, string classifierName,
    mrs_bool single_vector)
{
  (void) pluginName;
  (void) memSize;
  (void) classifierName;
  (void) single_vector;
  MarSystemManager mng;

  // Overall extraction and classification network
  MarSystem* bextractNetwork = mng.create("Series", "bextractNetwork");

  // Build the overall feature calculation network
  MarSystem* featureNetwork = mng.create("Series", "featureNetwork");

  // Add a fanout for sound file and audio source ...
  // sness - Now superfluous
  MarSystem *fanout = mng.create("Fanout", "fanout");

  // Add a sound file source (which can also read collections)
  MarSystem *src = mng.create("SoundFileSource", "src");
  fanout->addMarSystem(src);

  // Add the fanout to our feature Network ...
  featureNetwork->addMarSystem(fanout);

  featureNetwork->addMarSystem(mng.create("AimPZFC", "aimpzfc"));
  featureNetwork->addMarSystem(mng.create("AimHCL", "aimhcl"));
  featureNetwork->addMarSystem(mng.create("AimLocalMax", "aimlocalmax"));
  featureNetwork->addMarSystem(mng.create("AimSAI", "aimsai"));
  featureNetwork->addMarSystem(mng.create("AimBoxes", "aimBoxes"));
  featureNetwork->addMarSystem(mng.create("AimVQ", "aimvq"));
  // featureNetwork->addMarSystem(mng.create("Unfold", "unfold"));

  // featureNetwork->addMarSystem(mng.create("Centroid", "centroid"));
  // featureNetwork->addMarSystem(mng.create("TextureStats", "texturestats"));

  // // sness - A little hack to take the output and figure out the
  // // amount of signal in each channel
  // featureNetwork->addMarSystem(mng.create("MaxMin", "maxmin"));
  // featureNetwork->addMarSystem(mng.create("Square", "square"));

  cout << "accSize_ = " << accSize_ << endl;

  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->updControl("mrs_natural/nTimes", accSize_);
  acc->addMarSystem(featureNetwork);
  bextractNetwork->addMarSystem(acc);

  // MarSystem* song_statistics = mng.create("Fanout", "song_statistics");
  // song_statistics->addMarSystem(mng.create("Mean", "mn"));
  // song_statistics->addMarSystem(mng.create("StandardDeviation", "std"));
  // bextractNetwork->addMarSystem(song_statistics);

  bextractNetwork->addMarSystem(mng.create("Sum", "sum"));
  bextractNetwork->updControl("Sum/sum/mrs_string/mode", "sum_observations");
  // bextractNetwork->addMarSystem(mng.create("Normalize", "normalize"));

  bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/filename",
                               "mrs_string/filename"); // added Fanout ...
  bextractNetwork->linkControl("mrs_bool/hasData",
                               "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/hasData"); // added Fanout ...
  bextractNetwork->linkControl("mrs_natural/pos",
                               "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/pos"); // added Fanout ...
  bextractNetwork->linkControl("mrs_real/duration",
                               "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_real/duration"); // added Fanout ...

  // if (pluginName != EMPTYSTRING)
  //   bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/AudioSink/dest/mrs_bool/initAudio",
  //                             "mrs_bool/initAudio");
  bextractNetwork->linkControl("mrs_string/currentlyPlaying",
                               "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/currentlyPlaying"); // added Fanout ...

  bextractNetwork->linkControl("mrs_natural/currentLabel",
                               "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/currentLabel");
  bextractNetwork->linkControl("mrs_natural/nLabels",
                               "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/nLabels");
  bextractNetwork->linkControl("mrs_string/labelNames",
                               "Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/labelNames");

  bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/advance",
                               "mrs_natural/advance");

  // labeling, weka output, classifier and confidence for real-time output
  bextractNetwork->addMarSystem(mng.create("Annotator", "annotator"));
  bextractNetwork->addMarSystem(mng.create("WekaSink", "wsink"));

  bextractNetwork->linkControl("Annotator/annotator/mrs_natural/label",
                               "mrs_natural/currentLabel");

  bextractNetwork->linkControl("WekaSink/wsink/mrs_string/currentlyPlaying",
                               "mrs_string/currentlyPlaying");

  bextractNetwork->linkControl("WekaSink/wsink/mrs_string/labelNames",
                               "mrs_string/labelNames");
  bextractNetwork->linkControl("WekaSink/wsink/mrs_natural/nLabels", "mrs_natural/nLabels");

  // src has to be configured with hopSize frame length in case a ShiftInput
  // is used in the feature extraction network
  bextractNetwork->updControl("mrs_natural/inSamples", hopSize);

  // int offset = 0;
  // if (start > 0.0)
  //   offset = (mrs_natural) (start * src->getctrl("mrs_real/israte")->to<mrs_real>());
  // bextractNetwork->updControl("mrs_natural/pos", offset);
  // bextractNetwork->updControl("mrs_real/duration", length);

  // // load the collection which is automatically created by bextract
  // // based on the command-line arguments
  // string workspaceDir = "";
  // if (workspaceDir != EMPTYSTRING)
  //   bextractNetwork->updControl("mrs_string/filename", workspaceDir + "bextract_single.mf");
  // else
  bextractNetwork->updControl("mrs_string/filename", "bextract_single.mf");

  // // play sound if playback is enabled
  // bool playback = false;
  // if (pluginName != EMPTYSTRING && playback)
  // {
  //   featureNetwork->updControl("AudioSink/dest/mrs_bool/mute", false);
  //   featureNetwork->updControl("mrs_bool/initAudio", true);
  // }

  bextractNetwork->updControl("WekaSink/wsink/mrs_string/filename", wekafname);

  // main processing loop for training
  MarControlPtr ctrl_hasData = bextractNetwork->getctrl("mrs_bool/hasData");
  MarControlPtr ctrl_currentlyPlaying = bextractNetwork->getctrl("mrs_string/currentlyPlaying");
  mrs_string previouslyPlaying, currentlyPlaying;

  int n = 0;
  int advance = 1;

  vector<string> processedFiles;
  map<string, realvec> processedFeatures;

  bool seen;
  realvec fvec;
  int label;

  while (ctrl_hasData->to<mrs_bool>()) {

    currentlyPlaying = ctrl_currentlyPlaying->to<mrs_string>();

    label = bextractNetwork->getctrl("mrs_natural/currentLabel")->to<mrs_natural>();
    cout << "currentlyPlaying=" << currentlyPlaying << endl;
    cout << "label=" << label << endl;

    seen = false;

    for (size_t j=0; j<processedFiles.size(); j++)
    {
      if (processedFiles[j] == currentlyPlaying)
        seen = true;
    }

    if (seen)
    {
      advance++;
      bextractNetwork->updControl("mrs_natural/advance", advance);

      if (wekafname != EMPTYSTRING)
        bextractNetwork->updControl("WekaSink/wsink/mrs_string/injectComment", "% filename " + currentlyPlaying);

      fvec = processedFeatures[currentlyPlaying];
      fvec(fvec.getSize()-1) = label;

      if (wekafname != EMPTYSTRING)
      {
        bextractNetwork->updControl("WekaSink/wsink/mrs_realvec/injectVector", fvec);

        bextractNetwork->updControl("WekaSink/wsink/mrs_bool/inject", true);
      }
    }
    else
    {
      bextractNetwork->tick();
      // cout << *bextractNetwork << endl;

      fvec = bextractNetwork->getctrl("Annotator/annotator/mrs_realvec/processedData")->to<mrs_realvec>();

      bextractNetwork->updControl("mrs_natural/advance", advance);
      processedFiles.push_back(currentlyPlaying);
      processedFeatures[currentlyPlaying] = fvec;
      cout << "Processed: " << n << " - " << currentlyPlaying << endl;
      advance = 1;
      bextractNetwork->updControl("mrs_natural/advance", 1);
    }
    n++;
  }

  delete bextractNetwork;
  return;

}

void
readCollection(Collection& l, string name)
{
  MRSDIAG("sfplay.cpp - readCollection");
  ifstream from1(name.c_str());
  mrs_natural attempts=0;

  MRSDIAG("Trying current working directory: " + name);
  if (from1.good() == false)
  {
    attempts++;
  }
  else
  {
    from1 >> l;
    l.setName(name.substr(0, name.rfind(".", name.length())));
  }

  if (attempts == 1)
  {
    string warn;
    warn += "Problem reading collection ";
    warn += name;
    warn += " - tried both default mf directory and current working directory";
    MRSWARN(warn);
    exit(1);
  }
}


int
main(int argc, const char **argv)
{
  (void) argc;
  int i = 0;
  Collection l;

  string classNames = "";
  vector<Collection> cls;

  // vector<string> soundfiles = argv[1];
  // vector<string>::iterator sfi;

  // for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
  // {
  //   string sfname = *sfi;
  // Collection l;
  readCollection(l,argv[1]);

  if (!l.hasLabels())
  {
    l.labelAll(l.name());
    classNames += (l.name()+',');
  }


  cls.push_back(l);
  ++i;
  // }

  Collection single;
  single.concatenate(cls);
  if (single.getSize() == 0)
  {
    MRSERR("Collection has no files  - exiting");
    exit(1);
  }

  // if (shuffle_)
  //   single.shuffle();

  // if (workspaceDir != EMPTYSTRING)
  //   single.write(workspaceDir + "bextract_single.mf");
  // else
  single.write("bextract_single.mf");

  // read_collection(c,inCollectionName);
  // bextract_train_refactored(pluginName, wekafname, memSize, classifierName, single_vector_);
  aim("MARSYAS_EMPTY",argv[2],40,"SVM",1);


  exit(0);

}
