//#include <iostream>
//using namespace std;

#include "abstract-try.h"

Try::Try()
{
	tryArea = new QClickFrame();
	connect(tryArea, SIGNAL(clicked()), this, SLOT(clicked()));
	connect(tryArea, SIGNAL(doubleclicked()), this, SLOT(doubleclicked()));
	tryLayout = NULL;
	tryNumber_ = 0;
}

void Try::clicked()
{
//	cout<<"clicked "<<tryNumber_<<endl;
	emit tryNumber(tryNumber_);
}

void Try::doubleclicked()
{
//	cout<<"double clicked "<<tryNumber_<<endl;
//	emit tryNumber(tryNumber_);
}

