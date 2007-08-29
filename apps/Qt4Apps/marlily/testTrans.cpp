#include <fstream>
#include <iostream>
#include "Transcriber.h"
using namespace Marsyas;

//    ./marlily filename.wav
void writeOnsets(string filename, realvec* boundaries)
{
	ofstream file;
	file.open(filename.c_str());
	for (int i=0; i<boundaries->getSize(); i++)
		file<<(*boundaries)(i)<<"\t"<<90<<endl;
	file.close();
}


int main(int argc, char *argv[])
{
	std::string file = argv[1];
	realvec *pitchList, *ampList, *boundaries;
	Transcriber::getAllFromAudio(file, pitchList, ampList);
	Transcriber::toMidi(pitchList);
	boundaries = new realvec(2);
	(*boundaries)(0) = 0;
	(*boundaries)(1) = pitchList->getSize();
//	cout<<pitchList->getSize()<<endl;
//	cout<<ampList->getSize()<<endl;

	std::string writefile;
	writefile = file;
	writefile.append(".pitches.txt");
	pitchList->writeText(writefile);

	Transcriber::pitchSegment(pitchList, boundaries);
	writeOnsets("onsets.pitch.txt", boundaries);
	// relativize the amps in each pitch region
	int start, length;
	realvec region;
	mrs_real maxAmp;
	for (int i=0; i<boundaries->getSize()-1; i++)
	{
		start = (*boundaries)(i);
		length = (*boundaries)(i+1) - (*boundaries)(i);
		region = ampList->getSubVector(start, length);
		maxAmp = region.maxval();
//		cout<<maxAmp<<endl;
		for (int j=start; j<start+length; j++)
		{
			(*ampList)(j) = (*ampList)(j)/maxAmp;
		}
	}
	writefile = file;
	writefile.append(".amps.txt");
	ampList->writeText(writefile);

	Transcriber::ampSegment(ampList, boundaries);
	writeOnsets("onsets.amps.txt", boundaries);

//	realvec* notes;
//	notes = Transcriber::getNotes(pitchList, ampList, boundaries);
//	cout<<(*notes);
}

