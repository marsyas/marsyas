
/**
   \class WekaSource
   \brief Input sink for Weka-formatted (textual) feature data

*/

#ifndef MARSYAS_newWEKASOURCE_H
#define MARSYAS_newWEKASOURCE_H

#include <marsyas/system/MarSystem.h>
#include <marsyas/WekaData.h>
#include <list>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstring>
//using namespace std;

namespace Marsyas
{
class WekaFoldData : public WekaData
{
public:
  ~WekaFoldData() {}

  typedef enum
  {
    None,
    Training,
    Predict
  } nextMode;

private:
  mrs_natural foldCount_;

  mrs_real	rstep_;
//		mrs_natural predictSum_;
  mrs_natural excludeSectionStart_;
  mrs_natural excludeSectionEnd_;

  mrs_natural iteration_;
  mrs_natural currentIndex_;
//		mrs_natural predictIndex_;

public:
  void SetupkFoldSections(const WekaData& data, mrs_natural foldCount, mrs_natural classAttr=-1)
  {
    MRSASSERT(foldCount>0);
    foldCount_ = foldCount;

    //create the dataset with same number of columns as input data
    this->Create(data.getCols());
    if(classAttr<0)
    { //if no class specified, copy all data into this instance
      this->assign(data.begin(), data.end());
    }
    else
    { //otherwise only copy rows that match input class into this dataset
      for(mrs_natural ii=0; ii<(mrs_natural)data.size(); ++ii)
        if(data.GetClass(ii)==classAttr)
          this->Append(data[ii]);
    }//else

    //setup fold sections
    this->Reset();

  }//SetupkFoldSections

  //setup the fold sections for this dataset.
  void Reset()
  {


    this->Shuffle();

    rstep_ = (mrs_real)this->size() / (mrs_real)foldCount_;

    if (foldCount_ > (mrs_natural)this->size())
    {
      std::cout << "Folds exceed number of instances" << std::endl;
      std::cout << "foldCount_ = " << foldCount_ << std::endl;
      std::cout << "size = " << this->size() << std::endl;;
      exit(1);
    }


    iteration_ = 0;

    excludeSectionStart_ = 0;
    excludeSectionEnd_ = ((mrs_natural)rstep_) - 1;
    currentIndex_ = excludeSectionEnd_ + 1;
  }

  std::vector<mrs_real> *Next(nextMode& next)
  {

    std::vector<mrs_real> *ret = this->at(currentIndex_);

    if(currentIndex_ == excludeSectionEnd_)
    {
      iteration_++;
      if(iteration_ >= foldCount_)
      {
        next = None;
        return ret;
      }//if

      excludeSectionStart_ = excludeSectionEnd_ + 1;
      if(iteration_ == (foldCount_ - 1))
      {
        excludeSectionEnd_ = (mrs_natural)this->size() - 1;
        currentIndex_ = 0;
      }
      else
      {
        excludeSectionEnd_ = ((mrs_natural)((iteration_+1) * rstep_)) - 1;
        currentIndex_ = excludeSectionEnd_ + 1;
      }


      next = Training;
      return ret;
    }//if

    currentIndex_++;


    if(currentIndex_ >= (mrs_natural)this->size())
      currentIndex_ = 0;

    if(currentIndex_ >= excludeSectionStart_ && currentIndex_ <= excludeSectionEnd_)
      next = Predict;
    else
      next = Training;


    return ret;

  }//Next

};

typedef enum
{
  None,
  kFoldStratified,
  kFoldNonStratified,
  UseTestSet,
  PercentageSplit,
  OutputInstancePair
} ValidationModeEnum;

class marsyas_EXPORT WekaSource : public MarSystem
{
public:
  WekaSource(std::string name);
  WekaSource(const WekaSource& a);
  ~WekaSource();

  MarSystem *clone()const;
  void myProcess(realvec& in, realvec& out);

private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  //control values
  std::string filename_;						//name of arff file to read
  std::string attributesToInclude_;			//list of attributes to include in dataset

  //these are the class names froun in the arff file header
  std::vector<std::string>classesFound_;
  // if there are no classes, we're doing regression
  MarControlPtr ctrl_regression_;

  std::string relation_;

  //these are the attribute names found in the arff file header
  std::vector<std::string>attributesFound_;

  //Holds the actual attribute data read from the arff file
  WekaData data_;

  //an array of bools that specify if an attribute from the arff file should be included
  //in the dataset.
  std::vector<bool>attributesIncluded_;

  //the list of attributes that are to be included in the dataset
  std::vector<std::string>attributesIncludedList_;

  //the validation mode enum to use
  ValidationModeEnum validationModeEnum_;

  //Common validation method data members
  mrs_natural currentIndex_;

  //kFold Stratified validation method data members
  mrs_natural foldCount_;
  WekaFoldData foldData_;
  WekaFoldData::nextMode foldCurrentMode_;
  WekaFoldData::nextMode foldNextMode_;

  //kFold NonStratified validation method data members
  std::vector<WekaFoldData> foldClassData_;
  mrs_natural foldClassDataIndex_;

  //UseTestSet validation method data members
  WekaData useTestSetData_;

  //PercentageSplit validation method data members
  mrs_natural percentageIndex_;

  void handleDefault(bool trainMode, realvec& out);
  void handleInstancePair(realvec& out);
  void handleFoldingNonStratifiedValidation(bool trainMode, realvec &out);
  void handleFoldingStratifiedValidation(bool trainMode, realvec &out);
  void handleUseTestSet(bool trainMode, realvec &out);
  void handlePercentageSplit(bool trainMode, realvec &out);

private:
  mrs_natural findClass(const char *className)const;
  mrs_natural findAttribute(const char *attribute)const;
  mrs_natural parseAttribute(const char *attribute)const;

  void parseAttributesToInclude(const std::string& attributesToInclude);
  void loadFile(const std::string& filename, const std::string& attributesToExtract, WekaData& data);
  void parseHeader(std::ifstream& mis, const std::string& filename, const std::string& attributesToExtract);
  void parseData(std::ifstream& mis, const std::string& filename, WekaData& data);

};//class WekaSource
}//namespace Marsyas

#endif
