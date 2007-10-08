//#include <iostream>
//using namespace std;

#include "abstract-exercise.h"

Exercise::Exercise()
{
	instructionLayout = NULL;
	instructionImageLabel = NULL;
	resultLayout = NULL;
	current_ = -1;
	tries = new QList<Try *>;
}

Exercise::~Exercise()
{
	if (instructionLayout != NULL)
	{
		if (instructionImageLabel != NULL) {
			instructionLayout->removeWidget(instructionImageLabel);
			delete instructionImageLabel;
			instructionImageLabel = NULL;
		}
		delete instructionLayout;
	}
	if (resultLayout != NULL)
		delete resultLayout;
	while (!tries->isEmpty())
		delete tries->takeFirst();
	delete tries;
}

void Exercise::open(QString exerciseFilename) {
	instructionImageLabel = new QLabel;
	QPixmap *instructPixmap = new QPixmap(exerciseFilename);
	instructionImageLabel->setPixmap(*instructPixmap);
	instructionImageLabel->setScaledContents(false);
	instructionImageLabel->setMaximumHeight(instructPixmap->height());
	instructionImageLabel->setMinimumHeight(instructPixmap->height());
	instructionLayout = new QVBoxLayout;
	instructionLayout->addWidget(instructionImageLabel);
	height = instructPixmap->height()+4;
}

void Exercise::setupDisplay(QFrame* instructionArea, QFrame* resultArea) {
	instructionArea->setLayout(instructionLayout);
	instructionLayout->setContentsMargins(2,2,2,2);

			// TODO: somehow figure out height.  :(
//	const QLabel* instructionLabel = (QLabel*) instructionLayout->itemAt(0);
	//const int height = instructionLabel->height();
	//cout<<instructionLabel->height()<<endl;
	//cout<<height->pixmap()->height()<<endl;
	//const QPixmap* height = ( (QLabel*) instructionLayout->itemAt(0) )->pixmap();
	//const QPixmap *instructPixmap = ( (QLabel*) instructionLayout->itemAt(0) )->pixmap();
	//cout<<instructPixmap->height()<<endl;

	instructionArea->setMaximumHeight(height);
	instructionArea->setMinimumHeight(height);

	resultLayout = new QVBoxLayout;
	resultLayout->setContentsMargins(2,2,2,2);
	resultArea->setLayout(resultLayout);
}

void Exercise::addTryAbstract(Try* newTry) {
    resultLayout->addWidget( newTry->getDisplay() );
    newTry->setTryNumber( tries->count() );
	current_ = tries->count();
    connect(newTry, SIGNAL(selectTry(mrs_natural)),
        this, SLOT(selectTry(mrs_natural)));
    tries->append(newTry);
}

void Exercise::delTryAbstract() {
	Try* oldTry = tries->takeAt(current_);
	resultLayout->removeWidget( oldTry->getDisplay() );
	delete oldTry;
	resultLayout->activate();

	// renumber the remaining exercises
	for (int i=current_; i<tries->count(); i++) {
		(*tries)[i]->setTryNumber(i);
	}
	current_ = -1;
}

void Exercise::selectTry(mrs_natural selected)
{
    current_ = selected;
    emit analysisDone();
}


