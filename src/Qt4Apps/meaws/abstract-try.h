#ifndef MEAWS_TRY_ABSTRACT_H
#define MEAWS_TRY_ABSTRACT_H

#include "defs.h"
#include <QObject>
#include <QString>
#include <QGridLayout>
#include <QLabel>
#include <QDir>
#include "backend.h"
#include "QClickFrame.h"

class Try : public QObject {
	Q_OBJECT

public:
	Try();
	virtual QFrame* getDisplay() { return tryArea; };
	virtual void setTryNumber(mrs_natural num) { tryNumber_ = num; };

	virtual bool displayAnalysis(MarBackend *results) = 0;

public slots:
	virtual void clicked();

signals:
	void tryNumber(mrs_natural);

//signals:
//	void analysisDone();

protected:
	QClickFrame *tryArea;
	QLayout *tryLayout;

	mrs_natural tryNumber_;
};
#endif

