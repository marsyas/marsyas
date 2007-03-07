#include <iostream>
#include <QtGui>
#include "user.h"

// new user
User::User(const int type) {
	if (type==NewUser) { newUser(); }
	if (type==OpenUser) { openUser(); }

}

User::~User() {
}

void User::newUser() {
	bool ok;
  QString text = QInputDialog::getText(this, tr("User info"),
    tr("User name:"), QLineEdit::Normal,
    QDir::home().dirName(), &ok);
  if (ok && !text.isEmpty()) {
    name = text;
	} else {
		throw false;
	}
}

void User::openUser() {

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
	}
}


