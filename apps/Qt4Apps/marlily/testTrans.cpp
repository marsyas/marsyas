#include "Transcriber.h"

//    ./marlily filename.wav

int main(int argc, char *argv[]) {
	realvec pitchList, ampList;
	Transcriber::getAllFromAudio(argv[1], pitchList, ampList);
	Transcriber::toMidi(pitchList);
	pitchList.writeText("pitches.txt");
	ampList.writeText("amps.txt");

	realvec foo = pitchList;
	foo /= 100.0;
	foo.writeText("foo.txt");

	Transcriber::ampSegment(pitchList, ampList);
//	Transcriber::pitchSegment(pitchList, ampList);
//	cout<<ampList;
/*
	for (int i=0; i<pitchList.getRows(); i++)
	{
		pitchList.getRow(i, foo);
//		cout<<"********* "<<i<<endl;
//		cout<<foo;
	}
*/

}

