#ifndef MEAWS_TRY_INTONATION_H
#define MEAWS_TRY_INTONATION_H

#include "defs.h"
#include <QObject>
#include <QString>
#include <QGridLayout>
#include <QLabel>
#include <QDir>
#include "abstract-try.h"
#include "QtMarPlot.h"

class IntonationTry : public Try {
public:
	IntonationTry();
	~IntonationTry();
	QtMarPlot* getPlot() { return foo; };
	void setAnswer(const realvec answers);

	bool displayAnalysis(MarBackend *results);

protected:
	QLabel *resultDisplay;
	QtMarPlot *foo;
	realvec exerAnswer;
};
#endif

