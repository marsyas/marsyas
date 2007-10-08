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


