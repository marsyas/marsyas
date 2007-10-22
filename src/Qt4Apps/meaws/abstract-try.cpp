#include "abstract-try.h"

Try::Try()
{
	tryArea_ = new QClickFrame();
	connect(tryArea_, SIGNAL(clicked()), this, SLOT(clicked()));
	connect(tryArea_, SIGNAL(doubleclicked()), this, SLOT(doubleclicked()));
	tryLayout_ = NULL;
	tryNumber_ = 0;
}

Try::~Try()
{
	if (tryLayout_ != NULL)
		delete tryLayout_;
	delete tryArea_;
}

