
/**
   \class PeakFileCabinet
	\brief Contains information/data for a number of Peak files.
 */

#include "PeakFileCabinet.h"
#include "peakView.h"
#include "realvec.h"

#ifdef LORIS_LIB
#include "SdifFile.h"
#endif

using namespace std;
using namespace Marsyas;

#ifdef LORIS_LIB
using namespace Loris;
#endif

PeakFileCabinet::PeakFileCabinet()
{
}

PeakFileCabinet::~PeakFileCabinet()
{
}

int
PeakFileCabinet::openPeakFile(string filename)
{ 
   if( fileVector.read(filename) )
   {
      sortPeaksByFrameAndFreq(fileVector);
      return true;
   }
   return false;
}

#ifdef LORIS_LIB
int
PeakFileCabinet::openSdifFile(string filename)
{ 
   SdifFile f( filename );  
   filePartials.clear();
   filePartials.assign( f.partials().begin(), f.partials().end() );
      
   return true;
}
#endif

PeakFileCabinet::textFileType
PeakFileCabinet::openTextFile(string filename)
{
   ifstream ifs( filename.c_str() );
   string str0;
   PeakFileCabinet::textFileType fileType;
   
   if( ifs.is_open() )      
   {
      ifs >> str0;
      if( str0 == "par-text-frame-format")
      {
         fileType = TEXT_FRAME_FORMAT;
         readFromTextFrameFile( ifs, fileVector );
      }
      else if( str0 == "par-text-partials-format" )
      {
         fileType = TEXT_PARTIALS_FORMAT;
         readFromTextPartialsFile( ifs, fileVector );
      }      
   }
   else
      fileType = TEXT_FILE_OPEN_ERROR;
   
   return fileType;
}

void
PeakFileCabinet::readFromTextFrameFile( istream& is, realvec& vec )
{
   string str0, str1, str2, str3;
   int cols = 11;
   int nbFrames, nbPeaks, nbPartials, curPeakIndex, partialIndex;
   double time, freq, amp;
   double minTime, maxTime;
   
   vec.create( 1 , cols );   
   
   is >> str0 >> str1 >> str2 >> str3;
   if((str0 != "point-type") || (str1 != "index") || (str2 != "frequency") || (str3 != "amplitude"))
   {
      cerr << "Problem reading from TEXT FRAME FILE" << endl;
      cerr << "-str0 = " << str0 << endl;
      cerr << "-str1 = " << str1 << endl;
      cerr << "-str2 = " << str2 << endl;  
      cerr << "-str3 = " << str3 << endl; 
      return;
   }
   is >> str0 >> nbPartials; 
   is >> str0 >> nbFrames;
   is >> str0;   
   
   if( str0 != "frame-data" )
   {
      cerr << "Problem reading from TEXT FRAME FILE" << endl;
      cerr << "-str0 = " << str0 << endl;    
      return;
   }   
   
   //vec( 0 , 4 ) = nbFrames;
   vec( 0 , peakView::pkBin ) = nbPartials;
   
   curPeakIndex = 1;
   minTime = MAXREAL;
   maxTime = 0.;
   
   for( int i=0 ; i<nbFrames ; i++ )
   {
      is >> time >> nbPeaks;
      vec.stretch( curPeakIndex + nbPeaks, cols ); // costly, but total num of peaks not stored in file!
      
      if( time < minTime )
         minTime = time;
      if( time > maxTime )
         maxTime = time;
      
      for( int j=0 ; j<nbPeaks ; j++ )
      {
         is >> partialIndex >> freq >> amp;
         
         // Currently stores real-valued time.
         // Perhaps should store frame index (i) as from peak files? no.
         vec( curPeakIndex + j , peakView::pkFrame ) = time; // i;
         vec( curPeakIndex + j , peakView::pkFrequency ) = freq;
         vec( curPeakIndex + j , peakView::pkAmplitude ) = amp;
         // Store the index of the partial this peak belongs to in 'pkBin' for now.
         vec( curPeakIndex + j , peakView::pkBin ) = partialIndex;  
      }
      
      curPeakIndex += nbPeaks;
   }
   
   // Store real-valued time interval   
   vec( 0 , 4 ) = maxTime - minTime;
   sortPeaksByPartialAndFrame( vec );
}

// In this case, we don't know how many frames there are.
// We only know the time values/ranges in sec, so we store these instead
//
// WARNING : This format is slowest to read, since a MUCH larger number 
// of realvec stretches must be done than for the text frame format.
void
PeakFileCabinet::readFromTextPartialsFile( istream& is, realvec& vec )
{
   string str0, str1, str2, str3;
   int cols = 11;
   int nbPeaks, nbPartials, curPeakIndex, partialIndex;
   double time, freq, amp, minTime, maxTime, curMinTime, curMaxTime;
   
   vec.create( 1 , cols );   
   
   is >> str0 >> str1 >> str2 >> str3;
   if((str0 != "point-type") || (str1 != "time") || (str2 != "frequency") || (str3 != "amplitude"))
   {
      cerr << "Problem reading from TEXT PARTIALS FILE" << endl;
      cerr << "-str0 = " << str0 << endl;
      cerr << "-str1 = " << str1 << endl;
      cerr << "-str2 = " << str2 << endl;  
      cerr << "-str3 = " << str3 << endl; 
      return;
   }
   is >> str0 >> nbPartials;
   is >> str0;   
   
   if( str0 != "partials-data" )
   {
      cerr << "Problem reading from TEXT PARTIALS FILE" << endl;
      cerr << "-str0 = " << str0 << endl;    
      return;
   }   
   

   //vec( 0 , 4 ) = nbFrames;
   vec( 0 , peakView::pkBin ) = nbPartials;
   
   curPeakIndex = 1;
   minTime = MAXREAL;
   maxTime = 0.;
   
   for( int i=0 ; i<nbPartials ; i++ )
   {
      is >> partialIndex >> nbPeaks >> curMinTime >> curMaxTime;
      vec.stretch( curPeakIndex + nbPeaks, cols ); // costly, but total num of peaks not stored in file!
      
      if( curMinTime < minTime )
         minTime = curMinTime;
      if( curMaxTime > maxTime )
         maxTime = curMaxTime;
      
      for( int j=0 ; j<nbPeaks ; j++ )
      {
         is >> time >> freq >> amp;
         
         // Currently stores real-valued time.
         // Perhaps should store frame index (i) as from peak files? no.
         vec( curPeakIndex + j , peakView::pkFrame ) = time; // i;
         vec( curPeakIndex + j , peakView::pkFrequency ) = freq;
         vec( curPeakIndex + j , peakView::pkAmplitude ) = amp;
         // Store the index of the partial this peak belongs to in 'pkBin' for now.
         vec( curPeakIndex + j , peakView::pkBin ) = partialIndex;  
      }
      
      curPeakIndex += nbPeaks;
   }
   
   // Store real-valued time interval
   vec( 0 , 4 ) = maxTime - minTime;
   
}
   
//	std::cout << "found " << f.partials().size() << " partials.\n";
//	
//	int k = 0;
//	PartialList::iterator it;
//	std::cout << "start times:\n";
//	for ( it = f.partials().begin(); it != f.partials().end(); ++it )
//	{
//		std::cout << it->startTime() << "\n";
//		//SAME_PARAM_VALUES( it->startTime(), times[0] + (k*0.1) );
//		++k;
//	}
   
//	cout << "There are " << f.markers().size() << " markers." << endl;
//	for ( SdifFile::markers_type::iterator it = f.markers().begin(); 
//         it != f.markers().end(); 
//         ++it )
//	{
//		Marker & m = *it;
//		std::cout << m.name() << " at time " << m.time() << "\n";
//	}


realvec&
PeakFileCabinet::getFileData(/*int fileIndex*/)
{
   return fileVector; //fileVec[fileIndex];
}

#ifdef LORIS_LIB
PartialList&
PeakFileCabinet::getSdifFileData(/*int fileIndex*/)
{
   return filePartials; //fileVec[fileIndex];
}
#endif

void
PeakFileCabinet::sortPeaksByPartialAndFrame(realvec& rv)
{
   int rows = rv.getRows();
   
   if( rows <= 1 )
      return;
   
   // 'pkBin' actually houses partial index
   sortPeaks(rv, 1, rows-1, peakView::pkBin);
   
   // Start (quick)sorting at index 1 to ignore header row
   int first = 1, second = 1;
   
   for( ; second < rows ; second++ )
   {
      if( rv(first,(mrs_natural)peakView::pkBin) != rv(second,(mrs_natural)peakView::pkBin) )
      {
         sortPeaks(rv, first, second-1, peakView::pkFrame);
         first = second;
      }
   }      
   sortPeaks(rv, first, rows-1, peakView::pkFrame);   
}

void
PeakFileCabinet::sortPeaksByFrameAndFreq(realvec& rv)
{
   int rows = rv.getRows();
   
   if( rows <= 1 )
      return;
   
   // Start (quick)sorting at index 1 to ignore header row
   int first = 1, second = 1;
   
   for( ; second < rows ; second++ )
   {
      if( rv(first,(mrs_natural)peakView::pkFrame) != rv(second,(mrs_natural)peakView::pkFrame) )
      {
         sortPeaks(rv, first, second-1, peakView::pkFrequency);
         first = second;
      }
   }      
   sortPeaks(rv, first, rows-1, peakView::pkFrequency);
}

void
PeakFileCabinet::sortPeaks(realvec& rv, int left, int right, peakView::pkParameter sortCriterion)
{
   if( left < right )
   {
      int part = partition(rv, left, right, sortCriterion);
      sortPeaks(rv, left, part-1, sortCriterion);
      sortPeaks(rv, part+1, right, sortCriterion);
   }
}

int
PeakFileCabinet::partition(realvec& rv, int left, int right, peakView::pkParameter sortCriterion)
{
   // Not quite fair, but good enough random partitioning
   int pivot_i = rand()%(right-left+1) + left;
   // Place pivot val at the end of the series
   swap(rv, pivot_i, right);   
   mrs_real pivot_val = rv(right,(mrs_natural)sortCriterion);
   
   int i = left-1;   
   for( int j=left ; j<right ; j++ )
   {
      if( rv(j,(mrs_natural)sortCriterion) <= pivot_val )
      {
         i++;
         swap(rv, i, j);
      }
   }
   // re-insert pivot val
   swap(rv, i+1, right);
   
   return i+1;
}

void
PeakFileCabinet::swap(realvec& rv, int index1, int index2)
{
   int cols = rv.getCols();
   mrs_real tmp;
   
   for( int i=0 ; i<cols ; i++ )
   {
      tmp = rv(index1,i);
      rv(index1,i) = rv(index2,i);
      rv(index2,i) = tmp;
   }
}

//realvec&
//PeakFileCabinet::getPeakFileMesh(/*int fileIndex*/)
//{
//   return fileMesh; //fileMeshes[fileIndex];
//}

//mrs_real
//PeakFileCabinet::calcMinFreqDiff(realvec& rv)
//{
//   int rows = rv.getRows();
//   mrs_real minFreqDiff = MAXREAL;
//   
//   for( int i=1 ; i < rows-1 ; i++ )
//   {
//      if( rv(i,(mrs_natural)peakView::pkFrame) != rv(i+1,(mrs_natural)peakView::pkFrame) )
//         continue;
//
//      if( rv(i+1,(mrs_natural)peakView::pkFrequency) - rv(i,(mrs_natural)peakView::pkFrequency) < minFreqDiff )
//         minFreqDiff = rv(i+1,(mrs_natural)peakView::pkFrequency) - rv(i,(mrs_natural)peakView::pkFrequency);      
//   }
//   
//   return minFreqDiff;
//}


//void
//PeakFileCabinet::computeMesh(realvec& rv, realvec& mesh)
//{
//   sortPeaksByFrameAndFreq(rv);
//   int nbFrames = rv(0,4);
//   int firstFrame = rv(1,peakView::pkFrame);
//   
//   int fs = rv(0,1);
//   int minFreqDiff = (int)calcMinFreqDiff(rv);
//   int nbFreqBins = (int) (( fs/2. ) / ( minFreqDiff/2. ));
//   int binResolution = (int)((fs/2.) / nbFreqBins);
//   
//   mesh.create(nbFreqBins,nbFrames);
//   //double meshArray [nbFreqBins][nbFrames];
//   //double **meshArrayPtr = new double*[nbFreqBins];
//   //for( int i=0 ; i<nbFreqBins ; i++ )
//      //meshArrayPtr[i] = new double [nbFrames];
//   
//   for( int i=1 ; i<rv.getRows() ; i++ )
//   {
//      mesh( (int)(rv(i,peakView::pkFrequency)/binResolution) , rv(i,peakView::pkFrame)-firstFrame ) = rv(i,peakView::pkAmplitude);      
//      //meshArrayPtr[ (int)(rv(i,peakView::pkFrequency)/binResolution) ] [ (int)rv(i,peakView::pkFrame)-firstFrame ] = rv(i,peakView::pkAmplitude);
//      
//   }   
//   
//}

   
//int
//PeakFileCabinet::openPeakFile(string filename)
//{ 
//   if( fileVector.read(filename) )
//   {
//      computeMesh(fileVector, fileMesh);
//      return true;
//   }
//   return false;
//   
////   realvec rv, mesh;
////   //peakView peakFileView(rv);
////
////   if( rv.read(filename) )
////   {
////      //cout << "realvec BEFORE sort: " << endl << rv << endl;
////      fileVec.push_back(rv);
////      computeMesh(rv, mesh);
////      fileMeshes.push_back(mesh);
////      return true;
////   }
////      
////   return false;
//}