#include <iostream>
#include <QtGui>
#include "exercise.h"

Exercise::Exercise() {
	name = "scale";
	QString fileName;
	fileName = "music/";
	fileName.append(name);
	fileName.append(".ly");

	QFile in_file(fileName);
	in_file.open(QIODevice::ReadOnly | QIODevice::Text);
	lily_input = (QTextStream(&in_file).readAll()).split('\n');
	in_file.close();

	QString temp;
	for (int i = 0; i < lily_input.size(); ++i) {
		temp = lily_input.at(i);
//		cout<<qPrintable(temp)<<endl;
		//cout << lily_input.at(i).toLocal8Bit().constData() << endl;
	}
	note = 8;
}

Exercise::~Exercise() {
}

QString Exercise::getName() {
	return name;
}

void Exercise::getLily() {
	QString temp;

	QFile out_file("/Users/gperciva/tmp/out.ly");
	out_file.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream out(&out_file);

	for (int i = 0; i < lily_input.size(); ++i) {
		temp = lily_input.at(i);
		out<<qPrintable(temp)<<endl;
	}
	out_file.close();
}

void Exercise::nextNoteError(float error, int direction) {
	cout<<error<<"  "<<direction<<endl;
	QString color = "black";

	if (error < -0.001) { color = "Medium Blue"; }
  if (error < -0.002) { color = "Dodger Blue"; }
  if (error < -0.005) { color = "Light Sky Blue"; }
  
  if (error > 0.0001) { color = "Light Salmon"; }
  if (error > 0.0005) { color = "tomato"; }
  if (error > 0.001) { color = "red"; }

	color.insert(0,"\\colorNote #\"");
	color.append("\" ");
//	cout<<qPrintable(color)<<endl;
	QString lily_line = lily_input.at(note);
	lily_line.insert(0,color);

	color="^\\markup{ \\hspace #0.5 \\arrow-head #Y #LEFT ##f }";
	lily_line.append(color);

	lily_input.replace(note,lily_line);
	note++;
}

