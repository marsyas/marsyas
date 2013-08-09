
#include "MusicCollectionIO.h"

#include <QXmlSimpleReader>
#include <QXmlInputSource>

#include "iTunesXmlHandler.h"
#include "iTunesXmlWriter.h"

//bool Parser::parse(ParserTypes which, QFile &file, MusicCollection *collection) {
bool MusicCollectionIO::parse(QFile &file, MusicCollection *collection) {

  /*
  switch ( which ) {
  case ITUNES: {
  */
  iTunesXmlHandler parser(collection);
  QXmlSimpleReader reader;
  reader.setContentHandler(&parser);
  reader.setErrorHandler(&parser);

  QXmlInputSource xmlInputSource(&file);

  return reader.parse(xmlInputSource);
  /*
  }
  case MARCSV:
  case MARXML:
  default:
  	return false;
  }
  */
}

bool MusicCollectionIO::write(QFile &file, MusicCollection *collection)
{
  iTunesXmlWriter writer(file);
  writer << collection;
  return true; // [WTF]
}