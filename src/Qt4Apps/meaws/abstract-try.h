#ifndef MEAWS_TRY_ABSTRACT_H
#define MEAWS_TRY_ABSTRACT_H

#include "defs.h"
#include <QObject>
#include <QString>
#include <QGridLayout>
#include <QLabel>
#include <QDir>
#include "backend.h"

class Try : public QObject {
	Q_OBJECT

public:
	Try();
	virtual QFrame* getDisplay() { return tryArea; };

	virtual bool displayAnalysis(MarBackend *results) = 0;

//signals:
//	void analysisDone();

protected:
	QFrame *tryArea;
	QLayout *tryLayout;
};
#endif

