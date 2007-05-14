//testing only
#include <iostream>
using namespace std;

#include "user.h"

User::User() {
	isModified = false;
}

User::~User() {
}

void User::newUser() {
	if (maybeSave()) {
		bool ok;
		QString text = QInputDialog::getText(this, tr("User info"),
			tr("User name:"), QLineEdit::Normal,
			QDir::home().dirName(), &ok);
		if (ok && !text.isEmpty()) {
			name = text;
			isModified = true;
			emit enableActions(MEAWS_READY_USER);
		}
	}
}

void User::open() {

}

bool User::save() {
	return true;
}

void User::saveAs() {

}

void User::close() {
	if (maybeSave()) {
		name = "";
		emit enableActions(MEAWS_READY_NOTHING);
	}
}

bool User::maybeSave() {
	if (isModified) {
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


QString User::getName() {
	return name;
}

void User::setUserInfo() {
	bool ok;
	QString text = QInputDialog::getText(this, tr("User info"),
		tr("User name:"), QLineEdit::Normal,
		QDir::home().dirName(), &ok);
	if (ok && !text.isEmpty()) {
		name = text;
		emit enableActions(MEAWS_READY_USER);
	}
}


