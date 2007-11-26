#ifndef MEAWS_CAMPAIGN_H
#define MEAWS_CAMPAIGN_H

#include "defs.h"
#include <QFile>
#include <QTextStream>

class Campaign: public QObject
{
	Q_OBJECT

public:
	Campaign(QString campaignFile);


//private:

};
#endif

