//  Graham Percival  <gperciva@uvic.ca>
#ifndef QTMARINTONATIONBARS_H
#define QTMARINTONATIONBARS_H

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
class QtMarIntonationBars : public QtMarPlot
{
	Q_OBJECT
public:
	QtMarIntonationBars(QWidget *parent = 0);
	~QtMarIntonationBars();

protected:
	void paintEvent(QPaintEvent *event);

};

} //namespace
#endif

