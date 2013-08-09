
#include "iTunesPage.h"

#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

iTunesPage::iTunesPage(QWidget *parent)
  : QWidget(parent)
{
  _library = MusicCollection::getInstance();

  QGroupBox *trainingBox = new QGroupBox(tr("Training Configuration"));
  QLabel *trainingLabel = new QLabel(tr("Select Playlist for Training: "));
  _trainingCombo = new QComboBox;

  QGroupBox *predictBox = new QGroupBox(tr("Prediction Configuration"));
  QLabel *predictLabel = new QLabel(tr("Select Playlist for Prediction: "));
  _predictCombo = new QComboBox;

  QHBoxLayout *trainingLayout = new QHBoxLayout;
  trainingLayout->addWidget(trainingLabel);
  trainingLayout->addWidget(_trainingCombo);
  trainingBox->setLayout(trainingLayout);

  QHBoxLayout *predictLayout = new QHBoxLayout;
  predictLayout->addWidget(predictLabel);
  predictLayout->addWidget(_predictCombo);
  predictBox->setLayout(predictLayout);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(trainingBox);
  mainLayout->addWidget(predictBox);
  mainLayout->addStretch(1);
  setLayout(mainLayout);
}

void iTunesPage::showEvent(QShowEvent *) {
  MusicPlaylistIterator it = _library->getPlaylists();
  while( it.hasNext() ) {
    it.next();
    QString name = it.key();
    _trainingCombo->addItem(name);
    _predictCombo->addItem(name);
  }
}
