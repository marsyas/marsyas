
/**
   \class PeakVisMainWindow
	\brief QMainWindow for the PeakVis application
 */

#ifndef PEAKVISMAINWINDOW_H
#define PEAKVISMAINWINDOW_H

#include "PeakFileCabinet.h"
#include "PeakVis.h"
#include "ui_PeakVisForm.h"
   
class PeakVisMainWindow : public QMainWindow
{
   Q_OBJECT
   
public :
   PeakVisMainWindow();
   ~PeakVisMainWindow();
   
public slots:
   void open();
   
private:
   void createMenus();
   void createActions();
   void createConnections();
   
   Ui::PeakVisForm ui;                 // the main ui form created 
                                       // using Designer    
   PeakFileCabinet* fileCabinet;   
   PeakVis* peakVis;   
};

#endif // PEAKVISMAINWINDOW_H
