#include <QPainter>
#include <QPen>

#include "displayControl.h"

#include <iostream>
using namespace std;

DisplayControl::DisplayControl(QWidget *parent)
	: QWidget(parent)
{
	data = NULL;
	setPalette(QPalette(QColor(255, 255, 255)));
	setAutoFillBackground(true);
}

DisplayControl::~DisplayControl()
{
	if (data != NULL)
		data->~realvec();
}

void
DisplayControl::setData(realvec *getData)
{
	data = getData;
	update();
}


void
DisplayControl::paintEvent(QPaintEvent *)
{
	if (data==NULL)
		return;
	QPainter painter(this);

	if (data->getCols()!=0) {
		//cout<<"drawing 2D matrix"<<endl;
		int i, j;
		int myi, myj;
		for (i=0; i<width(); i++) {
			for (j=0; j<height(); j++) {
				myi = i / ( width()/ (data->getRows()-1) );
				myj = j / ( height()/ (data->getCols()-1) );
				if ( (*data)(myi,myj) != 0)
					painter.drawPoint( i,j);
			}
		}
	} else if (data->getSize() > 0) {
		//cout<<"drawing 1D matrix  ";
		int i;
		int x,y;
		int prevx, prevy;
		int catchup;
		int size = data->getSize();
		float slope;
		int newy;
		float hScale = width() / float(size);
		float vScale = height() / 1.0; // maximum scaled pitch/median

		prevx=0;
		prevy=0;
		// iterates over the data
		for (i=0; i<size; i++) {
			x = i * hScale;
			y = (*data)(i) * vScale;
			// draw a 2x2 blob for each value
			painter.drawPoint( x, height()/2 - y);
			painter.drawPoint( x, height()/2 - y-1);
			painter.drawPoint( x-1, height()/2 - y);
			painter.drawPoint( x-1, height()/2 - y-1);
			// if we have skipped over some x values
			if (x > prevx+1) {
				slope = (y - prevy) / (x - prevx);
				for (catchup=prevx+1; catchup<x; catchup++) {
					newy = prevy + (catchup-prevx)*slope;
					// draw blobs in the middle
					painter.drawPoint( catchup, height()/2 - newy);
					painter.drawPoint( catchup, height()/2 - newy-1);
					painter.drawPoint( catchup-1, height()/2 - newy);
					painter.drawPoint( catchup-1, height()/2 - newy-1);
				}
			}
			prevx = x;
			prevy = y;
		}

		// dotted line
		QPen pen(Qt::SolidPattern, 1, Qt::DashLine);
		painter.setPen(pen);
		painter.drawLine( 0, height()/2, width(), height()/2);
	}
}


