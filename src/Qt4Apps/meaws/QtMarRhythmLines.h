//  Graham Percival  <gperciva@uvic.ca>
#ifndef QTMAR_RHYTHM_LINES_H
#define QTMAR_RHYTHM_LINES_H

#include "QtMarPlot.h"

//#include "MarSystemManager.h"
//using namespace Marsyas;

namespace MarsyasQt
{
/**
	\brief A simple realvec plotting widget.
	\ingroup MarsyasQt

	Plots a realvec.  Is simple to use, but lacks many features of the
other plotting widget.

*/
class QtMarRhythmLines : public QtMarPlot
{
	Q_OBJECT
public:
	QtMarRhythmLines(QWidget *parent = 0);
	~QtMarRhythmLines();

    void setExpectedLines(realvec expectedLines)
    {
        expectedLines_ = expectedLines;
        update();
    }

	void setExpectedOffset(mrs_natural offset)
	{
		expectedOffset_ = offset;
	}

	void setDetectedOffset(mrs_natural offset)
	{
		detectedOffset_ = offset;
	}


protected:
	void paintEvent(QPaintEvent *event);

	realvec expectedLines_;
	mrs_natural expectedOffset_;
	mrs_natural detectedOffset_;
};

} //namespace
#endif

