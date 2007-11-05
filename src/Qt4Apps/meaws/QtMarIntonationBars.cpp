#include "QtMarIntonationBars.h"
#include <iostream>
using namespace std;

namespace MarsyasQt
{

QtMarIntonationBars::QtMarIntonationBars(QWidget *parent)
		: QtMarPlot(parent)
{
}

QtMarIntonationBars::~QtMarIntonationBars()
{
}

void
QtMarIntonationBars::paintEvent(QPaintEvent *)
{
	if (data_==NULL)
		return;

	QPainter painter(this);
	// plot name
	painter.drawText( 4, 14, plotName_);

	// constants
	mrs_real hScale = width() / (*data_)(data_->getRows()-1,1);
	mrs_real vScale = height() / (highVal_ - minVal_);
	mrs_natural midY = height()/2;

	// variables from data
	mrs_natural start, end;
	mrs_natural errorDirection;
	mrs_real errorMagnitude;

	// variables
	mrs_natural colorR, colorG, colorB;
	mrs_natural y;

	// fill in the bars
	painter.setPen(Qt::NoPen);
	end = 0;
	for (mrs_natural i=0; i<data_->getRows(); i++) {
		start = end;
		end = (mrs_natural) ( (*data_)(i,1) * hScale );
		errorDirection = (mrs_natural) (*data_)(i,2);
		errorMagnitude = (*data_)(i,0);

		y = (mrs_natural) (errorMagnitude*vScale/2.0);

		colorR = 0;
		colorG = 0;
		colorB = 0;
		// TODO: deal with this scaling.
		errorMagnitude *= 20;

		if (errorDirection == 1)
			colorR = (mrs_natural) (255-errorMagnitude*255);
		if (errorDirection == 0)
			colorG = (mrs_natural) (255-errorMagnitude*255);
		if (errorDirection == -1)
			colorB = (mrs_natural) (255-errorMagnitude*255);
		//cout<<colorR<<"\t"<<colorG<<"\t"<<colorB<<endl;
		painter.setBrush(QColor(colorR,colorG,colorB));

		if ( errorDirection == 0) {
			painter.drawRect(start,midY-y,end-start,y*2);
		} else {
			y = -y*errorDirection;
			painter.drawRect(start,midY,end-start,y);
		}
	}

	// dotted center line
	painter.setPen(QPen(Qt::SolidPattern, 1, Qt::DashLine));
	painter.drawLine( 0, height()/2, width(), height()/2);

	// draws the vertical lines
	painter.setPen(QColor(Qt::black));
	end = 0;
	for (mrs_natural i=0; i<data_->getRows(); i++) {
		start = end;
		end = (mrs_natural) ( (*data_)(i,1) * hScale );
		painter.drawLine(start, 0, start, height()-1);
	}

}

} //namespace

