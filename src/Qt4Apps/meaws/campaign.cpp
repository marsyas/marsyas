#include "campaign.h"

#include <iostream>
using namespace std;

Campaign::Campaign(QString campaignFile)
{
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
	while (!in.atEnd())
	{
		QString line = in.readLine();
		cout<<qPrintable( line );
		cout<<endl;
		//process_line(line);
	}

}

QString Campaign::getNextExercise()
{

	return "";
}

