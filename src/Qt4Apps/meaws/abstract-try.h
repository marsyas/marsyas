#ifndef MEAWS_TRY_ABSTRACT_H
#define MEAWS_TRY_ABSTRACT_H

#include "defs.h"
#include <QObject>
#include <QVBoxLayout>
#include "QClickFrame.h"
#include "backend.h"

class Try : public QObject {
	Q_OBJECT

public:
	Try();
	virtual QFrame* getDisplay() { return tryArea; };
	virtual void setTryNumber(mrs_natural num) { tryNumber_ = num; };
	virtual bool displayAnalysis(MarBackend *results) = 0;

public slots:
	virtual void clicked() { emit selectTry(tryNumber_); };
	virtual void doubleclicked() {};

signals:
	void selectTry(mrs_natural);

protected:
	QClickFrame *tryArea;
	QLayout *tryLayout;

	mrs_natural tryNumber_;
};
#endif

