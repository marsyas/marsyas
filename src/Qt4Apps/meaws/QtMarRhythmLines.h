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

    void setOtherData(realvec* getData)
    {
        otherData_ = getData;
        update();
    }

protected:
	void paintEvent(QPaintEvent *event);

	realvec *otherData_;
};

} //namespace
#endif

