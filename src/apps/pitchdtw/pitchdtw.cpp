//
// pitchdtw.cpp
//
// Output the DTW cost of two similarity matrices
//

#include <cstdio>
#include <vector> 
#include "MarSystemManager.h"
#include "CommandLineOptions.h"
#include "SimilarityMatrix.h"
#include "Metric2.h"
#include "DTW.h"

using namespace std;
using namespace Marsyas;

void usage()
{
  cout << "pitchdtw in1.txt in2.txt" << endl;
}

void pitchdtw(vector<string> filenames, vector<vector<float> > data)
{
  unsigned int max_size;
  realvec input_realvec;

  MarSystemManager mng;

  // A series to contain everything
  MarSystem* net = mng.create("Series", "series");
  MarSystem* sim = new SimilarityMatrix("sim");
  
  MarSystem* met = new Metric2("met");
  met->updControl("mrs_string/metric","euclideanDistance");

  sim->addMarSystem(met);
  net->addMarSystem(sim);
  
  // Calculate the DTW cost
  MarSystem* dtw = new DTW("dtw");
  dtw->updControl("mrs_string/lastPos","end");
  dtw->updControl("mrs_string/startPos","zero");
  dtw->updControl("mrs_bool/weight",false);
  dtw->updControl("mrs_string/mode","normal");
  net->addMarSystem(dtw);

  // We're comparing two files, so we always have two observations
  int numObservations = 2;

  // A vector of distances between all files
  vector<vector<mrs_real> > distances;

  // Find the DTW distance between all pairs of files
  for (unsigned int i = 0; i < data.size(); i++) {
	vector<mrs_real> tmp_distances;
  	for (unsigned int j = 0; j < data.size(); j++) {
		// Which of the two files is longer?
		if (data[i].size() > data[j].size()) {
			max_size = data[i].size();
		} else {
			max_size = data[j].size();
		}
		
		// Create a realvec to hold both pairs of data
		input_realvec.create(2,max_size);

		// Copy both data[i] and data[j] into input_realvec.  If one
		// file is shorter than the other, pad it with zeros.
 		for(unsigned int k = 0; k < max_size; k++) {
		  if (k < data[i].size()) {
			input_realvec(0,k) = data[i][k];
		  } else {
			input_realvec(0,k) = 0;
		  }

		  if (k < data[j].size()) {
			input_realvec(1,k) = data[j][k];
		  } else {
			input_realvec(1,k) = 0;
		  }
		}

		// Update the SimilarityMatrix with the sizes
		// of the two input vectors
		mrs_realvec sizes;
		sizes.create(2);
		sizes(0) = data[i].size();
		sizes(1) = data[j].size();
		sim->updControl("mrs_realvec/sizes",sizes);

		// Update the network with the size of the input data
		int numSamples = input_realvec.getCols();
		net->updControl("mrs_natural/inSamples", numSamples);
		net->updControl("mrs_natural/inObservations", numObservations);

		// Find the size of the output of the network
		mrs_natural onSamples = net->getControl("mrs_natural/onSamples")->to<mrs_natural>();
		mrs_natural onObservations = net->getControl("mrs_natural/onObservations")->to<mrs_natural>();

		// Create an output realvec to store the output of the network
		mrs_realvec output_realvec;
		output_realvec.create(onObservations,onSamples);

		// Process the data
		net->process(input_realvec,output_realvec);

		// The distance between this pair of files
		tmp_distances.push_back(dtw->getctrl("mrs_real/totalDistance")->to<mrs_real>());
	}
	distances.push_back(tmp_distances);
  }

  // Print out all the distances
  for (unsigned int i = 0; i < distances.size(); i++) {
	cout << filenames[i] << " ";
	for (unsigned int j = 0; j < distances[i].size(); j++) {
	  cout << distances[i][j] << " ";
	}
	cout << endl;
  }

}
  
void read_file_of_floats_into_vector(string name, vector<float> &file) {

  int readChars;
  char line[256];
  double f;

  FILE *inFile = fopen(name.c_str(), "r");

  do {
    readChars = fscanf(inFile,"%s", line );
	if (readChars < 0) 
	  break;
	f = atof(line);
	file.push_back(f);
  } while (readChars > 0);

}

int main(int argc, const char **argv)
{
  // Read in the filenames from the command line
  vector<string> filenames;
  for (int i = 1; i < argc; i++) {
	filenames.push_back(argv[i]);
  }

  // A vector of vectors of floats to contain all the data from the
  // files to be processed.
  vector<vector<float> > data;
  
  // Read in all the data
  for (unsigned int i = 0; i < filenames.size(); i++) {
	vector<float> tmp;
	read_file_of_floats_into_vector(filenames[i],tmp);
	data.push_back(tmp);
  }

  // Calculate the DTW distance for all pairs of files
  pitchdtw(filenames,data);
  
  // string inFileName1;
  // string inFileName2;

  // vector<float> data[i][j];
  // vector<float> data[i][k];

  // realvec input_realvec;

  // if (argc < 2) {
  // 	usage();
  // 	exit(1);
  // } else {
  // 	inFileName1 = argv[1];
  // 	inFileName2 = argv[2];
  // }
  
  // read_file_of_floats_into_vector(inFileName1,data[i][j]);
  // read_file_of_floats_into_vector(inFileName2,data[i][k]);

  // unsigned int max_size;

  // if (data[i][j].size() > data[i][k].size()) {
  // 	max_size = data[i][j].size();
  // } else {
  // 	max_size = data[i][k].size();
  // }

  //  input_realvec.create(2,max_size);

  //  // Copy both data[i][j] and data[i][k] into input_realvec
  //  for(unsigned int i = 0; i < max_size; i++) {
  // 	if (i < data[i][j].size()) {
  // 	  input_realvec(0,i) = data[i][j][i];
  // 	} else {
  // 	  input_realvec(0,i) = 0;
  // 	}

  // 	if (i < data[i][k].size()) {
  // 	  input_realvec(1,i) = data[i][k][i];
  // 	} else {
  // 	  input_realvec(1,i) = 0;
  // 	}
  //  }



  // exit(0);

}






