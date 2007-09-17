//#include <iostream>
//using namespace std;

#include "abstract-try.h"

Try::Try()
{
	tryArea = new QFrame();
	tryLayout = NULL;
	reportNumber_ = 0;
}

void Try::clicked()
{
	emit reportNumber(reportNumber_);
}


