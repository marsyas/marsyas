#include "Transcriber.h"

int main(int argc, char *argv[]) {
	realvec pitchList, ampList;
//	cout<<Transcriber::getPitchesFromAudio("foo.wav");
	Transcriber::getAllFromAudio("foo.wav", pitchList, ampList);
	pitchList.writeText("pitches.txt");
	ampList.writeText("amps.txt");

	Transcriber::pitchSegment(pitchList, ampList);
	cout<<pitchList;
//	Transcriber::ampSegment(pitchList, ampList);
//	cout<<ampList;

}

