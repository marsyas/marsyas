
#include "TrackListW.h"

#include <QDebug>
#include <QMouseEvent>

TrackListW::TrackListW(QWidget *parent)
  : QListWidget(parent)
{
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setDragEnabled(true);
  //setMouseTracking(true);
}

TrackListW::~TrackListW() {
}

void TrackListW::listTracks(MusicPlaylist *playlist) {
  clear();

  QListWidgetItem *item;
  MusicTrackIterator it = playlist->getTracks();
  while ( it.hasNext() ) {
    MusicTrack *track = it.next();
    item = new QListWidgetItem(track->getTitle());
    item->setData( Qt::UserRole, QVariant(track->getTrackId()));

    qDebug() << "adding track: " << track->getTitle()
             << " " << track->getTrackId();

    addItem(item);
  }
}

void TrackListW::listTracks(MusicTrackIterator *it) {
  clear();

  QListWidgetItem *item;
  while ( it->hasNext() ) {
    MusicTrack *track = it->next();
    item = new QListWidgetItem(track->getTitle());
    item->setData( Qt::UserRole, QVariant(track->getTrackId()));

    qDebug() << "adding track: " << track->getTitle()
             << " " << track->getTrackId();

    addItem(item);
  }
}

void TrackListW::mousePressEvent(QMouseEvent *event) {
  QListWidget::mousePressEvent(event);

  if ( event->button() == Qt::LeftButton ) {
    QListWidgetItem *item = currentItem();

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData();

    //Track Id, convoluted? indeed
    QByteArray trackIdba;
    trackIdba.setNum(item->data(Qt::UserRole).toInt());

    mimeData->setText(item->text());
    mimeData->setData("application/track-id", trackIdba);
    drag->setMimeData(mimeData);

    qDebug() << "Start Track Drag: " << mimeData->text()
             << " " << mimeData->data("application/track-id").toInt();

    Qt::DropAction dropAction = drag->start(Qt::CopyAction); // [WTF]
  }
}


