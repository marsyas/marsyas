#ifndef MARSYSTEM_H
#define MARSYSTEM_H

#include <vector>
#include <string>

class MarSystem
{
private:
	bool isComposite_;
	std::vector<MarSystem*> marsystems_;
	std::string type_,name_;

public:
	MarSystem(std::string type, std::string name);

	std::string getName();
	std::string getType();
	std::string getPrefix();

  virtual bool addMarSystem(MarSystem *marsystem);
	virtual MarSystem* getChildMarSystem(std::string childPath);
	virtual std::vector<MarSystem*> getChildren();
};

class MarSystemManager
{
public:
	MarSystemManager();
	MarSystem* create(std::string name, std::string type);
};

#endif

