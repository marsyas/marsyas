/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/** 
    \class Composite
    \brief Composite of MarSystem objects
    
    Combines a set MarSystem objects into a single MarSystem 
Different composites such as Fanout, Fanin and Series have 
different semantics of operations. One of the main advantages 
of composites is that control update propagates through 
all objects in the composite. 

*/

#include "Composite.h"

using namespace std;
using namespace Marsyas;

// Composite::Composite()//lmartins: this should be deprecated... [!]
// {
//   type_ = "Composite";
// 	name_ = "CompositePrototype";
//   
// 	marsystemsSize_ = 0;
// 
// 	//if there are any specific controls that
// 	//all composites should have, implement the
// 	//Composite::addControls() method and call it here.
// 	//
// 	//addControls();
// }

Composite::Composite(string type, string name):MarSystem(type, name)
{
	//type_ = "Composite";
	//name_ = name;

	marsystemsSize_ = 0;

	//if there are any specific controls that
	//all composites should have, implement the
	//Composite::addControls() method and call it here.
	//
	//addControls();
}

// copy constructor
Composite::Composite(const Composite& a):MarSystem(a)
{
//Now done by MarSystem copy constructor
//
//   type_ = a.type_;
//   name_ = a.name_;
//   ncontrols_ = a.ncontrols_; 		
//   synonyms_ = a.synonyms_;
//   
//   inSamples_ = a.inSamples_;
//   inObservations_ = a.inObservations_;
//   onSamples_ = a.onSamples_;
//   onObservations_ = a.onObservations_;
//   dbg_ = a.dbg_;
//   mute_ = a.mute_;
//   active_ = a.active_;
  marsystemsSize_ = 0;
  for (mrs_natural i=0; i< a.marsystemsSize_; i++)
  {
    addMarSystem((*a.marsystems_[i]).clone());
  }
}

Composite::~Composite()
{
  for (mrs_natural i=0; i< marsystemsSize_; i++)  
  {
    delete marsystems_[i];
  } 
}

/*
void
Composite::addControls()
{
	//add here any specific controls that all composites
	//should have
}*/


void 
Composite::addMarSystem(MarSystem *marsystem)
{
  marsystems_.push_back(marsystem);
  marsystemsSize_ = marsystems_.size();
  marsystem->update();
  update();
}

void 
Composite::updctrl(string cname, MarControlValue value)
{
  updControl("/" + type_ + "/" + name_ + "/" + cname, value);
}

bool
Composite::hasControlState(string cname)
{
  //bool controlFound = false;
  
  string prefix = "/" + type_ + "/" + name_;
  string childcontrol = cname.substr(prefix.length(), cname.length()-prefix.length());
  string nchildcontrol = childcontrol.substr(1, childcontrol.length());  

  // local controls
  if (hasControlLocal(cname))
  {
    //controlFound = true;
    return ncontrols_.hasState(cname);
  }

//lmartins: what is this for?!? [?]
//   // default composite controls
//   if ((nchildcontrol == "mrs_natural/inSamples")||
//       (nchildcontrol == "mrs_natural/inObservations")||
//       (nchildcontrol == "mrs_real/israte")||
//       (nchildcontrol == "mrs_natural/onSamples")||
//       (nchildcontrol == "mrs_natural/onObservations")||
//       (nchildcontrol == "mrs_real/osrate")||
//       (nchildcontrol == "mrs_bool/debug")||
//       (nchildcontrol == "mrs_string/inObsNames")||
//       (nchildcontrol == "mrs_string/onObsNames"))    
//   {
//     return ncontrols_.hasState(cname);
//   }  

  string cprefix = cname.substr(0, prefix.length());
  if (cprefix != prefix) 
    return false;
  else 
	{
		for (mrs_natural i=0; i< marsystemsSize_; i++)
		{
			if (marsystems_[i]->hasControl(childcontrol))
			{
				//controlFound = true;
				return marsystems_[i]->hasControlState(childcontrol);
			}
		}
	}

  //if (!controlFound) 
  MRSWARN("Composite::hasControlState - Unsupported control name = " + cname);
  return false;
}

// check for controls only in composite object not children
bool
Composite::hasControlLocal(string cname) 
{
  if (ncontrols_.hasControl(cname))
    return true;
  else 
    return false;
}

bool 
Composite::hasControl(string cname)
{
  // check first composite then children
  if (ncontrols_.hasControl(cname))
  {
    return true;
  }
  
  string prefix = "/" + type_ + "/" + name_;

  //Jen
  mrs_natural cname_l = cname.length();
  mrs_natural prefix_l = prefix.length();

  if ( cname_l <= prefix_l )
	  return false;

  string childcontrol = cname.substr(prefix_l, cname_l-prefix_l);  
  string cprefix = cname.substr(0, prefix_l);
  //end Jen
  
  // wrong type 
  if (cprefix != prefix) 
    return false;
  else
  {
    for (mrs_natural i=0; i < marsystemsSize_; i++)
		{
			if (marsystems_[i]->hasControl(childcontrol))
				return true;
		}
  }

  // no control found 
  return false;
}

void
Composite::setControl(string cname, mrs_real value)
{
  bool controlFound = false;
  
  string prefix = "/" + type_ + "/" + name_;
  string childcontrol = cname.substr(prefix.length(), cname.length()-prefix.length());
  string nchildcontrol = childcontrol.substr(1, childcontrol.length());

  if (hasControlLocal(cname))
  {
    controlFound = true;
    ncontrols_.updControl(cname,value);
    return; 
  }
  for (mrs_natural i=0; i< marsystemsSize_; i++)
  {
    if (marsystems_[i]->hasControl(childcontrol))
		{
			controlFound = true;
			marsystems_[i]->setControl(childcontrol,value);
		}
  }
  if (!controlFound)
    MRSWARN("Composite::setctrl - Unsupported control name = " + cname);
}

void
Composite::setControl(string cname, mrs_natural value)
{
  bool controlFound = false;
  
  string prefix = "/" + type_ + "/" + name_;
  string childcontrol = cname.substr(prefix.length(), cname.length()-prefix.length());
  string nchildcontrol = childcontrol.substr(1, childcontrol.length());
  
  if (hasControlLocal(cname))
  {
    controlFound = true;
    ncontrols_.updControl(cname,value);
    return;
  }

  for (mrs_natural i=0; i< marsystemsSize_; i++)
  {
    if (marsystems_[i]->hasControl(childcontrol))
		{
			controlFound = true;
			marsystems_[i]->setControl(childcontrol,value);
		}
  }
  
  if (!controlFound)
    MRSWARN("Composite::setctrl - Unsupported control name = " + cname);
}

void
Composite::setControl(string cname, MarControlValue value)
{
  bool controlFound = false;
  
  string prefix = "/" + type_ + "/" + name_;
  string childcontrol = cname.substr(prefix.length(), cname.length()-prefix.length());
  string nchildcontrol = childcontrol.substr(1, childcontrol.length());
 
  if (hasControlLocal(cname))
  {
    controlFound = true;
    ncontrols_.updControl(cname,value);
    return;
  }
  for (mrs_natural i=0; i< marsystemsSize_; i++)
  {
    if (marsystems_[i]->hasControl(childcontrol))
		{
			controlFound = true;
			marsystems_[i]->setControl(childcontrol,value);
		}
  }
  if (!controlFound)
    MRSWARN("Composite::setctrl - Unsupported control name = " + cname);
}

void 
Composite::updControl(string cname, MarControlValue value)
{ 
  // check for synonyms - call recursively to resolve them 
  map<string, vector<string> >::iterator ei;

  // remove prefix for synonyms
  string prefix = "/" + type_ + "/" + name_ + "/";
  string::size_type pos = cname.find(prefix, 0);
  string shortcname;
  
  if (pos == 0) 
    shortcname = cname.substr(prefix.length(), cname.length());
  
  ei = synonyms_.find(shortcname);
  if (ei != synonyms_.end())
  {
		vector<string> synonymList = synonyms_[shortcname];
    vector<string>::iterator si;
    for (si = synonymList.begin(); si != synonymList.end(); ++si)
		{
			updControl(prefix + *si, value);
		}
  }
  else
  {
	  string prefix = "/" + type_ + "/" + name_;
    string childcontrol = cname.substr(prefix.length(), cname.length()-prefix.length());
    string nchildcontrol = childcontrol.substr(1, childcontrol.length());  
    
    bool controlFound = false;
    
    // check local controls 
    if (hasControlLocal(cname))
		{
		  controlFound = true;
		  oldval_ = getControl(cname);
		  setControl(cname, value);
		  if (hasControlState(cname) && (value != oldval_)) 
			{
			  update();
			  dbg_ = getctrl("mrs_bool/debug").toBool();
			  mute_ = getctrl("mrs_bool/mute").toBool();
			  if ((inObservations_ != inTick_.getRows()) ||
			      (inSamples_ != inTick_.getCols())      ||
			      (onObservations_ != outTick_.getRows()) ||
			      (onSamples_ != outTick_.getCols()))
			    {
			      inTick_.create(inObservations_, inSamples_);
			      outTick_.create(onObservations_, onSamples_);
			    }
			}
		}
    
    // lmartins: what is this for?!? [?]
		// default controls - semantics of composites 
    if ((nchildcontrol == "mrs_natural/inSamples")||
	(nchildcontrol == "mrs_natural/inObservations")||
	(nchildcontrol == "mrs_real/israte")||
	(nchildcontrol == "mrs_natural/onSamples")||
	(nchildcontrol == "mrs_natural/onObservations")||
	(nchildcontrol == "mrs_real/osrate")||
	(nchildcontrol == "mrs_bool/debug")||
	(nchildcontrol == "mrs_string/inObsNames")||
	(nchildcontrol == "mrs_string/onObsNames"))
      {
	marsystems_[0]->updctrl(nchildcontrol, value);
	update();
	return;
      }
    
    else//if(!hasControlLocal(cname))
      {
	for (mrs_natural i=0; i< marsystemsSize_; i++)
	  {
	    if (marsystems_[i]->hasControl(childcontrol))
	      {
		controlFound = true;
		MarControlValue oldval;
		oldval = marsystems_[i]->getControl(childcontrol);
		marsystems_[i]->updControl(childcontrol, value);
		if (marsystems_[i]->hasControlState(childcontrol) && 
		    (value != oldval))
		  {
		    update();
		  }
	      }
	  }
      }
    
    if (!controlFound) 
      {
	MRSWARN("Composite::updControl - Unsupported control name = " + cname);
	MRSWARN("Composite::updControl - Composite name = " + name_);
      }
  }
}

MarControlValue
Composite::getctrl(string cname)
{
  MarControlValue v = getControl("/" + type_ + "/" + name_ + "/" + cname);  
  return v;
}


MarControlValue
Composite::getControl(string cname)
{
  // check for synonyms - call recursively to resolve them 
  map<string, vector<string> >::iterator ei;
  
  // remove prefix for synonyms
  string prefix = "/" + type_ + "/" + name_ + "/";
  string::size_type pos = cname.find(prefix, 0);
  string shortcname;
  
  if (pos == 0) 
    shortcname = cname.substr(prefix.length(), cname.length());

  ei = synonyms_.find(shortcname);
  
  if (ei != synonyms_.end())
  {
    vector<string> synonymList = synonyms_[shortcname];
    vector<string>::iterator si;
    for (si = synonymList.begin(); si != synonymList.end(); ++si)
		{
			return getControl(prefix + *si);
		}
  }
  else 
  {
    string prefix = "/" + type_ + "/" + name_;
    string childcontrol = cname.substr(prefix.length(), cname.length()-prefix.length());
    string nchildcontrol = childcontrol.substr(1, childcontrol.length());
    bool controlFound = false;      
    
    if (hasControlLocal(cname))
		{
			controlFound = true;
			return ncontrols_.getControl(cname);
		}
        
// lmartins: what is this for?!? [?]
//     // default composite controls 
//     if ((nchildcontrol == "mrs_natural/inSamples")||
// 				(nchildcontrol == "mrs_natural/inObservations")||
// 				(nchildcontrol == "mrs_real/israte")||
// 				(nchildcontrol == "mrs_natural/onSamples")||
// 				(nchildcontrol == "mrs_natural/onObservations")||
// 				(nchildcontrol == "mrs_real/osrate")||
// 				(nchildcontrol == "mrs_bool/debug") ||
// 				(nchildcontrol == "mrs_string/inObsNames")||
// 				(nchildcontrol == "mrs_string/onObsNames"))
// 		{
// 			return ncontrols_.getControl(cname);
// 		}

    for (mrs_natural i=0; i< marsystemsSize_; i++)
		{
			if (marsystems_[i]->hasControl(childcontrol))
			{
				MarControlValue v;
				v =  marsystems_[i]->getControl(childcontrol);
				controlFound = true;
				return v;
			}
		}
    
		if (!controlFound)
		{
			MRSWARN("Composite::getctrl - Unsupported control name = " + cname);
			return (mrs_natural)0;
		}
  }

  return (mrs_natural)0;
}

ostream& 
Composite::put(ostream& o) 
{
  o << "# MarSystemComposite" << endl;
  o << "# Type = " << type_ << endl;
  o << "# Name = " << name_ << endl;
  
  o << endl;
  o << ncontrols_ << endl;

  map<string,vector<string> >::iterator mi;
  o << "# Number of links = " << synonyms_.size() << endl;

  for (mi = synonyms_.begin(); mi != synonyms_.end(); ++mi)
  {
    vector<string> syns = mi->second;
    vector<string>::iterator vi;
    string prefix = "/" + type_ + "/" + name_ + "/";
    o << "# Synonyms of " << prefix + mi->first << " = " << endl;
    o << "# Number of synonyms = " << syns.size() << endl;
    
    for (vi = syns.begin(); vi != syns.end(); ++vi) 
			o << "# " << prefix + *vi << endl;
  }
  
  o << endl;
  o << "# nComponents = " << marsystemsSize_ << endl;
  o << endl;
  
  for (mrs_natural i=0; i < marsystemsSize_; i++)
    o << *(marsystems_[i]) << endl;
  return o;
}

void
Composite::localActivate(bool state)
{
	//call activate for all Composite's components
	for (mrs_natural i=0; i< marsystemsSize_; i++)
	{
		marsystems_[i]->activate(state);
	}
}





	
