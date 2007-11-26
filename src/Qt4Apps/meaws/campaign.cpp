#include "campaign.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include <iostream>
using namespace std;

Campaign::Campaign(QString campaignFile)
{
	// init random
	srand ( time(NULL) );

	passPercent = 0;
	openFile(campaignFile);
	currentLevel = 0;
}

Campaign::~Campaign()
{
}

void Campaign::openFile(QString filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream in(&file);
	QString line = in.readLine();
	passPercent = line.toInt();
	QStringList level;
	while (!in.atEnd())
	{
		QString line = in.readLine();
		if (line.isEmpty())
		{
			adventures.append(level);
			level.clear();
		}
		else
		{
			level.append(line);
		}
	}

	/*
		// debugging: prints info read from file.
		cout<<passPercent<<endl;
		for (int i=0; i < adventures.size(); i++)
		{
			cout<<"*** level "<<i<<endl;
			for (int j=0; j<adventures[i].size(); j++)
			{
				cout<< qPrintable(adventures[i][j]) <<endl;
			}
		}
	*/

}

Exercise* Campaign::getNextExercise(double lastScore)
{
	// pick next exercise
	if (lastScore > passPercent)
		currentLevel++;
	cout<<"CAMPAIGN level: "<<currentLevel;
	cout<<lastScore<<" "<<passPercent<<endl;
	QStringList level = adventures[currentLevel];
	int chooseFrom = level.size()-1;
	int number = rand() % chooseFrom + 1;
	QString next = level[number];

	// prepare new Exercise object
	QStringList part = next.split("/");
	Exercise *exercise = NULL;
	//cout<<qPrintable(part[0])<<"\t"<<qPrintable(part[1])<<endl;
	if (part[0]=="rhythm") exercise = new RhythmExercise();
	if (part[0]=="intonation") exercise = new IntonationExercise();
	exercise->open(exercise->exercisesDir()+part[1]);

	return exercise;
}

