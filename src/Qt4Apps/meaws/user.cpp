//testing only
#include <iostream>
using namespace std;

#include "user.h"

User::User()
{
	isModified_ = false;
	levels_ << "Novice" << "Beginner" << "Moderate" << "Good"
		<< "Expert" << "Fantastic";

	username_ = "";
	level_ = "Novice";

	usernameButton = new QPushButton(tr("Username: %1").arg(username_));
	connect(usernameButton, SIGNAL(clicked()), this,
	        SLOT(setName()));

	levelButton = new QPushButton(tr("Level: %1").arg(level_));
	connect(levelButton, SIGNAL(clicked()), this,
	        SLOT(setLevel()));
}


User::~User()
{
}

void User::newUser()
{
	if (maybeSave())
	{
		if (setName())
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

	QDataStream in(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	in>>username_;
	QApplication::restoreOverrideCursor();

	filename_ = openFilename;
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

	QDataStream out(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	out << username_;
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

//	int frameStyle = QFrame::Sunken | QFrame::Panel;
	QPushButton *okButton = new QPushButton(tr("Ok"));
	okButton->setDefault(true);

	layout->addWidget(usernameButton);
	layout->addWidget(levelButton);

	layout->addWidget(okButton);
	connect(okButton, SIGNAL(clicked()), userInfoWindow,
	        SLOT(accept()));

	userInfoWindow->setLayout(layout);
	userInfoWindow->exec();
}

bool
User::setName()
{
	bool ok;
	QString text = QInputDialog::getText(this, tr("User info"),
	                                     tr("User name:"), QLineEdit::Normal,
	                                     QDir::home().dirName(), &ok);
	if (ok && !text.isEmpty())
	{
		username_ = text;
		isModified_ = true;
		usernameButton->setText(tr("Username: %1").arg(username_));
	}
	return true;
}

bool
User::setLevel()
{
//	QStringList items = levels;
	
	//items << LEVEL;
	//items << tr("Spring") << tr("Summer") << tr("Fall") <<
// tr("Winter");

	bool ok;
	QString item = QInputDialog::getItem(this,
	                                     tr("QInputDialog::getLevel()"),
	                                     tr("Level:"), levels_, 0,
	                                     false, &ok);
	if (ok && !item.isEmpty()) {
		level_ = item;
		levelButton->setText(tr("Level: %1").arg(level_));
	}
	return true;
}


