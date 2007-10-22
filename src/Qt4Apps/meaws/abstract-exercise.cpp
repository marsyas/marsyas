//#include <iostream>
//using namespace std;

#include "abstract-exercise.h"

Exercise::Exercise()
{
	instructionLayout_ = NULL;
	instructionImageLabel_ = NULL;
	resultLayout_ = NULL;
	current_ = -1;
	tries_ = new QList<Try *>;
}

Exercise::~Exercise()
{
	if (instructionLayout_ != NULL)
	{
		if (instructionImageLabel_ != NULL)
		{
			instructionLayout_->removeWidget(instructionImageLabel_);
			delete instructionImageLabel_;
			instructionImageLabel_ = NULL;
		}
		delete instructionLayout_;
	}
	if (resultLayout_ != NULL)
		delete resultLayout_;
	while (!tries_->isEmpty())
		delete tries_->takeFirst();
	delete tries_;
}

void Exercise::open(QString exerciseFilename)
{
	instructionImageLabel_ = new QLabel;
	QPixmap *instructPixmap = new QPixmap(exerciseFilename);
	instructionImageLabel_->setPixmap(*instructPixmap);
	instructionImageLabel_->setScaledContents(false);
	instructionImageLabel_->setMaximumHeight(instructPixmap->height());
	instructionImageLabel_->setMinimumHeight(instructPixmap->height());
	instructionLayout_ = new QVBoxLayout;
	instructionLayout_->addWidget(instructionImageLabel_);
	height_ = instructPixmap->height()+4;
}

void Exercise::setupDisplay(QFrame* instructionArea, QFrame* resultArea)
{
	instructionArea->setLayout(instructionLayout_);
	instructionLayout_->setContentsMargins(2,2,2,2);

	// TODO: somehow figure out height_.  :(
//	const QLabel* instructionLabel = (QLabel*) instructionLayout_->itemAt(0);
	//const int height_ = instructionLabel->height_();
	//cout<<instructionLabel->height_()<<endl;
	//cout<<height_->pixmap()->height_()<<endl;
	//const QPixmap* height_ = ( (QLabel*) instructionLayout_->itemAt(0) )->pixmap();
	//const QPixmap *instructPixmap = ( (QLabel*) instructionLayout_->itemAt(0) )->pixmap();
	//cout<<instructPixmap->height_()<<endl;

	instructionArea->setMaximumHeight(height_);
	instructionArea->setMinimumHeight(height_);

	resultLayout_ = new QVBoxLayout;
	resultLayout_->setContentsMargins(2,2,2,2);
	resultArea->setLayout(resultLayout_);

	// FIXME: temp
	resultLayout_->setContentsMargins(40,2,2,2);
	resultArea->setMaximumWidth(628+2+2);
}

void Exercise::addTryAbstract(Try* newTry)
{
	resultLayout_->addWidget( newTry->getDisplay() );
	newTry->setTryNumber( tries_->count() );
	current_ = tries_->count();
	connect(newTry, SIGNAL(selectTry(mrs_natural)),
	        this, SLOT(selectTry(mrs_natural)));
	tries_->append(newTry);
}

void Exercise::delTryAbstract()
{
	Try* oldTry = tries_->takeAt(current_);
	resultLayout_->removeWidget( oldTry->getDisplay() );
	delete oldTry;
	resultLayout_->activate();

	// renumber the remaining exercises
	for (int i=current_; i<tries_->count(); i++)
	{
		(*tries_)[i]->setTryNumber(i);
	}
	current_ = -1;
}

void Exercise::selectTry(mrs_natural selected)
{
	current_ = selected;
	emit analysisDone();
}


