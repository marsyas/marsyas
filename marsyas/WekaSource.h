
#ifndef MARSYAS_WEKASOURCE_H
#define MARSYAS_WEKASOURCE_H

#include "MarSystem.h" 

#include <fstream>

#define WEKASOURCE_MAX_OBS	500

namespace Marsyas
{
/**
	\class WekaSource
	\ingroup IO
	\brief Input sink for Weka-formatted (textual) feature data

	Controls:
	- \b mrs_string/filename [w] : name of file to read.
	- \b mrs_string/obsToExtract [rw] : A string of ',' separated indexes
or index ranges (specified using '-') of the attributes desired from the
weka file.  From this, the number of inObservations is inferred.  (Note:
attribute indexing starts from 0)
	- \b mrs_natural/nLables [rw] : number of labels.
	- \b mrs_string/labelNames [rw] : names of labels.
	- \b mrs_bool/notEmpty [r] : is there more data to read?
*/ 


class WekaSource:public MarSystem 
{
private:
  void addControls();
	void myUpdate(MarControlPtr sender);

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
  WekaSource(std::string name);
  WekaSource(const WekaSource& a);
  
  ~WekaSource();
  
  MarSystem* clone() const;  
  
  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas


#endif
