//
// pitchdtw.cpp
//
// Output the DTW cost of two similarity matrices
//

#include <cstdio>
#include <vector>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>
#include <algorithm>
// #include "pngwriter.h"

using namespace std;
using namespace Marsyas;

void usage()
{
  cout << "pitchdtw in1.txt in2.txt" << endl;
}

void pitchdtw(realvec input_realvec,int size1, int size2)
{

  int numObservations = 2;
  int numSamples = input_realvec.getCols();

  MarSystemManager mng;

  // A series to contain everything
  MarSystem* net = mng.create("Series", "series");
  net->updControl("mrs_natural/inSamples", numSamples);
  net->updControl("mrs_natural/inObservations", numObservations);
  net->addMarSystem(mng.create("RealvecSource", "src"));
  net->updControl("RealvecSource/src/mrs_realvec/data", input_realvec);

  mrs_realvec sizes;
  sizes.create(2);
  sizes(0) = size1;
  sizes(1) = size2;

  MarSystem* sim = mng.create("SimilarityMatrix","sim");
  sim->updControl("mrs_realvec/sizes",sizes);

  MarSystem* met = mng.create("Metric2","met");
  met->updControl("mrs_string/metric","euclideanDistance");

  sim->addMarSystem(met);
  net->addMarSystem(sim);


  MarSystem* dtw = mng.create("DTW", "dtw");
  dtw->updControl("mrs_string/lastPos","end");
  dtw->updControl("mrs_string/startPos","zero");
  dtw->updControl("mrs_bool/weight",false);
  dtw->updControl("mrs_string/mode","normal");
  net->addMarSystem(dtw);

  cout << "in=" << input_realvec << endl;
  cout << "sizes=" << sizes << endl;

  net->tick();

  cout << net->getControl("mrs_realvec/processedData")->to<mrs_realvec>();

  cout << dtw->getctrl("mrs_real/totalDistance")->to<mrs_real>() << endl;
}

void read_file_of_floats_into_vector(string name, vector<float> &file) {

  int readChars;
  char line[256];

  FILE *inFile = fopen(name.c_str(), "r");

  do {
    readChars = fscanf(inFile,"%s", line );
    if (readChars < 0)
      break;
    file.push_back( (float) atof(line) );
  } while (readChars > 0);

}

int main(int argc, const char **argv)
{
  string inFileName1;
  string inFileName2;

  vector<float> file1;
  vector<float> file2;

  realvec input_realvec;

  if (argc < 2) {
    usage();
    exit(1);
  } else {
    inFileName1 = argv[1];
    inFileName2 = argv[2];
  }

  read_file_of_floats_into_vector(inFileName1,file1);
  read_file_of_floats_into_vector(inFileName2,file2);

  mrs_natural file1_size = (mrs_natural) file1.size();
  mrs_natural file2_size = (mrs_natural) file2.size();
  mrs_natural max_size = std::max(file1_size, file2_size);

  input_realvec.create(2,max_size);

  // Copy both file1 and file2 into input_realvec
  for(mrs_natural i = 0; i < max_size; i++)
  {
    if (i < file1_size) {
      input_realvec(0,i) = file1[i];
    } else {
      input_realvec(0,i) = 0;
    }

    if (i < file2_size) {
      input_realvec(1,i) = file2[i];
    } else {
      input_realvec(1,i) = 0;
    }
  }


  pitchdtw(input_realvec, file1_size, file2_size);

  exit(0);

}






