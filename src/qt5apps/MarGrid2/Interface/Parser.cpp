#include "Parser.h"

#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include "../Parsers/iTunesXmlHandler.h"

//bool Parser::parse(ParserTypes which, QFile &file, MusicCollection *collection) {
bool Parser::parse(QFile &file, MusicCollection *collection) {

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
