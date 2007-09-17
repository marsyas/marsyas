#include <iostream>
using namespace std;

#include "abstract-exercise.h"

Exercise::Exercise()
{
	instructionLayout = NULL;
	instructionImageLabel = NULL;
	resultLayout = NULL;

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

/*
void Exercise::addTry(Try* newTry) {

}
*/

