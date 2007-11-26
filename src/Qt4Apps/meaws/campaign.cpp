#include "campaign.h"

#include <iostream>
using namespace std;

Campaign::Campaign(QString campaignFile)
{
	passPercent = 0;
	openFile(campaignFile);
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

}

QString Campaign::getNextExercise()
{

	return "";
}

