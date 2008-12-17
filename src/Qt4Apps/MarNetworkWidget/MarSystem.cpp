#include "MarSystem.h"

MarSystem::MarSystem(std::string type, std::string name)
{
	type_=type;
	name_=name;
	isComposite_=(type_=="Series"||type_=="Fanout"||type_=="Parallel");
}

std::string
MarSystem::getName()
{
	return name_;
}

std::string
MarSystem::getType()
{
	return type_;
}

std::string
MarSystem::getPrefix()
{
	return type_ + "/" + name_;
}

bool
MarSystem::addMarSystem(MarSystem* marsystem)
{
	marsystems_.push_back(marsystem);
	return true;
}

MarSystem*
MarSystem::getChildMarSystem(std::string childPath)
{
//	for(unsigned int i=0;i<marsystems_.size();i++) {
//		MarSystem* m = marsystems_[i];
//	}
	return NULL;
}

std::vector<MarSystem*>
MarSystem::getChildren()
{
	return marsystems_;
}

MarSystemManager::MarSystemManager()
{
}

MarSystem*
MarSystemManager::create(std::string type, std::string name)
{
	return new MarSystem(type,name);
}

