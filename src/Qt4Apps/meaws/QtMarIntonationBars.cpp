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
	mrs_natural x,y;
	float hScale = width() / float(data_->getSize());
	float vScale = height() / (highVal_ - minVal_); // maximum scaled pitch/median
	float vMean = (minVal_+highVal_)/2;
	mrs_natural midY = height()/2;

	if (drawBars_)
	{
		if (otherData_ == NULL)
		{
			MRSERR("QtMarIntonationBars doesn't have other data to plot");
			return;
		}

		pen.setColor(QColor(255,0,0));
		painter.setPen(pen);
		painter.drawRect(20,20,100,100);

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

	}
	else
	{
		// iterates over the data_
		for (i=0; i<data_->getSize(); i++)
		{
			x = mrs_natural (i * hScale);
			y = mrs_natural ( ((*data_)(i)-vMean) * vScale );
			if ( (y>-midY) && (y<midY))
				painter.drawPoint( x, -y+midY);
			if (drawImpulses_)
			{
				for (y=y; y>-height()/2; y--)
					painter.drawPoint( x, -y+midY);
			}
		}
		if (otherData_ != NULL)
		{
			pen.setColor(QColor(255,0,0));
			painter.setPen(pen);
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
		}
	}
}

} //namespace

