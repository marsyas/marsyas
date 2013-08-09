#include "iTunesXmlWriter.h"

iTunesXmlWriter::iTunesXmlWriter(QFile &file) {
  _out = new QTextStream(&file);
  _out->setCodec(QTextCodec::codecForName("UTF-8"));
}

iTunesXmlWriter::~iTunesXmlWriter() {
  _out->flush();
  delete _out;
}

void iTunesXmlWriter::operator<<(MusicCollection *library) {

  *_out	<< "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        << "<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" "
        << "\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
        << "<plist version=\"1.0\">\n"
        << "<dict>\n"
        << "\t<key>Major Version</key><integer>1</integer>\n"
        << "\t<key>Minor Version</key><integer>1</integer>\n"
        << "\t<key>Application Version</key><string>7.0.2</string>\n"
        << "\t<key>Features</key><integer>1</integer>\n"
        << "\t<key>Show Content Ratings</key><true/>\n"
        << "\t<key>Music Folder</key><string>file://localhost/</string>\n"
        << "\t<key>Library Persistent ID</key><string>0EE836F8C5E73F60</string>\n"
        << "\t<key>Tracks</key>\n"
        << "\t<dict>\n";

  MusicTrackIterator it = library->getTracks();
  while ( it.hasNext() ) {
    MusicTrack *track = it.next();
    output(track);
  }

  *_out	<< "\t</dict>\n"
        << "\t<key>Playlists</key>\n"
        << "\t<array>\n"
        << "\t\t<dict>\n";

  MusicPlaylistIterator ip = library->getPlaylists();
  while ( ip.hasNext() ) {
    MusicPlaylist *playlist = ip.next().value();
    output(playlist);
  }

  *_out	<< "\t\t</dict>\n"
        << "\t</array>\n"
        << "\t</dict>\n"
        << "</plist>";
}

void iTunesXmlWriter::output(MusicTrack *track) {
  QString location = QUrl::toPercentEncoding(track->getLocation(), "/");
  *_out	<< "\t\t<key>" << track->getTrackId() << "</key>\n"
        << "\t\t<dict>\n"
        << "\t\t\t<key>Track ID</key><integer>" << track->getTrackId() << "</integer>\n"
        << "\t\t\t<key>Name</key><string>" << track->getTitle() << "</string>\n"
        << "\t\t\t<key>Artist</key><string>" << track->getArtist() << "</string>\n"
        << "\t\t\t<key>Album</key><string>" << track->getAlbum() << "</string>\n"
        << "\t\t\t<key>Genre</key><string>" << track->getGenre() << "</string>\n"
        << "\t\t\t<key>Location</key><string>file://localhost" << location << "</string>\n"
        << "\t\t\t<key>Persistent ID</key><string>" << track->getPersistentId() << "</string>\n"
        << "\t\t\t<key>MarGrid_xpos</key><integer>" << track->getX() << "</integer>\n"
        << "\t\t\t<key>MarGrid_ypos</key><integer>" << track->getY() << "</integer>\n"
        << "\t\t</dict>\n";
}
/*
			<key>Composer</key><string>Metric</string>
			<key>Kind</key><string>MPEG audio file</string>
			<key>Size</key><integer>6295387</integer>
			<key>Total Time</key><integer>262217</integer>
			<key>Track Number</key><integer>1</integer>
			<key>Year</key><integer>2003</integer>
			<key>Date Modified</key><date>2006-03-29T06:51:49Z</date>
			<key>Date Added</key><date>2006-01-17T04:27:27Z</date>
			<key>Bit Rate</key><integer>192</integer>
			<key>Sample Rate</key><integer>44100</integer>
			<key>Comments</key><string> 00000BDC 00000C75 00002FD5 00002BDD 00002727 0001D4EE 00007E28 00007B5B 00030D6E 0001ADB0</string>
			<key>Play Count</key><integer>7</integer>
			<key>Play Date</key><integer>3226982754</integer>
			<key>Play Date UTC</key><date>2006-04-04T16:05:54Z</date>
			<key>Normalization</key><integer>4874</integer>
			<key>Track Type</key><string>File</string>
			<key>File Folder Count</key><integer>-1</integer>
*/

void iTunesXmlWriter::output(MusicPlaylist *playlist) {
  *_out	<< "\t\t\t<key>Name</key><string>" << playlist->getName() << "</string>\n"
        << "\t\t\t<key>Playlist ID</key><integer>" << playlist->getPlaylistId() << "</integer>\n"
        << "\t\t\t<key>Playlist Persistent ID</key><string>"
        << playlist->getPersistentId() << "</string>\n"
        << "\t\t\t<key>All Items</key><true/>\n"
        << "\t\t\t<key>Playlist Items</key>\n"
        << "\t\t\t<array>\n";

  MusicTrackIterator it = playlist->getTracks();
  while ( it.hasNext() ) {
    MusicTrack *track = it.next();
    *_out	<< "\t\t\t\t<dict>\n"
          << "\t\t\t\t\t<key>Track ID</key><integer>" << track->getTrackId() << "</integer>\n"
          << "\t\t\t\t</dict>\n";
  }
  *_out	<< "\t\t\t</array>\n";
}

