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

#ifndef MARSYAS_MSLMODEL_H
#define MARSYAS_MSLMODEL_H

#include <marsyas/system/MarSystemManager.h>
#include "MarControl.h"
#include <marsyas/common_source.h>

namespace Marsyas
{
/**
    \class MslModel
    \ingroup Experimental
    \brief MslModel of MarSystem objects that are in a working set
    cd
    A working copy of all MarSystems in a current session.
    Keeps track of all actions performed and takes as
    input a simple command line string.

*/


typedef std::vector<std::string> cmdList_t;

typedef std::map<std::string, MarSystem * > marsysMap_t;

// forward declaration for the typedef below
class MslCommand;

typedef std::map<std::string, MslCommand *> cmdMap_t;

class MslModel
{
protected:

  marsysMap_t workingSet;
  MarSystemManager mng;


public:

  MslModel();
  ~MslModel();

  // the main interface to the model
  bool input( const std::string& inputString);

  // returns the controls to the GUI for a specific marsystem
  std::map<std::string, MarControlPtr> getControls( const std::string& name );
  std::vector<std::string> getCmds();

private:

  std::vector<std::string> tokenize( const std::string& inputString, const char delim );

  // Map to associate the std::strings with the command objects
  cmdMap_t commands;

};


//
// ----------------------------------------------------------------------
//    Msl Command Class Declarations Added By Adam Parkin, Summer 2005
// ----------------------------------------------------------------------
//

//
// Build commands using a pattern like the "Command" design pattern.  See
// "Design Patterns" - Gamma, Helm, Johnson, Vlissides, p238
//
class MslCommand
{
public:
  virtual ~MslCommand() {};
  virtual bool verify(cmdList_t &) { return true; };
  virtual bool execute() = 0;
  virtual std::string helpMsg() const { return " "; };
private:

protected:

};

//
// Base class for commands which involve a "working
// set of marsystems
//
class MslMarSystemCommand : public MslCommand
{
public:

private:


protected:
  // make constructor protected to prevent
  // users from instantiating
  MslMarSystemCommand (marsysMap_t &);

  marsysMap_t & workingSet;
};

// Creates a marSystem
class MslCommandCreate : public MslMarSystemCommand
{
public:
  MslCommandCreate (marsysMap_t &, MarSystemManager &);
  virtual bool verify(cmdList_t &);
  virtual bool execute();
  virtual std::string helpMsg() const;
  virtual ~MslCommandCreate() {};
private:
  std::string type;
  std::string name;
  MarSystemManager & mng;

protected:
};

// Shows the command help
class MslCommandHelp : public MslCommand
{
public:
  MslCommandHelp (cmdMap_t & commands);
  virtual bool execute();
private:
  cmdMap_t & commands;
protected:
};

// Quits the program
class MslCommandQuit : public MslCommand
{
public:
  virtual bool verify(cmdList_t &);
  virtual bool execute();
private:

protected:

};

class MslCommandScript: public MslCommand
{
public:
  MslCommandScript (MslModel &);
  virtual bool verify(cmdList_t &);
  virtual bool execute();
  virtual std::string helpMsg() const;
  virtual ~MslCommandScript () { };
private:
  MslModel & model;
  std::string filename;
protected:

};

class MslCommandAdd : public MslMarSystemCommand
{
public:
  MslCommandAdd (marsysMap_t &);
  virtual bool verify(cmdList_t &);
  virtual bool execute();
  virtual std::string helpMsg() const;
  virtual ~MslCommandAdd() {};
private:
  std::string child;
  std::string parent;

protected:
};

class MslCommandUpdateControl : public MslMarSystemCommand
{
public:
  MslCommandUpdateControl (marsysMap_t &);
  virtual bool verify(cmdList_t &);
  virtual bool execute();
  virtual std::string helpMsg() const;
  virtual ~MslCommandUpdateControl() {};
private:
  std::string name;
  std::string control;
  MarControlPtr value;
protected:

};

class MslCommandRun : public MslMarSystemCommand
{
public:
  MslCommandRun (marsysMap_t &);
  virtual bool verify(cmdList_t &);
  virtual bool execute();
  virtual std::string helpMsg() const;
  virtual ~MslCommandRun () {};
private:
  std::string name;
protected:

};

class MslCommandShow : public MslMarSystemCommand
{
public:
  MslCommandShow (marsysMap_t &);
  virtual bool verify(cmdList_t &);
  virtual bool execute();
  virtual std::string helpMsg() const;
  virtual ~MslCommandShow() {};
private:
  std::string name;
protected:

};

class MslCommandCurrent : public MslMarSystemCommand
{
public:
  MslCommandCurrent (marsysMap_t &);
  virtual bool execute();
  virtual ~MslCommandCurrent () {};
private:

protected:

};

class MslCommandWrite : public MslMarSystemCommand
{
public:
  MslCommandWrite (marsysMap_t &);
  virtual bool verify(cmdList_t &);
  virtual bool execute();
  virtual std::string helpMsg() const;
  virtual ~MslCommandWrite() {};
private:
  std::string name;
  std::string filename;
protected:

};

class MslCommandRead : public MslMarSystemCommand
{
public:
  MslCommandRead (marsysMap_t &, MarSystemManager &);
  virtual bool verify(cmdList_t &);
  virtual bool execute();
  virtual std::string helpMsg() const;
  virtual ~MslCommandRead () {};
private:
  std::string filename;
  MarSystemManager & mng;
protected:

};

class MslCommandLink : public MslMarSystemCommand
{
public:
  MslCommandLink (marsysMap_t &);
  virtual bool verify(cmdList_t &);
  virtual bool execute();
  virtual std::string helpMsg() const;
  virtual ~MslCommandLink () {};
private:
  std::string msys;
  std::string msysCtrl;
  std::string subCtrl;
protected:

};

class MslCommandAlias : public MslCommand
{
public:
  MslCommandAlias (cmdMap_t & commands);
  virtual bool verify(cmdList_t &);
  virtual bool execute();
  virtual std::string helpMsg() const;
  virtual ~MslCommandAlias() {};
private:
  cmdMap_t & cmds;
  std::string cmd;
  std::string alias;
protected:

};

class MslCommandUnalias : public MslCommand
{
public:
  MslCommandUnalias (cmdMap_t & commands);
  virtual bool verify(cmdList_t &);
  virtual bool execute();
  virtual std::string helpMsg() const;
  virtual ~MslCommandUnalias() {};
private:
  cmdMap_t & cmds;
  std::string alias;
protected:

};

class MslCommandDelete : public MslMarSystemCommand
{
public:
  MslCommandDelete (marsysMap_t &);
  virtual bool verify(cmdList_t &);
  virtual bool execute();
  virtual std::string helpMsg() const;
  virtual ~MslCommandDelete() {};
private:
  std::string name;
protected:

};

class MslCommandComposite : public MslMarSystemCommand
{
public:
  MslCommandComposite (marsysMap_t &, MarSystemManager &,
                       MslModel &);
  virtual bool verify(cmdList_t &);
  virtual bool execute();
  virtual std::string helpMsg() const;
  virtual ~MslCommandComposite () {};

private:
  MarSystemManager & mng;
  cmdList_t subSystems;
  std::string compositeName;
  std::string compositeType;
  MslModel & msl;

protected:

};

class MslCommandComment : public MslCommand
{
public:
  virtual bool execute();
private:

protected:
};

}//namespace Marsyas

#endif
