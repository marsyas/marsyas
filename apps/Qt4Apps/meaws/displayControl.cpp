#include <QPainter>

#include "displayControl.h"

#include <iostream>
using namespace std;

DisplayControl::DisplayControl(QWidget *parent)
	: QWidget(parent)
{
	data = NULL;
//	setPalette(QPalette(QColor(255, 0, 0)));
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

	// scale data for sax display
	if (data->getRows()==0) {
		mrs_real median = data->median();
		//cout<<"median: "<<median<<endl;
		for (int i=0; i<data->getSize(); i++) {
			(*data)(i) = (*data)(i) - median;
//			cout<<(*data)(i)<<endl;
		}
	}
	update();
}

void
DisplayControl::makeupData()
{
	data = new realvec();
	data->create(10,10);
//	cout<<*data;
	int i, j;
	for (i=0; i<data->getRows(); i++)
		for (j=0; j<data->getCols(); j++)
			(*data)(i,4)=2;
//	cout<<*data;
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
		float hScale = width() / float(size);
		float vScale = height() / 1.0; // maximum scaled pitch/median
		//cout<<hScale<<"   "<<vScale<<endl;
		painter.drawLine( 0, height()/2, width(), height()/2);
		for (i=0; i<size; i++) {
			x = i * hScale;
			y = (*data)(i) * vScale;
			painter.drawPoint( x, height()/2 - y);
			if (x > prevx+1) {
				for (catchup=prevx; catchup<x; catchup++) {
					painter.drawPoint( catchup, height()/2 - prevy);
				}
			}
			prevx = x;
			prevy = y;
		}
	}
}


