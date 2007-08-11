#include "Transcriber.h"

//    ./marlily filename.wav

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
	Transcriber::pitchSegment(pitchList, boundaries);
	Transcriber::ampSegment(ampList, boundaries);

	realvec notes;
	notes = Transcriber::getNotes(pitchList, ampList, boundaries);

	cout<<notes;

}

