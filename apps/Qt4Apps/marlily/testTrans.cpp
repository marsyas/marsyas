#include "Transcriber.h"

//    ./marlily filename.wav

int main(int argc, char *argv[])
{
	std::string file = argv[1];
	realvec pitchList, ampList, boundaries;
	Transcriber::getAllFromAudio(file, pitchList, ampList);
	Transcriber::toMidi(pitchList);
	std::string writefile;
	writefile = file;
	writefile.append(".pitches.txt");
	pitchList.writeText(writefile);
	writefile = file;
	writefile.append(".amps.txt");
	ampList.writeText(writefile);
	Transcriber::pitchSegment(pitchList, ampList, boundaries);
//	Transcriber::ampSegment(pitchList, ampList, boundaries);

	realvec foo;
	for (int i=0; i<pitchList.getRows(); i++)
	{
//		pitchList.getRow(i, foo);
//		cout<<"********* "<<i<<endl;
//		cout<<foo;
	}
	/*
			realvec foo = pitchList;
			foo /= 100.0;
			foo.writeText("foo.txt");
		//	cout<<ampList;
	*/
}

