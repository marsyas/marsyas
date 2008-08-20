/**
   \class WekaSource
   \brief Input source for Weka-formatted (textual) feature data

   This class was rewritten and extended to handle built in classifier validation.

*/
#include "WekaSource.h"

using namespace std;
using namespace Marsyas;

WekaSource::WekaSource(string name):MarSystem("WekaSource",name)
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

	//The mode that the weka source is currently in.
	//Can be  "train" or "predict"
	addctrl("mrs_string/mode", "train");

	//number of output samples will always be 1, regardless of the input samples
	setctrl("mrs_natural/onSamples", 1 );

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

}

void 
WekaSource::myUpdate(MarControlPtr sender)
{
	
	(void) sender;
	MRSDIAG("WekaSource.cpp - WekaSource:myUpdate");
  

	// If 'filename' was updated, or the attributes desired from the Weka file has changed,
	// parse the header portion of the file to get the required attribute names and possible output labels (if any)...
	if (strcmp(filename_.c_str(), getctrl("mrs_string/filename")->to<mrs_string>().c_str()) != 0)
	{
		this->updctrl("mrs_bool/done", false);	  
		filename_ = getctrl("mrs_string/filename")->to<mrs_string>();
		attributesToInclude_ = getctrl("mrs_string/attributesToInclude")->to<mrs_string>();
	  
      
		loadFile(filename_, attributesToInclude_, data_);
		// data_.Dump("org.txt", classesFound_);
	  
	  
	  
		string names;
		bool first = true;
		for(vector<string>::const_iterator citer = classesFound_.begin(); citer!= classesFound_.end(); citer++)
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
		for(vector<string>::const_iterator citer = attributesFound_.begin(); citer!= attributesFound_.end(); citer++,index++)
		{
			if(attributesIncluded_[index])
			{
				if(!first)
					names += ",";
			  
				names += (*citer);
				first = false;
			}//if
		}
		MRSASSERT(index==attributesIncluded_.size());
	  
	  
	  
		setctrl("mrs_string/attributeNames", names);
	  
		setctrl("mrs_natural/onSamples", 1);
		setctrl("mrs_natural/nAttributes", (mrs_natural)attributesFound_.size());
		setctrl("mrs_natural/onObservations", (mrs_natural)attributesFound_.size()+1);
	  
		setctrl("mrs_natural/nInstances", (mrs_natural)data_.getRows());
  
	  
	  
		string mode = getctrl("mrs_string/validationMode")->to<mrs_string>();
		validationMode_ = mode;
	  
		if (validationMode_ == "") 
		{
			validationModeEnum_ = None;
			currentIndex_ = 0;
			return;
		}
	  
	  
	  
		char *cp = strtok((char *)mode.c_str(), ",");
		MRSASSERT(cp!=NULL);
	  
	  
	  
		if(strcmp(cp ,"kFold")==0)
		{//Validation mode is Folding, now extract the fold count.
			data_.Shuffle();
			// data_.Dump("shuffle.txt", classesFound_);
		  
			cp = (char *)strtok(NULL, ",");
			MRSASSERT(cp!=NULL);
		  
			validationModeEnum_ = kFoldStratified;
			if(strcmp(cp,"NS")==0)
				validationModeEnum_ = kFoldNonStratified;
			else if(strcmp(cp,"S")==0)
				validationModeEnum_ = kFoldStratified;
			else
			{
				cout << "cp = " << cp << endl;
				MRSASSERT(0);
				(void)42;
			}
		  
			cp = (char *)strtok(NULL, ",");
			MRSASSERT(cp!=NULL);
		  
			foldCount_ = ::atol(cp);
			MRSASSERT(foldCount_>=2&&foldCount_<=10);
		  
			if( validationModeEnum_ == kFoldStratified)
			{//in stratified mode we simply use all the available data
				cout << "=== Stratified cross-validation (" <<  foldCount_ << " folds) ===" << endl;
				foldData_.SetupkFoldSections(data_, foldCount_);
			}
			else
			{//in non-stratified we seperate the data according to class
				cout << "=== Non-Stratified cross-validation (" <<  foldCount_ << " folds) ===" << endl;
				foldClassData_.clear();
				foldClassData_.resize(classesFound_.size());
			  
				//load each dataset with rows for each class
				for(mrs_natural ii=0; ii<(mrs_natural)classesFound_.size(); ii++)
				{
					WekaFoldData data;
					data.SetupkFoldSections(data_, foldCount_, ii);
					foldClassData_[ii] = data;
				}
				foldClassDataIndex_ = 0;
			}
		  
		}//if "kFold"
		else if(strcmp(cp ,"UseTestSet")==0)
		{
			cout << "WekaSource:UseTestSet" << endl;
			validationModeEnum_ = UseTestSet;
		  
			cp = (char *)strtok(NULL, ",");
			MRSASSERT(cp!=NULL);
			useTestSetFilename_ = cp;
			loadFile(cp, attributesToInclude_, useTestSetData_);
			MRSASSERT(data_.getCols()==useTestSetData_.getCols());
			currentIndex_ = 0;
      
			cout << "=== Evaluation on test set === (" <<  useTestSetFilename_.c_str() << ") ===" << endl;
      
		}//else if "UseTestSet"
		else if(strcmp(cp ,"PercentageSplit")==0)
		{
			data_.Shuffle();
			data_.Dump("shuffle.txt", classesFound_);
			validationModeEnum_ = PercentageSplit;
      
			cp = (char *)strtok(NULL, ",");
			MRSASSERT(cp!=NULL);
			percentageSplit_ = ::atoi(cp);
			cout << "=== Evaluation on percentage split " << percentageSplit_ << "% ===" << endl;
			MRSASSERT(percentageSplit_>0&&percentageSplit_<100);
      
			percentageIndex_ = ((mrs_natural)data_.size() * percentageSplit_) / 100;

			percentageIndex_--; //adjust to count from 0 

			if(percentageIndex_ < 1) percentageIndex_ = 1;
			currentIndex_ = 0;
      
		}//else if "PercentageSplit"
  
		//		cout << "=== Summary ===" << endl << endl;
  

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
      
		default:
			handleDefault(trainMode, out);
    }//switch

  

}//myProcess


void 
WekaSource::handleDefault(bool trainMode, realvec &out)
{
	// FIXME Unused parameter
	(void) trainMode;

	vector<mrs_real> *row = NULL;
	row = data_.at(currentIndex_++);
	if(currentIndex_ >= (mrs_natural)data_.size())
    {
		this->updctrl("mrs_bool/done", true);
    }
	for(mrs_natural ii=0; ii<(mrs_natural)row->size(); ii++)
    {
		out(ii, 0) = row->at(ii);
    }
}

void WekaSource::handlePercentageSplit(bool trainMode, realvec &out)
{
	vector<mrs_real> *row = NULL;
	if(trainMode)
    {
		MRSASSERT(currentIndex_<percentageIndex_);
		row = data_.at(currentIndex_++);


		if(currentIndex_>=percentageIndex_)
		{
			this->updctrl("mrs_string/mode", "predict");
		}
    }
	else
    {
		row = data_.at(currentIndex_++);
		if(currentIndex_ >= (mrs_natural)data_.size())
		{
			this->updctrl("mrs_bool/done", true);
		}//if
    }//else
//  MRSASSERT(row->size()==out.getCols()); //[!]
	for(mrs_natural ii=0; ii<(mrs_natural)row->size(); ii++)
    {
		out(ii, 0) = row->at(ii);
    }//for ii
}//handlePercentageSplit

void WekaSource::handleUseTestSet(bool trainMode, realvec &out)
{
	vector<mrs_real> *row = NULL;
	if(trainMode)
    {//train mode
		row = data_.at(currentIndex_++);

		if(currentIndex_ >= (mrs_natural)data_.size())
		{
			this->updctrl("mrs_string/mode", "predict");
			currentIndex_ = 0;
		}//if
    }
	else
    {//predict mode
		row = useTestSetData_.at(currentIndex_++);

		if(currentIndex_ >= (mrs_natural)useTestSetData_.size())
		{
			this->updctrl("mrs_bool/done", true);
			currentIndex_ = 0;
		}
    }//else
	MRSASSERT(row->size()==out.getCols());
	for(mrs_natural ii=0; ii<(mrs_natural)row->size(); ii++)
    {
		out(ii, 0 ) = row->at(ii);
    }//for ii
}//handleUseTestSet

void WekaSource::handleFoldingNonStratifiedValidation(bool trainMode, realvec &out)
{
	WekaFoldData::nextMode currentMode = trainMode ? WekaFoldData::Training : WekaFoldData::Predict;

	WekaFoldData::nextMode next;
	vector<mrs_real> *row = foldClassData_[foldClassDataIndex_].Next(next);

	switch(currentMode)
    {
		case WekaFoldData::Training:
		{
			if(next == WekaFoldData::Predict)
			{
				foldClassDataIndex_++;
				if(foldClassDataIndex_ >= (mrs_natural)foldClassData_.size())
				{
					foldClassDataIndex_ = 0;
					this->updctrl("mrs_string/mode", "predict");
				}
			}
		}break;
		case WekaFoldData::Predict:
		{
			if(next == WekaFoldData::None)
			{
				foldClassDataIndex_++;
				if(foldClassDataIndex_ >= (mrs_natural)foldClassData_.size())
				{
					foldClassDataIndex_ = 0;
					this->updctrl("mrs_bool/done", true);
				}
			}
			else if(next == WekaFoldData::Training)
			{
				foldClassDataIndex_++;
				if(foldClassDataIndex_ >= (mrs_natural)foldClassData_.size())
				{
					foldClassDataIndex_ = 0;
					this->updctrl("mrs_string/mode", "train");
				}
			}
		}break;
		// FIXME There are unhandled cases in this switch;
		default:
			(void) 42;
    }//switch


	MRSASSERT(row->size()==out.getRows());
	for(mrs_natural ii=0; ii<(mrs_natural)row->size(); ii++)
    {
		out(ii, 0) = row->at(ii);
    }//for ii
}

void WekaSource::handleFoldingStratifiedValidation(bool trainMode, realvec &out)
{
	WekaFoldData::nextMode next;
	vector<mrs_real> *row = foldData_.Next(next);

	if(next == WekaFoldData::None)
		this->updctrl("mrs_bool/done", true);
	else if(next == WekaFoldData::Training && !trainMode)
		this->updctrl("mrs_string/mode", "train");
	else if(next == WekaFoldData::Predict && trainMode)
		this->updctrl("mrs_string/mode", "predict");

	MRSASSERT(row->size()==out.getCols());
	for(mrs_natural ii=0; ii<(mrs_natural)row->size(); ii++)
    {
		out(ii, 0) = row->at(ii);
    }//for ii
}//handleFoldingValidation

void WekaSource::loadFile(const std::string& filename, const std::string& attributesToExtract, WekaData& data)
{

	ifstream *mis = new ifstream;
  
	mis->open(filename.c_str());
	MRSASSERT( mis->is_open() );
  
  
	parseHeader(*mis, filename, attributesToExtract);
	parseData(*mis, filename, data);
	mis->close();
	delete mis;
  
}//loadFile

void WekaSource::parseHeader(ifstream& mis, const string& filename, const std::string& attributesToExtract)
{
	// FIXME Unused parameter
	(void) attributesToExtract;
	char str[1024];
	// skip comment lines 
	while (mis.peek() == '%') 
    {
		mis.getline(str, 1023);
    }
  
	string token1,token2,token3;
  
	mis >> token1;
	if ((token1 != "@relation")&&(token1 != "@RELATION"))
    {
		MRSERR("Not proper weka .arff file");
		return;
    }
	
	mis >> token2;
//  MRSASSERT ( strcmp( token2.c_str(), "marsyas") == 0 ); //[!]
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
		mis >> token3;
	  
		if ((token3 == "real") || (token3 == "REAL"))
		{
			attributesFound_.push_back(token2);
			attributesIncluded_.push_back(true);
		}
		else if (token3[0] == '{')
		{
			string token = token3.substr( 1, token3.length()-2 );	// Remove curly braces
			char *cp = (char *)token.c_str();
			cp = strtok(cp, ",");
			while(cp)
			{

				classesFound_.push_back(cp);
				cp = strtok(NULL, ",");
			}//while
		}
		else
		{
			attributesFound_.push_back(token2);
			attributesIncluded_.push_back(false);
			MRSWARN("Incompatible datatype " + token3 + " found in file '" + filename + "'.  " + 
					"attribute " + token2 + "will be ignored!");
		}//else
    }//while

	//Now we parse the attributes to include string and decide which attributes
	//are to be extracted from the arff file. An empty include list means all
	//attributes.
	
	
	for(vector<string>::const_iterator citer = attributesFound_.begin(); citer!= attributesFound_.end(); citer++)
    {

    }
	
	
	
	
	parseAttributesToInclude(attributesToInclude_);
	
}//parseHeader

void WekaSource::parseData(ifstream& mis, const string& filename, WekaData& data)
{
	// FIXME Unused parameter
	(void) filename;



	MRSASSERT(!mis.eof());
  
	data.Create(attributesIncludedList_.size()+1);

	char str[1024];  
	while (mis.peek() == '%') 
    {
		mis.getline(str, 1023);
    }




  
  
	string token;
	mis >> token;



	mrs_natural lineCount = 0;
	while(!mis.eof())
    {
		char *cp = (char *)token.c_str();
		if (cp[0] != '%')
		{
			cp = strtok(cp, ",");
	  
			vector<mrs_real> *lineBuffer = new vector<mrs_real>(attributesIncludedList_.size()+1);
	  
			mrs_natural index = 0;
			for(mrs_natural ii=0; ii < (mrs_natural)attributesFound_.size(); ii++)
			{
				MRSASSERT( cp!=NULL );
				if(attributesIncluded_[ii])
				{
					lineBuffer->at(index++) = ::atof( cp );

				}
				cp = strtok(NULL, ",");
			}//for index
			MRSASSERT(index==lineBuffer->size()-1);
	  
			//now extract the class name for this record
			MRSASSERT( cp!=NULL );
			mrs_natural classIndex = findClass(cp);

			MRSASSERT(classIndex>=0);
			lineBuffer->at(index) = (mrs_real)classIndex;
	  
			data.Append(lineBuffer);
			lineCount++;
		}
		else // skip comment line 
		{
			mis.getline(str, 1023);
		}
		mis >> token;
    }//while
}

//Given a string, check if it is an class found in the arff file header.
//If it is, return its index, otherwise return -1
mrs_natural WekaSource::findClass(const char *className) const
{
	MRSASSERT(className!=NULL);
	mrs_natural index = 0;
	for(vector<string>::const_iterator citer = classesFound_.begin(); citer!= classesFound_.end(); citer++,index++)
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
	for(vector<string>::const_iterator citer = attributesFound_.begin(); citer!= attributesFound_.end(); citer++,index++)
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
	for(mrs_natural ii=0; attribute[ii]!='\0'; ii++)
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
		for(mrs_natural ii=0; ii<(mrs_natural)attributesIncluded_.size(); ii++)
		{
			attributesIncluded_[ii] = true;

		}

		return;
    }//if

	//Otherwise lets assume all attributes are out for now
	for(mrs_natural ii=0; ii<(mrs_natural)attributesIncluded_.size(); ii++)
		attributesIncluded_[ii] = false;

	//get a copy of the attributes to include list and start parsing for the "," seperators
	string str = attributesToInclude_;
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
			for(mrs_natural ii=left; ii<=right; ii++)
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
	for(mrs_natural ii=0; ii<(mrs_natural)attributesIncluded_.size(); ii++)
    {
		if(attributesIncluded_[ii])
			attributesIncludedList_.push_back(attributesFound_[ii]);
    }//for ii
}//parseAttributesToExtract
