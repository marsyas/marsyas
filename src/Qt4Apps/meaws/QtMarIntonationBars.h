//  Graham Percival  <gperciva@uvic.ca>
#ifndef QTMARINTONATIONBARS_H
#define QTMARINTONATIONBARS_H

#include "QtMarPlot.h"

#include "MarSystemManager.h"
using namespace Marsyas;

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
	void setBarData(realvec *getData); // hack

protected:
	void paintEvent(QPaintEvent *event);

private:
	realvec *data_;
	bool drawBars_;
	void plot1d();
/*
	realvec *otherData_;
	QString plotName_;
	mrs_real minVal_, highVal_;
	mrs_natural width_;
	bool drawCenter_;
	bool drawImpulses_;
	bool drawBars_;

	//void plot2d(); // not implemented yet
	void putBlob(int x, int y, QPainter painter); // for a 2x2 blob of pixels
//	void plotBars();
*/
};

} //namespace
#endif

