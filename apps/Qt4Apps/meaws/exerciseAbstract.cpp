#include <iostream>
using namespace std;

#include "exerciseAbstract.h"

Exercise::Exercise() {
	instructionArea = NULL;
	resultArea = NULL;
}

void Exercise::setArea(QGridLayout *getInstructionArea, QGridLayout *getResultArea) {
	instructionArea = getInstructionArea;
	resultArea = getResultArea;
}

void Exercise::open(QString exerciseFilename) {
	QLabel *imageLabel = new QLabel;
	imageLabel->setPixmap(QPixmap::fromImage(QImage(exerciseFilename)));
	imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	imageLabel->setScaledContents(false);
	imageLabel->setMaximumHeight(120);
	instructionArea->addWidget(imageLabel,0,0,Qt::AlignTop);
}

