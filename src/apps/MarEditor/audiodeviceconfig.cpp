#include "audiodeviceconfig.h"
#include <QMessageBox>
#include <math.h>

using namespace Ui;

AudioDeviceConfig::AudioDeviceConfig(QWidget *parent, SoundDevice* soundDevice)
  : QDialog(parent)
{
  setupUi(this);

  soundDevice_ = soundDevice;

  btnOK->setEnabled(false);

  selectedAPI_ = RtAudio::UNSPECIFIED; //RtAudio automatic API selection
  selectedDeviceID_ = 0; //RtAudio automatic device selection
  selectedDeviceName_ = "default device";
  selectedBufferSizeSamples_ = 0; //RtAudio automatic buffer size selection
  selectedNrBuffers_ = 0; //RtAudio automatic nr of buffers selection

  //APInames
  APInames_[RtAudio::UNSPECIFIED] = "UNSPECIFIED";
  APInames_[RtAudio::LINUX_ALSA] = "LINUX_ALSA";
  APInames_[RtAudio::LINUX_OSS] = "LINUX_OSS";
  APInames_[RtAudio::LINUX_JACK] = "LINUX_JACK";
  APInames_[RtAudio::MACOSX_CORE] = "MACOSX_CORE";
  APInames_[RtAudio::IRIX_AL] = "IRIX_AL";
  APInames_[RtAudio::WINDOWS_ASIO] = "WINDOWS_ASIO";
  APInames_[RtAudio::WINDOWS_DS] = "WINDOWS_DS";

  //audioDevicesList
  audioDevicesList->setColumnCount(4);
  QStringList list;
  list << "Installed Audio Devices" << "ID" << "Default" << "Available";
  audioDevicesList->setHeaderLabels(list);
  //populate device list
  listDSdevices();
  listASIOdevices();

  //bufferSizeList
  bufferSizeList->setColumnCount(2);
  bufferSizeList->setItemHidden(bufferSizeList->headerItem(),true);
  QTreeWidgetItem* item = new QTreeWidgetItem(bufferSizeList, QStringList(QString("BufferSize")));
  QFont font;
  font.setPointSize(8);
  font.setBold(true);
  item->setFont(0, font);
  item->setFont(1, font);
  new QTreeWidgetItem(bufferSizeList, QStringList(QString("@ 48KHz")));
  new QTreeWidgetItem(bufferSizeList, QStringList(QString("@ 44.1KHz")));
  new QTreeWidgetItem(bufferSizeList, QStringList(QString("@ 22.05KHz")));
  new QTreeWidgetItem(bufferSizeList, QStringList(QString("@ 16KHz")));
  new QTreeWidgetItem(bufferSizeList, QStringList(QString("@ 11.025KHz")));
  new QTreeWidgetItem(bufferSizeList, QStringList(QString("@ 8KHz")));

  //infoList
  infoList->setColumnCount(1);
  infoList->setHeaderLabels(QStringList("Device Info"));

  //configList
  configList->setColumnCount(1);
  configList->setItemHidden(configList->headerItem(),true);
  listCurrentSoundDeviceConfig();
}

AudioDeviceConfig::~AudioDeviceConfig()
{
  delete audioDS_;
  delete audioASIO_;
}

void
AudioDeviceConfig::listCurrentSoundDeviceConfig()
{
  configList->clear();

  if(soundDevice_)
  {
    //DeviceName
    new QTreeWidgetItem(configList, QStringList(soundDevice_->deviceName()));
    //RtApi name
    new QTreeWidgetItem(configList, QStringList(APInames_[soundDevice_->API()]));
    //outDeviceID
    new QTreeWidgetItem(configList, QStringList("device ID: " + QString::number(soundDevice_->deviceID())));
    //BufferSize
    new QTreeWidgetItem(configList, QStringList("Buffer Size (samples): " + QString::number(soundDevice_->bufferSize())));
    //number of Buffers
    new QTreeWidgetItem(configList, QStringList("Number of Buffers: " + QString::number(soundDevice_->nrBuffers())));
  }
}

void
AudioDeviceConfig::listDSdevices()
{
  try {
    audioDS_ = new RtAudio(RtAudio::WINDOWS_DS);
  }
  catch (RtError &error) {
    QMessageBox::warning(this,"Error opening DirectShow Audio Devices!", QString::fromStdString(error.getMessage()), QMessageBox::Ok,
                         QMessageBox::NoButton,QMessageBox::NoButton);
    audioDS_ = NULL;
    return;
  }

  listDevices(audioDS_, "DirectShow");
}

void
AudioDeviceConfig::listASIOdevices()
{
  try {
    audioASIO_ = new RtAudio(RtAudio::WINDOWS_ASIO);
  }
  catch (RtError &error) {
    QMessageBox::warning(this,"Error opening ASIO Audio Devices!", QString::fromStdString(error.getMessage()), QMessageBox::Ok,
                         QMessageBox::NoButton,QMessageBox::NoButton);
    audioASIO_ = NULL;
    return;
  }

  listDevices(audioASIO_, "ASIO");
}

void
AudioDeviceConfig::listDevices(RtAudio* audio, QString apiName)
{
  QTreeWidgetItem* item = NULL;
  RtAudioDeviceInfo info;

  QTreeWidgetItem* headerAPI = new QTreeWidgetItem(audioDevicesList, QStringList(apiName));
  QFont APInameFont;
  APInameFont.setPointSize(9);
  //APInameFont.setBold(true);
  headerAPI->setFont(0, APInameFont);
  headerAPI->setFlags(Qt::ItemIsEnabled);

  int devices = audio->getDeviceCount();

  for (int i=1; i<= devices; i++)
  {
    try {
      info = audio->getDeviceInfo(i);
    }
    catch (RtError &error) {
      QMessageBox::warning(this,"Error obtaining DirectShow device info!", QString::fromStdString(error.getMessage()), QMessageBox::Ok,
                           QMessageBox::NoButton,QMessageBox::NoButton);

      break;
    }

    item = new QTreeWidgetItem(headerAPI);
    if(info.probed)	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    else item->setFlags(Qt::ItemIsSelectable);

    item->setText(0, QString::fromStdString(info.name));

    item->setText(1, QString::number(i));
    item->setTextAlignment(1,Qt::AlignHCenter);

    (info.isDefault ? item->setCheckState(2,Qt::Checked) : item->setCheckState(2,Qt::Unchecked));
    item->setTextAlignment(2,Qt::AlignHCenter);

    (info.probed ? item->setCheckState(3,Qt::Checked) : item->setCheckState(3,Qt::Unchecked));
    item->setTextAlignment(3,Qt::AlignHCenter);
  }

  audioDevicesList->expandItem(headerAPI);

}

//------------------------------------------------------------------------------------------------------------
//		SLOTS
//------------------------------------------------------------------------------------------------------------
void
AudioDeviceConfig::on_audioDevicesList_itemClicked(QTreeWidgetItem* item, int column)
{
  RtAudio* audio;
  RtAudioDeviceInfo info;

  infoList->clear();
  btnOK->setEnabled(false);

  selectedAPI_ = RtAudio::UNSPECIFIED;
  selectedDeviceID_ = 0;
  selectedDeviceName_ = "default device";

  if(item->text(0) == "DirectShow" || item->text(0) == "ASIO")
  {
    return;
  }

  if(item->parent()->text(0)== "DirectShow")
  {
    audio = audioDS_;
    selectedAPI_ = RtAudio::WINDOWS_DS;
  }
  else if(item->parent()->text(0)== "ASIO")
  {
    audio = audioASIO_;
    selectedAPI_ = RtAudio::WINDOWS_ASIO;
  }
  else
  {
    QMessageBox::warning(this,"Invalid RtAudio API ", item->parent()->text(0)+" is not a supported RtAudio API!", QMessageBox::Ok,
                         QMessageBox::NoButton,QMessageBox::NoButton);
    return;
  }

  int deviceID =  item->text(1).toInt();

  //Get Sound Device Info
  try {
    info = audio->getDeviceInfo(deviceID);
  }
  catch (RtError &error) {
    QMessageBox::warning(this,"Error obtaining audio device info!", QString::fromStdString(error.getMessage()), QMessageBox::Ok,
                         QMessageBox::NoButton,QMessageBox::NoButton);
    infoList->clear();
    return;
  }

  //populate Info List
  QTreeWidgetItem* infoHeader;

  //Sound Device name
  infoHeader = new QTreeWidgetItem(infoList, QStringList(QString("Device Name:")));
  QTreeWidgetItem* name = new QTreeWidgetItem(infoHeader, QStringList(QString::fromStdString(info.name)));
  QFont font;
  font.setBold(true);
  name->setFont(0, font);
  infoList->expandItem(infoHeader);
  //Probe Successful?
  if (!info.probed)
  {
    infoHeader = new QTreeWidgetItem(infoList, QStringList(QString("Device info not available...")));
    infoList->expandItem(infoHeader);
    return;
  }

  btnOK->setEnabled(true);
  selectedDeviceID_ = deviceID;
  selectedDeviceName_ = QString::fromStdString(info.name);

  //Default device?
  if(info.isDefault)
    new QTreeWidgetItem(name, QStringList(QString("DEFAULT device")));
  infoList->expandItem(name);
  infoList->expandItem(infoHeader);

  //Sound Device Output Channels
  infoHeader = new QTreeWidgetItem(infoList, QStringList(QString("Output Channels:")));
  new QTreeWidgetItem(infoHeader, QStringList(QString::number(info.outputChannels)));
  infoList->expandItem(infoHeader);

  //Sound Device Input Channels
  infoHeader = new QTreeWidgetItem(infoList, QStringList(QString("Input Channels:")));
  new QTreeWidgetItem(infoHeader, QStringList(QString::number(info.inputChannels)));
  infoList->expandItem(infoHeader);

  //Sound Device Duplex Channels
  infoHeader = new QTreeWidgetItem(infoList, QStringList(QString("Duplex Channels:")));
  new QTreeWidgetItem(infoHeader, QStringList(QString::number(info.duplexChannels)));
  infoList->expandItem(infoHeader);

  //Native Formats
  infoHeader = new QTreeWidgetItem(infoList, QStringList(QString("Natively supported data formats:")));
  if(info.nativeFormats == 0)
    new QTreeWidgetItem(infoHeader, QStringList(QString("No natively supported data formats(?)!")));
  else
  {
    if ( info.nativeFormats & RTAUDIO_SINT8 )
      new QTreeWidgetItem(infoHeader, QStringList(QString("8-bit signed")));
    if ( info.nativeFormats & RTAUDIO_SINT16 )
      new QTreeWidgetItem(infoHeader, QStringList(QString("16-bit signed")));
    if ( info.nativeFormats & RTAUDIO_SINT24 )
      new QTreeWidgetItem(infoHeader, QStringList(QString("24-bit signed")));
    if ( info.nativeFormats & RTAUDIO_SINT32 )
      new QTreeWidgetItem(infoHeader, QStringList(QString("32-bit signed")));
    if ( info.nativeFormats & RTAUDIO_FLOAT32 )
      new QTreeWidgetItem(infoHeader, QStringList(QString("32-bit float")));
    if ( info.nativeFormats & RTAUDIO_FLOAT64 )
      new QTreeWidgetItem(infoHeader, QStringList(QString("64-bit float")));
  }
  infoList->expandItem(infoHeader);

  //Sample rates
  infoHeader = new QTreeWidgetItem(infoList, QStringList(QString("Supported Sample Rates:")));
  if ( info.sampleRates.size() < 1 )
    new QTreeWidgetItem(infoHeader, QStringList(QString("No supported sample rates found!")));
  else
    for (unsigned int j=0; j<info.sampleRates.size(); j++)
      new QTreeWidgetItem(infoHeader, QStringList(QString::number(info.sampleRates[j])));
  infoList->expandItem(infoHeader);
}

void
AudioDeviceConfig::on_sldrBufferSize_valueChanged(int value)
{
  QTreeWidgetItem* item;

  //RtAudio Buffer size (in samples) must be a power of two
  selectedBufferSizeSamples_ = (int)pow(2.0, value);

  //buffer samples
  item = bufferSizeList->topLevelItem(0);
  item->setText(1,QString::number(selectedBufferSizeSamples_)+ QString(" samples"));
  //@48KHz
  item = bufferSizeList->topLevelItem(1);
  item->setText(1,QString::number(selectedBufferSizeSamples_ / 48000.0 * 1000) + QString(" ms"));
  //@44.1KHz
  item = bufferSizeList->topLevelItem(2);
  item->setText(1,QString::number(selectedBufferSizeSamples_ / 44100.0 * 1000) + QString(" ms"));
  //@22.05KHz
  item = bufferSizeList->topLevelItem(3);
  item->setText(1,QString::number(selectedBufferSizeSamples_ / 22050.0 * 1000) + QString(" ms"));
  //@16KHz
  item = bufferSizeList->topLevelItem(4);
  item->setText(1,QString::number(selectedBufferSizeSamples_ / 16000.0 * 1000) + QString(" ms"));
  //@11.025KHz
  item = bufferSizeList->topLevelItem(5);
  item->setText(1,QString::number(selectedBufferSizeSamples_ / 11025.0 * 1000) + QString(" ms"));
  //@8KHz
  item = bufferSizeList->topLevelItem(6);
  item->setText(1,QString::number(selectedBufferSizeSamples_ / 8000.0 * 1000) + QString(" ms"));
}

void
AudioDeviceConfig::on_spinNBuffers_valueChanged(int value)
{
  selectedNrBuffers_ = value;
}

void
AudioDeviceConfig::on_btnOK_clicked()
{
  if(soundDevice_->API() != selectedAPI_ || soundDevice_->deviceID() != selectedDeviceID_)
    soundDevice_->setSoundDevice(selectedAPI_, selectedDeviceID_, selectedDeviceName_);

  if(soundDevice_->bufferSize() !=  selectedBufferSizeSamples_ ||
      soundDevice_->nrBuffers() != selectedNrBuffers_)
    soundDevice_->setBuffers(selectedBufferSizeSamples_, selectedNrBuffers_);
}