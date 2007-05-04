//  Graham Percival  <gperciva@uvic.ca>
#ifndef QTMARPLOT_H
#define QTMARPLOT_H

#include <QWidget>
#include <QPainter>

#include "MarSystemManager.h"
using namespace Marsyas;

class QtMarPlot : public QWidget
{
	Q_OBJECT

public:
	QtMarPlot(QWidget *parent = 0);
	~QtMarPlot();
	void setPlotName(QString plotName);
	void setBackgroundColor(QPalette color);
	void setPixelWidth(mrs_natural width);
	void setVertical(mrs_real minVal, mrs_real highVal);

	/*
	 * you must do setData() --AFTER-- setVertical()
	 * also, the realvec* getData  will be modified at will.  Only
	 * pass in a temporary / unnecessary realvec.
	 */
	void setData(realvec* getData);

protected:
	void paintEvent(QPaintEvent *event);

private:
	realvec *data_;
	QString plotName_;
	mrs_real minVal_, highVal_;
	qreal width_;

	void plot1d();
	//void plot2d(); // not implemented yet
	void putBlob(int x, int y, QPainter painter); // for a 2x2 blob of pixels
};
#endif

