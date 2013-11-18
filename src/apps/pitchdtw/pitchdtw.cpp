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

using namespace std;
using namespace Marsyas;

void usage()
{
  cout << "pitchdtw file1.txt file2.txt file3.txt ..." << endl;
}

void pitchdtw(vector<string> filenames, vector<vector<float> > data)
{
  mrs_natural max_size;

  MarSystemManager mng;

  // A series to contain everything
  MarSystem* net = mng.create("Series", "series");
  MarSystem* sim = mng.create("SimilarityMatrix","sim");

  MarSystem* met = mng.create("Metric2", "met");
  met->updControl("mrs_string/metric","euclideanDistance");

  sim->addMarSystem(met);
  net->addMarSystem(sim);

  // Calculate the DTW cost
  MarSystem* dtw = mng.create("DTW", "dtw");
  dtw->updControl("mrs_string/lastPos","end");
  dtw->updControl("mrs_string/startPos","zero");
  dtw->updControl("mrs_bool/weight",false);
  dtw->updControl("mrs_string/mode","normal");
  net->addMarSystem(dtw);

  // We're comparing two files, so we always have two observations
  int numObservations = 2;

  // A vector of distances between all files
  vector<vector<mrs_real> > distances;
  vector<mrs_real> tmp_distances;

  mrs_realvec sizes;
  sizes.create(2);

  // Find the DTW distance between all pairs of files
  mrs_natural data_size = (mrs_natural) data.size();
  for (mrs_natural i = 0; i < data_size; i++)
  {
    tmp_distances.clear();
    cerr << "Processing query " << i << endl;
    for (mrs_natural j = 0; j < data_size; j++)
    {
      mrs_natural data_i_size = (mrs_natural) data[i].size();
      mrs_natural data_j_size = (mrs_natural) data[j].size();

      // Which of the two files is longer?
      max_size = std::max(data_i_size, data_j_size);

      // Create a realvec to hold both pairs of data
      realvec input_realvec;
      input_realvec.create(2,max_size);

      // Copy both data[i] and data[j] into input_realvec.  If one
      // file is shorter than the other, pad it with zeros.
      for(mrs_natural k = 0; k < max_size; k++)
      {
        if (k < data_i_size) {
          input_realvec(0,k) = data[i][k];
        } else {
          input_realvec(0,k) = 0;
        }

        if (k < data_j_size) {
          input_realvec(1,k) = data[j][k];
        } else {
          input_realvec(1,k) = 0;
        }
      }

      // Update the SimilarityMatrix with the sizes
      // of the two input vectors
      sizes(0) = (mrs_real) data_i_size;
      sizes(1) = (mrs_real) data_j_size;
      sim->updControl("mrs_realvec/sizes",sizes);
      sim->update();

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
      mrs_real d = dtw->getctrl("mrs_real/totalDistance")->to<mrs_real>();
      tmp_distances.push_back(d);
    }
    distances.push_back(tmp_distances);
  }
  cerr << "Done with distances " << endl;




  // Print out all the distances
  for (size_t i = 0; i < distances.size(); i++) {
    mrs_string className = filenames[i].substr(0, filenames[i].find("_"));
    cout << className << " ";
    // cout << filenames[i] << " ";
    for (size_t j = 0; j < distances[i].size(); j++) {
      cout << distances[i][j] << " ";
      flush(cout);
    }
    cout << endl;
  }

}

void read_file_of_floats_into_vector(string name, vector<float> &file) {

  int readChars;
  char line[256];
  float f;

  FILE *inFile = fopen(name.c_str(), "r");

  do {
    readChars = fscanf(inFile,"%s", line );
    if (readChars < 0)
      break;
    f = (float)atof(line);
    file.push_back(f);
  } while (readChars > 0);

  fclose(inFile);
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

  cerr << "Finished reading" << endl;

  // Calculate the DTW distance for all pairs of files
  pitchdtw(filenames,data);

  exit(0);

}






