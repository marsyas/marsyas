#ifndef MEAWS_TRY_INTONATION_H
#define MEAWS_TRY_INTONATION_H

#include "defs.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QGridLayout>
#include <QLabel>
#include <QDir>
#include "abstract-try.h"
#include "QtMarPlot.h"

class IntonationTry : public Try {
public:
	IntonationTry();
	~IntonationTry();
	bool displayAnalysis(MarBackend *results);

	void setAnswer(const realvec answers);
	void setLily(const QStringList);
	void calcErrors(const realvec& pitch, const realvec& bounds);

private:
	void colorNote(int note, double error, double direction);

	QLabel *resultDisplay;
	QtMarPlot *pitchPlot;

	realvec exerAnswer;
	QStringList lilyInput_;

};
#endif

