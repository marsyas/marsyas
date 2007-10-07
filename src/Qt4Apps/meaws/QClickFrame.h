//  Graham Percival  <gperciva@uvic.ca>
#ifndef CLICKFRAME_H
#define CLICKFRAME_H

#include <QFrame>

#include "MarSystemManager.h"
using namespace Marsyas;

//namespace MarsyasQt
//{

/*
	\brief A simple realvec plotting widget.
	\ingroup MarsyasQt

	Plots a realvec.  Is simple to use, but lacks many features of the
other plotting widget.

*/
class QClickFrame : public QFrame
{
	Q_OBJECT
public:
	QClickFrame(QWidget *parent = 0);

protected:
	void mousePressEvent(QMouseEvent *event);

signals:
	void clicked();

};

//} //namespace
#endif

