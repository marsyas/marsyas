#ifndef MEAWS_TRY_ABSTRACT_H
#define MEAWS_TRY_ABSTRACT_H

#include "defs.h"
#include <QObject>
#include <QVBoxLayout>
#include <QTemporaryFile>
#include "QClickFrame.h"
#include "backend.h"

class Try : public QObject
{
	Q_OBJECT

public:
	Try();
	~Try();
	QFrame* getDisplay()
	{
		return tryArea_;
	};
	void setTryNumber(mrs_natural num)
	{
		tryNumber_ = num;
	};
	mrs_real getScore()
	{
		return score_;
	};
	mrs_bool hasAudio()
	{
		return hasAudio_;
	};
	mrs_string getFilename()
	{
		return filename_;
	};
	void setFilename(mrs_string filename);
	virtual void reset();

	virtual void display(mrs_natural state) = 0;


	virtual void displayAnalysis(MarBackend *results)
	{
		(void) results;
		hasAudio_ = true;
	};

public slots:
	virtual void clicked()
	{
		emit selectTry(tryNumber_);
	};
	virtual void doubleclicked() {};

signals:
	void selectTry(mrs_natural);

protected:
	QClickFrame *tryArea_;
	QLayout *tryLayout_;

	mrs_natural tryNumber_;
	mrs_bool hasAudio_;
	mrs_real score_;

	QTemporaryFile* file_;
	mrs_string filename_;
};
#endif

