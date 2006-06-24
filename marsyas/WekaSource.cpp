
/**
   \class WekaSource
   \brief Input sink for Weka-formatted (textual) feature data

*/


#include "WekaSource.h"

using namespace std;
using namespace Marsyas;

WekaSource::WekaSource()
{
  type_ = "WekaSource";
  name_ = "";
  
  mis_ = new ifstream;
  
  addControls();
}



WekaSource::WekaSource(string name)
{
  type_ = "WekaSource";
  name_ = name;
  
  mis_ = new ifstream;
  
  addControls();
}



WekaSource::~WekaSource()
{
  if (mis_ != NULL) 
    mis_->close();
  delete mis_;
}



WekaSource::WekaSource(const WekaSource& a)
{
  type_ = a.type_;
  name_ = a.name_;
  ncontrols_ = a.ncontrols_; 		
  
  obsToExtract_ = a.obsToExtract_;	// String specifying the attributes to be extracted from a Weka file
  labelNames_ = a.labelNames_;		// String specifying all possible output labels indicated in a Weka file
  
  inSamples_ = a.inSamples_;
  inObservations_ = a.inObservations_;
  onSamples_ = a.onSamples_;
  onObservations_ = a.onObservations_;
  
  dbg_ = a.dbg_;
  mute_ = a.mute_;
  
  mis_ = new ifstream(); 
  
  update();
}



MarSystem* 
WekaSource::clone() const 
{
  return new WekaSource(*this);
}



void 
WekaSource::addControls()
{
  addDefaultControls();
  
  addctrl("mrs_string/filename", "");
  setctrlState("mrs_string/filename", true);
  
  // A string of ',' separated indexes or index 
  // ranges (specified using '-') of the 
  // attributes desired from the weka file.
  // From this, the number of inObservations is inferred.
  // (Note: attribute indexing starts from 0)
  addctrl("mrs_string/obsToExtract", "");
  setctrlState("mrs_string/obsToExtract", true );
  
  addctrl("mrs_natural/nLabels", 2);
  addctrl("mrs_string/labelNames", "");
  addctrl("mrs_bool/notEmpty", (MarControlValue)false); 	
}



void
WekaSource::update()
{
  MRSDIAG("WekaSource.cpp - WekaSource:update");
  
  char token_  [2000];
  char token2_ [2000];	
  
  
  // If 'inSamples' was updated ...
  if( inSamples_ != getctrl("mrs_natural/inSamples").toNatural() ){
    
    inSamples_ = getctrl("mrs_natural/inSamples").toNatural();
    setctrl("mrs_natural/onSamples", inSamples_ );	
  }
  
  
  
  // If 'filename' was updated, or the attributes desired from the Weka file has changed,
  // parse the header portion of the file to get the required attribute names and possible output labels (if any)...
  if( filename_ != getctrl("mrs_string/filename").toString() || obsToExtract_ != getctrl("mrs_string/obsToExtract").toString() ){
    
    setctrl("mrs_bool/notEmpty", (MarControlValue)true);
    
    filename_ = getctrl("mrs_string/filename").toString();
    inObservations_ = 0;
    string inObsNames = "";
    
    // Parse string 'obsToExtract_' to determine the list of 
    // the required attributes from the Weka file.
    // Store this information in the 'bool extract []' for 
    // simple querying
    obsToExtract_ = getctrl("mrs_string/obsToExtract").toString();
    parseObsToExtract();
    
    if( mis_->is_open() ){
      mis_->close();
    }
    mis_->open(filename_.c_str());  
    MRSASSERT( mis_->is_open() );
    
    (*mis_) >> token_;
    MRSASSERT ( strcmp( token_ , "@relation") == 0 );
    (*mis_) >> token_;
    MRSASSERT ( strcmp( token_, "marsyas") == 0 );
    
    string curLabel;
    string::size_type curIndex;
    mrs_natural nLabels;
    
    totalObs_ = 0;
    
    // Parse the attribute definitions and store their names...
    while( 	(*mis_) >> token_ && strcmp( token_, "@attribute" ) == 0 ){
      
      (*mis_) >> token_;
      (*mis_) >> token2_;
      
      // ... but only if the current attribute was specified by the obsToExtract_ ctrl,
      // or it is the output labeling (always included)
      if( !extract[totalObs_++] && strcmp( token_ , "output" ) != 0 )
	continue;
      
      // If the attribute is 'mrs_real' valued (most are),
      // store the attribute name
      if( strcmp( token2_, "mrs_real" ) == 0 ){
	
	inObsNames += string( token_ ) + ',';
	inObservations_++;
      }
      // If the attribute is an output labelling,
      // store the labels and the number of labels.
      else if( strcmp( token_ , "output" ) == 0 ){
	
	inObsNames += "Label,";
	inObservations_++;
	
	// Store the labels
	labelNames_ = string(token2_);
	labelNames_ = labelNames_.substr( 1, labelNames_.length()-2 ); // Remove curly braces
	if( labelNames_[ labelNames_.length()-1 ] != ',' )
	  labelNames_ += ',';
	setctrl("mrs_string/labelNames", labelNames_);
	
	
	// Now find the number of labels
	curIndex = 0;
	nLabels  = 0;
	
	curIndex = labelNames_.find(",", curIndex);
	while( curIndex != string::npos ){
	  nLabels++;
	  curIndex = labelNames_.find(",", curIndex+1);
	}
	setctrl("mrs_natural/nLabels", nLabels);			
      }
      else
	MRSWARN("Incompatible datatype " + string(token2_) + " found in file '" + filename_ + "'.  " + 
		"Mismatch between attribute names and values will occur!");
    }
    
    MRSASSERT( strcmp( token_ , "@data") == 0 );      
    
    // Check that at least one additional sample is available.
    // If not, set 'notEmpty' to false.
    (*mis_) >> token_;
    if( !mis_->eof() )
      data_ = string(token_);
    else{
      setctrl("mrs_bool/notEmpty", (MarControlValue)false);
      return;		
    }		
    
    setctrl("mrs_string/inObsNames", inObsNames );    
    setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));
    
    setctrl("mrs_natural/inObservations", inObservations_ );
    setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations") );
  }
  
  defaultUpdate();
}



void 
WekaSource::process(realvec& in,realvec &out)
{
	if( getctrl("mrs_bool/notEmpty").toBool() == false )
		return;

  	checkFlow(in,out);
	
  	string ob;
	string::size_type curIndex;
	string::size_type curIndexF;
	
	mrs_natural labelIndex;
	mrs_natural nObs;
	
	mrs_real parsedVal = 0.;

  	char token_  [2000];
		
	for (t = 0; t < inSamples_; t++)
	{		
		curIndex = 0;
		nObs = 0;
		
		for (o=0; o < totalObs_ && getctrl("mrs_bool/notEmpty").toBool() ; o++)
		{		
			// Get the next attributes from the current sample of data...
			curIndexF = data_.find(",", curIndex);
			
			if( curIndexF == string::npos )
				curIndexF = data_.length();

			ob = data_.substr( curIndex, curIndexF-curIndex );
			
			curIndex = curIndexF + 1;
			
			// ... and ignore it if it is not selected specifically by the obsToExtract_ ctrl
			// (but we always extract the last attribute IF it holds the output labeling)
			if( !extract[o] && ( o<totalObs_-1 || getctrl("mrs_string/inObsNames").toString().find("Label") == string::npos ) )
				continue;
			
			
			if( o < totalObs_-1 || getctrl("mrs_string/inObsNames").toString().find("Label") == string::npos ){
			  
			  parsedVal = (mrs_real) atof( ob.c_str() );
				
			  out(nObs++,t) = parsedVal;
			  parsedVal = 0.;
			}
			else{						
				// Determine if the label is in the label list (and not a substring of another label)...
				if( ( curIndexF = labelNames_.find(ob) ) != string::npos && 
				     (curIndexF==0 || labelNames_[ curIndexF-1 ] == ',') &&
				     labelNames_[ curIndexF + ob.length() ] == ',' ){
				     		
					// ...If it is, then determine its index within the list
					labelIndex = 0;
					curIndex = labelNames_.find(",", 0 );

					while( curIndex != string::npos && curIndex < curIndexF  ){
							labelIndex++;
							curIndex = labelNames_.find(",", curIndex+1 );
					}						
					
					out(nObs++,t) = (mrs_real)labelIndex;
				}
				// ... If not, then assign label -1
				else
					out(nObs++,t) = (mrs_real) -1.;
					
			}
			
		}

		// Check for more data at the end of the loop in order to set 'notEmpty' so that we  
		// don't end up producing an empty realvec after processing the file
		if( t<inSamples_ ){
			(*mis_) >> token_;
			
			if( !mis_->eof() )
				data_ = string(token_);
			else{
				setctrl("mrs_bool/notEmpty", (MarControlValue)false);
				return;
			}				
		}					
	}
}



void
WekaSource::parseObsToExtract()
{
	mrs_natural obs1 = -1;
	mrs_natural obs2 = -1;
	mrs_natural tmp;
	
	char c;
	bool range = false;
	
	// If the string specifying the desired attributes is empty,
	// assume all attributes are to be extracted from the Weka file.
	if( obsToExtract_.empty() ){
		for( mrs_natural i=0 ; i<WEKASOURCE_MAX_OBS ; i++ )
			extract[i] = true;
		return;	
	}
	
	for( mrs_natural i=0 ; i<WEKASOURCE_MAX_OBS ; i++ )
		extract[i] = false;
	
	// Parse the obsToExtract_ string, and store info in a
	// bool [] representation for easy querying
	for( string::size_type i=0 ; i<=obsToExtract_.size() ; i++ ){
	
		if( i==obsToExtract_.size() )
			c = ',';
		else
			c = obsToExtract_[i];
	
		MRSASSERT( !( c == ',' && obs1 == -1 ) );
		MRSASSERT( !( c == '-' && obs1 == -1 ) );
		
		switch (c){
		
			case ',' :	MRSASSERT( obs1 < WEKASOURCE_MAX_OBS );
					if( !range )
						extract[obs1] = true;	
					else{
						MRSASSERT( obs2 < WEKASOURCE_MAX_OBS && obs1 <= obs2 );
						for( mrs_natural j=obs1 ; j<=obs2 ; j++ )
							extract[j] = true; 
						range = false;
					}
					obs1 = -1;
					obs2 = -1;
					break;
				
					
						
			case '-' :	range = true;		
					break;
					
					
					
			default  :	MRSASSERT( isdigit(c) );

					tmp = atoi(&c);		
					
					if( obs1 == -1 )
						obs1 = tmp;
					else if( !range )
						obs1 = 10*obs1 + tmp;
					else if( obs2 == -1 )
						obs2 = tmp;
					else
						obs2 = 10*obs2 + tmp;

		}
	
	}

}









