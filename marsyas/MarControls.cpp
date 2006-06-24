/** 
    \class MarControls
    \brief MarControls handle the controls for MarSystems
    
    MarControls is the class that handles the controls for 
a particular MarSystem. 

 Created by Ari Lazier on Mon Aug 11 2003.
 Modified by George Tzanetakis 

*/

#include "MarControls.h"

using namespace std;
using namespace Marsyas;


bool 
MarControls::hasState(string cname) 
{
  if (hasState_.find(cname) == hasState_.end())
    {
    MRSWARN("MarControls::hasState Unsupported control name = " + cname);
    return false;
  }
  bool res = hasState_[cname];
  return res;
}

void 
MarControls::setState(string cname, bool st) 
{
  map<string, bool>::iterator iter = hasState_.find(cname);
  if (iter == hasState_.end())
  {
    MRSWARN("MarControls::setState Unsupported control name = " + cname);
  }
  else 
    hasState_[cname] = st;
  
}





MarControlValue
MarControls::getControl(string cname)
{
  if (controls_.find(cname) == controls_.end())
  {
    MRSWARN("MarControls::getctrl Unsupported control name = " + cname);
    return false;
  }
  return controls_[cname];
}

  
void
MarControls::addControl(string cname, MarControlValue value)
{
  controls_[cname] = value;
  hasState_[cname] = false;
}

map<string, MarControlValue>
MarControls::getControls()
{
  return controls_;
}


bool
MarControls::updControl(string cname, bool value)
{
  iter_ = controls_.find(cname);
  if (iter_ == controls_.end())
  {
    MRSWARN("MarControls::updctrl Unsupported bool control name = " + cname);
    return false;
  }
  else if(!(iter_->second.update(value)))
  {
    MRSWARN("MarControls::updctrl Unable to update " + cname);
  }
  else return true;

  return false;
}


bool
MarControls::updControl(string cname, mrs_real value)
{
  iter_ = controls_.find(cname);
  if (iter_ == controls_.end())
  {
    MRSWARN("MarControls::updctrl Unsupported mrs_real control name = " + cname);
    return false;
  }
  else if(!(iter_->second.update(value)))
  {
    MRSWARN("MarControls::updctrl Unable to update " + cname);
  }
  else return true;
  
  return false;
}


bool
MarControls::updControl(string cname, mrs_natural value)
{
  
  iter_ = controls_.find(cname);
  if (iter_ == controls_.end())
  {
    MRSWARN("MarControls::updctrl Unsupported mrs_natural control name = " + cname);
    return false;
  }
  else if(!(iter_->second.update(value)))
  {
    MRSWARN("MarControls::updctrl Unable to update " + cname);
  }
  else return true;

  return false;
}


bool 
MarControls::hasControl(string cname) 
{
  iter_ = controls_.find(cname);
  return (iter_ != controls_.end());
}


bool
MarControls::updControl(string cname, MarControlValue value)
{
  
  iter_ = controls_.find(cname);
  if (iter_ == controls_.end())
  {
    MRSWARN("MarControls::updctrl Unsupported MarControlValue control name = " + cname);
    return false;
  }
  else if(!(iter_->second.update(value)))
  {
    MRSWARN("MarControls::updctrl Unable to update " + cname);
  }
  else return true;
  
  return false;
}


void
MarControls::clear()
{
  controls_.clear();
  hasState_.clear();
}

int
MarControls::size()
{
  return controls_.size();
}


istream& 
Marsyas::operator>> (istream& is, MarControls& c)
{
  string skipstr;
  
  is >> skipstr >> skipstr >> skipstr;
  
  mrs_natural nControls;
  is >> nControls;
  
  
  mrs_natural i;
  string type;
  string rstr = "mrs_real";
  string nstr = "mrs_natural";
  string bstr = "mrs_bool";
  string sstr = "mrs_string";
  string vstr = "mrs_realvec";
  mrs_real   rcvalue;
  string scvalue;
  mrs_natural ncvalue;
  bool bcvalue;
  string cname;
  map<string, MarControlValue>::iterator iter;  
  
  for (i=0; i < nControls; i++)
    {
      is >> skipstr;
      is >> cname;
      string ctype1;
      string ctype;
      
      // string::size_type pos = cname.rfind("/");
      ctype1 = cname.substr(0,cname.rfind("/", cname.length()));
      ctype = ctype1.substr(ctype1.rfind("/", ctype1.length())+1, ctype1.length());
      

      iter = c.controls_.find(cname);


      if (ctype == rstr)
	{
	  is >> skipstr >> rcvalue;
	  if (iter == c.controls_.end())
	    c.addControl(cname, rcvalue);
	  else
	    c.updControl(cname, rcvalue);
	}
      if (ctype == sstr)
	{
	  is >> skipstr >> scvalue;

	  if (iter == c.controls_.end())
	    c.addControl(cname, scvalue);
	  else
	    c.updControl(cname, scvalue);
	  
	}
      if (ctype == nstr)
	{
	  is >> skipstr >> ncvalue;



	  if (iter == c.controls_.end())
	    c.addControl(cname, ncvalue);
	  else
	    c.updControl(cname, ncvalue);

	}
      if (ctype == bstr)
	{
	  is >> skipstr >> bcvalue;

	  if (iter == c.controls_.end())
	    c.addControl(cname, bcvalue);
	  else
	    c.updControl(cname, bcvalue);


	}
      if (ctype == vstr)
	{
	  realvec vcvalue;
  	  is >> skipstr >> vcvalue;

	  if (iter == c.controls_.end())
	    c.addControl(cname, vcvalue);
	  else
	    c.updControl(cname, vcvalue);
	}
    }
  return is;
}

ostream&
Marsyas::operator<< (ostream& o, const MarControls& c)
{
  o << "# MarControls = " << c.controls_.size() << endl;
  map<string, MarControlValue>::const_iterator iter;
  for (iter=c.controls_.begin(); iter != c.controls_.end(); ++iter)
    {
      o << "# " << iter->first << " = " << iter->second << endl;
    }
  return o;
  
}





