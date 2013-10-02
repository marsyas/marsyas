//
// mirex_train_and_predict
//
//
//
// Pseudocode :
//
// 1) Read in a training .arff file
//
// 2) Train a SVM classifier with the training data
//
// 3) Read in a test .arff file
//
// 4) Predict the labels of the test data by using the SVM classifier
// built in step 2
//
//

#include <cstdio>
#include <cstring>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>

#include <vector>

using namespace std;
using namespace Marsyas;

void usage()
{
  cout << "mirex_train_and_predict.cpp train.arff test.arff" << endl;
}

void train_and_predict(string trainFileName,string testFileName)
{
  MarSystemManager mng;

  ////////////////////////////////////////////////////////////
  //
  // The network that we will use to train and predict
  //
  MarSystem* net = mng.create("Series", "series");

  ////////////////////////////////////////////////////////////
  //
  // The WekaSource we read the train and test .arf files into
  //
  net->addMarSystem(mng.create("WekaSource", "wsrc"));

  ////////////////////////////////////////////////////////////
  //
  // The classifier
  //
  MarSystem* classifier = mng.create("Classifier", "cl");
  net->addMarSystem(classifier);

  ////////////////////////////////////////////////////////////
  //
  // Which classifier function to use
  //
  string classifier_ = "SVM";
  if (classifier_ == "GS")
    net->updControl("Classifier/cl/mrs_string/enableChild", "GaussianClassifier/gaussiancl");
  if (classifier_ == "ZEROR")
    net->updControl("Classifier/cl/mrs_string/enableChild", "ZeroRClassifier/zerorcl");
  if (classifier_ == "SVM")
    net->updControl("Classifier/cl/mrs_string/enableChild", "SVMClassifier/svmcl");

  ////////////////////////////////////////////////////////////
  //
  // The training file we are feeding into the WekaSource
  //
  net->updControl("WekaSource/wsrc/mrs_string/filename", trainFileName);
  net->updControl("mrs_natural/inSamples", 1);

  ////////////////////////////////////////////////////////////
  //
  // Set the classes of the Summary and Classifier to be
  // the same as the WekaSource
  //
  net->updControl("Classifier/cl/mrs_natural/nClasses", net->getctrl("WekaSource/wsrc/mrs_natural/nClasses"));
  net->updControl("Classifier/cl/mrs_string/mode", "train");

  ////////////////////////////////////////////////////////////
  //
  // Tick over the training WekaSource until all lines in the
  // training file have been read.
  //
  while (!net->getctrl("WekaSource/wsrc/mrs_bool/done")->to<mrs_bool>()) {
    string mode = net->getctrl("WekaSource/wsrc/mrs_string/mode")->to<mrs_string>();
    net->tick();
    net->updControl("Classifier/cl/mrs_string/mode", mode);
  }

//     cout << "------------------------------" << endl;
//     cout << "Class names" << endl;
//     cout << net->getctrl("WekaSource/wsrc/mrs_string/classNames") << endl;
//     cout << "------------------------------\n" << endl;


  vector<string> classNames;
  string s = net->getctrl("WekaSource/wsrc/mrs_string/classNames")->to<mrs_string>();
  char *str = (char *)s.c_str();
  char * pch;
  pch = strtok (str,",");
  classNames.push_back(pch);
  while (pch != NULL) {
    pch = strtok (NULL, ",");
    if (pch != NULL)
      classNames.push_back(pch);
  }

  ////////////////////////////////////////////////////////////
  //
  // Predict the classes of the test data
  //
  net->updControl("WekaSource/wsrc/mrs_string/filename", testFileName);
  net->updControl("Classifier/cl/mrs_string/mode", "predict");

  ////////////////////////////////////////////////////////////
  //
  // Tick over the test WekaSource until all lines in the
  // test file have been read.
  //
  realvec data;
  while (!net->getctrl("WekaSource/wsrc/mrs_bool/done")->to<mrs_bool>()) {
    net->tick();
    data = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    cout << classNames[(int)data(0,0)] << endl;
    //	cout << data(0,0) << endl;
  }

  cout << "DONE" << endl;

  // sness - hmm, I really should be able to delete net, but I get a
  // coredump when I do.  Maybe I need to destroy something else first?
  //  delete net;

}

int main(int argc, const char **argv)
{
  string trainFileName;
  string testFileName;

  if (argc < 3) {
    usage();
    exit(1);
  } else {
    trainFileName = argv[1];
    testFileName = argv[2];
  }

  train_and_predict(trainFileName,testFileName);

  exit(0);

}






