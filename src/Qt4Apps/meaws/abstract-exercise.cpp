#include <iostream>
using namespace std;

#include "abstract-exercise.h"

Exercise::Exercise()
{
	instructionLayout = NULL;
	instructionImageLabel = NULL;

	instructionArea = new QFrame;
	resultArea = new QFrame;
}

Exercise::~Exercise()
{
	cout<<"abs delete"<<endl;

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

void Exercise::setArea(QFrame *getInstructionArea, QFrame *getResultArea)
{
	instructionArea = getInstructionArea;
	resultArea = getResultArea;
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
	instructionArea->setLayout(instructionLayout);
	int ml, mt, mr, mb;
	instructionLayout->getContentsMargins(&ml, &mt, &mr, &mb);
	mt = 0;
	mb = 0;
	instructionLayout->setContentsMargins(ml, mt, mr, mb);

	instructionArea->setMaximumHeight(instructPixmap->height()+mt+mb);
	instructionArea->setMinimumHeight(instructPixmap->height()+mt+mb);
}


