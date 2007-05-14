//testing only
#include <iostream>
using namespace std;

#include "user.h"

User::User() {

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
	emit enableActions(MEAWS_READY_USER);
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


