
#include "PlayGroupBoxW.h"

#include <QtGui>

PlayGroupBoxW::PlayGroupBoxW(QWidget *parent)
  : QGroupBox(parent)
{
  setTitle(tr("Currently Playing"));
  QGridLayout *layout = new QGridLayout;

  QFont font("Helvetica", 11);

  QLabel *title = new QLabel(tr("Title:"));
  title->setFont(font);
  _title = new QLabel(tr(""));
  _title->setFont(font);
  layout->addWidget(title, 0, 0);
  layout->addWidget(_title, 0, 1, 1, 2);

  QLabel *artist = new QLabel(tr("Artist:"));
  artist->setFont(font);
  _artist = new QLabel(tr(""));
  _artist->setFont(font);
  layout->addWidget(artist, 1, 0);
  layout->addWidget(_artist, 1, 1, 1, 2);

  QLabel *album = new QLabel(tr("Album:"));
  album->setFont(font);
  _album = new QLabel(tr(""));
  _album->setFont(font);
  layout->addWidget(album, 2, 0);
  layout->addWidget(_album, 2, 1, 1, 2);

  QLabel *genre = new QLabel(tr("Genre:"));
  genre->setFont(font);
  _genre = new QLabel(tr(""));
  _genre->setFont(font);
  layout->addWidget(genre, 3, 0);
  layout->addWidget(_genre, 3, 1, 1, 2);

  _location = new QLabel(tr(""));
  _kind = new QLabel(tr(""));
  _year = new QLabel(tr(""));
  _length = new QLabel(tr(""));
  /*
  QLabel *location = new QLabel(tr("Location:"));
  layout->addWidget(location, 0, 1);
  layout->addWidget(_location, 0, 1, 1, 2);

  QLabel *kind = new QLabel(tr("Kind:"));
  layout->addWidget(kind, 0, 1);
  layout->addWidget(_kind, 0, 1, 1, 2);

  QLabel *year = new QLabel(tr("Year:"));
  layout->addWidget(year, 0, 1);
  layout->addWidget(_year, 0, 1, 1, 2);

  QLabel *length = new QLabel(tr("Length:"));
  layout->addWidget(length, 0, 1);
  layout->addWidget(_length, 0, 1, 1, 2);
  */
  setLayout(layout);
  setVisible(false);
}

void PlayGroupBoxW::updateCurrentlyPlaying(MusicTrack *track) {
  _title->setText(track->getTitle());
  _artist->setText(track->getArtist());
  _album->setText(track->getAlbum());
  _genre->setText(track->getGenre());
  _kind->setText(track->getKind());
  _year->setText(track->getYear());
  //_length->setText(track->getLength());
  _location->setText(track->getLocation());
  setVisible(true);
}

