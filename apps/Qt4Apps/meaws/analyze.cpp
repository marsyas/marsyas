#include <QObject>
#include <iostream>
#include <fstream>
#include "analyze.h"

Analyze::Analyze() {

}
Analyze::~Analyze() {

}


void Analyze::calculate(string filename) {
	string command;
	command = "python2.4 praat-to-pitch.py ";
	command.append(filename);
	command.append(" 120");  // tempo
	cout<<"DOING: "<<command.c_str()<<endl;
	system(command.c_str());

// please don't look at this code.  I feel embarrassed for having
// written it, and it will be rewritten next weekend.
	float notepitch;
	int note;
	int tatum;
	float sumPitch;
	float avgPitch;
	float pitchError;
	int expected_pitch[8] = {60,62,64,65,67,69,71,72};
	int expected_duration[8] = {2,1,1,2,2,3,1,4};
	note=0;
	ifstream inFile;
	inFile.open("notepitches.txt");
//	ofstream outFile("toperl.txt",ios::out);
	sumPitch=0;
	tatum=0;
	while (inFile >> notepitch) {
// do whatever I want with the note data.
		tatum++;

// I feel so dirty... AND RIGHTLY SO!
		if ((note==0) && (tatum==4)) { tatum=0; } else
		if (notepitch>0) {
		//if (tatum > (2*2-1)) {  // intro beats.  DOESN'T WORK!
			sumPitch += notepitch;
		//	cout<<notepitch<<"   "<<note<<"  "<<tatum<<endl;
			if (tatum>=expected_duration[note]) {
				avgPitch = (sumPitch / tatum);
				pitchError = avgPitch - expected_pitch[note];
	//			cout<<note<<"  "<<pitchError;
	//			cout<<"   "<<avgPitch<<"  "<<expected_pitch[note]<<endl;
//				outFile<<pitchError<<endl;
				emit nextNoteError(pitchError,1);
				sumPitch=0;
				tatum=0;
				note++;
				if (note>=8) break;
			}
		}
	}
	inFile.close();
//	outFile.close();
//	command = "perl color-aud-output.pl toperl.txt ";
//	system(command.c_str());
}


