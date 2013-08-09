#ifndef ITUNESXMLHANDLER_XML
#define ITUNESXMLHANDLER_XML

#include <QXmlDefaultHandler>

#include "MusicTrack.h"
#include "MusicCollection.h"

class iTunesXmlHandler : public QXmlDefaultHandler
{
public:
  iTunesXmlHandler(MusicCollection *collection);
  //~iTunesXmlHandler();

  bool startElement(const QString &namespaceURI, const QString &localName,
                    const QString &qName, const QXmlAttributes &attributes);
  bool endElement(const QString &namespaceURI, const QString &localName,
                  const QString &qName);
  bool characters(const QString &str);
  bool fatalError(const QXmlParseException &exception);
  QString errorString() const;

  void addIntegerAttribute(const QString &key, const QString &text);
  void addStringAttribute(const QString &key, const QString &qtext);

private:
  QString _currentKey;
  QString _currentText;
  QString _errorStr;

  MusicTrack* _currentTrack;
  MusicPlaylist* _currentPlaylist;
  MusicCollection* _collection;

  bool _okToParse;
  bool _inLibrary;
  bool _inTrackList;
  bool _inTrack;

  bool _inPlaylists;
  bool _inPlaylist;
};


#endif /* ITUNESXMLHANDLER_XML */

