
#include "PlayListW.h"

#include <QDebug>
#include <QMouseEvent>

PlayListW::PlayListW(TrackListW *tracklist, QWidget *parent)
  : QListWidget(parent), _tracklist(tracklist)
{
  _library = MusicCollection::getInstance();

  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setDragEnabled(true);

  connect(this, SIGNAL(currentRowChanged(int)), this, SLOT(playlistClicked(int)));
}

PlayListW::~PlayListW() {}

void PlayListW::playlistClicked(int /* currentRow */) {
  QString current = currentItem()->text();
  qDebug() << current << " Playlist Clicked";
  MusicPlaylist *playlist = _library->getPlaylistByName(current);
  _tracklist->listTracks(playlist);
}

void PlayListW::updatePlaylist() {
  qDebug("Updating Playlist\n");
  clear();

  MusicPlaylistIterator ip = _library->getPlaylists();
  while ( ip.hasNext() ) {
    MusicPlaylist *playlist = ip.next().value();
    addItem( playlist->getName() );
  }
}

void PlayListW::mousePressEvent(QMouseEvent *event) {
  QListWidget::mousePressEvent(event);

  if ( event->button() == Qt::LeftButton ) {
    QListWidgetItem *item = currentItem();

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData();

    //Playlist Id, convoluted? indeed
    QByteArray playlistba;
    playlistba.append(item->text());

    mimeData->setText(item->text());
    mimeData->setData("application/playlist-id", playlistba);
    drag->setMimeData(mimeData);

    qDebug() << "Start Playlist Drag: " << mimeData->text()
             << " " << mimeData->data("application/playlist-id").toInt();

    Qt::DropAction dropAction = drag->start(Qt::CopyAction); // [WTF]
  }
}
