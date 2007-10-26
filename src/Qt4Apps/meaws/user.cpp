//testing only
#include <iostream>
using namespace std;

#include "user.h"

User::User()
{
	isModified_ = false;
	levelList_ << "Novice" << "Beginner" << "Moderate" << "Good"
	<< "Expert" << "Fantastic";
	weekPracticeList_ << "Never" << "0-1" << "2-4" << "5-9" <<
	"10-14" << "15-19" << "20+";
	weekPlayList_ << "Never" << "0-1" << "2-4" << "5-9" <<
	"10-14" << "15-19" << "20+";
	yearsPlayingList_ << "0" << "1" << "2" << "3-4" << "5-6" << "7-10"
	<< "11-14" << "15-19" << "20+";

	username_ = "";
	level_ = "Novice";
	weekPractice_ = "Never";
	weekPlay_ = "Never";
	yearsPlaying_= "0";


	usernameButton_ = new QPushButton;
	connect(usernameButton_, SIGNAL(clicked()), this,
	        SLOT(queryName()));
	levelButton_ = new QPushButton;
	connect(levelButton_, SIGNAL(clicked()), this,
	        SLOT(queryLevel()));
	weekPracticeButton_ = new QPushButton;
	connect(weekPracticeButton_, SIGNAL(clicked()), this,
	        SLOT(queryWeekPractice()));
	weekPlayButton_ = new QPushButton;
	connect(weekPlayButton_, SIGNAL(clicked()), this,
	        SLOT(queryWeekPlay()));
	yearsPlayingButton_ = new QPushButton;
	connect(yearsPlayingButton_, SIGNAL(clicked()), this,
	        SLOT(queryYearsPlaying()));
}


User::~User()
{
}

void User::newUser()
{
	if (maybeSave())
	{
		if (queryName())
		{
			emit enableActions(MEAWS_READY_USER);
		}
	}
}

void User::open()
{
	if (maybeSave())
	{
		QString openFilename = QFileDialog::getOpenFileName(0,tr("Open File"),
		                       MEAWS_DIR);
		if (!openFilename.isEmpty())
			openFile(openFilename);
	}
}

void User::openFile(const QString &openFilename)
{
	QFile file(openFilename);
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::warning(this, tr("Application"),
		                     tr("Cannot read file %1:\n%2.")
		                     .arg(openFilename)
		                     .arg(file.errorString()));
		return;
	}

	QTextStream in(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);

	filename_ = openFilename;
	in>>username_;
	in>>level_;
	in>>weekPractice_;
	in>>weekPlay_;
	in>>yearsPlaying_;

	updateUserInfoDisplay();
	isModified_ = false;
	QApplication::restoreOverrideCursor();
	emit enableActions(MEAWS_READY_USER);
}

bool User::save()
{
	if (filename_.isEmpty())
	{
		return saveAs();
	}
	else
	{
		return saveFile(filename_);
	}
}

bool User::saveAs()
{
	QString saveAsFilename = QFileDialog::getSaveFileName(0,tr("Save File"),
	                         MEAWS_DIR);
	if (saveAsFilename.isEmpty())
		return false;
	return saveFile(saveAsFilename);
}

bool User::saveFile(const QString &saveFilename)
{
	QFile file(saveFilename);
	if (!file.open(QFile::WriteOnly | QFile::Text))
	{
		QMessageBox::warning(this, tr("Application"),
		                     tr("Cannot write file %1:\n%2.")
		                     .arg(saveFilename)
		                     .arg(file.errorString()));
		return false;
	}

	QTextStream out(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	out << username_ << "\n";
	out << level_ << "\n";
	out << weekPractice_ << "\n";
	out << weekPlay_ << "\n";
	out << yearsPlaying_ << "\n";
	QApplication::restoreOverrideCursor();

	filename_ = saveFilename;
	isModified_ = false;
	return true;
}

bool User::close()
{
	if (maybeSave())
	{
		username_ = "";
		isModified_ = false;
		emit enableActions(MEAWS_READY_NOTHING);
		return true;
	}
	return false;
}

bool User::maybeSave()
{
// TODO: disable temporarily.
/*
	if (isModified_)
	{
		QMessageBox::StandardButton ret;
		ret = QMessageBox::warning(this, tr("Meaws"),
		                           tr("This user has unsaved data.\n"
		                              "Do you want to save your changes?"),
		                           QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		if (ret == QMessageBox::Save)
			return save();
		else if (ret == QMessageBox::Cancel)
			return false;
	}
*/
	return true;
}


QString User::getName()
{
	return username_;
}

void User::setUserInfo()
{
	QDialog *userInfoWindow;
	userInfoWindow = new QDialog(this);
	userInfoWindow->setWindowTitle(tr("User Info"));
	QGridLayout *layout = new QGridLayout;
	QPushButton *okButton_ = new QPushButton(tr("Ok"));
	okButton_->setDefault(true);

	layout->addWidget(usernameButton_);
	layout->addWidget(levelButton_);
	layout->addWidget(weekPracticeButton_);
	layout->addWidget(weekPlayButton_);
	layout->addWidget(yearsPlayingButton_);


	layout->addWidget(okButton_);
	connect(okButton_, SIGNAL(clicked()), userInfoWindow,
	        SLOT(accept()));
	userInfoWindow->setLayout(layout);
	userInfoWindow->exec();
}

void
User::updateUserInfoDisplay()
{
	usernameButton_->setText(tr("Username: %1").arg(username_));
	levelButton_->setText(tr("Level: %1").arg(level_));

	weekPracticeButton_->setText(
	    tr("Weekly pratice: %1 hours").arg(weekPractice_));
	weekPlayButton_->setText(
	    tr("Weekly playing : %1 hours").arg(weekPlay_));
	yearsPlayingButton_->setText(
	    tr("Years playing:%1").arg(yearsPlaying_));
}

bool
User::queryName()
{
	bool ok;
	QString text = QInputDialog::getText(this, tr("Name"),
	                                     tr("User name:"), QLineEdit::Normal,
	                                     QDir::home().dirName(), &ok);
	if (ok && !text.isEmpty())
	{
		username_ = text;
		isModified_ = true;
		updateUserInfoDisplay();
	}
	return true;
}

bool
User::queryLevel()
{
	bool ok;
	QString item = QInputDialog::getItem(this,
	                                     tr("Level"),
	                                     tr("How would you "
	                                        "describe your ability?"), levelList_, 0,
	                                     false, &ok);
	if (ok && !item.isEmpty())
	{
		level_ = item;
		isModified_ = true;
		updateUserInfoDisplay();
	}
	return true;
}


bool
User::queryWeekPractice()
{
	bool ok;
	QString item = QInputDialog::getItem(this,
	                                     tr("Weekly practice"),
	                                     tr("How many hours a "
	                                        "week do you practice (not simply playing)?"), weekPracticeList_,
	                                     0, false, &ok);
	if (ok && !item.isEmpty())
	{
		weekPractice_ = item;
		isModified_ = true;
		updateUserInfoDisplay();
	}
	return true;
}

bool
User::queryWeekPlay()
{
	bool ok;
	QString item = QInputDialog::getItem(this,
	                                     tr("Weekly playing"),
	                                     tr("How many hours a "
	                                        "week do you play (for fun, not individual practice."),
	                                     weekPlayList_, 0,
	                                     false, &ok);
	if (ok && !item.isEmpty())
	{
		weekPlay_ = item;
		isModified_ = true;
		updateUserInfoDisplay();
	}
	return true;
}

bool
User::queryYearsPlaying()
{
	bool ok;
	QString item = QInputDialog::getItem(this,
	                                     tr("Years playing"),
	                                     tr("How long have you "
	                                        "been playing this instrument?"), yearsPlayingList_, 0,
	                                     false, &ok);
	if (ok && !item.isEmpty())
	{
		yearsPlaying_ = item;
		isModified_ = true;
		updateUserInfoDisplay();
	}
	return true;
}

