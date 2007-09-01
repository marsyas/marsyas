
/**
	\class PeakFileCabinet
	\brief Contains information/data for a number of Peak files.
*/

#ifndef PEAKFILECABINET_H
#define PEAKFILECABINET_H


#include <QObject>
#include <vector>
#include "peakView.h"
#include "realvec.h"

#ifdef LORIS_LIB
#include "PartialList.h"
#endif

/**
   \class PeakFileCabinet
	\brief Contains information/data for a number of Peak files.
*/

class PeakFileCabinet : public QObject
{
	Q_OBJECT

public:
   
   enum textFileType {
      TEXT_FRAME_FORMAT,
      TEXT_PARTIALS_FORMAT,
      TEXT_FILE_OPEN_ERROR
   } ;
   
	PeakFileCabinet();
	~PeakFileCabinet();

public slots:
	int openPeakFile(std::string filename);
   textFileType openTextFile(std::string filename);
   
   Marsyas::realvec& getFileData(/*int fileIndex*/);

#ifdef LORIS_LIB   
   Loris::PartialList& getSdifFileData(/*int fileIndex*/);
   int openSdifFile(std::string filename);   
#endif
   
   //Marsyas::realvec& getPeakFileMesh(/*int fileIndex*/);
   
signals:

private:
	//std::vector<Marsyas::realvec> fileVec;
   //std::vector<Marsyas::realvec> fileMeshes;
   Marsyas::realvec fileVector;
   
#ifdef LORIS_LIB   
   Loris::PartialList filePartials;
#endif   
   //Marsyas::realvec fileMesh;
   
   //void computeMesh(Marsyas::realvec& rv, Marsyas::realvec& mesh);
   //Marsyas::mrs_real calcMinFreqDiff(Marsyas::realvec& rv);
   
   void readFromTextFrameFile( std::istream& is, Marsyas::realvec& vec );
   void readFromTextPartialsFile( std::istream& is, Marsyas::realvec& vec );   
   
   // Recursive quick sort functions
   void sortPeaksByPartialAndFrame(Marsyas::realvec& rv);   
   void sortPeaksByFrameAndFreq(Marsyas::realvec& rv);
   void sortPeaks(Marsyas::realvec& rv, int left, int right, Marsyas::peakView::pkParameter sortCriterion);
   int partition(Marsyas::realvec& rv, int left, int right, Marsyas::peakView::pkParameter sortCriterion);
   void swap(Marsyas::realvec& rv, int index1, int index2);
};

#endif
