
/* MSL: Marsyas Scripting Language is a simple domain specific
scripting language that can be used to created networks
of MarSystems at run-time. Although initially it was designed
mainly as a backend to user interfaces we are considering
making it functional by itself.
*/

/* Written by Stuart Bray: Summer 2004 */
/* Revisions by Adam Parkin: Summer 2005 */


#include <iostream>
#include <string>
#include "MslModel.h"

#ifdef MARSYAS_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif
using namespace std;
using namespace Marsyas;

//
// global command list for tab completion
//
vector <string> commands;

#ifdef MARSYAS_READLINE
//
// Function prototypes
//
void initReadline(void);
char * command_generator (const char * text, int state);
char ** command_completion (const char * text, int start, int end);
char * dupstr (const char * strIn);
#endif

int
main(int argc, char** argv)
{
  MslModel model;

  cout << "Marsyas Script Language Interpreter v1.0" << endl << endl;

#ifdef MARSYAS_READLINE
  initReadline();
  using_history();
#endif

  while( true )
  {
#ifdef MARSYAS_READLINE

    // update tab completion list in case of aliases/unaliases
    commands = model.getCmds();
    commands.push_back("history");

    char *inputString = readline("[ msl-RL ] ");

    // if EOF then break
    if (inputString == NULL)
      break;

    // if line not blank process it
    // and add it to the command history
    if (inputString[0])
    {
      char * expansion = NULL;
      int result = history_expand (inputString, &expansion);

      // if there was no error during history expansion, swap
      // expansion and the inputString so that the expanded string
      // gets passed on to MslModel
      if (result == 1 || result == 0)
      {
        char * tmp = inputString;
        inputString = expansion;
        expansion = tmp;

        // add to command history
        add_history(inputString);

        // if history command, show the command history
        if (strncmp (inputString, "history", 7) == 0)
        {
          // check what's the correct call for this
          // 	HIST_ENTRY ** iter = history_list();

          /* if (iter)
          	for (int i = 0; iter[i]; i++)
          		cout << (i + history_base) << ". "
          			<< iter[i]->line << endl;
          */
        }
        // else process command in MslModel
        else
          model.input (inputString);
      }

      // if there was an error expanding the command, show error message
      else if (result == -1)
        cout << expansion << endl;

      // free memory if necessary
      if (expansion) free (expansion);
    }

    // free memory allocated by the readline() call
    free (inputString);
#else
    const int MAXLINE = 1024;
    char inputString[MAXLINE];

    cout << "[ msl ] ";
    cin.getline(inputString, MAXLINE);

    // if EOF then break
    if ((inputString == NULL) || cin.eof())
      break;

    if (inputString[0] != 0)
      model.input (inputString);
#endif
  }

}

#ifdef MARSYAS_READLINE

//
// Initialize the Readline routines.
//
void initReadline(void)
{
  // allow conditional parsing of ~/.inputrc file
  rl_readline_name = "MSL";

  // indicate function to do command completion before
  // filename completion
  rl_attempted_completion_function = command_completion;
}

//
// Comment and general code taken from the Readline fileman.c example:
//
//   Attempt to complete on the contents of TEXT.  START and END
//   bound the region of rl_line_buffer that contains the word to
//   complete.  TEXT is the word to complete.  We can use the entire
//   contents of rl_line_buffer in case we want to do some simple
//   parsing.  Return the array of matches, or NULL if there aren't any.
//
char ** command_completion (const char * text, int start, int end)
{
  char ** matches = (char **) NULL;

  /* If this word is at the start of the line, then it is a command
     to complete.  Otherwise it is the name of a file in the current
     directory, in which case we will return NULL and allow the filename
     completion to work. */
  // if (start == 0)
  // matches = rl_completion_matches (text, command_generator);

  // return (matches);
}

//
// Comment and general code taken from the Readline fileman.c example:
//
//   Generator function for command completion.  STATE lets us
//   know whether to start from scratch; without any state
//   (i.e. STATE == 0), then we start at the top of the list.
//
char * command_generator (const char * text, int state)
{
  static unsigned int list_index, len;
  const char * name;

  // If this is a new word to complete, initialize now.  This
  //   includes saving the length of TEXT for efficiency, and
  //   initializing the index variable to 0.
  if (!state)
  {
    list_index = 0;
    len = strlen (text);
  }

  // Return the next name which partially matches from the
  //   command list.
  while (list_index < commands.size())
  {
    // name = cmds[list_index].name;
    name = commands[list_index].c_str();
    list_index++;

    if (strncmp (name, text, len) == 0)
      return (dupstr(name));
  }

  // If no names matched, then return NULL.
  return ((char *)NULL);
}

//
// dupstr - duplicates a c-style string using malloc.
//	Returns a pointer to the newly allocated string if
//	successful, NULL otherwise.
//
//	NOTE: the readline routines require that return
//	values be allocated with malloc (the internal
//	readline routines will call free on them)
//
char * dupstr (const char * strIn)
{
  // if string passed in was NULL, just return NULL
  // (basically a sanity check)
  if (strIn == NULL)
    return NULL;

  // +1 for NULL terminator
  int len = strlen(strIn) + 1;

  char * retVal = (char *) malloc (len);

  // if memory was allocated, copy the string
  if (retVal != NULL)
    memcpy (retVal, strIn, len);

  return retVal;
}

#endif
