#include "MedianFilter.h"
#include <map>
#include <algorithm>

using std::ostringstream;
using std::map;
using std::multimap;
using std::less;
using std::pair;
using std::min;
using std::max;

using namespace Marsyas;

MedianFilter::MedianFilter(mrs_string inName)
  :MarSystem("MedianFilter",inName)
{
  addControls();
}

MedianFilter::MedianFilter(const MedianFilter& inToCopy)
  :MarSystem(inToCopy)
{
  ctrl_WindowSize_ = getctrl("mrs_natural/WindowSize");
  WindowSize_ = inToCopy.WindowSize_;
}

MedianFilter::~MedianFilter() {}

MarSystem* MedianFilter::clone() const
{
  return new MedianFilter(*this);
}

void MedianFilter::addControls()
{
  addctrl("mrs_natural/WindowSize",10,ctrl_WindowSize_);
  ctrl_WindowSize_->setState(true);
  WindowSize_ = 10;
}

void MedianFilter::myUpdate(MarControlPtr inSender)
{
  WindowSize_ = ctrl_WindowSize_->to<mrs_natural>();
  MarSystem::myUpdate(inSender);
}

void MedianFilter::myProcess(realvec& inVec, realvec& outVec)
{
  // Each element in the input vector is replaced by the median of
  // the elements that fall in a window surrounding the element, defined as:
  // element index + [0,1.. N-1] - floor(N/2) with N, the size of the window.

  // Define moving window: once inserted,
  // the elements in the moving window are automatically sorted
  mrs_natural N = WindowSize_;
  multimap<mrs_real,mrs_natural,less<mrs_real> > theWindow;
  typedef multimap<mrs_real,mrs_natural,less<mrs_real> >::iterator iter;
  typedef pair<mrs_real,mrs_natural> element;

  // M = floor(N/2)
  mrs_natural M = (mrs_natural)floor((mrs_real)N/2.);

  // Initialize the moving window
  // For i=0, the window = [v(0), v(0),.. v(0),v(1),.. v(N-M)]
  for (int p=-M; p<=0; p++)
    theWindow.insert(element(inVec(0),p));
  for (int p=1; p<=N-M-1; p++)
    theWindow.insert(element(inVec(p),p));

  mrs_natural I = inVec.getSize();
  for (int i=0; i<I; ++i)
  {
    // Define to-be-inserted element
    element theNewOne(inVec(min(i+N-M,I-1)),i+N-M);

    // Search for median, insert and delete position
    int theOffset = -M;

    iter theMedian, theToDelete, theToInsert = theWindow.begin();
    for (iter theIter=theWindow.begin(); theIter!=theWindow.end(); theIter++)
    {
      if (theOffset == 0)
        theMedian = theIter;
      if (theIter->second == i-M)
        // Index i-M = first inserted element (oldest)
        theToDelete = theIter;
      if (theNewOne.first > theIter->first)
      {
        // Update theToInsert as long as theIter
        // is smaller than the to-be-inserted element
        theToInsert = theIter;
        theToInsert++;
      }
      theOffset++;
    }

    if (theToInsert == theToDelete)
      theToInsert++;

    outVec(i) = theMedian->first;
    theWindow.erase(theToDelete);
    theWindow.insert(theToInsert,theNewOne);
  }
}
