#include <iostream>
using namespace std;

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
}

void Exercise::open(QString exerciseFilename) {
	instructionImageLabel = new QLabel;
	QPixmap *instructPixmap = new QPixmap(exerciseFilename);
	instructionImageLabel->setPixmap(*instructPixmap);
	instructionImageLabel->setScaledContents(false);
	instructionImageLabel->setMaximumHeight(instructPixmap->height());
	instructionImageLabel->setMinimumHeight(instructPixmap->height());
	instructionLayout = new QVBoxLayout;
	//instructionLayout = new ResultList;
	instructionLayout->addWidget(instructionImageLabel);
	foo = instructPixmap->height();
}

void Exercise::setupDisplay(QFrame* instructionArea, QFrame* resultArea) {
	instructionArea->setLayout(instructionLayout);
	int ml, mt, mr, mb;
	instructionLayout->getContentsMargins(&ml, &mt, &mr, &mb);
	mt = 0;
	mb = 0;
	instructionLayout->setContentsMargins(ml, mt, mr, mb);

			// TODO: somehow figure out height.  :(
//	const QLabel* instructionLabel = (QLabel*) instructionLayout->itemAt(0);
	//const int height = instructionLabel->height();
	//cout<<instructionLabel->height()<<endl;
	//cout<<foo->pixmap()->height()<<endl;
	//const QPixmap* foo = ( (QLabel*) instructionLayout->itemAt(0) )->pixmap();
	//const QPixmap *instructPixmap = ( (QLabel*) instructionLayout->itemAt(0) )->pixmap();
	//cout<<instructPixmap->height()<<endl;

	instructionArea->setMaximumHeight(foo);
	instructionArea->setMinimumHeight(foo);

	resultLayout = new QVBoxLayout;
	resultArea->setLayout(resultLayout);
}

void Exercise::addTryAbstract(Try* newTry) {
    resultLayout->addWidget( newTry->getDisplay() );
    newTry->setTryNumber( tries->count() );
	current_ = tries->count();
    tries->append(newTry);
}

void Exercise::delTryAbstract() {
	Try* oldTry = tries->takeAt(current_);
	resultLayout->removeWidget( oldTry->getDisplay() );
	delete oldTry;
	resultLayout->activate();

	// renumber the remaining exercises
	for (int i=current_; i<tries->count(); i++) {
		//cout<<i<<endl;
		(*tries)[i]->setTryNumber(i);
	}
	current_ = -1;
}


