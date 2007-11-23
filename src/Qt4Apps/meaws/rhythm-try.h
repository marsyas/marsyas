#ifndef MEAWS_TRY_RHYTHM_H
#define MEAWS_TRY_RHYTHM_H

#include "defs.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QGridLayout>
#include <QLabel>
#include <QDir>
#include "abstract-try.h"
#include "QtMarRhythmLines.h"
//#include "Marx2DGraph.h"

class RhythmTry : public Try
{
public:
	RhythmTry();
	~RhythmTry();
	void displayAnalysis(MarBackend *results);

	void setAnswer(const realvec answers);
	void setLily(const QStringList);
	void calcErrors(const realvec& pitch, const realvec& bounds);

	void display(mrs_natural state);
//public slots:
//	void doubleclicked();

private:
	mrs_natural calcOffsetAndScore(
	    const realvec exerciseOnsets, const realvec audioOnsets);
	void calcScore(const realvec exerciseOnsets,
	               const realvec audioOnsets, mrs_natural& offset,
	               mrs_real& score);
	void colorNote(int note, double error, double direction);

	QLabel *resultDisplay;
	QtMarRhythmLines *pitchPlot;

	realvec exerAnswer;
	QStringList lilyInput_;

//	Marx2DGraph* graph;
};
#endif

