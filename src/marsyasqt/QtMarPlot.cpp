#include "QtMarPlot.h"

namespace MarsyasQt
{
QtMarPlot::QtMarPlot(QWidget *parent)
		: QWidget(parent)
{
//	data_ = NULL;
	plotName_ = "";
	minVal_ = -1;
	highVal_ = 1;
	pixelWidth_ = 1;
	startOffset_ = 0;
	endOffset_ = -1;
	drawCenter_ = false;
	drawImpulses_ = false;
	setAutoFillBackground(true);
	setBackgroundColor(QColor(255,255,255));
}

QtMarPlot::~QtMarPlot()
{
}

void
QtMarPlot::paintEvent(QPaintEvent *)
{
	if (data_.getSize()==0)
		return;

	QPainter painter(this);
	// plot name
	painter.drawText( 4, 14, plotName_);

	// dotted line
	QPen pen(Qt::SolidPattern, 1, Qt::DashLine);
	painter.setPen(pen);
	if (drawCenter_)
		painter.drawLine( 0, height()/2, width(), height()/2);

	pen.setWidth(pixelWidth_);
	pen.setStyle(Qt::SolidLine);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	painter.setPen(pen);

	mrs_natural i;
	mrs_natural x,y;
	mrs_real hScale = width() / (mrs_real) (endOffset_ - startOffset_);
	mrs_real vScale = height() / (highVal_ - minVal_); // maximum scaled pitch/median
	mrs_real vMean = (minVal_+highVal_)/2;
	mrs_natural midY = height()/2;

	// iterates over the data_
	for (i=0; i<data_.getSize(); i++)
	{
		x = mrs_natural (i * hScale);
		y = mrs_natural ( (data_(i)-vMean) * vScale );
		if ( (y>-midY) && (y<midY))
			painter.drawPoint( x, -y+midY);
		if (drawImpulses_)
		{
			for (y=y; y>-height()/2; y--)
				painter.drawPoint( x, -y+midY);
		}
	}
}

} //namespace

