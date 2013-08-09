#ifndef MUSICCOLLECTIONIO_H_
#define MUSICCOLLECTIONIO_H_

#include <QFile>

#include "MusicCollection.h"

class MusicCollectionIO
{
public:
  enum ParserTypes { ITUNES, MARXML, MARCSV };
  //static bool parse(ParserTypes which, QFile &file, MusicCollection *collection);
  static bool parse(QFile &file, MusicCollection *collection);
  static bool write(QFile &file, MusicCollection *collection);
};


#endif /* MUSICCOLLECTIONIO_H_ */

