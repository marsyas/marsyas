//#include <iostream>
//using namespace std;

#include "abstract-try.h"

Try::Try()
{
	tryArea = new QFrame();
	tryLayout = NULL;
	tryNumber_ = 0;
}

void Try::clicked()
{
	emit tryNumber(tryNumber_);
}


