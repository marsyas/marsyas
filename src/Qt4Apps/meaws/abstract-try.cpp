#include "abstract-try.h"

Try::Try()
{
	tryArea_ = new QClickFrame();
	connect(tryArea_, SIGNAL(clicked()), this, SLOT(clicked()));
	connect(tryArea_, SIGNAL(doubleclicked()), this, SLOT(doubleclicked()));
	tryLayout_ = NULL;
	tryNumber_ = 0;
	hasAudio_ = false;

	file_ = new QTemporaryFile("meaws-XXXXXXXX.wav");
	file_->open();
//	file_->setAutoRemove(true);
	filename_ = qPrintable(file_->fileName());
	cout<<"temp file is: "<<filename_<<endl;
}

Try::~Try()
{
//	cout<<"removing "<<endl;
//	cout<<file_->remove()<<endl;;
	delete file_;
	//file_->close();
	if (tryLayout_ != NULL)
		delete tryLayout_;
	delete tryArea_;
}

