#include "Transcriber.h"

//    ./marlily filename.wav

int main(int argc, char *argv[]) {
	realvec pitchList, ampList;
	Transcriber::getAllFromAudio(argv[1], pitchList, ampList);
	Transcriber::toMidi(pitchList);
	pitchList.writeText("pitches.txt");
	ampList.writeText("amps.txt");

	Transcriber::pitchSegment(pitchList, ampList);
	Transcriber::ampSegment(pitchList, ampList);
//	cout<<ampList;
	realvec foo;
	for (int i=0; i<pitchList.getRows(); i++)
	{
		pitchList.getRow(i, foo);
//		cout<<"********* "<<i<<endl;
//		cout<<foo;
	}

}

