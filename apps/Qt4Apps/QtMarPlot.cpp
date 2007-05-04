#include "QtMarPlot.h"

QtMarPlot::QtMarPlot(QWidget *parent)
	: QWidget(parent)
{
	data_ = NULL;
	plotName_ = "";
	minVal_ = -1.0;
	highVal_ = 1.0;
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
	update();
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
	if (data_->getCols()!=0) {
//		plot2d();
	} else if (data_->getSize() > 0) {
		plot1d();
	}
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
QtMarPlot::putBlob(int x, int y, QPainter painter)
{
	painter.drawPoint(x, y);
	painter.drawPoint(x, y-1);
	painter.drawPoint(x-1, y);
	painter.drawPoint(x-1, y-1);
}

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
	int size = data_->getSize();
	float hScale = width() / float(size);
	float vScale = height() / 1.0; // maximum scaled pitch/median

	// iterates over the data_
	for (i=0; i<size; i++) {
		x = i * hScale;
		y = (*data_)(i) * vScale;
		painter.drawPoint( x, height()/2 - y);

	}
}


