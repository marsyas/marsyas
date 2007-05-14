#ifndef USER_H
#define USER_H

#include "defs.h"

#include <QDialog>
#include <QString>
#include <QtGui>

class QAction;
class QMenu;
class QTextEdit;
class QLabel;
class QErrorMessage;

class User : public QDialog {
	Q_OBJECT

public:
	User();
	~User();
	QString getName();

public slots:
	void newUser();
	void open();
	bool save();
	void saveAs();
	void close();
	void setUserInfo();

signals:
	void enableActions(int state);

private:
	bool maybeSave();

	bool isModified;
	QString name;

};
#endif

