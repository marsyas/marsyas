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

}


