#include "Transcriber.h"

//    ./marlily filename.wav

int main(int argc, char *argv[])
{
	std::string file = argv[1];
	realvec pitchList, ampList;
	Transcriber::getAllFromAudio(file, pitchList, ampList);
	Transcriber::toMidi(pitchList);
	std::string writefile;
	writefile = file;
	writefile.append(".pitches.txt");
	pitchList.writeText(writefile);
	writefile = file;
	writefile.append(".amps.txt");
	ampList.writeText(writefile);
	Transcriber::ampSegment(pitchList, ampList);
	/*
		realvec foo = pitchList;
		foo /= 100.0;
		foo.writeText("foo.txt");
	//	Transcriber::pitchSegment(pitchList, ampList);
	//	cout<<ampList;

		for (int i=0; i<pitchList.getRows(); i++)
		{
			pitchList.getRow(i, foo);
			cout<<"********* "<<i<<endl;
			cout<<foo;
		}
	*/


}

