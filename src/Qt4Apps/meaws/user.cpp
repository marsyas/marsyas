//testing only
#include <iostream>
using namespace std;

#include "user.h"

User::User()
{
	isModified = false;
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
	in>>username;
	QApplication::restoreOverrideCursor();

	filename = openFilename;
	emit enableActions(MEAWS_READY_USER);
}

bool User::save()
{
	if (filename.isEmpty())
	{
		return saveAs();
	}
	else
	{
		return saveFile(filename);
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
	out << username;
	QApplication::restoreOverrideCursor();

	filename = saveFilename;
	isModified = false;
	return true;
}

bool User::close()
{
	if (maybeSave())
	{
		username = "";
		isModified = false;
		emit enableActions(MEAWS_READY_NOTHING);
		return true;
	}
	return false;
}

bool User::maybeSave()
{
	if (isModified)
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
	return username;
}

void User::setUserInfo()
{
	QDialog *userInfoWindow;
	userInfoWindow = new QDialog(this);
	userInfoWindow->setWindowTitle(tr("User Info"));
	QGridLayout *layout = new QGridLayout;

//	int frameStyle = QFrame::Sunken | QFrame::Panel;

	QPushButton *textButton = new
		QPushButton(tr("Username: %1").arg(username));
	connect(textButton, SIGNAL(clicked()), this,
	        SLOT(setName()));
	layout->addWidget(textButton);

	QPushButton *okButton = new QPushButton(tr("Ok"));
	connect(okButton, SIGNAL(clicked()), userInfoWindow,
	        SLOT(accept()));
	layout->addWidget(okButton);
	okButton->setDefault(true);

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
		username = text;
		isModified = true;
	}
	return true;
}


