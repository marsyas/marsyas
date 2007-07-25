
/**
   \class PeakVisMainWindow
	\brief QMainWindow for the PeakVis application
 */

#include <QtGui>

#include "realvec.h"
#include "PeakVisMainWindow.h"

#ifdef LORIS_LIB
#include "PartialList.h"
#endif

using namespace std;
using namespace Marsyas;

#ifdef LORIS_LIB   
using namespace Loris;
#endif

PeakVisMainWindow::PeakVisMainWindow()
{
   ui.setupUi(this);
   peakVis = new PeakVis(this);
   
   QGridLayout *grid = new QGridLayout( ui.frame );
   grid->addWidget( peakVis, 0, 0 );
   
   fileCabinet = new PeakFileCabinet();
   
   createActions();
   createMenus();
   createConnections();   
}

PeakVisMainWindow::~PeakVisMainWindow()
{
   delete peakVis;
   delete fileCabinet;
}

void
PeakVisMainWindow::createMenus()
{
}

void
PeakVisMainWindow::createActions()
{
}

void
PeakVisMainWindow::createConnections()
{
   connect( ui.actionOpen, SIGNAL( triggered() ), this, SLOT( open() ) );
   connect( ui.openFileButton, SIGNAL( clicked() ), this, SLOT( open() ) );   
}

void
PeakVisMainWindow::open()
{
#ifdef LORIS_LIB   
   QString s = QFileDialog::getOpenFileName( this, "", "../..", "PeakData Files (*.peak *.PEAK *.sdif *.SDIF *.txt *.TXT)");
#else
   QString s = QFileDialog::getOpenFileName( this, "", "../..", "PeakData Files (*.peak *.PEAK *.txt *.TXT)");   
#endif
   
	if ( s.isEmpty() )
      return;
   
   QFileInfo fi( s );   
   
	ui.filenameLabel->setToolTip(s);
   QString ext = fi.suffix(); 
      
	ui.filenameLabel->setText(fi.fileName());
   //   qApp->processEvents(); // enforces repaint;       
   
   if( ext.toUpper() == "PEAK" )
   {
      fileCabinet->openPeakFile( s.toStdString() );
      realvec data = fileCabinet->getFileData(/*0*/);  
      peakVis->addPeakObjects( data );
   }
#ifdef LORIS_LIB   
   else if( ext.toUpper() == "SDIF" )
   {
      fileCabinet->openSdifFile( s.toStdString() );
      PartialList data = fileCabinet->getSdifFileData();
      peakVis->addPartialObjects( data );
   }
#endif   
   else if( ext.toUpper() == "TXT" )
   {  
      realvec data; 
      
      switch( fileCabinet->openTextFile( s.toStdString() ) )
      {
         case PeakFileCabinet::TEXT_FRAME_FORMAT:
            data = fileCabinet->getFileData();
            peakVis->addPeakObjects( data );   // for testing purposes only
            //peakVis->addPartialObjects( data );   
            break;
         case PeakFileCabinet::TEXT_PARTIALS_FORMAT:
            data = fileCabinet->getFileData();
            peakVis->addPeakObjects( data );   // for testing purposes only
            //peakVis->addPartialObjects( data );
            break;
         default:            
            ;
      }
   }   
}

