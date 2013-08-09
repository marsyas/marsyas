#include <QApplication>
#include "../Music/MusicCollection.h"
#include "iTunesXmlHandler.h"

int main( int argc, char *argv[] ) {

  MusicCollection *library = MusicCollection::getInstance();
  iTunesXmlHandler *handler = new iTunesXmlHandler(library);

  QXmlSimpleReader reader;
  reader.setContentHandler(handler);
  reader.setErrorHandler(handler);

  //QFile file("../iTunes-training.xml");
  QFile file("../iTunesMusicLibrary-grid.xml");
  //QFile file("../iTunesMusicLibrary.xml");
  //QFile file("../iTunesMusicLibrary-small.xml");

  if ( file.open(QFile::ReadOnly | QFile::Text) ) {

    QXmlInputSource xmlSource(&file);
    if ( reader.parse(xmlSource) ) {

      library->display();

    } else {
      std::cout << "Error iTunes Parse Reader Error\n";
    }
  } else {
    std::cout << "Error iTunes XML Library File Not Found\n";
  }

  delete handler;
  delete library;

  return 0;
}
