#ifndef USER_H
#define USER_H

#include "defs.h"

#include <QDialog>
#include <QInputDialog>
#include <QErrorMessage>
#include <QStringList>
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
	void reset();

public slots:
	void newUser();
	void open();
	bool save();
	bool saveAs();
	bool close();
	void setUserInfo();

	void saveExercise(std::string exerciseName, double score);

signals:
	void updateMain(int state);

private slots:
	bool queryName();
	bool queryLevel();
	bool queryWeekPractice();
	bool queryWeekPlay();
	bool queryYearsPlaying();

private:
	bool maybeSave();
	bool saveFile(const QString &saveFilename);
	void openFile(const QString &openFilename);
	void setupInfoWindow();
	void updateUserInfoWindow();

	QStringList levelList_;
	QStringList weekPracticeList_;
	QStringList weekPlayList_;
	QStringList yearsPlayingList_;

	bool isModified_;

	// User info
	QString username_;
	QString filename_;
	QString level_;
	QString weekPractice_;
	QString weekPlay_;
	QString yearsPlaying_;

	QPushButton *usernameButton_;
	QPushButton *levelButton_;
	QPushButton *weekPracticeButton_;
	QPushButton *weekPlayButton_;
	QPushButton *yearsPlayingButton_;
	QPushButton *okButton_;

	QStringList exercises_;
	QList<double> scores_;
};
#endif

