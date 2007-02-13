#include <iostream>
#include <QtGui>
#include "user.h"

User::User() {
//	name = new QString();
}

User::~User() {
}

void User::dataDialogue() {
	bool ok;
	QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
		tr("User name:"), QLineEdit::Normal,
		QDir::home().dirName(), &ok);
	if (ok && !text.isEmpty())
		name=text;

}

