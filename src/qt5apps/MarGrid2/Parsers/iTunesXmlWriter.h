#ifndef ITUNESXMLWRITER_XML
#define ITUNESXMLWRITER_XML

#include <QUrl>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>

#include "../Music/MusicCollection.h"

class iTunesXmlWriter
{
public:
  iTunesXmlWriter(QFile &file);
  ~iTunesXmlWriter();
  void operator<<(MusicCollection *library);

protected:
  //QTextStream& operator<<(QTextStream& out, MusicTrack *track);
  //QTextStream& operator<<(QTextStream& out, MusicPlaylist *playlist);
  void output(MusicTrack *track);
  void output(MusicPlaylist *playlist);

private:
  QTextStream *_out;
};

#endif /* ITUNESXMLWRITER_XML */

