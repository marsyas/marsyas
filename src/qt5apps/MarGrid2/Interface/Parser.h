#ifndef PARSER_H
#define PARSER_H

#include <QFile>
#include "../Music/MusicCollection.h"

class Parser
{
public:
  enum ParserTypes { ITUNES, MARXML, MARCSV };
  //static bool parse(ParserTypes which, QFile &file, MusicCollection *collection);
  static bool parse(QFile &file, MusicCollection *collection);
};


#endif /* PARSER_H */

