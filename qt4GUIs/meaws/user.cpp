#include <iostream>
#include <QtGui>
#include "user.h"

User::User() {
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

User::~User() {
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


