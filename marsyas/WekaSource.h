
/**
   \class WekaSource
   \brief Input sink for Weka-formatted (textual) feature data

*/

#ifndef MARSYAS_WEKASOURCE_H
#define MARSYAS_WEKASOURCE_H

#include "MarSystem.h" 

#include <fstream>

#define WEKASOURCE_MAX_OBS	500

namespace Marsyas
{

class WekaSource:public MarSystem 
{
private:
  void addControls();
  void parseObsToExtract();
  
  std::string filename_;
  std::string obsToExtract_;	   //Indexes to the features that are to be extracted from the current weka file
  std::string labelNames_;
  std::string data_;
  
  mrs_natural totalObs_;	   //Total number of features available per sample in the current weka file
  
  bool extract [WEKASOURCE_MAX_OBS];  //Represents the features to be extracted from the current weka file 
  			   //(Upper limit of 500 features per sample)
  std::ifstream* mis_;
  

public:
  WekaSource();
  WekaSource(std::string name);
  WekaSource(const WekaSource& a);
  
  ~WekaSource();
  
  MarSystem* clone() const;  
  
  void update();
  void process(realvec& in, realvec& out);
};

}//namespace Marsyas


#endif
