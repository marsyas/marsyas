#include "QtMarPlot.h"
//#include <iostream>
//using namespace std;

QtMarPlot::QtMarPlot(QWidget *parent)
	: QWidget(parent)
{
	data_ = NULL;
	plotName_ = "";
	minVal_ = -0.5;
	highVal_ = 0.5;
	width_ = 1.0;
	setAutoFillBackground(true);
}

QtMarPlot::~QtMarPlot()
{
	if (data_ != NULL)
		data_->~realvec();
}

void
QtMarPlot::setData(realvec *getData)
{
	data_ = getData;
//	cout<<*data_;
	update();
}

void
QtMarPlot::setVertical(mrs_real minVal, mrs_real highVal)
{
	minVal_ = minVal;
	highVal_ = highVal;
}

void
QtMarPlot::setPlotName(QString plotName)
{
	plotName_ = plotName;
}

void
QtMarPlot::setBackgroundColor(QPalette color)
{
	setPalette(color);
}

void
QtMarPlot::setPixelWidth(mrs_natural width)
{
	width_ = width;
}

void
QtMarPlot::paintEvent(QPaintEvent *)
{
	if (data_==NULL)
		return;
	plot1d();
}

/*
void
QtMarPlot::plot2d()
{
	QPainter painter(this);
	//cout<<"drawing 2D matrix"<<endl;
	int i, j;
	int myi, myj;
	for (i=0; i<width(); i++) {
		for (j=0; j<height(); j++) {
			myi = i / ( width()/ (data_->getRows()-1) );
			myj = j / ( height()/ (data_->getCols()-1) );
			if ( (*data_)(myi,myj) != 0)
				painter.drawPoint( i,j);
		}
	}
}
*/

void
QtMarPlot::plot1d()
{
	QPainter painter(this);
	// plot name
	painter.drawText( 4, 14, plotName_);

	// dotted line
	QPen pen(Qt::SolidPattern, 1, Qt::DashLine);
	painter.setPen(pen);
	painter.drawLine( 0, height()/2, width(), height()/2);

	pen.setWidth(width_);
	pen.setStyle(Qt::SolidLine);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	painter.setPen(pen);

	int i;
	int x,y;
	float hScale = width() / float(data_->getSize());
	float vScale = height() / (highVal_ - minVal_); // maximum scaled pitch/median
	float vMean = (minVal_+highVal_)/2;
	int midY = height()/2;

	// iterates over the data_
	for (i=0; i<data_->getSize(); i++) {
		x = i * hScale;
		y = ((*data_)(i)-vMean) * vScale;
		if ( (y>-midY) && (y<midY))
			painter.drawPoint( x, -y+midY);

	}
}


