/**
   \class WekaSource
   \brief Input source for Weka-formatted (textual) feature data

   This class was rewritten and extended to handle built in classifier validation.

*/
#include "WekaSource.h"
#include "../common_source.h"
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace Marsyas;

class argument_list_stream
{
  istringstream m_stream;
  bool m_ok;

public:
  argument_list_stream(const string & text):
    m_stream(text),
    m_ok(true)
  {}

  operator bool() { return m_ok; }

  template <typename T>
  argument_list_stream & operator>>(T & value)
  {
    string part;
    if (std::getline(m_stream, part, ','))
    {
      istringstream part_stream(part);
      if (part_stream >> value)
      {
        istringstream::sentry can_read_more(part_stream);
        if (!can_read_more)
          return *this;
      }
    }

    m_ok = false;
    value = T();
    return *this;
  }
};

WekaSource::WekaSource(mrs_string name):MarSystem("WekaSource",name)
{
  addControls();
  validationModeEnum_ = None;
  currentIndex_ = 0;
}

WekaSource::~WekaSource()
{
  data_.Clear();
  useTestSetData_.Clear();
}

WekaSource::WekaSource(const WekaSource& a) : MarSystem(a) {
  validationModeEnum_ = None;
  currentIndex_  = 0;
  ctrl_regression_ = getctrl("mrs_bool/regression");
}

MarSystem *WekaSource::clone() const
{
  return new WekaSource(*this);
}

void
WekaSource::addControls()
{
  addctrl("mrs_string/filename", "");
  setctrlState("mrs_string/filename", true);

  //comma seperated list of attributes to extract from the feature data
  //Can be attribute name, index, or range of indexes.
  //ie: "Mean_Mem40_Centroid, Mean_Mem40_Kurtosis, 4-7, 9,  .... "
  addctrl("mrs_string/attributesToInclude", "");
  setctrlState("mrs_string/attributesToInclude", true);

  //comma seperated list of class names found in feature file
  //ie: "Music, Speech, .... "
  addctrl("mrs_string/classNames", "");

  //number of classes found
  addctrl("mrs_natural/nClasses", 0);
  addctrl("mrs_bool/regression", false, ctrl_regression_);

  //The mode that the weka source is currently in.
  //Can be  "train" or "predict"
  addctrl("mrs_string/mode", "train");

  //number of output samples will always be 1, regardless of the input samples
  setctrl("mrs_natural/onSamples", 1 ); //FIXME: this should not be done here but in myProcess instead...

  //number of attributes and attribute names that will be reported.
  addctrl("mrs_natural/nAttributes", 0);
  addctrl("mrs_string/attributeNames", "");

  //type of classifier validation to do.
  //Blank or not set means none.
  //Other supported types:
  //"kFold,[S,NS],xx"  where xx is an integer 2-10
  //if S is specified, use Stratified
  //if NS is specified, use Non-Stratified
  //"UseTestSet,wekafilename"
  //"PercentageSplit,percent" where percent is 1-99
  //others to come
  addctrl("mrs_string/validationMode", "");
  addctrl("mrs_bool/done", false);

  addctrl("mrs_natural/nInstances", 0);

  // The current filename that we are processing
  // We get this from looking for comment strings that begin with "%
  // filename"
  addctrl("mrs_string/currentFilename", "");
  addctrl("mrs_real/currentSrate", 22050.0);

  //TODO: lmartins: document...
  addctrl("mrs_realvec/instanceIndexes", realvec());

  addctrl("mrs_bool/normMaxMin", false);

}

void
WekaSource::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("WekaSource.cpp - WekaSource:myUpdate");

  // If 'filename' was updated, or the attributes desired from the Weka file has changed,
  // parse the header portion of the file to get the required attribute names and possible output labels (if any)...
  if (filename_ == getctrl("mrs_string/filename")->to<mrs_string>())
    return;

  this->updControl("mrs_bool/done", false);
  filename_ = getctrl("mrs_string/filename")->to<mrs_string>();
  attributesToInclude_ = getctrl("mrs_string/attributesToInclude")->to<mrs_string>();

  mrs_bool normMaxMin = getctrl("mrs_bool/normMaxMin")->to<mrs_bool>();

  loadFile(filename_, attributesToInclude_, data_);
  if (normMaxMin)
  {
    data_.NormMaxMin();
  }

  // data_.Dump("org.txt", classesFound_);

  mrs_string names;
  bool first = true;
  for(vector<mrs_string>::const_iterator citer = classesFound_.begin(); citer!= classesFound_.end(); citer++)
  {
    if(!first)
      names += ",";

    names += (*citer);
    first = false;
  }
  setctrl("mrs_string/classNames", names);
  setctrl("mrs_natural/nClasses", (mrs_natural)classesFound_.size());

  names = "";
  first = true;
  mrs_natural index = 0;
  for(vector<mrs_string>::const_iterator citer = attributesFound_.begin(); citer!= attributesFound_.end(); citer++,index++)
  {
    if(attributesIncluded_[index])
    {
      if(!first)
        names += ",";

      names += (*citer);
      first = false;
    }//if
  }
  MRSASSERT(index == (mrs_natural)attributesIncluded_.size());

  setctrl("mrs_string/attributeNames", names);
  ctrl_onObsNames_->setValue(names);
  setctrl("mrs_natural/onSamples", 1);
  setctrl("mrs_natural/nAttributes", (mrs_natural)attributesFound_.size());
  setctrl("mrs_natural/onObservations", (mrs_natural)attributesFound_.size()+1);
  setctrl("mrs_natural/nInstances", (mrs_natural)data_.getRows());


  validationModeEnum_ = None;
  currentIndex_ = 0;

  const mrs_string & validation_mode_spec = getctrl("mrs_string/validationMode")->to<mrs_string>();
  if (validation_mode_spec.empty())
    return;

  argument_list_stream validation_mode_args(validation_mode_spec);

  string v_mode_error_msg;
  {
    ostringstream text;
    text << "WekaSource: Error in value of control 'validationMode' = "
         << "\"" << validation_mode_spec << "\":";
    v_mode_error_msg = text.str();
  }

  string validation_mode;
  if (!(validation_mode_args >> validation_mode))
  {
    MRSERR(v_mode_error_msg << " Can not parse validation mode.");
    return;
  }

  if (validation_mode == "OutputInstancePair")
  {
    validationModeEnum_ = OutputInstancePair;
    MarControlAccessor acc(getctrl("mrs_realvec/instanceIndexes"));
    realvec& instIdxs = acc.to<mrs_realvec>();
    instIdxs.create(0.0, 1, 2); //init row vector
    setctrl("mrs_natural/onSamples", 2);
    return;
  }

  if(validation_mode == "kFold")
  { //Validation mode is Folding, now extract the fold count.

    string fold_type_str;
    if (!(validation_mode_args >> fold_type_str))
    {
      MRSERR(v_mode_error_msg << " Could not parse fold type.");
      return;
    }

    ValidationModeEnum fold_type;
    if(fold_type_str == "NS")
      fold_type = kFoldNonStratified;
    else if(fold_type_str == "S")
      fold_type = kFoldStratified;
    else
    {
      MRSERR(v_mode_error_msg << " Invalid fold type: " << fold_type_str);
      return;
    }

    mrs_natural fold_count = -1;
    if ( !(validation_mode_args >> fold_count) )
    {
      MRSERR(v_mode_error_msg << " Can not parse fold count.");
      return;
    }
    if ( !(fold_count >= 2 && fold_count <= 10) )
    {
      MRSERR(v_mode_error_msg << " Invalid fold count: " << fold_count);
      return;
    }

    validationModeEnum_ = fold_type;
    foldCount_ = fold_count;

    data_.Shuffle();
    // data_.Dump("shuffle.txt", classesFound_);

    if( validationModeEnum_ != kFoldStratified)
    {
      cout << "=== Non-Stratified cross-validation (" <<  foldCount_ << " folds) ===" << endl;

      //in non-stratified mode we simply use all the available data
      foldData_.SetupkFoldSections(data_, foldCount_);
    }
    else
    {
      cout << "=== Stratified cross-validation (" <<  foldCount_ << " folds) ===" << endl;

      //in non-stratified we seperate the data according to class
      foldClassData_.clear();
      foldClassData_.resize(classesFound_.size());

      //load each dataset with rows for each class
      for(mrs_natural ii=0; ii<(mrs_natural)classesFound_.size(); ++ii)
      {
        WekaFoldData data;
        data.setFold(true);
        data.SetupkFoldSections(data_, foldCount_, ii);
        foldClassData_[ii] = data;
      }
      foldClassDataIndex_ = 0;
    }

    foldCurrentMode_ = foldNextMode_ = WekaFoldData::Training;

  }//if "kFold"
  else if(validation_mode == "UseTestSet")
  {
    string test_set_filename;
    validation_mode_args >> test_set_filename;
    if (test_set_filename.empty())
    {
      MRSERR(v_mode_error_msg << " Can not parse test set filename.");
      return;
    }

    loadFile(test_set_filename, attributesToInclude_, useTestSetData_);
    MRSASSERT(data_.getCols()==useTestSetData_.getCols());

    cout << "=== Evaluation on test set === (" <<  test_set_filename << ") ===" << endl;

    validationModeEnum_ = UseTestSet;
    currentIndex_ = 0;

  }//else if "UseTestSet"
  else if(validation_mode == "PercentageSplit")
  {
    mrs_natural percentage_split = -1;

    if ( !(validation_mode_args >> percentage_split) )
    {
      MRSERR(v_mode_error_msg << " Can not parse percentage split.");
      return;
    }

    if ( !(percentage_split > 0 && percentage_split < 100) )
    {
      MRSERR(v_mode_error_msg << " Invalid percentage split: " << percentage_split);
      return;
    }

    cout << "=== Evaluation on percentage split " << percentage_split << "% ===" << endl;

    data_.Shuffle();
    data_.Dump("shuffle.txt", classesFound_);

    percentageIndex_ = ((mrs_natural)data_.size() * percentage_split) / 100;
    percentageIndex_--; //adjust to count from 0
    if(percentageIndex_ < 1) percentageIndex_ = 1;

    validationModeEnum_ = PercentageSplit;
    currentIndex_ = 0;
  } //else if "PercentageSplit"
  else
  {
    MRSERR("Invalid validation mode: " << validation_mode);
    return;
  }
}//myUpdate

void WekaSource::myProcess(realvec& in,realvec &out)
{
  (void) in;

  if(getctrl("mrs_bool/done")->to<mrs_bool>()) return;
  bool trainMode = (strcmp(getctrl("mrs_string/mode")->to<mrs_string>().c_str(), "train") == 0);
  switch(validationModeEnum_)
  {
  case kFoldNonStratified:
    handleFoldingNonStratifiedValidation(trainMode, out);
    break;
  case kFoldStratified:
    handleFoldingStratifiedValidation(trainMode, out);
    break;
  case UseTestSet:
    handleUseTestSet(trainMode, out);
    break;
  case PercentageSplit:
    handlePercentageSplit(trainMode, out);
    break;
  case OutputInstancePair:
    handleInstancePair(out);
    break;
  default:
    handleDefault(trainMode, out);
  }//switch
}//myProcess


void
WekaSource::handleDefault(bool trainMode, realvec &out)
{
  //FIXME: Unused parameter
  (void) trainMode;

  if(currentIndex_ >= (mrs_natural)data_.size())
  {
    this->updControl("mrs_bool/done", true);
    return;
  }

  vector<mrs_real> *row = NULL;
  mrs_string fname = data_.GetFilename(currentIndex_);
  row = data_.at(currentIndex_++);

  for(mrs_natural ii=0; ii<(mrs_natural)row->size(); ++ii)
  {
    out(ii, 0) = row->at(ii);
    this->updControl("mrs_string/currentFilename", fname); //???: why are we always updating this control to fname?? (which does not change inside the for loop...)
  }
}

void
WekaSource::handleInstancePair(realvec& out)
{
  const realvec& instIdxs = getctrl("mrs_realvec/instanceIndexes")->to<mrs_realvec>();

  mrs_natural i = (mrs_natural)instIdxs(0);
  mrs_natural j = (mrs_natural)instIdxs(1);

  if(i >= (mrs_natural)data_.size() || j >= (mrs_natural)data_.size())
  {
    //this->updControl("mrs_bool/done", true); //!!!: done?
    MRSWARN("WekaSource::handlePair - out of bound file indexes!");
    return;
  }

  vector<mrs_real> *rowi = NULL;
  vector<mrs_real> *rowj = NULL;

  mrs_string fnamei = data_.GetFilename(i);
  mrs_string fnamej = data_.GetFilename(j);

  rowi = data_.at(i);
  rowj = data_.at(j);

  for(mrs_natural ii=0; ii<(mrs_natural)rowi->size(); ++ii)
  {
    out(ii, 0) = rowi->at(ii);
    out(ii, 1) = rowj->at(ii);
  }
  this->updControl("mrs_string/currentFilename", fnamei+"_"+fnamej);

}

void WekaSource::handlePercentageSplit(bool trainMode, realvec &out)
{
  vector<mrs_real> *row = NULL;

  if(trainMode)
  {
    if (currentIndex_ >= percentageIndex_)
    {
      this->updControl("mrs_string/mode", "predict");
      trainMode = false;
    }
    else
    {
      row = data_.at(currentIndex_++);
    }
  }

  if(!trainMode)
  {
    if(currentIndex_ >= (mrs_natural)data_.size())
    {
      this->updControl("mrs_bool/done", true);
      return;
    }
    else
    {
      row = data_.at(currentIndex_++);
    }
  }

  //  MRSASSERT(row->size()==out.getCols()); //[!]
  for(mrs_natural ii=0; ii<(mrs_natural)row->size(); ++ii)
  {
    out(ii, 0) = row->at(ii);
  }
} //handlePercentageSplit

void WekaSource::handleUseTestSet(bool trainMode, realvec &out)
{
  vector<mrs_real> *row = NULL;
  if(trainMode)
  {
    if(currentIndex_ >= (mrs_natural)data_.size())
    {
      this->updControl("mrs_string/mode", "predict");
      trainMode = false;
      currentIndex_ = 0;
    }
    else
    {
      row = data_.at(currentIndex_++);
    }
  }

  if(!trainMode)
  {
    if(currentIndex_ >= (mrs_natural)useTestSetData_.size())
    {
      this->updControl("mrs_bool/done", true);
      currentIndex_ = 0;
      return;
    }
    else
    {
      row = useTestSetData_.at(currentIndex_++);
    }
  }

  MRSASSERT((mrs_natural)row->size() == out.getCols());
  for(mrs_natural ii=0; ii<(mrs_natural)row->size(); ++ii)
  {
    out(ii, 0 ) = row->at(ii);
  }
}//handleUseTestSet

void WekaSource::handleFoldingStratifiedValidation(bool trainMode, realvec &out)
{
  (void) trainMode;

  if (foldCurrentMode_ != foldNextMode_)
  {
    foldClassDataIndex_++;
    if(foldClassDataIndex_ >= (mrs_natural)foldClassData_.size())
    {
      foldClassDataIndex_ = 0;
      foldCurrentMode_ = foldNextMode_;
      switch(foldCurrentMode_)
      {
      case WekaFoldData::Training:
        updControl("mrs_string/mode", "train");
        break;
      case WekaFoldData::Predict:
        updControl("mrs_string/mode", "predict");
        break;
      case WekaFoldData::None:
        updControl("mrs_bool/done", true);
        return;
      }
    }
  }

  vector<mrs_real> *row = foldClassData_[foldClassDataIndex_].Next(foldNextMode_);

  MRSASSERT((mrs_natural)row->size() == out.getRows());
  for(mrs_natural ii=0; ii<(mrs_natural)row->size(); ++ii)
  {
    out(ii, 0) = row->at(ii);
  }
}

void WekaSource::handleFoldingNonStratifiedValidation(bool trainMode, realvec &out)
{
  (void) trainMode;

  if( foldCurrentMode_ != foldNextMode_ )
  {
    foldCurrentMode_ = foldNextMode_;
    switch (foldCurrentMode_)
    {
    case WekaFoldData::Training:
      updControl("mrs_string/mode", "train");
      break;
    case WekaFoldData::Predict:
      updControl("mrs_string/mode", "predict");
      break;
    case WekaFoldData::None:
      updControl("mrs_bool/done", true);
      return;
    }
  }

  vector<mrs_real> *row = foldData_.Next(foldNextMode_);

  MRSASSERT((mrs_natural) row->size() == out.getRows());

  for(mrs_natural ii=0; ii<(mrs_natural)row->size(); ++ii)
  {
    out(ii, 0) = row->at(ii);
  }
}

void WekaSource::loadFile(const std::string& filename, const std::string& attributesToExtract, WekaData& data)
{
  ifstream mis;

  mis.open(filename.c_str());

  if (!mis.is_open()) {
    std::string msg = std::string("WekaSource: could not open file: ") + filename;
    MRSERR(msg);
    throw std::runtime_error(msg);
  }

  data_.Clear();

  parseHeader(mis, filename, attributesToExtract);

  parseData(mis, filename, data);

  mis.close();
}//loadFile

void WekaSource::parseHeader(ifstream& mis, const mrs_string& filename, const std::string& attributesToExtract)
{
  (void) attributesToExtract; // FIXME: suspiciously not used!
  (void) filename; // only used for debug messages; see ifstream& mis
  // FIXME: This method does not parse all valid relation or attribute names.
  //        The ARFF spec allows for names that include spaces, iff those
  //        names are quoted.
  // FIXME: Parsing errors should probably be fatal.

  char str[1024];
  // skip comment lines
  while (mis.peek() == '%')
  {
    mis.getline(str, 1023);
  }


  mrs_string token1,token2,token3;
  mrs_string whitespace = " \t\v\f\r\n";
  mrs_string::size_type startpos;

  // Read in the relation line
  mis >> token1;
  getline(mis, token2);

  // Strip leading whitespace from the relation name
  startpos = token2.find_first_not_of(whitespace);
  if (startpos != mrs_string::npos)
  {
    token2 = token2.substr(startpos);
  }

  if ((token1 != "@relation")&&(token1 != "@RELATION"))
  {
    MRSERR("Badly formatted .arff file: file must begin with @relation." + token1);
    return;
  }
  if (token2.find("\t") != mrs_string::npos)
  {
    MRSERR("Badly formatted .arff file: Relation name cannot contain tab characters.");
    return;
  }
  if (token2.find_first_of(whitespace) != mrs_string::npos)
  {
    MRSERR("Badly formatted .arff file: Marsyas cannot handle relation names with whitespace.");
    return;
  }
  relation_ = token2;

  attributesFound_.clear();
  attributesIncluded_.clear();
  classesFound_.clear();
  attributesIncludedList_.clear();

  // Parse the attribute definitions and store their names...
  //ie: @attribute Mean_Mem40_Centroid real
  while( mis >> token1 && (token1 == "@attribute" || (token1 == "@ATTRIBUTE")))
  {
    mis >> token2;
    getline(mis, token3);

    // skip leading spaces of token3
    startpos = token3.find_first_not_of(" \t");
    if (mrs_string::npos != startpos)
      token3 = token3.substr(startpos);

    if ((token3 == "real") || (token3 == "REAL"))
    {
      attributesFound_.push_back(token2);
      attributesIncluded_.push_back(true);
    }
    else if (token3[0] == '{')
    {
      mrs_string token = token3.substr( 1, token3.length()-2 );	// Remove curly braces

      std::stringstream  tokenStream(token);
      std::string        cell;
      while(std::getline(tokenStream,cell,','))
      {
        classesFound_.push_back(cell);
      }
    }
    else
    {
      attributesFound_.push_back(token2);
      attributesIncluded_.push_back(false);
      MRSWARN("Incompatible datatype " + token3 + " found in file '" + filename + "'.  " +
              "attribute " + token2 + "will be ignored!");
    }//else
  }//while

  if ((token1 != "@data") && (token1 != "@DATA")) {
    MRSERR("Badly formatted .arff file: Finished parsing attributes but did not find @data section.");
  }

  //Now we parse the attributes to include string and decide which attributes
  //are to be extracted from the arff file. An empty include list means all
  //attributes.

  for(vector<mrs_string>::const_iterator citer = attributesFound_.begin(); citer!= attributesFound_.end(); citer++)
  {
  }

  if (classesFound_.size() == 0) {
    ctrl_regression_->setValue(true);
    // remove the final "output" attribute
    attributesFound_.pop_back();
    attributesIncluded_.pop_back();
  }

  parseAttributesToInclude(attributesToInclude_);
}//parseHeader

void WekaSource::parseData(ifstream& mis, const mrs_string& filename, WekaData& data)
{
  // FIXME Unused parameter
  (void) filename;
  mrs_string currentFname;

  MRSASSERT(!mis.eof());

  data.Create((mrs_natural)attributesIncludedList_.size()+1);

  char str[1024];

  while (mis.peek() == '%') 			// skip comment lines
  {
    mis.getline(str, 1023);
  }


  mrs_string token;
  // mis >> token;

  while (token == "")
    getline(mis, token);

  mrs_natural lineCount = 0;
  while(!mis.eof())
  {
    char *cp = (char *)token.c_str();
    if (cp[0] != '%')
    {
      cp = strtok(cp, ",");

      vector<mrs_real> *lineBuffer = new vector<mrs_real>(attributesIncludedList_.size()+1);

      mrs_natural index = 0;
      for(mrs_natural ii=0; ii < (mrs_natural)attributesFound_.size(); ++ii)
      {
        MRSASSERT( cp!=NULL );
        if(attributesIncluded_[ii])
        {
          lineBuffer->at(index++) = ::atof( cp );
        }
        cp = strtok(NULL, ",");
      }//for index
      MRSASSERT(index == (mrs_natural)lineBuffer->size()-1);

      if (ctrl_regression_->isTrue()) {
        // no change needed
        lineBuffer->at(index) =  ::atof( cp );
      } else {
        //now extract the class name for this record
        MRSASSERT( cp!=NULL );

        mrs_natural classIndex = findClass(cp);
        MRSASSERT(classIndex>=0);
        lineBuffer->at(index) = (mrs_real)classIndex;
      }

      data.Append(lineBuffer);
      data.AppendFilename(currentFname);
      lineCount++;

      // apparently not copied?
      //delete lineBuffer; // info was copied in Append()
    }
    else // skip comment line
    {
//		    mis.getline(str, 1023);
      // If the line starts with "% filename" set the current_filename
      if (strncmp(token.c_str(),"% filename",10) == 0) {
        currentFname = token.substr(11);
      }

      if (strncmp(token.c_str(),"% srate",7) == 0) {
        mrs_real currentSrate = atof(token.substr(8).c_str());
        this->updControl("mrs_real/currentSrate", currentSrate);
      }
    }
    getline(mis,token);
  }//while



}

//Given a string, check if it is an class found in the arff file header.
//If it is, return its index, otherwise return -1
mrs_natural WekaSource::findClass(const char *className) const
{
  MRSASSERT(className!=NULL);
  mrs_natural index = 0;
  for(vector<mrs_string>::const_iterator citer = classesFound_.begin(); citer!= classesFound_.end(); citer++,index++)
  {
    if(*citer == className)
      return index;
  }//for citer
  return -1;
}//FindClass

//Given a string, check if it is an attribute found in the arff file header.
//If it is, return its index, otherwise return -1
mrs_natural WekaSource::findAttribute(const char *attribute) const
{
  MRSASSERT(attribute!=NULL);

  mrs_natural index = 0;
  for(vector<mrs_string>::const_iterator citer = attributesFound_.begin(); citer!= attributesFound_.end(); citer++,index++)
  {
    if(*citer == attribute)
      return index;
  }//for citer
  return -1;
}//FindAttribute

//Given a string, determine if it is an attribute name or an integer
//It is an attribute name if it is contained in the list of attributes
//parsed from the header of the arff file.
//If it is neither
mrs_natural WekaSource::parseAttribute(const char *attribute) const
{
  MRSASSERT(attribute!=NULL);

  //check for attribute in list found in header and if found,
  //return its index.
  mrs_natural ret = findAttribute(attribute);
  if(ret >= 0) return ret;

  //otherwise, check if the string is a valid integer. If not return -1
  for(mrs_natural ii=0; attribute[ii]!='\0'; ++ii)
    if(!isdigit(attribute[ii]))
      return -1;

  //otherwise return the index.
  return ::atoi(attribute);
}//parseAttribute

//Given an attribute string check if it is a single attribute name, or range of attributes.
//Attributes can be expressed as a string or an integer index.
//Some examples are:
// 1-5										; attributes at index 1 to 5 inclusive
// Mean_Mem40_MFCC_1 - Mean_Mem40_MFCC_3	; these attributes and all between inclusive
// Mean_Mem40_MFCC_1 - 7
// Mean_Mem40_Centroid
// Note that the attribute index on the left must be <= attribute index on the right
// Once the indexes are known, set those index values to true in the attributes to include array
void WekaSource::parseAttributesToInclude(const std::string& attributesToInclude)
{
  // FIXME Unused parameter.
  (void) attributesToInclude;
  //resize the included attribute bool array to the same size as the actual number of attributes
  //in the arff file.
  attributesIncluded_.resize(attributesFound_.size());

  //if null string specified, set all attributes to include to true
  //and set attributes to include list the same as attributes found
  if(attributesToInclude_.size()==0)
  {

    attributesIncludedList_.assign(attributesFound_.begin(), attributesFound_.end());
    for(mrs_natural ii=0; ii<(mrs_natural)attributesIncluded_.size(); ++ii)
    {
      attributesIncluded_[ii] = true;

    }

    return;
  }//if

  //Otherwise lets assume all attributes are out for now
  for(mrs_natural ii=0; ii<(mrs_natural)attributesIncluded_.size(); ++ii)
    attributesIncluded_[ii] = false;

  //get a copy of the attributes to include list and start parsing for the "," seperators
  mrs_string str = attributesToInclude_;
  char *cp = strtok((char *)str.c_str(), ",");

  //find each string seperated by a "," and parse it for attributes
  while(cp)
  {
    //check if this string has a "-" seperator
    char *mp = strstr(cp,"-");
    if(mp)
    {
      //yes it does, so lets parse each side of the "-"
      *mp++ = '\0';

      //check the left side. Check for valid
      mrs_natural left = parseAttribute(cp);
      MRSASSERT(left>=0&&left<(mrs_natural)attributesFound_.size());

      //check the right side. Check for valid
      mrs_natural right = parseAttribute(mp);
      MRSASSERT(right>=0&&right<(mrs_natural)attributesFound_.size());

      //make sure numbers are in the right order
      MRSASSERT(right>=left);

      //and set the attributes included flag for this range of attributes
      for(mrs_natural ii=left; ii<=right; ++ii)
        attributesIncluded_[ii] = true;
    }//if
    //No "-" seperator, just parse this one attribute or index
    else
    {
      mrs_natural index = parseAttribute(cp);
      MRSASSERT(index>=0&&index<(mrs_natural)attributesFound_.size());
      attributesIncluded_[index] = true;
    }//else

    //next token
    cp = strtok(NULL, ",");
  }//while

  //Now build the attributes included list from the original attributes found list.
  //Use the included flags array to determine which attributes to copy
  attributesIncludedList_.clear();
  for(mrs_natural ii=0; ii<(mrs_natural)attributesIncluded_.size(); ++ii)
  {
    if(attributesIncluded_[ii])
      attributesIncludedList_.push_back(attributesFound_[ii]);
  }//for ii
}//parseAttributesToExtract
