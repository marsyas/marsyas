#include "MarControlManager.h"
#include "MarControlValue.h"
//#include "MarSystem.h"

using namespace Marsyas;
using namespace std;

MarControlManager::MarControlManager()
{
	registerPrototype("mrs_natural", new MarControl(0L));
	registerPrototype("mrs_real", new MarControl(0.0));
	registerPrototype("mrs_string", new MarControl("empty")); // [?]
}

MarControlManager::~MarControlManager()
{
	registry.clear();
}

void 
MarControlManager::registerPrototype(string name, MarControlPtr marsystem)
{
	registry[name] = marsystem;
}

MarControlPtr 
MarControlManager::getPrototype(string type) 
{

	if (registry.find(type) != registry.end())
		return registry[type];
	else 
	{
		MRSWARN("MarControlManager::getPrototype No prototype found for " + type);
		return MarControlPtr(); // invalid ptr
	}
}

MarControlPtr
MarControlManager::create(string type, string name, MarSystem *msys, bool state) 
{
	if (registry.find(type) != registry.end())
	{
		MarControl *mc = registry[type]->clone();
		mc->setName(name);
		mc->setMarSystem(msys);
		mc->setState(state);
		return mc;
	}

	else 
	{
		MRSWARN("MarControlManager::getPrototype No prototype found for " + type);
		return MarControlPtr();
	}
}

/*MarSystem* 
MarControlManager::getMarSystem(istream& is)
{
	string skipstr;
	mrs_natural i;
	is >> skipstr;
	string mcomposite;
	bool   isComposite;

	is >> mcomposite;
	string marSystem = "MarSystem";
	string marSystemComposite = "MarSystemComposite";  
	if (mcomposite == marSystem)
		isComposite = false;
	else if (mcomposite == marSystemComposite)
		isComposite = true;
	else
		return 0;

	is >> skipstr >> skipstr >> skipstr;
	string mtype;
	is >> mtype;

	is >> skipstr >> skipstr >> skipstr;
	string mname;
	is >> mname;

	MarSystem* msys = getPrototype(mtype);

	if (msys == 0)
	{
		MRSWARN("MarSystem::getMarSystem - MarSystem not supported");
		return 0;
	}
	else
	{
		msys->setName(mname);
		is >> (*(msys->ncontrols_));

		msys->update();

		workingSet[msys->getName()] = msys; // add to workingSet

		is >> skipstr;
		is >> skipstr;
		is >> skipstr;
		is >> skipstr;

		if (skipstr != "links") 
		{
			MRSWARN("Problem with reading links");
			MRSWARN("mtype = " << mtype);
			MRSWARN("mname = " << mname);
			MRSWARN("skipstr = " << skipstr);
		}

		is >> skipstr;

		mrs_natural nLinks;
		is >> nLinks;

		for (i=0; i < nLinks; i++)
		{
			is >> skipstr;
			is >> skipstr;
			is >> skipstr;
			string visible;
			string vshortcname;

			is >> visible;

			string prefix = "/" + mtype + "/" + mname + "/";
			string::size_type pos = visible.find(prefix, 0);

			if (pos == 0) 
				vshortcname = visible.substr(prefix.length(), visible.length());

			//[?] and what happens if this is not a composite?!?
			//if not a composite, the visible link does not include prefix...
			//e.g.:
			// instead of
			//
			//# Synonyms of /Series/s1/mrs_bool/notEmpty =
			//
			// we only have (see MarSystem::put())
			//
			//# Synonyms of mrs_bool/notEmpty =
			//
			//in this case vshortcname should be "mrs_bool/notEmpty", shouldn't it?
			//but it is kept uninitialized... [?][!]
			//
			// unless simple MarSystems are not supposed to have links...

			is >> skipstr;
			is >> skipstr;
			is >> skipstr;
			is >> skipstr;
			is >> skipstr;
			is >> skipstr;

			mrs_natural nSynonyms = 0;
			is >> nSynonyms;

			vector<string> synonymList;
			synonymList = msys->synonyms_[vshortcname]; //vshortcname[!]

			for (int j=0; j < nSynonyms; j++)
			{
				string inside;
				is >> skipstr;
				is >> inside;

				prefix = "/" + mtype + "/" + mname + "/";
				pos = inside.find(prefix, 0);
				string shortcname;

				if (pos == 0) //and what happens if the prefix is not found?!? [?][!]
					shortcname = inside.substr(prefix.length(), inside.length());

				synonymList.push_back(shortcname);
				msys->synonyms_[vshortcname] = synonymList;
			}
		}

		if (isComposite == true)
		{
			is >> skipstr >> skipstr >> skipstr;
			mrs_natural nComponents;
			is >> nComponents;
			for (i=0; i < nComponents; i++)
			{
				MarSystem* cmsys = getMarSystem(is);
				if (cmsys == 0)
					return 0;
				msys->addMarSystem(cmsys);
			}
			msys->update();
		}
	}

	return msys;
}*/



//
// Added by Adam Parkin, Jul 2005, invoked by MslModel
//
// Returns a list of all registered prototypes
//
vector <string> MarControlManager::registeredPrototypes()
{
	vector <string> retVal;

	map<string, MarControlPtr>::const_iterator iter;

	for (iter=registry.begin(); iter != registry.end(); ++iter)
	{
		retVal.push_back (iter->first);
	}

	return retVal;
} 

/*// Added by Stuart Bray Dec 2004. invoked by MslModel
map<string, MarSystem*> MarControlManager::getWorkingSet(istream& is) {
	getMarSystem(is);
	return workingSet;
}*/

//
// Added by Adam Parkin Jul 2005, invoked by MslModel
//
// Takes as an argument a string, if the string corresponds
// to a registered MarSystem, the function returns true,
// otherwise returns false
//
bool MarControlManager::isRegistered (string name)
{
	return (registry.find(name) != registry.end());
}
