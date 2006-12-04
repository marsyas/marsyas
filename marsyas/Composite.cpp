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

Composite::Composite(string type, string name):MarSystem(type, name)
{
	marsystemsSize_ = 0;

	//if there are any specific controls that
	//all composites should have, implement the
	//Composite::addControls() method and call it here.
	//
	//addControls();
}

Composite::Composite(const Composite& a):MarSystem(a)
{
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
	vector<MarSystem*>::iterator it;
	bool replaced = false;

	//check if a child MarSystem with the same type/name
	//exists. If it does, replace it with the new one.
	for(it = marsystems_.begin(); it != marsystems_.end(); ++it)
	{
		if((*it)->getName() == marsystem->getName() &&
			 (*it)->getType() == marsystem->getType())
		{
			//delete current child MarSystem
			delete (*it);
			//and replace it with the new one
			(*it) = marsystem;
			replaced = true;
			break;
		}
	}

	//if no replacement took place, then add the
	//new MarSystem as a new child
	if (!replaced)
	{
		marsystems_.push_back(marsystem);
		marsystemsSize_ = (mrs_natural)marsystems_.size();
	}
	
	//update new child's path
	marsystem->addFatherPath(getPath());
	marsystem->update();

	//update parent MarSystem
	update();
}

void
Composite::addFatherPath(string fpath)
{
	//update local composite path...
	//e.g. if path_ was "/Gain/g/" it will become 
	//"/Series/s" + "/Gain/g/" = "/Series/s/Gain/g/"
	path_ = fpath.substr(0, fpath.length()-1) + path_; //[!]

	//...and propagate path update on all children
	for (mrs_natural i=0; i< marsystemsSize_; i++)
	{
		marsystems_[i]->addFatherPath(fpath);
	}
}

MarSystem*
Composite::getMarSystem(std::string path) //thread-safe?!? [?]
{
	if(path == getPrefix())
		return this;
	else
	{
		string childPath;
		//if using getPath() above and in MarSystem::getMarSystem
		//there would be no need to have this childPath string... [!]
		if(path.length() > prefix_.length())
			childPath = path.substr(prefix_.length()-1, path.length()); //includes leading "/" [!]
		else
		{
			MRSWARN("Composite::getMarsystem(): " + path + " not found!");
			return NULL;
		}
		
		vector<MarSystem*>::const_iterator iter;
		for(iter = marsystems_.begin(); iter != marsystems_.end(); ++iter)
		{
			MarSystem* msys = (*iter)->getMarSystem(childPath);
			if (msys)
				return msys;
		}
		
		MRSWARN("Composite::getMarsystem(): " + path + " not found!");
		return NULL;
	}
}

std::vector<MarSystem*>
Composite::getChildren()
{
	return marsystems_;
}

bool
Composite::hasControlState(string cname)
{
	MarControlPtr control = this->getControl(cname);
	if(control.isInvalid())
	{
		MRSWARN("Composite::hasControlState - Unsupported control name = " + cname);
		return false;
	}
	else
		return control->hasState();
}

bool 
Composite::hasControl(string cname)
{
	MarControlPtr control = this->getControl(cname);
	return !control.isInvalid();
	
	//////////////////////////////////////////////////////////////////////////
// 	size_t prefix_l = prefix_.length();
// 	size_t cname_l = cname.length();
// 	
// 	//if control prefix is not equal to this composite prefix, 
// 	//it means the control does not belong to this composite
// 	// (this check is just for efficiency reasons)
// 	string cprefix = cname.substr(0, prefix_l);
// 	if (cprefix != prefix_)
// 		return false;
// 	
// 	//check if this is a local control
// 	//(i.e. owned by the composite itself)
// 	if (hasControlLocal(cname))
// 	{
// 		return true;
// 	}
// 	else //if not a local control, then search over children
// 	{
// 		//pre-check before iterating over children (more efficient):
// 		//cname should include prefix, so it must always be 
// 		//longer in order to be a control worth looking for
// 		if (cname_l <= prefix_l) 
// 			return false;
// 
// 		//if the above check passed, then take the time to iterate
// 		//over children and look for the control among them
// 		string childcontrol = cname.substr(prefix_l-1, cname_l-(prefix_l-1));//includes leading "/" [!]
// 		for (mrs_natural i=0; i< marsystemsSize_; i++)
// 		{
// 			if (marsystems_[i]->hasControl(childcontrol))
// 				return true;
// 		}
// 	}
// 	//if no control found anywhere, return false
// 	return false;
}

bool 
Composite::updControl(string cname, MarControlPtr newcontrol, bool upd)
{ 
	// get the control (local or from children)...
	MarControlPtr control = getControl(cname);
	
	// ...and check if the control really exists locally or among children
	if(control.isInvalid())
	{
		MRSWARN("Composite::updControl - Unsupported control name = " + cname);
		MRSWARN("Composite::updControl - Composite name = " + name_);
		return false;
	}

	// since the control exists somewhere, set its value...
	if(!control->setValue(newcontrol, upd))
		return false; //some error occurred in setValue()

	// call the composite update (only if the control has state,
	// upd is true, and if it's not a local control (otherwise update 
	// was already called by control->setValue())).
	if(upd && control->hasState() && !hasControlLocal(cname))
		update();

	// TODO: USE LINKED CONTROLS TO AVOID THIS CODE BLOCK! [!]
	// certain controls must also be propagated inside the composite
	// (must find a way to avoid this hard-coded control list! [!] )
	string childcontrol = cname.substr(prefix_.length()-1, cname.length()-(prefix_.length()-1));//includes leading "/"
	string nchildcontrol = childcontrol.substr(1, childcontrol.length()); //no leading "/" 
	if ((nchildcontrol == "mrs_natural/inSamples")|| 
		(nchildcontrol == "mrs_natural/inObservations")||
		(nchildcontrol == "mrs_real/israte")||
		(nchildcontrol == "mrs_string/inObsNames"))
	{
		//if there is at least a child MarSystem in this composite...
		if (marsystemsSize_ > 0)
		{
			if(!marsystems_[0]->updctrl(nchildcontrol, control, upd))
				return false;//some error occurred in updctrl()
 			if(upd && marsystems_[0]->hasControlState(nchildcontrol))
 				update();
		}
	}

	return true;

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
// 	string childcontrol = cname.substr(prefix_.length()-1, cname.length()-(prefix_.length()-1));//includes leading "/" [!]
// 	string nchildcontrol = childcontrol.substr(1, childcontrol.length()); //no leading "/" [!]  
// 
// 	//check for local control
// 	MarControlPtr control = controls_.find(cname);
// 	if(control != controls_.end())
// 	{
// 		//set its value...
// 		if(!control->setValue(newcontrol, NOUPDATE));
// 			return false;
// 		//...and update composite if necessary
// 		if(upd && control->hasState())
// 			update();
// 
// 		//certain controls must also be propagated inside the composite
// 		// (must find a way to avoid this hard-coded control list! [!] )
// 		if ((nchildcontrol == "mrs_natural/inSamples")|| 
// 			 (nchildcontrol == "mrs_natural/inObservations")||
// 			 (nchildcontrol == "mrs_real/israte")||
// 			 (nchildcontrol == "mrs_string/inObsNames"))
// 		{
// 			//if there is at least a child MarSystem in this composite...
// 			if (marsystemsSize_ > 0)
// 			{
// 				if(!marsystems_[0]->updctrl(nchildcontrol, control, upd))
// 					return false;
// 				if(upd && marsystems_[0]->hasControlState(nchildcontrol))
// 					update();
// 			}
// 		}
// 		return true;
// 	}
// 	else //if not a local control, look for it in its children
// 	{
// 		bool controlFound = false;
// 		for (mrs_natural i=0; i< marsystemsSize_; i++)
// 		{
// 			if (marsystems_[i]->hasControl(childcontrol))
// 			{
// 				controlFound = true;
// 				if(marsystems_[i]->updControl(childcontrol, control, upd))
// 					if (upd && marsystems_[i]->hasControlState(childcontrol))
// 						update();
// 					else
// 						return false;//in case of an error
// 			}
// 		}
// 		if (!controlFound) 
// 		{
// 			MRSWARN("Composite::updControl - Unsupported control name = " + cname);
// 			MRSWARN("Composite::updControl - Composite name = " + name_);
// 			return false;
// 		}
// 		else
// 			return true;
// 	}
}

void
Composite::controlUpdate(MarControlPtr cvalue) //[?]
{
	//this method is called by MarControl each time the value of
	//the control (if it has state) is modified

	//check if this object owns the control //is this really needed? [!]
	//if(cvalue.getMarSystem() != this || getControl(cvalue.getName())->getType() == 0)
	//	return;

	//if(!cvalue.hasState()) //[!] this check is already performed at MarControlValue.cpp... [!]
	//	 return;

	string cname = cvalue->getName();
	string shortcname = cname.substr(prefix_.length(), cname.length()-prefix_.length()); //no leading "/" [!]

	// check local controls 
	if (hasControlLocal(cname))
	{
		update(); //update composite
	}

	// lmartins: should find a way to avoid this hard-coded check... [!]
	// default controls - semantics of composites 
	if ((shortcname == "mrs_natural/inSamples")||
		(shortcname == "mrs_natural/inObservations")||
		(shortcname == "mrs_real/israte")||
		(shortcname == "mrs_string/inObsNames"))
	{
		//marsystems_[0]->updctrl(childcontrol, cvalue);
		marsystems_[0]->getctrl(shortcname)->setValue(cvalue, true);
		update();
		return;
	}
}

MarControlPtr
Composite::getControl(string cname)
{
  //USE A CACHE FOR MORE EFFICIENT LOOK-UP?? [!]
  
  //check if this is a local control
  //(i.e. owned by the composite itself)
  if(controls_.find(cname) != controls_.end())
    {
#ifdef MARSYAS_QT
      QReadLocker locker(&rwLock_); //reading controls_ [!]
#endif

      return controls_[cname];
    }
  else //if not a local control, then search it over children
    {
      // not a child control probably tryng to setup a link 
      if (cname.length() < prefix_.length() - 1) 
	return MarControlPtr();
      

      string childcontrol = cname.substr(prefix_.length()-1, cname.length()-(prefix_.length()-1));//includes leading "/" [!]
      MarControlPtr childCtrl;
      //iterate recursively over children and just
      //return the first control that matches the requested one
      for (mrs_natural i=0; i< marsystemsSize_; i++)
	{
	  childCtrl = marsystems_[i]->getControl(childcontrol);
	  if(!childCtrl.isInvalid())
	    return childCtrl;
	}
    }
  //if no control found anywhere, return an invalid control...
  MRSWARN("Composite::getControl - Unsupported control name = " + cname);
  return MarControlPtr();
}

ostream& 
Composite::put(ostream& o) 
{
	o << "# MarSystemComposite" << endl;
	o << "# Type = " << type_ << endl;
	o << "# Name = " << name_ << endl;

	o << endl;
	o << controls_ << endl;

	// SYNONYMS!!!
// 	map<string,vector<string> >::iterator mi;
// 	o << "# Number of links = " << synonyms_.size() << endl;
// 
// 	for (mi = synonyms_.begin(); mi != synonyms_.end(); ++mi)
// 	{
// 		vector<string> syns = mi->second;
// 		vector<string>::iterator vi;
// 		o << "# Synonyms of " << prefix_ + mi->first << " = " << endl;
// 		o << "# Number of synonyms = " << syns.size() << endl;
// 
// 		for (vi = syns.begin(); vi != syns.end(); ++vi) 
// 			o << "# " << prefix_ + *vi << endl;
// 	}

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
		//marsystems_[i]->activate(state);
		marsystems_[i]->updctrl("mrs_bool/active", state); //thread-safe
	}
}






