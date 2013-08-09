#ifndef AUDIODEVICECONFIG_H
#define AUDIODEVICECONFIG_H

#include <QDialog>
#include <QVector>
#include <QMap>
#include "ui_audiodeviceconfig.h"
#include "RtAudio.h"

#include "MarEditor.h"
#include "SoundDevice.h"

class AudioDeviceConfig : public QDialog, private Ui::AudioDeviceConfigClass
{
  Q_OBJECT

public:
  AudioDeviceConfig(QWidget *parent = 0, SoundDevice* soundDevice = 0);
  ~AudioDeviceConfig();

private:

  QMap<RtAudio::RtAudioApi, QString> APInames_;

  RtAudio* audioDS_;
  RtAudio* audioASIO_;

  SoundDevice* soundDevice_;

  RtAudio::RtAudioApi selectedAPI_;
  int selectedDeviceID_;
  QString selectedDeviceName_;
  int selectedBufferSizeSamples_;
  int selectedNrBuffers_;

  void listDSdevices();
  void listASIOdevices();
  void listDevices(RtAudio* audio, QString apiName);
  void listCurrentSoundDeviceConfig();

private slots:
  void on_audioDevicesList_itemClicked(QTreeWidgetItem* item, int column);
  void on_sldrBufferSize_valueChanged(int value);
  void on_spinNBuffers_valueChanged(int value);
  void on_btnOK_clicked();

};

#endif // AUDIODEVICECONFIG_H
