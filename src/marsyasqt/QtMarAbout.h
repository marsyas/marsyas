/* Filename: QtMarAbout.h
* Purpose: provide a simple "about marsyas" message, including the
* version number.
*/

#ifndef QT_MAR_ABOUT_H 
#define QT_MAR_ABOUT_H

#include <QMessageBox> 


namespace MarsyasQt
{
/**
	\ingroup MarsyasQt
	\brief Provides a simple static "about marsyas" box.

	TODO: Long descr.
*/

class QtMarAbout: private QObject
{
	Q_OBJECT
public:
	QtMarAbout() {};
	static void aboutMarsyas(QWidget *parent)
	{
		QMessageBox::about(parent, tr("About Marsyas"),
			tr("marsyas is a blah one two three four"
			"\n"
			"Marsyas Version 0.2.14"));
	};
};
} // namespace
#endif // MARSYSTEMWRAPPER_H
