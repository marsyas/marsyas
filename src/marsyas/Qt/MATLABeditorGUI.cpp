/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/**
\class MATLABeditorGUI
\brief GUI for viewing/editing MATLAB engine scripts for MarSystems 
*/

#include "MATLABeditorGUI.h"

#ifdef MARSYAS_QT

#include <QtCore>
#include <QtGui>

#ifdef MARSYAS_MATLAB

//#include "MarSystem.h"
#include "MATLABengine.h"

#define OUTPUT_BUFFER_SIZE 512

using namespace std;
using namespace Marsyas;

MATLABeditorGUI::MATLABeditorGUI(string script, QWidget* parent, Qt::WFlags f) : QMainWindow(parent, f)
{
	//create a Frame widget for the editor
	QFrame* editorFrm = new QFrame();

	//create Menus
	QMenuBar* menuBar = QMainWindow::menuBar();
	QMenu* fileMenu = menuBar->addMenu("File");
	QAction* openAct = fileMenu->addAction("Open mfile...");
	QAction* saveAct = fileMenu->addAction("Save as mfile...");
	//connect menu signals to slots
	connect(openAct, SIGNAL(triggered(bool)),
		this, SLOT(openMfile()));
	connect(saveAct, SIGNAL(triggered(bool)),
		this, SLOT(saveMfile()));

	//create the text editor
	scriptEditor_ = new QTextEdit;
	scriptEditor_->setAcceptRichText(true);
	scriptEditor_->setWordWrapMode(QTextOption::NoWrap);
	QFont scriptFont("Courier New", 10);
	scriptEditor_->setFont(scriptFont);
	scriptEditor_->setTabStopWidth(20); 
	scriptEditor_->setPlainText(QString::fromStdString(script));
	connect(scriptEditor_, SIGNAL(textChanged()),
		this, SLOT(enableApplyBtn()));

	//create the "Apply" button
	applyBtn_ = new QPushButton("Apply");
	applyBtn_->setEnabled(false);
	connect(applyBtn_, SIGNAL(clicked(bool)),
		this, SLOT(updateScript()));

	//create a layout manager for editor frame
	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(scriptEditor_);
	layout->addWidget(applyBtn_);
	editorFrm->setLayout(layout);

// 	//create the MATLAB output display
// 	outputDisplay_ = new QTextEdit();
// 	outputDisplay_->setReadOnly(true);
// 	//and link it to MATLAB engine output
// 	outputBuffer_ = new char[OUTPUT_BUFFER_SIZE+1];
// 	outputBuffer_[OUTPUT_BUFFER_SIZE] = '\0';
// 	MATLAB->setOutputBuffer(outputBuffer_, OUTPUT_BUFFER_SIZE);
// 
// 	//create a Splitter
// 	QSplitter* splitter = new QSplitter(Qt::Vertical);
// 	splitter->addWidget(editorFrm);
// 	splitter->addWidget(outputDisplay_);

	//set the central widget
	//setCentralWidget(splitter);
	setCentralWidget(editorFrm);
}

MATLABeditorGUI::~MATLABeditorGUI()
{
	//MATLAB->setOutputBuffer(NULL, 0);
	//delete [] outputBuffer_;
}

void
MATLABeditorGUI::enableApplyBtn()
{
	applyBtn_->setEnabled(true);
}

void 
MATLABeditorGUI::updateScript()
{
	applyBtn_->setEnabled(false);
	emit scriptChanged(scriptEditor_->toPlainText().toStdString());
	//msys_->setMATLABscript(scriptEditor_->toPlainText().toStdString());
}

// void
// MATLABeditorGUI::updateOutputDisplay()
// {
// 	//append any MATLAB output messages/results to the outputDisplay widget
// 	//outputDisplay_->append(QString(outputBuffer_));
//}

void
MATLABeditorGUI::setScript(string script)
{
	scriptEditor_->setPlainText(QString::fromStdString(script));
}

void
MATLABeditorGUI::openMfile()
{
	//open mfile
	QString fileName = QFileDialog::getOpenFileName(
		this,
		"Open mfile",
		"",
		"mfiles (*.m);;All files (*.*)");
	
	if (!fileName.isEmpty())
	{
		QFile inFile(fileName);
		if (inFile.open(QFile::ReadOnly)) 
		{
			QTextStream in(&inFile);
			scriptEditor_->setPlainText(in.readAll());
		}
	}
	else return;
}

void
MATLABeditorGUI::saveMfile()
{
	//open mfile
	QString fileName = QFileDialog::getSaveFileName(
		this,
		"Save mfile",
		"",
		"mfiles (*.m);;All files (*.*)");

	if (!fileName.isEmpty())
	{
		QFile outFile(fileName);
		if (outFile.open(QFile::WriteOnly | QFile::Truncate)) 
		{
			QTextStream out(&outFile);
			out << scriptEditor_->toPlainText();
		}
	}
}

#endif //MARSYAS_MATLAB
#endif //MARSYAS_QT