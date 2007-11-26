#include "abstract-try.h"

Try::Try()
{
	tryArea_ = new QClickFrame();
	connect(tryArea_, SIGNAL(clicked()), this, SLOT(clicked()));
	connect(tryArea_, SIGNAL(doubleclicked()), this, SLOT(doubleclicked()));
	tryLayout_ = NULL;
	tryNumber_ = 0;
	file_ = NULL;
	reset();
}

Try::~Try()
{
	if (file_ != NULL)
	{
		delete file_;
		file_ = NULL;
	}
	if (tryLayout_ != NULL)
	{
		delete tryLayout_;
		tryLayout_ = NULL;
	}
	delete tryArea_;
}

void Try::reset()
{
	if (file_ != NULL)
	{
		delete file_;
		file_ = NULL;
	}
	file_ = new QTemporaryFile("meaws-XXXXXXXX.wav");
	file_->open();
	filename_ = qPrintable(file_->fileName());
	hasAudio_ = false;
	score_ = -1;
}

void Try::setFilename(mrs_string filename)
{
	if (file_ != NULL)
	{
		delete file_;
		file_ = NULL;
	}
	filename_ = filename;
};

