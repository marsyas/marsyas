
#ifndef MARPHASEVOCODERWINDOW_H
#define MARPHASEVOCODERWINDOW_H

#include <QApplication>
#include <QFont>
#include <QGridLayout>
#include <QPushButton>
#include <QMainWindow>
#include <QSlider>

#include "common.h"
#include "MarSystem.h"
#include "MarSystemManager.h" 
#include "MarSystemQtWrapper.h" 

#include "plot.h"
#include "SpectrogramData.h"
using namespace MarsyasQt;

class MarPeaks : public QMainWindow
{
    Q_OBJECT

private:
	void createMenus();
	void createActions();
	void createNetwork();
	void startNetwork();

	MarSystem* msys_;
	MarSystemQtWrapper* mwr_;

	QMenu*   fileMenu;  
	QMenu*   helpMenu;
	QAction* openAct;
	QAction* aboutAct;

	QSlider* posSlider_;

	MarControlPtr ctrl_pos_;
	MarControlPtr ctrl_Spectrum_;
	MarControlPtr ctrl_initAudio_;
	MarControlPtr ctrl_notEmpty_;

	SpectrogramData data_;
	Plot* spectrogram_;
	bool plot_;

public:
   MarPeaks();

public slots: 
   void about();
   void open();  
   void ctrlChanged(MarControlPtr ctrl);
};

#endif

	
