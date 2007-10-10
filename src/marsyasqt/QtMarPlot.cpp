#include "QtMarPlot.h"

namespace MarsyasQt
{
QtMarPlot::QtMarPlot(QWidget *parent)
		: QWidget(parent)
{
	data_ = NULL;
	otherData_ = NULL;
	plotName_ = "";
	minVal_ = -0.5;
	highVal_ = 0.5;
	width_ = 1;
	drawCenter_ = true;
	drawImpulses_ = false;
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
QtMarPlot::setOtherData(realvec *getData)
{
	otherData_ = getData;
	update();
}

void
QtMarPlot::setCenterLine(bool drawit)
{
	drawCenter_ = drawit;
}

void
QtMarPlot::setImpulses(bool drawit)
{
	drawImpulses_ = drawit;
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

void
QtMarPlot::plot1d()
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

} //namespace

