#include <QPainter>

#include "displayControl.h"

#include <iostream>
using namespace std;

DisplayControl::DisplayControl(QWidget *parent)
	: QWidget(parent)
{
	data = NULL;
	setPalette(QPalette(QColor(255, 0, 0)));
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
		cout<<"drawing 2D matrix"<<endl;
		int i, j;
		int myi, myj;
		for (i=0; i<width(); i++) {
			for (j=0; j<height(); j++) {
				myi = i / ( width()/ (data->getRows()-1) );
				myj = j / ( height()/ (data->getCols()-1) );
				//cout<<myi<<" "<<myj<<"   ";
				if ( (*data)(myi,myj) != 0)
					painter.drawPoint( i,j);
				//cout<<myi<<" "<<myj<<endl;
				//cout<<(*data)(i,j)<<endl;
			}
			//cout<<endl;
		}
	} else if (data->getSize() > 0) {
		cout<<"drawing 1D matrix  ";
		cout<<"height: "<<height()<<"   width: "<<width()<<endl;
		int i;
		int x,y;
		int size = data->getSize();
		float hScale = width() / float(size);
		float vScale = height() / 90.0; // maximum MIDI pitch
		cout<<hScale<<"   "<<vScale<<endl;
		for (i=0; i<size; i++) {
			x = i * hScale;
			y = (*data)(i) * vScale;
			if ( y > 0) {
				painter.drawPoint( x, height() - y);
//				cout<<x<<" "<<y<<endl;
			}
		}
	}
}


