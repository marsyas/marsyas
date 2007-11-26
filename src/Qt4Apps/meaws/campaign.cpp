#include "campaign.h"

#include <iostream>
using namespace std;

Campaign::Campaign(QString campaignFile)
{
	QFile file(campaignFile);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream in(&file);
	while (!in.atEnd())
	{
		QString line = in.readLine();
		cout<<qPrintable( line )<<endl;;
		//process_line(line);
	}

}



