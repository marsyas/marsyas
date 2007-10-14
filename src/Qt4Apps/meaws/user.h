#ifndef USER_H
#define USER_H

#include "defs.h"

#include <QDialog>
#include <QErrorMessage>
#include <QString>
#include <QtGui>

class QAction;
class QMenu;
class QTextEdit;
class QLabel;
class QErrorMessage;

class User : public QDialog
{
	Q_OBJECT

public:
	User();
	~User();
	QString getName();

public slots:
	void newUser();
	void open();
	bool save();
	bool saveAs();
	bool close();
	void setUserInfo();

signals:
	void enableActions(int state);

private slots:
	bool setName();

private:
	bool maybeSave();
	bool saveFile(const QString &saveFilename);
	void openFile(const QString &openFilename);

	bool isModified;
	QString username;
	QString filename;

};
#endif

