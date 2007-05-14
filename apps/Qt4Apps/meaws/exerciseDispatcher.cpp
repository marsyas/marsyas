#include <iostream>
using namespace std;

#include "exerciseDispatcher.h"

ExerciseDispatcher::ExerciseDispatcher() {
}

ExerciseDispatcher::~ExerciseDispatcher() {
}

void ExerciseDispatcher::setInstructionArea(QLayout *getInstructionArea){
	instructionArea = getInstructionArea;
}
void ExerciseDispatcher::setResultArea(QLayout *getResultArea){
	resultArea = getResultArea;
}


void ExerciseDispatcher::open() {
	QString openFilename = QFileDialog::getOpenFileName(0,tr("Save file"),
		MEAWS_DIR);
	if (!openFilename.isEmpty()) {
		cout<<qPrintable(openFilename)<<endl;

		exerciseName = openFilename;
        QImage image(openFilename);
		imageLabel = new QLabel;
        imageLabel->setPixmap(QPixmap::fromImage(image));
//        exerciseTitle->setText( tr("Exercise: %1").arg(QFileInfo(exerciseName).baseName()) );
		instructionArea->addWidget(imageLabel);
        enableActions(MEAWS_READY_EXERCISE);
	}
}


