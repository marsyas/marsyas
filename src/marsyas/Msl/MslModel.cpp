/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

/* Written by Stuart Bray with modifications by gtzan Summer 2004 */

#include "MslModel.h"
#include "SocketException.h"
#include <marsyas/system/MarSystem.h>

#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <functional>

using namespace std;
using namespace Marsyas;

MslModel::MslModel()
{
  commands["create"]	= new MslCommandCreate (workingSet, mng);
  commands["add"]	= new MslCommandAdd (workingSet);
  commands["updctrl"] 	= new MslCommandUpdateControl (workingSet);
  commands["run"]	= new MslCommandRun (workingSet);
  commands["show"]	= new MslCommandShow (workingSet);
  commands["current"]	= new MslCommandCurrent (workingSet);
  commands["write"]	= new MslCommandWrite (workingSet);
  commands["read"]	= new MslCommandRead (workingSet, mng);
  commands["link"]	= new MslCommandLink (workingSet);
  commands["quit"]	= new MslCommandQuit ();
  commands["exit"]	= commands["quit"];
  commands["help"]	= new MslCommandHelp (commands);
  commands["script"]	= new MslCommandScript (*this);
  commands["alias"]	= new MslCommandAlias (commands);
  commands["unalias"]	= new MslCommandUnalias (commands);
  commands["del"]	= new MslCommandDelete (workingSet);
  commands["delete"]	= commands["del"];
  commands["composite"] = new MslCommandComposite(workingSet, mng, *this);
  commands["//"]	= new MslCommandComment();
}


MslModel::~MslModel() {
  map<string, MarSystem *>::const_iterator iter;
  for (iter=workingSet.begin(); iter != workingSet.end(); ++iter) {

    // we actually should only have to delete the most top
    // level composite marsystem as they take care of
    // deleting their internal marsystems...  here we assume
    // that there is only one type Series and its the top level.
    MarSystem* tmp = (MarSystem*)iter->second;
    string type = tmp->getType();
    if (type.compare("Series") == 0) {
      delete iter->second;
    }
  }
}

//
// Function: input
// Returns: true if the command was completed, false if not
// Description: this function acts as the interface to the model. it is used
//              by both the interpreter and the GUI.
//
bool
MslModel::input( const string& inputString )
{
  const char SEPARATOR = ' ';
  vector<string> tokens = tokenize( inputString, SEPARATOR );
  bool implicit = false;

  string cmdToken = tokens[0];

  MslCommand * cmd = NULL;

  // check if command exists
  if (commands.find(cmdToken) != commands.end())
  {
    // remove command token
    tokens.erase (tokens.begin());

    // lookup command
    cmd = commands[cmdToken];
  }

  // else check if it's an implicit create statement
  else if (mng.isRegistered(cmdToken))
  {
    cmd = commands["create"];
    implicit = true;
  }

  // else check if it's an implicit composite statement
  else if (cmdToken[0] == '(')
  {
    cmd = commands["composite"];
  }

  // else it's an invalid command
  else
  {
    cout << "Bad command. Try \"help\" for MSL "
         << "command listing." << endl;
    return false;
  }

  // verify & execute command
  if (cmd->verify (tokens))
  {
    if (cmd->execute())
      return true;
    else
      return false;
  }
  else
  {
    cout << "Usage: " << cmdToken << " " <<
         (implicit ? "<name>" : cmd->helpMsg())
         << endl;
    return false;
  }

} // input


//
// Function: getControls
// Parameters: name of the marsystem to get controls
// Return: map of controls for a specified marsystem
//
map<string, MarControlPtr>
MslModel::getControls( const string& name )
{
  return workingSet[name]->getLocalControls();
}

//
// Function: getCmds
// Parameters: None
// Returns: a vector of valid commands associated with the MslModel
//
vector<string>
MslModel::getCmds()
{
  vector<string> cmds;

  for (cmdMap_t::const_iterator iter = commands.begin();
       iter != commands.end(); ++iter)
  {
    cmds.push_back (iter->first);
  }

  // now add implicit create commands
  vector<string> types = mng.registeredPrototypes();
  vector<string> retval (cmds.size() + types.size());
  merge (cmds.begin(), cmds.end(), types.begin(), types.end(),
         retval.begin());

  return retval;
}

//
// Function: tokenize
// Parameters: string inputString to tokenize, char delimiter
// Returns: a vector of string tokens
//

vector<string>
MslModel::tokenize(const string& inputString, const char delim)
{
  vector<string> tokens;
  stringstream ss( inputString );
  string tmp;


  //sstr.getline(s,sizeof s,',');
  // get all the tokens from the stream and fill vector...
  while ( getline( ss, tmp, delim ) ) {
    tokens.push_back( tmp );
  }

  return tokens;
}


//
// --------------------------------------------------------------
//    MSL COMMAND CLASS DEFINITIONS ADDED BY ADAM PARKIN
// --------------------------------------------------------------
//

// ----------- MslMarSystemCommand methods begin ------------
MslMarSystemCommand::MslMarSystemCommand(marsysMap_t & inSet)
  : workingSet(inSet)
{
}
// ------------ MslMarSystemCommand methods end -------------

// ------------- MslCommandCreate methods begin -------------
MslCommandCreate::MslCommandCreate (marsysMap_t & inSet,
                                    MarSystemManager & inManager)
  : MslMarSystemCommand (inSet), mng (inManager)
{
}

bool MslCommandCreate::verify(cmdList_t & tokens)
{
  if (tokens.size() != 2)
    return false;

  // save local *COPY* of tokens.
  // the string class' assignment operator does
  // deep copy rather than shallow
  type = tokens[0];
  name = tokens[1];

  return true;
}

bool MslCommandCreate::execute()
{
  // check if a marsystem of that name exists already...
  if (workingSet.find(name) != workingSet.end()) {
    cout << "MarSystem: " << name <<
         " already exists in working set." << endl;

    return false;
  }

  MarSystem * m = mng.create( type, name );

  if ( m != NULL ) {
    workingSet[name] = m;
    return true;
  } else {
    return false;
  }
}

string MslCommandCreate::helpMsg() const
{
  return "<type> <name>";
}
// -------------- MslCommandCreate methods end --------------

// --------------- MslCommandAdd methods begin --------------
MslCommandAdd::MslCommandAdd (marsysMap_t & inSet)
  : MslMarSystemCommand (inSet)
{
}

bool MslCommandAdd::verify(cmdList_t & tokens)
{
  if ( tokens.size() != 3 || tokens[1] != ">" )
    return false;

  child = tokens[0];
  parent = tokens[2];
  return true;
}

bool MslCommandAdd::execute()
{
  // search the working set to ensure both the parent and child exist
  MarSystem *p, *c;

  if (workingSet.find(parent) != workingSet.end()) {
    p = (MarSystem *) workingSet[parent];
  } else {
    cout << "Cannot find MarSystem: " << parent << endl;
    return false;
  }

  if (workingSet.find(child) != workingSet.end()) {
    c = (MarSystem *) workingSet[child];
  } else {
    cout << "Cannot find MarSystem: " << child << endl;
    return false;
  }

  p->addMarSystem(c);
  return true;
}

string MslCommandAdd::helpMsg() const
{
  return "<child> > <parent>";
}

// ---------------- MslCommandAdd methods end ---------------


// ------------ MslCommandComment methods Begin -------------
bool MslCommandComment::execute()
{
  return true;
}
// ------------- MslCommandComment methods End --------------

// -------------- MslCommandHelp methods Begin --------------
MslCommandHelp::MslCommandHelp (cmdMap_t & inCmds)
  : commands (inCmds)
{
}

bool MslCommandHelp::execute()
{
  cout << "+================================================================+" << endl;
  cout << "| MSL v1.0 Command List                                          |" << endl;
  cout << "+================================================================+" << endl;
  cout << endl;

  int x = 1;
  for (cmdMap_t::const_iterator iter = commands.begin();
       iter != commands.end(); ++iter, x++)
  {

    // we call showHelp() on each mapped object to get
    // syntax for each individual command.  The beauty of this
    // is that the help is generated *at run time*, and furthermore
    // additions of new commands do not require any modification
    // to this help output.

    cout << left << setw(3) << x << iter->first
         << " " << (iter->second)->helpMsg()
         << endl;

  }

  cout << endl << endl;

  return true;
}
// --------------- MslCommandHelp methods end ---------------

// -------------- MslCommandQuit methods Begin --------------
bool MslCommandQuit::execute()
{
  cout << "Bye.\n";
  exit (0);

  // irrelevant as this line is never reached, but some
  // compilers will complain if a function which has a
  // non-void return value does not contain a return call
  return true;
}

bool MslCommandQuit::verify(cmdList_t & tokens)
{
  return (tokens.size() == 0);
}
// --------------- MslCommandQuit methods end ---------------

// ------------- MslCommandScript methods Begin -------------

MslCommandScript::MslCommandScript (MslModel & inModel)
  : model (inModel)
{
}

bool MslCommandScript::verify(cmdList_t & tokens)
{
  if (tokens.size() != 1)
    return false;

  filename = tokens[0];
  return true;
}

bool MslCommandScript::execute()
{
  const int BUFSIZE = 256;
  char * buffer = new char[BUFSIZE];

  ifstream inFile( filename.c_str(), ios::in);

  // if script file doesn't exist, return false
  if (!inFile)
    return false;

  while ( ! inFile.eof() ) {
    inFile.getline(buffer,BUFSIZE);
    const string inputString(buffer);

    if (inputString.compare("") == 0 )
      continue;

    if ( inputString.rfind( "#", inputString.length()) != string::npos )
      continue;

    model.input(inputString);
  }
  inFile.close();

  return true;
}

string MslCommandScript::helpMsg() const
{
  return "<filename>";
}
// -------------- MslCommandScript methods end --------------

// --------- MslCommandUpdateControl methods begin ----------

MslCommandUpdateControl::MslCommandUpdateControl (marsysMap_t & inSet)
  : MslMarSystemCommand (inSet)
{
}

bool MslCommandUpdateControl::verify (cmdList_t & tokens)
{
  if (tokens.size() != 3)
    return false;

  name = tokens[0];
  control = tokens[1];
  value = tokens[2];

  return true;
}

bool MslCommandUpdateControl::execute()
{
  if ( control.rfind( "mrs_real", control.length()) != string::npos )
  {
    if ( workingSet.find(name) != workingSet.end() ) {
      mrs_real val = (mrs_real)atof( value->to<mrs_string>().c_str() );

      workingSet[name]->updctrl( control, val );
      return true;
    } else {

      return false;
    }
  }
  else if ( control.rfind( "mrs_string", control.length() ) != string::npos ) {

    if ( workingSet.find(name) != workingSet.end() ) {
      workingSet[name]->updctrl( control, value->to<mrs_string>() );
      return true;
    } else {

      return false;
    }
  }
  else if ( control.rfind( "mrs_natural", control.length()) != string::npos ) {
    if ( workingSet.find(name) != workingSet.end() ) {
      mrs_natural val = atoi( value->to<mrs_string>().c_str() );
      workingSet[name]->updctrl( control, val );
      return true;
    } else {

      return false;
    }
  }
  else if ( control.rfind( "mrs_bool", control.length()) != string::npos ) {

    if( workingSet.find(name) != workingSet.end() ) {
      mrs_natural val = atoi( value->to<mrs_string>().c_str() );
      if ( val == 0 ) {
        workingSet[name]->updctrl( control, false );
      }
      else if ( val == 1 ) {
        workingSet[name]->updctrl( control, true );
      }
      return true;
    } else {

      return false;
    }
  }

  return false;
}

string MslCommandUpdateControl::helpMsg() const
{
  return "<name> <ctrl> <value>";
}

// ---------- MslCommandUpdateControl methods end -----------

// -------------- MslCommandRun methods begin ---------------
MslCommandRun::MslCommandRun (marsysMap_t & inSet)
  : MslMarSystemCommand (inSet)
{
}

bool MslCommandRun::verify(cmdList_t & tokens)
{
  if (tokens.size() != 1)
    return false;

  name = tokens[0];
  return true;
}

bool MslCommandRun::execute()
{
  MarSystem* msys;

  if ( workingSet.find(name) != workingSet.end() ) {
    msys = (MarSystem *)workingSet[name];
  } else {
    cout << "Cannot find MarSystem: " << name << endl;
    return false;
  }

  mrs_natural wc=0;
  mrs_natural samplesPlayed = 0;
  mrs_natural onSamples = msys->getctrl("mrs_natural/onSamples")->to<mrs_natural>();

  mrs_real* controls = 0;

  while (true) {

    try {

      controls = msys->recvControls();

      if ( controls != 0 ) {

        // get some reference controls, so if they have changed we update them
        mrs_natural inSamples = msys->getctrl("mrs_natural/inSamples")->to<mrs_natural>();
        mrs_natural inObservations = msys->getctrl("mrs_natural/inObservations")->to<mrs_natural>();
        mrs_real israte = msys->getctrl("mrs_real/israte")->to<mrs_real>();

        if ( (mrs_natural)controls[1] != inSamples || (mrs_natural)controls[2] != inObservations
             || controls[3] != israte ) {

          msys->updctrl("mrs_natural/inSamples",(mrs_natural) controls[1]);
          msys->updctrl("mrs_natural/inObservations", (mrs_natural)controls[2]);
          msys->updctrl("mrs_real/israte", controls[3]);
        }
      }

      msys->tick();
    }
    catch( SocketException e ) {
      cout << "Played " << wc << " slices of " << onSamples << " samples" << endl;
      exit(1);
    }

    wc ++;

    if ( !msys->getctrl("mrs_bool/hasData")->isTrue() ) {
      break;
    }
    samplesPlayed += onSamples;

  } // while

  cout << "Played " << wc << " slices of " << onSamples << " samples" << endl;
  return true;
}

string MslCommandRun::helpMsg() const
{
  return "<name>";
}

// --------------- MslCommandRun methods end ----------------

// -------------- MslCommandShow methods begin --------------

MslCommandShow::MslCommandShow (marsysMap_t & inSet)
  : MslMarSystemCommand (inSet)
{
}

bool MslCommandShow::verify(cmdList_t & tokens)
{
  if (tokens.size() != 1)
    return false;

  name = tokens[0];
  return true;
}

bool MslCommandShow::execute()
{
  if (workingSet.find(name) != workingSet.end()) {
    workingSet[name]->put( std::cout );
    return true;
  } else {
    cout << "MarSystem: " << name << " not found." << endl;
    return false;
  }
}

string MslCommandShow::helpMsg() const
{
  return "<name>";
}

// --------------- MslCommandShow methods end ---------------


// ------------- MslCommandCurrent methods begin ------------

MslCommandCurrent::MslCommandCurrent (marsysMap_t & inSet)
  : MslMarSystemCommand (inSet)
{
}

bool MslCommandCurrent::execute()
{
  map<string, MarSystem *>::const_iterator iter;
  cout << endl;
  cout << "Working set" << endl;
  cout << "===========" << endl;
  for (iter=workingSet.begin(); iter != workingSet.end(); ++iter) {
    MarSystem* tmp = (MarSystem*)iter->second;
    cout << "MarSystem: \"" << iter->first << "\"" << " of type: " << tmp->getType() << endl;
  }
  cout << endl;

  return true;
}

// -------------- MslCommandCurrent methods end -------------

// -------------- MslCommandWrite methods begin -------------

MslCommandWrite::MslCommandWrite (marsysMap_t & inSet)
  : MslMarSystemCommand (inSet)
{
}

bool MslCommandWrite::verify(cmdList_t & tokens)
{
  if (tokens.size() != 2)
    return false;

  name = tokens[0];
  filename = tokens[1];
  return true;
}

bool MslCommandWrite::execute()
{
  ofstream saveFile( filename.c_str(), ios::ate );

  if ( workingSet.find( name ) != workingSet.end() ) {
    workingSet[name]->put( saveFile );
    saveFile.close();
    return true;
  } else {
    cout << "MarSystem: " << name << " not found." << endl;
    return false;
  }
}

string MslCommandWrite::helpMsg() const
{
  return "<name> filename.mpl";
}

// --------------- MslCommandWrite methods end --------------

// -------------- MslCommandRead methods begin --------------

MslCommandRead::MslCommandRead (marsysMap_t & inSet,
                                MarSystemManager & inManager)
  : MslMarSystemCommand (inSet), mng (inManager)
{
}

bool MslCommandRead::verify(cmdList_t & tokens)
{
  if (tokens.size() != 1)
    return false;

  filename = tokens[0];
  return true;
}

bool MslCommandRead::execute()
{
  ifstream inFile( filename.c_str(), ios::in );

  if (!inFile)
  {
    cout << "File " << filename << " could not be opened\n";
    return false;
  }

  workingSet = mng.getWorkingSet(inFile);

  inFile.close();
  return true;

}

string MslCommandRead::helpMsg() const
{
  return "filename.mpl";
}

// --------------- MslCommandRead methods end ---------------


// -------------- MslCommandLink methods begin --------------

MslCommandLink::MslCommandLink (marsysMap_t & inSet)
  : MslMarSystemCommand (inSet)
{
}

bool MslCommandLink::verify(cmdList_t & tokens)
{
  if (tokens.size() != 3)
    return false;

  msys = tokens[0];
  msysCtrl = tokens[1];
  subCtrl = tokens[2];
  return true;
}

bool MslCommandLink::execute()
{
  if ( workingSet.find(msys) != workingSet.end() ) {
    workingSet[msys]->linkctrl( msysCtrl, subCtrl );
    return true;
  } else {
    cout << "Could not link control." << endl;
    return false;
  }
}

string MslCommandLink::helpMsg() const
{
  return "<msys> <msys-ctrl> <sub-ctrl>";
}

// --------------- MslCommandLink methods end ---------------

// -------------- MslCommandAlias methods begin -------------
MslCommandAlias::MslCommandAlias (cmdMap_t & commands)
  : cmds (commands)
{
}

bool MslCommandAlias::verify(cmdList_t & tokens)
{
  if (tokens.size() != 2)
    return false;

  cmd = tokens[0];
  alias = tokens[1];
  return true;
}

bool MslCommandAlias::execute()
{
  if (cmds.find(cmd) != cmds.end())
  {
    cmds[alias] = cmds[cmd];

    return true;
  }
  else
  {
    cout << "Command " << cmd << " is not a valid existing "
         << "command" << endl;
    return false;
  }

}

string MslCommandAlias::helpMsg() const
{
  return "<command> <alias>";
}

// --------------- MslCommandAlias methods end --------------

// ------------- MslCommandUnalias methods begin ------------

MslCommandUnalias::MslCommandUnalias (cmdMap_t & commands)
  : cmds (commands)
{
}

bool MslCommandUnalias::verify(cmdList_t & tokens)
{
  if (tokens.size() != 1)
    return false;

  alias = tokens[0];
  return true;
}

bool MslCommandUnalias::execute()
{
  if (cmds.find(alias) == cmds.end())
  {
    cout << "Alias " << alias << " does not exist" << endl;
    return false;
  }

  cmds.erase(alias);

  return true;
}

string MslCommandUnalias::helpMsg() const
{
  return "<alias>";
}

// -------------- MslCommandUnalias methods end -------------

// ------------- MslCommandDelete methods Begin -------------
MslCommandDelete::MslCommandDelete (marsysMap_t & inSet)
  : MslMarSystemCommand (inSet)
{
}

bool MslCommandDelete::verify(cmdList_t & tokens)
{
  if (tokens.size() != 1)
    return false;

  name = tokens[0];
  return true;
}

bool MslCommandDelete::execute()
{
  MarSystem* p;

  if (workingSet.find(name) != workingSet.end()) {
    p = (MarSystem *) workingSet[name];
    delete p;

    workingSet.erase(name);

    return true;
  } else {
    cout << "Cannot find MarSystem: " << name << endl;
    return false;
  }
}

string MslCommandDelete::helpMsg() const
{
  return "<msys>";
}

// -------------- MslCommandDelete methods end --------------

// ------------- MslCommandComposite methods begin -------------

MslCommandComposite::MslCommandComposite (marsysMap_t & inSet,
    MarSystemManager & inManager, MslModel & modelIn)
  : MslMarSystemCommand (inSet), mng (inManager), msl(modelIn)
{
}

bool MslCommandComposite::verify(cmdList_t & tokens)
{
  // need at least: (<compositeType> <name> := <name> <compositeType>)
  if (tokens.size() < 5)
    return false;

  string firstToken = tokens[0];
  string lastToken = tokens[tokens.size() - 1];

  // must begin with opening bracket and end with closing bracket
  if ((firstToken[0] != '(') || (lastToken[lastToken.length() - 1] != ')'))
    return false;

  // 3rd token must be the "assignment" operator
  if (tokens[2].compare(":=") != 0)
    return false;

  // the first token and last token should be equal after removing brackets
  string foo = firstToken.substr(1);
  if (foo.compare(lastToken.substr(0, lastToken.length() - 1)) != 0)
    return false;


  // if first token is "-" then this should be mapped to "Series", and
  // if it's "|" then it should be mapped to "Fanout"
  if (foo.compare ("-") == 0)
    foo = "Series";
  else if (foo.compare ("|") == 0)
    foo = "Fanout";

  // command is syntactically valid, save values into object
  compositeType = foo;
  compositeName = tokens[1];

  subSystems.clear();
  for (unsigned int x = 3; x < tokens.size() - 1; x++)
    subSystems.push_back(tokens[x]);

  return true;
}

bool MslCommandComposite::execute()
{
  // if there's already a marsystem with the supplied name, show error
  if (workingSet.find (compositeName) != workingSet.end())
  {
    cout << compositeName << " already exists" << endl;
    return false;
  }

  cmdList_t::const_iterator iter;

  // make sure the subsystems already exist
  for (iter = subSystems.begin(); iter != subSystems.end(); iter++)
    if (workingSet.find (*iter) == workingSet.end())
    {
      cout << *iter << " does not exist" << endl;
      return false;
    }

  // create the composite in the working set, relying on the msl object to
  // handle errors
  if (!(msl.input(compositeType + " " + compositeName)))
    return false;

  // add the subsystems to the composite
  for (iter = subSystems.begin(); iter != subSystems.end(); iter++)
  {
    // if cant add then abort
    if (! msl.input ("add " + *iter + " > " + compositeName))
      return false;
  }

  return true;
}

string MslCommandComposite::helpMsg() const
{
  return "(<compositeType> <name> := [<type>] <name> [<type>] <name> ... <compositeType>)";
}

// -------------- MslCommandComposite methods end --------------
