#ifndef QTCHECK_BACKEND_H
#define QTCHECK_BACKEND_H

#include <QObject>
#include "MarSystemManager.h"
#include "MarSystemQtWrapper.h"
using namespace Marsyas;
using namespace MarsyasQt;

using namespace std;
#include <iostream>

class MarBackend: public QObject
{
	Q_OBJECT
public:
	MarBackend(string infile, string outfile);
	~MarBackend();

public slots:
	void ctrlChanged(MarControlPtr changed);

private:
	MarSystemManager mng;
	MarSystemQtWrapper *mrsWrapper;
	MarSystem *pnet;
	MarControlPtr isEmptyPtr;
};
#endif

