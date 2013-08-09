#include "audioproperties.h"
#include <QTreeWidgetItem>
#include <QString>

using namespace Ui;

AudioProperties::AudioProperties(QWidget *parent, Signal* const audio)
  : QFrame(parent), audio_(audio)
{
  setupUi(this);

  populate();

}

AudioProperties::~AudioProperties()
{

}

void
AudioProperties::populate()
{
  if(!audio_)
    return;

  //bufferSizeList
  audioPropsList->setColumnCount(1);
  audioPropsList->setItemHidden(audioPropsList->headerItem(),true);

  QTreeWidgetItem* item;

  item = new QTreeWidgetItem(audioPropsList, QStringList("Audio Duration:"));
  new QTreeWidgetItem(item, QStringList(QString::number(audio_->sizeSeconds()) + " secs" ));
  new QTreeWidgetItem(item, QStringList(QString::number(audio_->sizeSamples()) + " samples" ));
  new QTreeWidgetItem(item, QStringList(QString::number(audio_->sizeBytes()) + " bytes" ));
  audioPropsList->expandItem(item);

  item = new QTreeWidgetItem(audioPropsList, QStringList("Sampling Rate:"));
  new QTreeWidgetItem(item, QStringList(QString::number(audio_->srate()) + " Hz" ));
  audioPropsList->expandItem(item);

  item = new QTreeWidgetItem(audioPropsList, QStringList("Channels:"));
  new QTreeWidgetItem(item, QStringList(QString::number(audio_->channels())));
  audioPropsList->expandItem(item);

  item = new QTreeWidgetItem(audioPropsList, QStringList("Sample Format:"));
  new QTreeWidgetItem(item, QStringList(QString::number(audio_->bits()) + " bits/sample" ));
  audioPropsList->expandItem(item);
}
