#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/common_source.h>
#include <marsyas/FileName.h>

#include <vector>

using namespace std;
using namespace Marsyas;

/* global variables for various commandline options */
int helpopt_;
int usageopt_;
string train_weka_fname_;
string test_weka_fname_;

string predictcollectionfname_;
string predicttimeline_;

string mode_;
CommandLineOptions cmd_options_;
string inputdir_;
string outputdir_;
string distancematrix_;
string classifier_;
mrs_real minspan_;
mrs_string trainedclassifier_;
mrs_string svm_svm_;
mrs_string svm_kernel_;
mrs_string label_;
mrs_natural svm_gamma_;
mrs_natural ncomponents_;
mrs_real svm_C_;


int
printUsage(string progName)
{
  MRSDIAG("kea.cpp - printUsage");
  cerr << "Usage : " << progName << " [-m mode -c classifier -id inputdir -od outputdir -w weka file -tw test_weka file -tc test collection file -pr predict collection file] " << endl;
  cerr << endl;
  return 1;
}

int
printHelp(string progName)
{
  MRSDIAG("kea.cpp - printHelp");
  cerr << "kea, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Kea - machine learning ala Weka " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  cerr << "-w --wekafname : .arff file for training " << endl;
  cerr << "-tw --testwekafname: .arff file for testing " << endl;
  cerr << "-cl --classifier : classifier to use " << endl;
  cerr << "-m --mode: mode of operation" << endl;
  cerr << "-id --inputdir: input directory" << endl;
  cerr << "-od --outputdir: output directory" << endl;
  cerr << "-dm --distancematrix: distance matrix in MIREX format" << endl;
  cerr << "-pr --predictcollectionfname : .mf output prediction file " << endl;
  cerr << "-prtl --predicttimeline: predicted timeline" << endl;
  cerr << "-msp  --minspan: minimum duration of predicted timeline region" << endl;
  cerr << "-trcl --trainedclassifier: plugin file for storing the trained classifier" << endl;
  cerr << "-ss --svm_svm: parameters for libsvm" <<endl;
  cerr << "-sk --svm_kernel: parameters for libsvm" <<endl;
  cerr << "-sC --svm_C: cost parameter for libsvm" <<endl;
  cerr << "-sG --svm_gamma: cost parameter for libsvm" <<endl;
  return 1;
}

bool
twekafname_Set()
{
  if (test_weka_fname_ == EMPTYSTRING)
  {
    MRSERR("No test .arff file specified. Use the -tw option");
    cout << "No test .arff file specified. Use the -tw option" << endl;
    return false;
  }
  return true;
}

bool
wekafname_Set()
{
  if (train_weka_fname_ == EMPTYSTRING)
  {
    cout << "Weka .arff file not specified" << endl;
    return false;
  }
  return true;
}

void
distance_matrix_MIREX()
{
  if (!wekafname_Set()) return;

  string train_file_path =
      FileName(inputdir_).append(train_weka_fname_).fullname();
  string predict_collection_path =
      FileName(inputdir_).append(predictcollectionfname_).fullname();

  cout << "Distance matrix calculation using " << train_file_path << endl;

  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");
  MarSystem* accum = mng.create("Accumulator", "accum");
  MarSystem* wsrc = mng.create("WekaSource", "wsrc");
  accum->addMarSystem(wsrc);
  accum->updControl("WekaSource/wsrc/mrs_bool/normMaxMin", true);
  accum->updControl("WekaSource/wsrc/mrs_string/filename", train_file_path);
  mrs_natural nInstances =
    accum->getctrl("WekaSource/wsrc/mrs_natural/nInstances")->to<mrs_natural>();
  accum->updControl("mrs_natural/nTimes", nInstances);



  MarSystem* dmatrix = mng.create("SelfSimilarityMatrix", "dmatrix");
  dmatrix->addMarSystem(mng.create("Metric", "dmetric"));
  dmatrix->updControl("Metric/dmetric/mrs_string/metric", "euclideanDistance");

  net->addMarSystem(accum);
  net->addMarSystem(dmatrix);

  net->tick();

  string out_file_name = FileName(outputdir_).append(distancematrix_).fullname();
  ofstream oss(out_file_name.c_str());
  if (!oss.is_open())
  {
    MRSERR("Failed to open output file: " << out_file_name);
    return;
  }

  oss << "Marsyas-kea distance matrix for MIREX 2007 Audio Similarity Exchange " << endl;


  // collection simply for naming the entries
  Collection l;
  l.read(predict_collection_path);
  for (mrs_natural i=1; i <= l.size(); ++i)
  {
    oss << i << "\t" << l.entry(i-1) << endl;
  }


  oss << "Q/R";
  const mrs_realvec& dmx = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

  for (int i=1; i <= nInstances; ++i)
  {
    oss << "\t" << i;
  }
  oss << endl;

  for (int i=1; i <= nInstances; ++i)
  {
    oss << i;
    for (int j=0; j < nInstances; j++)
      oss <<"\t" << dmx(i-1, j);
    oss << endl;
  }

  oss << endl;
}

void
distance_matrix()
{
  if (!wekafname_Set()) return;

  string train_file_path =
      FileName(inputdir_).append(train_weka_fname_).fullname();

  cout << "Distance matrix calculation using " << train_file_path << endl;

  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");

  MarSystem* wsrc = mng.create("WekaSource", "wsrc");
  net->addMarSystem(wsrc);
  //!!!: mode control
  net->updControl("WekaSource/wsrc/mrs_string/validationMode", "OutputInstancePair");
  net->updControl("WekaSource/wsrc/mrs_bool/normMaxMin", true);
  net->updControl("WekaSource/wsrc/mrs_string/filename", train_file_path);


  MarSystem* dmatrix = mng.create("SelfSimilarityMatrix", "dmatrix");
  dmatrix->addMarSystem(mng.create("Metric", "dmetric"));
  dmatrix->updControl("Metric/dmetric/mrs_string/metric", "euclideanDistance");
  //!!!: lmartins: normalization can only be applied when we have all feature vectors in memory...
  //... which is what we are trying to avoid here (having big realvecs in memory)...
  //dmatrix->updControl("mrs_string/normalize", "MinMax");
  net->addMarSystem(dmatrix);
  //!!!: mode control
  net->updControl("SelfSimilarityMatrix/dmatrix/mrs_natural/mode", 1); //FIXME: replace use of enum for strings?

  //link controls between WekaSource and SelfSimilarityMatrix
  net->linkControl("SelfSimilarityMatrix/dmatrix/mrs_natural/nInstances",
                   "WekaSource/wsrc/mrs_natural/nInstances");
  net->linkControl("WekaSource/wsrc/mrs_realvec/instanceIndexes",
                   "SelfSimilarityMatrix/dmatrix/mrs_realvec/instanceIndexes");

  string out_file_name = FileName(outputdir_).append(distancematrix_).fullname();
  ofstream oss(out_file_name.c_str());
  if (!oss.is_open())
  {
    MRSERR("Failed to open output file: " << out_file_name);
    return;
  }

  oss << "Marsyas-kea distance matrix" << endl;

  while(!net->getctrl("SelfSimilarityMatrix/dmatrix/mrs_bool/done")->to<bool>())
  {
    const mrs_realvec& idxs = net->getctrl("SelfSimilarityMatrix/dmatrix/mrs_realvec/instanceIndexes")->to<mrs_realvec>();
    oss << "(" << mrs_natural(idxs(0)) << "," << mrs_natural(idxs(1)) << ") = ";

    net->tick();

    const mrs_realvec& value = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    oss << value(0) << endl;
  }

  oss << endl;
}


void
pca()
{
  cout << "Principal Component Analysis of .arff file" << endl;

  if (!wekafname_Set()) return;

  string train_file_path =
      FileName(inputdir_).append(train_weka_fname_).fullname();

  cout << "PCA using .arff file: " << train_file_path << endl;

  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");
  MarSystem* accum = mng.create("Accumulator", "accum");
  MarSystem* wsrc = mng.create("WekaSource", "wsrc");
  accum->addMarSystem(wsrc);
  accum->updControl("WekaSource/wsrc/mrs_string/filename", train_file_path);
  mrs_natural nInstances =
    accum->getctrl("WekaSource/wsrc/mrs_natural/nInstances")->to<mrs_natural>();
  cout << "nInstances = " << nInstances << endl;
  accum->updControl("mrs_natural/nTimes", nInstances);

  net->addMarSystem(accum);
  net->addMarSystem(mng.create("PCA", "pca"));
  net->addMarSystem(mng.create("NormMaxMin", "norm"));
  net->addMarSystem(mng.create("WekaSink", "wsink"));

  net->updControl("PCA/pca/mrs_natural/npc", ncomponents_);
  net->updControl("NormMaxMin/norm/mrs_natural/ignoreLast", 1);
  net->updControl("NormMaxMin/norm/mrs_string/mode", "twopass");
  net->updControl("NormMaxMin/norm/mrs_real/lower", 0.0);
  net->updControl("NormMaxMin/norm/mrs_real/upper", 11.0); 

  net->updControl("WekaSink/wsink/mrs_natural/nLabels",
                  net->getctrl("Accumulator/accum/WekaSource/wsrc/mrs_natural/nClasses"));
  net->updControl("WekaSink/wsink/mrs_string/labelNames", net->getctrl("Accumulator/accum/WekaSource/wsrc/mrs_string/classNames"));
  net->updControl("WekaSink/wsink/mrs_string/filename", "pca_out.arff");

  net->tick();
  return;

  // the output of the PCA
  const mrs_realvec& pca_transformed_data = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

  cout << "Output transformed features using PCA" << endl;

  string classNames = net->getctrl("Accumulator/accum/WekaSource/wsrc/mrs_string/classNames")->to<mrs_string>();
  vector<string> labelNames;

  labelNames.clear();

  for (int i = 0; i < net->getctrl("Accumulator/accum/WekaSource/wsrc/mrs_natural/nClasses")->to<mrs_natural>(); ++i)
  {
    string labelName;
    string temp;

    labelName = classNames.substr(0, classNames.find(","));
    temp = classNames.substr(classNames.find(",")+1, classNames.length());
    classNames = temp;
    labelNames.push_back(labelName);
  }


  cout << "12" << endl;
  cout << "12" << endl;

  for (int t=0; t < pca_transformed_data.getCols(); t++)
  {

    cout << (int)pca_transformed_data(0,t) * 12 + (int)pca_transformed_data(1,t) << ",";
    cout << labelNames[(int)pca_transformed_data(2,t)];
    cout << endl;

    // cout << (int)pca_transformed_data(0,t) << "\t";
    // cout << (int)pca_transformed_data(1,t) << "\t";
    // cout << (int)pca_transformed_data(2,t) << "\t";
    // cout << pca_transformed_data(3,t) << "\t";
    // cout << endl;
  }





}


void
train_classifier()
{

  if (!wekafname_Set()) return;

  string train_file_path =
      FileName(inputdir_).append(train_weka_fname_).fullname();

  cout << "Training classifier using .arff file: " << train_file_path << endl;
  cout << "Classifier type : " << classifier_ << endl;

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
  net->updControl("WekaSource/wsrc/mrs_string/filename", train_file_path);
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


  string out_file_name = FileName(outputdir_).append(trainedclassifier_).fullname();
  ofstream clout(out_file_name.c_str());
  if (!clout.is_open())
  {
    MRSERR("Failed to open output file: " << out_file_name);
    return;
  }

  net->updControl("Classifier/cl/mrs_string/mode", "predict");

  clout << *net << endl;

  cout << "Done training " << endl;
}


void
predict(mrs_string mode)
{

  MarSystemManager mng;

  string classifier_file_path =
      FileName(inputdir_).append(trainedclassifier_).fullname();

  cout << "Predicting using " << classifier_file_path << endl;

  ifstream pluginStream(classifier_file_path.c_str());
  if (!pluginStream.is_open())
  {
    MRSERR("Failed to open input file: " << classifier_file_path);
    return;
  }

  MRS_WARNINGS_OFF;
  MarSystem* net = mng.getMarSystem(pluginStream);
  MRS_WARNINGS_ON;

  if (!twekafname_Set()) return;

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
  net->updControl("WekaSource/wsrc/mrs_string/filename", test_weka_fname_);
  net->updControl("Classifier/cl/mrs_string/mode", "predict");
  ////////////////////////////////////////////////////////////
  //
  // Tick over the test WekaSource until all lines in the
  // test file have been read.
  //




  string pr_out_path = FileName(outputdir_).append(predictcollectionfname_).fullname();
  string prt_out_path = FileName(outputdir_).append(predicttimeline_).fullname();

  ofstream prout(pr_out_path.c_str());
  if (!prout.is_open())
  {
    MRSERR("Failed to open output file: " << pr_out_path);
    return;
  }

  ofstream prtout(prt_out_path.c_str());
  if (!prtout.is_open())
  {
    MRSERR("Failed to open output file: " << prt_out_path);
    return;
  }


  realvec data;
  int end=0;
  int start=0;

  mrs_string prev_name = "";
  mrs_string name;

  mrs_real srate;




  while (!net->getctrl("WekaSource/wsrc/mrs_bool/done")->to<mrs_bool>()) {
    net->tick();
    data = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    srate = net->getctrl("WekaSource/wsrc/mrs_real/currentSrate")->to<mrs_real>();


    if (mode == "default")
    {
      cout << net->getctrl("WekaSource/wsrc/mrs_string/currentFilename")->to<mrs_string>() << "\t";
      cout << classNames[(int)data(0,0)] << endl;
      prout << net->getctrl("WekaSource/wsrc/mrs_string/currentFilename")->to<mrs_string>() << "\t";
      prout << classNames[(int)data(0,0)] << endl;
    }
    else if (mode == "timeline")
    {

      name = classNames[(int)data(0,0)];

      if (name != prev_name)
      {
        if ((end * (1.0/srate)-start*(1.0 / srate) > minspan_))
        {
          if (predicttimeline_ == EMPTYSTRING)
          {
            cout << start*(1.0 / srate) << "\t" << end*(1.0 / srate) << "\t";
            cout << prev_name << endl;
          }
          else
          {
            prtout << start*(1.0 / srate) << "\t" << end*(1.0 / srate) << "\t";
            prtout << prev_name << endl;
          }

        }
        start = end;

      }
      else
      {

      }

      prev_name = name;


    }
    else
      cout << "Unsupported mode" << endl;

    //	cout << data(0,0) << endl;
    end++;
  }

  // cout << "DONE" << endl;

  // sness - hmm, I really should be able to delete net, but I get a
  // coredump when I do.  Maybe I need to destroy something else first?
  //  delete net;


}



void
train_predict(mrs_string mode)
{
  if (!wekafname_Set()) return;
  if (!twekafname_Set()) return;

  string train_input_path = FileName(inputdir_).append(train_weka_fname_).fullname();
  string test_input_path = FileName(inputdir_).append(test_weka_fname_).fullname();

  cout << "Training classifier using .arff file: " << train_input_path << endl;
  cout << "Classifier type : " << classifier_ << endl;
  cout << "Predicting classes for .arff file: " << test_input_path << endl;



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
  net->updControl("WekaSource/wsrc/mrs_string/filename", train_input_path);
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

  cout << "Done training " << endl;


//   cout << "------------------------------" << endl;
//   cout << "Class names" << endl;
//   cout << net->getctrl("WekaSource/wsrc/mrs_string/classNames") << endl;
//   cout << "------------------------------\n" << endl;



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
  net->updControl("WekaSource/wsrc/mrs_string/filename", test_input_path);
  net->updControl("Classifier/cl/mrs_string/mode", "predict");
  ////////////////////////////////////////////////////////////
  //
  // Tick over the test WekaSource until all lines in the
  // test file have been read.
  //


  string pr_out_path = FileName(outputdir_).append(predictcollectionfname_).fullname();
  string prt_out_path = FileName(outputdir_).append(predicttimeline_).fullname();

  ofstream prout(pr_out_path.c_str());
  if (!prout.is_open())
  {
    MRSERR("Failed to open output file: " << pr_out_path);
    return;
  }

  ofstream prtout(prt_out_path.c_str());
  if (!prtout.is_open())
  {
    MRSERR("Failed to open output file: " << prt_out_path);
    return;
  }


  realvec data;
  int end=0;
  int start=0;

  mrs_string prev_name = "";
  mrs_string output_name = "";

  mrs_string name;

  //mrs_real srate;




  while (!net->getctrl("WekaSource/wsrc/mrs_bool/done")->to<mrs_bool>()) {
    net->tick();
    data = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    //srate = net->getctrl("WekaSource/wsrc/mrs_real/currentSrate")->to<mrs_real>();


    if (mode == "default")
    {
      cout << net->getctrl("WekaSource/wsrc/mrs_string/currentFilename")->to<mrs_string>() << "\t";
      cout << classNames[(int)data(0,0)] << endl;
      prout << net->getctrl("WekaSource/wsrc/mrs_string/currentFilename")->to<mrs_string>() << "\t";
      prout << classNames[(int)data(0,0)] << endl;
    }
    else if (mode == "timeline")
    {

      mrs_real srate = net->getctrl("WekaSource/wsrc/mrs_real/currentSrate")->to<mrs_real>();

      name = classNames[(int)data(0,0)]; \
      // cout << name << endl;
      if (name != prev_name)
      {
        if (end * (1.0/srate)-start*(1.0 / srate) <= minspan_) // not background
        {

        }
        else
        {

          if (predicttimeline_ == EMPTYSTRING)
          {
            if (label_ == EMPTYSTRING)
            {
              cout << start*(1.0 / srate) << "\t" << end*(1.0 / srate) << "\t";
              cout << prev_name << endl;
            }
            else
            {
              if (label_ == prev_name)
              {
                cout << start*(1.0 / srate) << "\t" << end*(1.0 / srate) << "\t";
                cout << prev_name << endl;
              }
            }


            output_name = prev_name;
          }
          else
          {
            if (label_ == EMPTYSTRING)
            {
              prtout << start*(1.0 / srate) << "\t" << end*(1.0 / srate) << "\t";
              prtout << prev_name << endl;
            }
            else
            {
              if (label_ == prev_name)
              {
                prtout << start*(1.0 / srate) << "\t" << end*(1.0 / srate) << "\t";
                prtout << prev_name << endl;
              }
            }


            output_name = prev_name;
          }



        }
        start = end;

      }

      // else
      // {

      // }

      prev_name = name;


    }
    else
      cout << "Unsupported mode" << endl;

    //	cout << data(0,0) << endl;
    end++;
  }


  cout << "DONE" << endl;

  delete net;





}



void
train_evaluate()
{
  if (!wekafname_Set()) return;


  string train_input_path = FileName(inputdir_).append(train_weka_fname_).fullname();

  cout << "Training classifier using .arff file: " << train_input_path << endl;
  cout << "Classifier type : " << classifier_ << endl;


  MarSystemManager mng;

  MarSystem* net;
  net = mng.create("Series", "net");
  net->addMarSystem(mng.create("WekaSource", "wsrc"));
  net->addMarSystem(mng.create("Classifier", "cl"));
  net->addMarSystem(mng.create("ClassificationReport", "summary"));

  if (classifier_ == "GS")
    net->updControl("Classifier/cl/mrs_string/enableChild", "GaussianClassifier/gaussiancl");
  if (classifier_ == "ZEROR")
    net->updControl("Classifier/cl/mrs_string/enableChild", "ZeroRClassifier/zerorcl");
  if (classifier_ == "SVM")
    net->updControl("Classifier/cl/mrs_string/enableChild", "SVMClassifier/svmcl");
  // net->updControl("WekaSource/wsrc/mrs_string/attributesToInclude", "1,2,3");

  // net->updControl("WekaSource/wsrc/mrs_string/validationMode", "PercentageSplit,50%");
  net->updControl("WekaSource/wsrc/mrs_string/validationMode", "kFold,NS,10");
  // net->updControl("WekaSource/wsrc/mrs_string/validationMode", "UseTestSet,lg.arff");
  net->updControl("WekaSource/wsrc/mrs_string/filename", train_input_path);
  net->updControl("mrs_natural/inSamples", 1);

  if (classifier_ == "SVM") {
    if (svm_svm_ != EMPTYSTRING) {
      net->updControl("Classifier/cl/SVMClassifier/svmcl/mrs_string/svm",
                      svm_svm_);
    }
    if (svm_kernel_ != EMPTYSTRING) {
      net->updControl("Classifier/cl/SVMClassifier/svmcl/mrs_string/kernel",
                      svm_kernel_);
    }
    net->updControl("Classifier/cl/SVMClassifier/svmcl/mrs_natural/gamma",
                    svm_gamma_);
    net->updControl("Classifier/cl/SVMClassifier/svmcl/mrs_real/C",
                    svm_C_);
  }

  if (net->getctrl("WekaSource/wsrc/mrs_bool/regression")->isTrue()) {
    // TODO: enable regression for ZeroRClassifier and GaussianClassifier,
    // and don't assume we're only dealing with svm
    net->updControl("Classifier/cl/SVMClassifier/svmcl/mrs_bool/output_classPerms", false);
    net->updControl("Classifier/cl/mrs_natural/nClasses", 1);

    net->updControl("ClassificationReport/summary/mrs_natural/nClasses", 1);
    net->updControl("ClassificationReport/summary/mrs_bool/regression", true);
  } else {
    net->updControl("ClassificationReport/summary/mrs_natural/nClasses", net->getctrl("WekaSource/wsrc/mrs_natural/nClasses"));
    net->updControl("ClassificationReport/summary/mrs_string/classNames",
                    net->getctrl("WekaSource/wsrc/mrs_string/classNames"));

    net->updControl("Classifier/cl/mrs_natural/nClasses", net->getctrl("WekaSource/wsrc/mrs_natural/nClasses"));
  }

  net->linkControl("Classifier/cl/mrs_string/mode", "WekaSource/wsrc/mrs_string/mode");
  net->linkControl("ClassificationReport/summary/mrs_string/mode", "Classifier/cl/mrs_string/mode");

  int i = 0;
  while(net->getctrl("WekaSource/wsrc/mrs_bool/done")->to<mrs_bool>() == false)
  {
    net->tick();
    //  cout << net->getControl("WekaSource/wsrc/mrs_realvec/processedData")->to<mrs_realvec>() << endl;
    ++i;
  }

  net->updControl("Classifier/cl/mrs_string/mode", "predict");
  net->updControl("ClassificationReport/summary/mrs_bool/done", true);
  net->tick();
  delete net;
}

//
//
//
bool
alreadySeen(mrs_string currentlyPlaying, vector<string> previouslySeenFilenames)
{
  // Check if we have seen the currently 'playing' filename already;
  // often we don't need to process the same file twice.
  for (size_t i=0; i < previouslySeenFilenames.size(); ++i)
  {
    if (currentlyPlaying == previouslySeenFilenames[i])
    {
      return true;
    }
  }
  return false;
}

void tags() {
  if (!wekafname_Set()) return;
  if (!twekafname_Set()) return;

  // The file paths we will be reading/writing to.
  string testing_arff = FileName(inputdir_).append(test_weka_fname_).fullname();
  string training_arff = FileName(inputdir_).append(train_weka_fname_).fullname();

  string testing_predictions = FileName(outputdir_).append(predictcollectionfname_).fullname();

  string testing_predictions_arff =
      FileName(test_weka_fname_).name() + ".affinities.arff";
  string training_predictions_arff =
      FileName(train_weka_fname_).name() + ".affinities.arff";

  testing_predictions_arff =
      FileName(outputdir_).append( testing_predictions_arff ).fullname();
  training_predictions_arff =
      FileName(outputdir_).append( training_predictions_arff ).fullname();

  // Initialize the network, classifier, and weka source through which
  // we will read our .arff files
  MarSystemManager mng;
  MarSystem* net = mng.create("Series", "series");
  net->addMarSystem(mng.create("WekaSource", "wsrc"));
  MarSystem* classifier = mng.create("Classifier", "cl");
  net->addMarSystem(classifier);
  net->addMarSystem(mng.create("Gain/gain"));

  // Instantiate the correct classifier:
  cout << "Selected classifier type = " << classifier_ << endl;
  if (classifier_ == "GS") {
    net->updControl("Classifier/cl/mrs_string/enableChild", "GaussianClassifier/gaussiancl");
  } else if (classifier_ == "ZEROR") {
    net->updControl("Classifier/cl/mrs_string/enableChild", "ZeroRClassifier/zerorcl");
  } else if (classifier_ == "SVM") {
    net->updControl("Classifier/cl/mrs_string/enableChild", "SVMClassifier/svmcl");
  } else {
    // TODO: ERROR CONDITION; ADD ERROR HANDLING HERE
  }


  /**
   * TRAINING
   *
   *     Read in the training arff data, and train the classifier.
   **/

  // Set up the weka source to read the training .arff
  // and hook together some controls.
  cout << "Training Filename = " << training_arff << endl;
  net->updControl("WekaSource/wsrc/mrs_string/filename", training_arff);
  net->updControl("mrs_natural/inSamples", 1);
  net->updControl("Classifier/cl/mrs_natural/nClasses", net->getctrl("WekaSource/wsrc/mrs_natural/nClasses"));
  net->linkControl("Classifier/cl/mrs_string/mode", "mrs_string/train");

  // Tick over the training WekaSource until all lines in the training file have been read.
  // FIXME: Remove the mode updates, unless someone can justify their existence.
  //        The mode is not switched to 'predict' until further down.
  cout << "Reading features" << endl;
  while (!net->getctrl("WekaSource/wsrc/mrs_bool/done")->to<mrs_bool>())
  {
    string mode = net->getctrl("WekaSource/wsrc/mrs_string/mode")->to<mrs_string>();
    net->tick();
    net->updControl("Classifier/cl/mrs_string/mode", mode);
  }

  // Switch the Classifier's mode to predict:
  // This causes the classifier to train itself on all input data so far.
  cout << "Training" << endl;
  net->updControl("Classifier/cl/mrs_string/mode", "predict");

  // Collect information about the labels (classes) in this dataset
  mrs_natural nLabels = net->getctrl("WekaSource/wsrc/mrs_natural/nClasses")->to<mrs_natural>();
  mrs_string labelNames = net->getctrl("WekaSource/wsrc/mrs_string/classNames")->to<mrs_string>();
  vector<string> classNames;
  // TODO: you could probably replace "s = ..." with "s = labelNames"
  string s = net->getctrl("WekaSource/wsrc/mrs_string/classNames")->to<mrs_string>();
  for (int i=0; i < nLabels; ++i)
  {
    string className;
    string temp;
    className = s.substr(0, s.find(","));
    temp = s.substr(s.find(",") + 1, s.length());
    s = temp;
    classNames.push_back(className);
  }

  /**
   * PREDICT STEP 1
   *
   *     Predictions for the testing arff data.
   **/

  // Initialize the weka sink that we will use to write an .arff file
  // for the testing dataset, where the features are the predicted
  // probabilities from our classifier
  MarSystem* testpSink = mng.create("WekaSink/testpSink");
  testpSink->updControl("mrs_natural/inSamples", 1);
  testpSink->updControl("mrs_natural/inObservations", nLabels+1);
  testpSink->updControl("mrs_natural/nLabels", nLabels);
  testpSink->updControl("mrs_string/labelNames", labelNames);
  testpSink->updControl("mrs_string/inObsNames", labelNames);
  testpSink->updControl("mrs_string/filename", testing_predictions_arff);

  // Set up the weka source to read the testing data
  cout << "Testing Filename = " << testing_arff << endl;
  net->updControl("WekaSource/wsrc/mrs_string/filename", testing_arff);
  cout << "Starting Prediction for Testing Collection" << endl;
  cout << "Writing .mf style predictions to " << testing_predictions << endl;
  cout << "The following output file can serve as a stacked testing .arff" << endl;
  cout << "Writing .arff style predictions to " << testing_predictions_arff << endl;

  mrs_realvec probs;
  mrs_realvec testpSinkOut;
  mrs_string currentlyPlaying;
  realvec data;
  realvec wsourcedata;
  vector<string> previouslySeenFilenames;

  // Open the non-stacked predictions output file to write to.
  ofstream prout(testing_predictions.c_str());
  if (!prout.is_open())
  {
    MRSERR("Failed to open output file: " << testing_predictions);
    return;
  }

  // Tick over the test WekaSource, saving our predictions for each line,
  // until all lines in the test file have been read.
  testpSinkOut.create(nLabels+1,1);
  while (!net->getctrl("WekaSource/wsrc/mrs_bool/done")->to<mrs_bool>())
  {
    net->tick();

    wsourcedata = net->getctrl("WekaSource/wsrc/mrs_realvec/processedData")->to<mrs_realvec>();
    data = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    currentlyPlaying = net->getctrl("WekaSource/wsrc/mrs_string/currentFilename")->to<mrs_string>();

    if (!alreadySeen(currentlyPlaying, previouslySeenFilenames))
    {
      probs = net->getctrl("Classifier/cl/mrs_realvec/processedData")->to<mrs_realvec>();

      for (mrs_natural i=0; i < probs.getSize()-2; i++)
      {
        testpSinkOut(i,0) = probs(2+i);
        prout << currentlyPlaying << "\t" << classNames[i] << "\t" << probs(2+i) << endl;
      }

      testpSinkOut(probs.getSize()-2,0) = probs(0);
      testpSink->updControl("mrs_string/currentlyPlaying", currentlyPlaying);
      testpSink->process(testpSinkOut, testpSinkOut);

      // Mark this filename as seen!
      previouslySeenFilenames.push_back(currentlyPlaying);
    }
  }

  // Close the non-stacked predictions; they are written!
  prout.close();

  /**
   * PREDICT STEP 2
   *
   *     Predictions for the training arff data
   **/

  // Initialize the weka sink that we will use to write an .arff file
  // for the training dataset, where the features are the predicted
  // probabilities from our classifier
  MarSystem* trainpSink = mng.create("WekaSink/trainpSink");
  trainpSink->updControl("mrs_natural/inSamples", 1);
  trainpSink->updControl("mrs_natural/inObservations", nLabels+1);
  trainpSink->updControl("mrs_natural/nLabels", nLabels);
  trainpSink->updControl("mrs_string/labelNames", labelNames);
  trainpSink->updControl("mrs_string/inObsNames", labelNames);
  trainpSink->updControl("mrs_string/filename", training_predictions_arff);

  cout << "Starting prediction for training collection (for stacked generalization)" << endl;
  cout << "The following output file can serve as a stacked training .arff" << endl;
  cout << "Writing .arff style predictions to " << training_predictions_arff << endl;

  // Empty our list of previously seen filenames; we will reuse it.
  previouslySeenFilenames.clear();
  mrs_realvec trainpSinkOut;
  trainpSinkOut.create(nLabels+1,1);
  mrs_natural label;

  net->updControl("WekaSource/wsrc/mrs_string/filename", training_arff);
  // Tick over the test WekaSource, saving our predictions for each line in
  // the training file until all lines in the training file have been read.
  while (!net->getctrl("WekaSource/wsrc/mrs_bool/done")->to<mrs_bool>())
  {
    net->tick();
    currentlyPlaying = net->getctrl("WekaSource/wsrc/mrs_string/currentFilename")->to<mrs_string>();
    wsourcedata = net->getctrl("WekaSource/wsrc/mrs_realvec/processedData")->to<mrs_realvec>();
    label = wsourcedata(wsourcedata.getSize()-1,0);
    probs = net->getctrl("Classifier/cl/mrs_realvec/processedData")->to<mrs_realvec>();

    if (!alreadySeen(currentlyPlaying, previouslySeenFilenames))
    {
      // Store the predicted probabilities for this file and mark the file as seen!
      for (mrs_natural i=0; i < probs.getSize()-2; i++)
      {
        trainpSinkOut(i,0) = probs(2+i);
      }
      previouslySeenFilenames.push_back(currentlyPlaying);
    }

    // Write out a line in the arff file.
    trainpSinkOut(probs.getSize()-2,0) = label;
    trainpSink->updControl("mrs_string/currentlyPlaying", currentlyPlaying);
    trainpSink->process(trainpSinkOut, trainpSinkOut);
  }

  cout << "DONE" << endl;
}

void
initOptions()
{
  cmd_options_.addBoolOption("help", "h", false);
  cmd_options_.addBoolOption("usage", "u", false);
  cmd_options_.addStringOption("wekafname", "w", EMPTYSTRING);
  cmd_options_.addStringOption("testwekafname", "tw", EMPTYSTRING);
  cmd_options_.addStringOption("predictcollectionfname", "pr", EMPTYSTRING);
  cmd_options_.addStringOption("mode", "m", "train_evaluate");
  cmd_options_.addStringOption("inputdir", "id", "");
  cmd_options_.addStringOption("outputdir", "od", "");
  cmd_options_.addStringOption("distancematrix", "dm", "dm.txt");
  cmd_options_.addStringOption("classifier", "cl", "SVM");
  cmd_options_.addStringOption("predicttimeline", "prtl", EMPTYSTRING);
  cmd_options_.addRealOption("minspan", "msp", 0.25);
  cmd_options_.addStringOption("trainedclassifier", "trcl", "trained.mpl");
  cmd_options_.addStringOption("svm_svm", "ss", EMPTYSTRING);
  cmd_options_.addStringOption("svm_kernel", "sk", EMPTYSTRING);
  cmd_options_.addStringOption("label", "lb", EMPTYSTRING);
  cmd_options_.addNaturalOption("svm_gamma", "sG", 4);
  cmd_options_.addRealOption("svm_C", "sC", 1.0);
  cmd_options_.addNaturalOption("ncomponents", "nc", 2);
}


void
loadOptions()
{
  helpopt_ = cmd_options_.getBoolOption("help");
  usageopt_ = cmd_options_.getBoolOption("usage");
  train_weka_fname_ = cmd_options_.getStringOption("wekafname");
  test_weka_fname_ = cmd_options_.getStringOption("testwekafname");
  predictcollectionfname_ = cmd_options_.getStringOption("predictcollectionfname");
  mode_ = cmd_options_.getStringOption("mode");
  inputdir_ = cmd_options_.getStringOption("inputdir");
  outputdir_ = cmd_options_.getStringOption("outputdir");
  distancematrix_ = cmd_options_.getStringOption("distancematrix");
  classifier_ = cmd_options_.getStringOption("classifier");
  predicttimeline_ = cmd_options_.getStringOption("predicttimeline");
  minspan_ = cmd_options_.getRealOption("minspan");
  trainedclassifier_ = cmd_options_.getStringOption("trainedclassifier");
  svm_svm_ = cmd_options_.getStringOption("svm_svm");
  svm_kernel_ = cmd_options_.getStringOption("svm_kernel");
  label_ = cmd_options_.getStringOption("label");
  svm_C_ = cmd_options_.getRealOption("svm_C");
  svm_gamma_ = cmd_options_.getNaturalOption("svm_gamma");
  ncomponents_ = cmd_options_.getNaturalOption("ncomponents");
}


int
main(int argc, const char **argv)
{
  MRSDIAG("kea.cpp - main");

  cout << "Kea - Machine Learning in Marsyas ala Weka" << endl;

  string progName = argv[0];
  if (argc == 1)
    return printUsage(progName);

  // handling of command-line options
  initOptions();
  cmd_options_.readOptions(argc, argv);
  loadOptions();

  vector<string> soundfiles = cmd_options_.getRemaining();
  if (helpopt_)
    return printHelp(progName);

  if (usageopt_)
    return printUsage(progName);

  cout << "Mode = " << mode_ << endl;

  if (mode_ == "train_evaluate")
    train_evaluate();
  else if (mode_ == "distance_matrix_MIREX")
    distance_matrix_MIREX();
  else if (mode_ == "distance_matrix")
    distance_matrix();
  else if (mode_ == "pca")
    pca();
  else if (mode_ == "tags")
    tags();
  else if (mode_ == "train_predict")
    train_predict("default");
  else if (mode_ == "train_predict_timeline")
    train_predict("timeline");
  else if (mode_ == "predict")
    predict("default");
  else if (mode_ == "predict_timeline")
    predict("timeline");
  else if (mode_ == "train_classifier")
    train_classifier();
  else
    cerr << "Unsupported mode: " << mode_ << endl;



  return 0;
}
