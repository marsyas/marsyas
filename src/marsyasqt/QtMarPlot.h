//  Graham Percival  <gperciva@uvic.ca>
#ifndef QTMARPLOT_H
#define QTMARPLOT_H

#include <QWidget>
#include <QPainter>

#include "MarSystemManager.h"
using namespace Marsyas;

namespace MarsyasQt
{
/**
	\brief A realvec plotting widget.
	\ingroup MarsyasQt

	Plots a realvec.  Is simple to use, but lacks many features of the
other plotting widget.

*/
class QtMarPlot : public QWidget
{
	Q_OBJECT
public:
	QtMarPlot(QWidget *parent = 0);
	~QtMarPlot();
	void setPlotName(QString plotName);
	void setBackgroundColor(QPalette color);
	void setPixelWidth(mrs_natural width);
	void setVertical(mrs_real minVal, mrs_real highVal); // scales data
	void setData(realvec* getData);
	void setCenterLine(bool drawit);

protected:
	void paintEvent(QPaintEvent *event);

private:
	realvec *data_;
	QString plotName_;
	mrs_real minVal_, highVal_;
	mrs_natural width_;
	bool drawCenter_;

	void plot1d();
	//void plot2d(); // not implemented yet
	void putBlob(int x, int y, QPainter painter); // for a 2x2 blob of pixels
};

} //namespace
#endif

