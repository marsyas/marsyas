#include <iostream>
using namespace std;

#include <QImage>
#include "exerciseIntonation.h"

ExerciseIntonation::ExerciseIntonation() {
}

void ExerciseIntonation::open(QString exerciseFilename) {
	cout<<"in ExerInt"<<endl;
	cout<<qPrintable(exerciseFilename)<<endl;

    QImage image(exerciseFilename);
    imageLabel = new QLabel;
    imageLabel->setPixmap(QPixmap::fromImage(image));
//        exerciseTitle->setText( tr("Exercise: %1").arg(QFileInfo(exerciseName).baseName()) );
    instructionArea->addWidget(imageLabel,0,0,Qt::AlignTop);

}
