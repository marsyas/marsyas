#include "QtMarIntonationBars.h"
#include <iostream>
using namespace std;


namespace MarsyasQt
{

QtMarIntonationBars::QtMarIntonationBars(QWidget *parent)
		: QtMarPlot(parent)
{
	data_ = NULL;
}

QtMarIntonationBars::~QtMarIntonationBars()
{
}

void
QtMarIntonationBars::setBarData(realvec *getData)
{
	data_ = getData;
	update();
}

void
QtMarIntonationBars::paintEvent(QPaintEvent *)
{
	if (data_==NULL)
		return;
	plot1d();
}

void
QtMarIntonationBars::plot1d()
{
	if (data_ == NULL)
		return;

	QPainter painter(this);
	// plot name
	painter.drawText( 4, 14, plotName_);

	// dotted line
	QPen pen(Qt::SolidPattern, 1, Qt::DashLine);
	painter.setPen(pen);
	if (drawCenter_)
		painter.drawLine( 0, height()/2, width(), height()/2);

	pen.setWidth(width_);
	pen.setStyle(Qt::SolidLine);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	painter.setPen(pen);

	mrs_natural i;
	highVal_ = 0.03;
	minVal_ = 0;
	mrs_real hScale = width() / (*data_)(data_->getRows()-1,1);
	mrs_real vScale = height() / (highVal_ - minVal_); // maximum scaled pitch/median
	float vMean = (minVal_+highVal_)/2;
	mrs_natural midY = height()/2;

	painter.setPen(Qt::NoPen);

	mrs_natural colorR, colorG, colorB;
	mrs_natural start, end;
	end = 0;
	mrs_natural y;
//	cout<<endl<<endl;
//	cout<<(*data_);
//	cout<<hScale<<" "<<vScale<<endl;
	mrs_natural errorDirection;
	mrs_real errorMagnitude;
	for (i=0; i<data_->getRows(); i++) {
		start = end;
		end = (mrs_natural) ( (*data_)(i,1) * hScale );
		errorDirection = (mrs_natural) (*data_)(i,2);
		errorMagnitude = (*data_)(i,0);

		y = errorMagnitude*vScale/2.0;

		colorR = 0;
		colorG = 0;
		colorB = 0;
		errorMagnitude *= 20;
		if (errorDirection == 1)
			colorR = errorMagnitude*255;
		if (errorDirection == 0)
			colorG = errorMagnitude*255;
		if (errorDirection == -1)
			colorB = errorMagnitude*255;
			
//		cout<<i<<" "<<start<<" "<<end<<" "<<y<<endl;
cout<<colorR<<colorG<<colorB<<endl;
		painter.setBrush(QColor(colorR,colorG,colorB));
		if ( errorDirection == 0) {
			painter.drawRect(start,midY-y,end-start,y*2);
		} else {
			y = -y*errorDirection;
			painter.drawRect(start,midY,end-start,y);
		}


	}
/*
	// iterates over the otherData_
	for (i=0; i<otherData_->getSize(); i++)
	{
		x = mrs_natural (i * hScale);
		y = mrs_natural ( ((*otherData_)(i)-vMean) * vScale );
		if ( (y>-midY) && (y<midY))
			painter.drawPoint( x, -y+midY);
		if (drawImpulses_)
		{
			for (y=y; y>-height()/2; y--)
				painter.drawPoint( x, -y+midY);
		}
	}
*/
}

} //namespace

