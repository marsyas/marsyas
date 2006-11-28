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
	if(path == getPrefix())//use getPath() instead?! [?]
		return this;
	else
	{
		string childPath;
		//if using getPath() above and in MarSystem::getmarSystem
		//there would be no need to have this childPath string... [!]
		if(path.length() > prefix_.length())
			childPath = path.substr(prefix_.length()-1, path.length()); //includes leading "/" [!]
		else
			return NULL;

		//std::vector<MarSystem*> marsystems_;
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
	bool controlFound = false;

	string childcontrol = cname.substr(prefix_.length()-1, cname.length());

	// local controls
	if (hasControlLocal(cname))
	{
		controlFound = true;
		return MarSystem::hasControlState(cname);
	}

	string cprefix = cname.substr(0, prefix_.length());
	if (cprefix != prefix_) //lmartins: this should be the first thing to check when entering this function! [!]
		return false;//if cprefix is not equal to this composite prefix, return (no point looking for anything in its children)
	else 
	{
		for (mrs_natural i=0; i< marsystemsSize_; i++)
		{
			if (marsystems_[i]->hasControl(childcontrol))
			{
				controlFound = true;
				return marsystems_[i]->hasControlState(childcontrol);
			}
		}
	}

	if (!controlFound)
	{
		MRSWARN("Composite::hasControlState - Unsupported control name = " + cname);
	}
	return false;
}


// check for controls only in composite object not children
bool
Composite::hasControlLocal(string cname) 
{
	return(controls_.find(cname) != controls_.end());
}

bool 
Composite::hasControl(string cname)
{
	// remove prefix for synonyms
	string::size_type pos = cname.find(prefix_, 0);
	string shortcname;
	if (pos == 0)
		shortcname = cname.substr(prefix_.length(), cname.length());//no leading "/", as expected for links [!]
	// check for synonyms - call recursively to resolve them 
	map<string, vector<string> >::iterator ei;
	ei = synonyms_.find(shortcname);
	if (ei != synonyms_.end())
	{
		return true;
	}
	
	// check composite first, then search in children
	if (controls_.find(cname)!= controls_.end())
	{
		return true;
	}

	size_t cname_l = cname.length();
	size_t prefix_l = prefix_.length();

	if ( cname_l <= prefix_l ) //cname should include prefix, so it must always be longer in order to be valid
		return false;

	string childcontrol = cname.substr(prefix_l-1, cname_l-(prefix_l-1));//includes leading "/" [!]  
	string cprefix = cname.substr(0, prefix_l);

	// wrong type 
	if (cprefix != prefix_) //lmartins: this should be the first thing to check when entering this function! [!]
		return false;//if cprefix is not equal to this composite prefix, return (no point looking for anything in its children)
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

bool
Composite::setControl(string cname, mrs_real value)
{
	bool controlFound = false;

	string childcontrol = cname.substr(prefix_.length()-1, cname.length()-(prefix_.length()-1));//includes leading "/" [!]

	if (hasControlLocal(cname))
	{
		controlFound = true;
		updControl(cname,value);
		return true; 
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
	{
		MRSWARN("Composite::setctrl - Unsupported control name = " + cname);
		return false;
	}
	return true;
}

bool
Composite::setControl(string cname, mrs_natural value)
{
	bool controlFound = false;
	string childcontrol = cname.substr(prefix_.length()-1, cname.length()-(prefix_.length()-1));//includes leading "/" [!]

	if (hasControlLocal(cname))
	{
		controlFound = true;
		updControl(cname,value);
		return true;
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
	{
		MRSWARN("Composite::setctrl - Unsupported control name = " + cname);
		return false;
	}
	return true;
}

bool
Composite::setControl(string cname, MarControlPtr value)
{
	bool controlFound = false;

	if (hasControlLocal(cname))
	{
		controlFound = true;
		controls_[cname]->setValue(value,NOUPDATE);
		return true;
	}

	string childcontrol = cname.substr(prefix_.length()-1, cname.length()-(prefix_.length()-1));//includes leading "/" [!]
	for (mrs_natural i=0; i< marsystemsSize_; i++)
	{
		if (marsystems_[i]->hasControl(childcontrol))
		{
			controlFound = true;
			marsystems_[i]->setControl(childcontrol,value);
		}
	}

	if (!controlFound)
	{
		MRSWARN("Composite::setctrl - Unsupported control name = " + cname);
		return false;
	}
	return true;
}

void 
Composite::updControl(string cname, MarControlPtr control)
{ 
	// remove prefix for synonyms
	string::size_type pos = cname.find(prefix_, 0);
	string shortcname;
	if (pos == 0)
		shortcname = cname.substr(prefix_.length(), cname.length());//no leading "/", as expected for links [!]
	// check for synonyms - call recursively to resolve them 
	map<string, vector<string> >::iterator ei;
	ei = synonyms_.find(shortcname);
	if (ei != synonyms_.end())
	{
		vector<string> synonymList = synonyms_[shortcname];
		vector<string>::iterator si;
		for (si = synonymList.begin(); si != synonymList.end(); ++si)
		{
			updControl(prefix_ + *si, control);
		}
	}
	else //no link found...
	{
		string childcontrol = cname.substr(prefix_.length()-1, cname.length()-(prefix_.length()-1));//includes leading "/" [!]
		string nchildcontrol = childcontrol.substr(1, childcontrol.length()); //no leading "/" [!]  
		bool controlFound = false;

		// check local controls 
		if (hasControlLocal(cname))
		{
			controlFound = true;
			MarControlPtr oldval = getControl(cname);

			// control != oldval will not work if setControl 
			// is done before the test because of aliasing 
			if (hasControlState(cname) && (control != oldval)) 
			{
				setControl(cname, control);
				update(); //update composite
			}
			else 
				setControl(cname, control);

		}
		// lmartins: should find a way to avoid this hard-coded check... [!]
		// default controls - semantics of composites 
		if ((nchildcontrol == "mrs_natural/inSamples")|| 
			(nchildcontrol == "mrs_natural/inObservations")||
			(nchildcontrol == "mrs_real/israte")||
			//(nchildcontrol == "mrs_natural/onSamples")|| //lmartins: does it make any sense to check for this control?![?]
			//(nchildcontrol == "mrs_natural/onObservations")|| //lmartins: does it make any sense to check for this control?![?]
			//(nchildcontrol == "mrs_real/osrate")|| //lmartins: does it make any sense to check for this control?![?]
			//(nchildcontrol == "mrs_bool/debug")|| //lmartins: does it make any sense to check for this control?![?]
			(nchildcontrol == "mrs_string/inObsNames"))//||
			//(nchildcontrol == "mrs_string/onObsNames")) //lmartins: does it make any sense to check for this control?![?]
		{
			if (marsystemsSize_ > 0)
			{
				marsystems_[0]->updctrl(nchildcontrol, control);
			}
			update();
			return;
		}
		else//if(!hasControlLocal(cname)) => no local controls!
		{
			//if control is not from composite,
			//check if it exists among the children composites and update them
			for (mrs_natural i=0; i< marsystemsSize_; i++)
			{
				if (marsystems_[i]->hasControl(childcontrol))
				{
					controlFound = true;
					marsystems_[i]->updControl(childcontrol, control); //updcontrol or setcontrol?! [!]
					if (marsystems_[i]->hasControlState(childcontrol))
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

void
Composite::controlUpdate(MarControlPtr cvalue)
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
Composite::getctrl(string cname)
{
	//MarControlValue v = getControl(type_ + "/" + name_ + "/" + cname);
	return getControl(prefix_ + cname);  
}

MarControlPtr
Composite::getControl(string cname)
{
	// check for synonyms - call recursively to resolve them 
	map<string, vector<string> >::iterator ei;

	// remove prefix for synonyms
	string::size_type pos = cname.find(prefix_, 0);
	string shortcname;

	if (pos == 0) 
		shortcname = cname.substr(prefix_.length(), cname.length());//no leading "/", as expected for links [!] 

	ei = synonyms_.find(shortcname);
	if (ei != synonyms_.end())
	{
		vector<string> synonymList = synonyms_[shortcname];
		vector<string>::iterator si;
		for (si = synonymList.begin(); si != synonymList.end(); ++si)
		{
			return getControl(prefix_ + *si);
		}
	}
	else 
	{
		string childcontrol = cname.substr(prefix_.length()-1, cname.length()-(prefix_.length()-1));//includes leading "/" [!]

		bool controlFound = false;      

		if (hasControlLocal(cname))
		{
			controlFound = true;
			MarControlPtr mytest = MarSystem::getControl(cname);
			return controls_[cname];
		}

		//lmartins: [!]
		//Contrary to the similar code block in updControl(),
		//this code does not seem to be doing anything here! 
		//(if check is true, it was also true before and so it 
		//already returned to calling code on the previous "if"!) => commenting out!
		//
		// default composite controls 
		//       if ((nchildcontrol == "mrs_natural/inSamples")||
		// 					(nchildcontrol == "mrs_natural/inObservations")||
		// 					(nchildcontrol == "mrs_real/israte")||
		// 					(nchildcontrol == "mrs_natural/onSamples")||
		// 					(nchildcontrol == "mrs_natural/onObservations")||
		// 					(nchildcontrol == "mrs_real/osrate")||
		// 					(nchildcontrol == "mrs_bool/debug") ||
		// 					(nchildcontrol == "mrs_string/inObsNames")||
		// 					(nchildcontrol == "mrs_string/onObsNames"))
		// 				{
		// 					return ncontrols_->getControl(cname);
		// 				}

		for (mrs_natural i=0; i< marsystemsSize_; i++)
		{
			if (marsystems_[i]->hasControl(childcontrol))
			{
				MarControlPtr v =  marsystems_[i]->getControl(childcontrol);
				controlFound = true;
				return v;
			}
		}

		if (!controlFound)
		{
			MRSWARN("Composite::getctrl - Unsupported control name = " + cname);
			//return (mrs_natural)0;
			return MarControlPtr();
		}
	}

	//return (mrs_natural)0;
	return MarControlPtr();
}

ostream& 
Composite::put(ostream& o) 
{
	o << "# MarSystemComposite" << endl;
	o << "# Type = " << type_ << endl;
	o << "# Name = " << name_ << endl;

	o << endl;

	o << "# MarControls = " << controls_.size() << endl;
	for (ctrlIter_=controls_.begin(); ctrlIter_ != controls_.end(); ++ctrlIter_)
	{
		o << "# " << ctrlIter_->first << " = " << ctrlIter_->second << endl;
	}

	map<string,vector<string> >::iterator mi;
	o << "# Number of links = " << synonyms_.size() << endl;

	for (mi = synonyms_.begin(); mi != synonyms_.end(); ++mi)
	{
		vector<string> syns = mi->second;
		vector<string>::iterator vi;
		o << "# Synonyms of " << prefix_ + mi->first << " = " << endl;
		o << "# Number of synonyms = " << syns.size() << endl;

		for (vi = syns.begin(); vi != syns.end(); ++vi) 
			o << "# " << prefix_ + *vi << endl;
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
		//marsystems_[i]->activate(state);
		marsystems_[i]->updctrl("mrs_bool/active", state); //thread-safe
	}
}






