#include <fstream>
#include <iostream>
#include "Transcriber.h"
using namespace Marsyas;

//    ./marlily filename.wav
void writeOnsets(string filename, realvec boundaries)
{
  ofstream file;
  file.open(filename.c_str());
  for (int i=0; i<boundaries.getSize(); i++)
    file<<boundaries(i)<<"\t"<<90<<endl;
  file.close();
}


int main(int argc, char *argv[])
{
  std::string file = argv[1];
  realvec pitchList, ampList, boundaries;
  Transcriber::getAllFromAudio(file, pitchList, ampList, boundaries);
  Transcriber::toMidi(pitchList);

  std::string writefile;
  writefile = file;
  writefile.append(".pitches.txt");
  pitchList.writeText(writefile);
  writefile = file;
  writefile.append(".amps.txt");
  ampList.writeText(writefile);

//	Transcriber::pitchSegment(pitchList, boundaries);
//	writeOnsets("onsets.pitch.txt", boundaries);
  Transcriber::ampSegment(ampList, boundaries);
  writeOnsets("onsets.amps.txt", boundaries);
  Transcriber::discardBeginEndSilencesAmpsOnly(ampList, boundaries);
  cout<<boundaries;

//	cout<<boundaries;

  /*
  	cout<<"***********************"<<endl;
  	for (int i=0; i<boundaries.getSize()-1; i++){
  		realvec foo = pitchList.getSubVector( (mrs_natural)boundaries(i),
  (mrs_natural) (boundaries(i+1)-boundaries(i)));
  		cout<<foo;

  	}
  */
  /*
  	writefile = file;
  	writefile.append(".amps.txt");
  	ampList.writeText(writefile);
  	Transcriber::ampSegment(ampList, boundaries);
  	writeOnsets("onsets.amps.txt", boundaries);

  	realvec notes;
  	notes = Transcriber::getNotes(pitchList, ampList, boundaries);
  	cout<<notes;
  */
}

