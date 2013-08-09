#include <QtGui>
#include "Tracklist.h"

Tracklist::Tracklist(QWidget *parent)
  : QListWidget(parent)
{
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setDragEnabled(true);
  //setMouseTracking(true);
}

Tracklist::~Tracklist() {
}

void Tracklist::listTracks(MusicPlaylist *playlist) {
  clear();

  QListWidgetItem *item;
  MusicTrackIterator it = playlist->getTracks();
  while ( it.hasNext() ) {
    MusicTrack *track = it.next();
    item = new QListWidgetItem(track->getTitle());
    item->setData( Qt::UserRole, QVariant(track->getTrackId()));
    item->setData( TRACK_LOCATION, QVariant(track->getLocation()));

    qDebug() << "adding track: " << track->getTitle()
             << " " << track->getTrackId();

    addItem(item);
  }
}

void Tracklist::listTracks(MusicTrackIterator *it) {
  clear();

  QListWidgetItem *item;
  while ( it->hasNext() ) {
    MusicTrack *track = it->next();
    item = new QListWidgetItem(track->getTitle());
    item->setData( Qt::UserRole, QVariant(track->getTrackId()));
    item->setData(TRACK_LOCATION, QVariant(track->getLocation()));

    qDebug() << "adding track: " << track->getTitle()
             << " " << track->getTrackId();

    addItem(item);
  }
}


void Tracklist::mousePressEvent(QMouseEvent *event) {
  QListWidget::mousePressEvent(event);

  if ( event->button() == Qt::LeftButton ) {
    QListWidgetItem *item = currentItem();

    if (item != 0)
    {
      QDrag *drag = new QDrag(this);
      QMimeData *mimeData = new QMimeData();

      //Track Id, convoluted? indeed
      QByteArray trackIdba;
      QByteArray trackLocation;
      trackLocation = (item->data(TRACK_LOCATION)).toString().toStdString().c_str()  ;

      trackIdba.setNum(item->data(Qt::UserRole).toInt());
      mimeData->setText(item->text());
      mimeData->setData("application/track-id", trackIdba);
      mimeData->setData("application/track-location", trackLocation);
      drag->setMimeData(mimeData);

      qDebug() << "Start Track Drag: " << mimeData->text()
               << " " << mimeData->data("application/track-id").toInt();

      Qt::DropAction dropAction = drag->start(Qt::CopyAction);
    }
  }
}


