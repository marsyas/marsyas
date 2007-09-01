#include <iostream>
using namespace std;

#include "exerciseAbstract.h"

Exercise::Exercise() {
	instructionArea = NULL;
	resultArea = NULL;
	instructionImageLabel = NULL;
}

void Exercise::setArea(QGridLayout *getInstructionArea, QGridLayout *getResultArea) {
	instructionArea = getInstructionArea;
	resultArea = getResultArea;
}

void Exercise::open(QString exerciseFilename) {
	instructionImageLabel = new QLabel;
	instructionImageLabel->setPixmap(QPixmap::fromImage(QImage(exerciseFilename)));
	instructionImageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	instructionImageLabel->setScaledContents(false);
	instructionImageLabel->setMaximumHeight(120);
	instructionArea->addWidget(instructionImageLabel,0,0,Qt::AlignTop);
}

