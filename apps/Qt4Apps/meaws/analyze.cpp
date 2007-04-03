#include <iostream>
#include <fstream>
#include <math.h>
#include "analyze.h"

Analyze::Analyze(string audioFileName, string exerciseFileName) {
	if ( exerciseFileName != "" ) {
		getExercise(exerciseFileName);
		PITCH_CORRECT=true;
	} else {
		PITCH_CORRECT=false;
	}
	outputFileName = audioFileName;
	outputFileName.append(".analysis.txt");
	getPitches(audioFileName);
//	cout<<numPitches<<endl;

	detected = realvec(numPitches,9);  // this is way overkill; we
// probably only need numPitches/100 or so.  But in the worst case
// we'll need O(n), so that's what I'll allocate here, to avoid
// having to allocate more memory later. -gp
}

Analyze::~Analyze() {
	delete exercise;
	detected.~realvec();
	pitchList.~realvec();
}

void Analyze::calcDurations() {
	int MEAN_RADIUS = 20.0;
	float prevNote=0.0;
	float median;
	int i;
	int detectedIndex=0;
	int prevSamp=0;
	for (i=MEAN_RADIUS; i<numPitches-MEAN_RADIUS; i++) {
		median = findMedian(i-MEAN_RADIUS, 2*MEAN_RADIUS, pitchList);
		if ( fabs(median-prevNote) > 0.5) {
			if (i>prevSamp+MEAN_RADIUS) {
//				cout<<"---: "<<i<<" "<<prevNote<<" "<<median<<endl;
				prevNote = median;
				prevSamp = i;
				detected(detectedIndex,0)=i;
				detectedIndex++;
			}
			else {
				prevNote = median;
				prevSamp = i;
			}
		}
//		cout<<i<<" "<<pitchList(i)<<" "<<median<<endl;
//		if ( detected(detectedIndex-1) == i)
//			cout<<"------------"<<endl;
	}
	detected(detectedIndex,0) = numPitches;
	for (i=detectedIndex; i<detected.getRows(); i++) {
		detected(i,0) = -1;
	}
}

void Analyze::getPitches(string audioFilename) {
  MarSystemManager mng;
  MarSystem* pnet = mng.create("Series", "pnet");

  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
	pnet->addMarSystem(mng.create("ShiftInput", "sfi"));
  pnet->updctrl("SoundFileSource/src/mrs_string/filename", audioFilename);
  pnet->addMarSystem(mng.create("PitchPraat", "pitch")); 
  // pnet->addMarSystem(mng.create("PitchSACF", "pitch")); 
  pnet->addMarSystem(mng.create("RealvecSink", "rvSink")); 

  mrs_real lowPitch = 36;
  mrs_real highPitch = 79;
  mrs_real lowFreq = pitch2hertz(lowPitch);
  mrs_real highFreq = pitch2hertz(highPitch);

  mrs_natural lowSamples = 
     hertz2samples(highFreq, pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal());
  mrs_natural highSamples = 
     hertz2samples(lowFreq, pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal());
 
  pnet->updctrl("PitchPraat/pitch/mrs_natural/lowSamples", lowSamples);
  pnet->updctrl("PitchPraat/pitch/mrs_natural/highSamples", highSamples);
  
  //  The window should be just long
  //  enough to contain three periods (for pitch detection) 
  //  of MinimumPitch. E.g. if MinimumPitch is 75 Hz, the window length
  //  is 40 ms and padded with zeros to reach a power of two.
  mrs_real windowSize = 3/lowPitch*pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal();
  pnet->updctrl("mrs_natural/inSamples", 512);
	// pnet->updctrl("ShiftInput/sfi/mrs_natural/Decimation", 256);
	pnet->updctrl("ShiftInput/sfi/mrs_natural/WindowSize", powerOfTwo(windowSize));
	//pnet->updctrl("ShiftInput/sfi/mrs_natural/WindowSize", 1024);

  while (pnet->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->toBool())
   pnet->tick();

	realvec data = pnet->getctrl("RealvecSink/rvSink/mrs_realvec/data")->toVec();
   for (mrs_natural i=1; i<data.getSize();i+=2)
	   data(i) = samples2hertz(data(i), pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal());
   
   pnet->updctrl("RealvecSink/rvSink/mrs_bool/done", true); 



// my addition to the marsyasTest pitch stuff:
  numPitches = data.getSize()/2;
	pitchList.allocate(numPitches);
	for (int i=0; i<numPitches; i++) {
		if ( data(2*i+1)>0 )
			pitchList(i) = hertz2pitch( data(2*i+1) );
		else
			pitchList(i) = 0;
	}
	delete pnet;
}

void Analyze::getExercise(string exerciseFilename) {
	ifstream infile;	
	infile.open( exerciseFilename.c_str() );
	infile >> exerLength;
	exercise = (int*) malloc(exerLength*sizeof(int));

	int i=0;
	while ( infile>>exercise[i] ) {
		i++;
	}
	infile.close();
}

// smooth out 0s.   Not used anymore
void Analyze::smoothPitches() {
	int i,j,k;
	float prevPitch;
	i=0;
	while (i<numPitches) {
		if (pitchList(i)==0) {
			j=i;
			while (true) {
				if (j>=numPitches) break;
				if (pitchList(j)>0) {
					if (i==0)
						prevPitch=0;
						else
						prevPitch=pitchList(i-1);
					for (k=i; k<j; k++)	{
						pitchList(k)=prevPitch;
					}
					break;
				}
				j++;
			}
			i=j;
		}
		i++;
	}
}

void Analyze::writePitches() {
	int i;
	ofstream file;
	file.open("notepitches.txt");
	for (i=0; i<numPitches; i++)
		file<<fmod(pitchList(i),12)<<endl;
	file.close();
}

void Analyze::writeNotes() {
	int i,j;
	ofstream file;
	file.open("calcNotes.txt");
	for (i=0; i<detected.getRows(); i++) {
		if ( detected(i,0) >=0 ) {
			for (j=0; j<detected.getCols(); j++) {
				file<<detected(i,j)<<" ";
			}
			file<<endl;
		}
	}
	file.close();
}

void Analyze::writeTemp(realvec temp) {
	ofstream file;
	file.open( outputFileName.c_str() );
	file<<temp;
	file.close();
}

void Analyze::writeHarmData() {
	int i,j;
	realvec temp;
	temp = realvec(numPitches,detected.getCols());

	int pos=0;
	i=0;
	while (true) {
		while (i >= detected(pos,0)) {
			pos++;
			//cout<<pos<<endl;
			if (pos >= detected.getRows()) {
				writeTemp(temp);
				temp.~realvec();
				return;
			}
		}

		if ( detected(pos,0) >=0 ) {
			temp(i,0)=i;
			for (j=1; j<detected.getCols(); j++) {
				temp(i,j) = detected(pos,j);
				//file<<detected(pos,j)<<" ";
			}
			//file<<endl;
		}
		i++;
	}
}

void Analyze::printNotes() {
	int i,j;
	for (i=0; i<detected.getRows(); i++) {
		if ( detected(i,0) >=0 ) {
			for (j=0; j<detected.getCols(); j++) {
				cout<<detected(i,j)<<" ";
			}
			cout<<endl;
		}
	}
}

mrs_real Analyze::findMedian(int start, int length, realvec array) {
//	cout<<start<<" "<<length<<endl;
	if ( !(length>0) ) return 0;
	mrs_real toReturn;
	realvec myArray;
	myArray.allocate(length);
	for (int i=0; i<length; i++) {
		myArray(i) = array(start+i);
	}
	toReturn = myArray.median();

	myArray.~realvec();
	return toReturn;
}

void Analyze::calcNotes(){

	int i, j;

	//first pass of average pitch
	int start, len;
	float sampSum;
	for (i=0; i<detected.getRows(); i++) {
		start = detected(i,0);
		if (start<0) {
			detected(i,1)=0;
		} else { 
			len = detected(i+1,0)-start;
			sampSum=0.0;
			detected(i,1) = findMedian(start, len, pitchList);
		}
	}

	//second pass
	int sampCount;
	float oldAverage;
	for (i=0; i<detected.getRows(); i++) {
		start = detected(i,0);
		if (start>0) {
			len = detected(i+1,0)-start;
			sampSum=0.0;
			sampCount=0;
			oldAverage=detected(i,1);
			for (j=start; j<start+len; j++) {
				if ( fabs( pitchList(j)-oldAverage) < 1 ) {
					sampSum+=pitchList(j);
					sampCount++;
				}
			}
			if (sampCount>0) {
				detected(i,1)=sampSum/sampCount;
			}
		}
	}
}

void Analyze::calcMultipliers(){
	int i;
	int j;
	mrs_real desired;
	mrs_real curPitch;
	// display output
	for (i=0; i<detected.getRows(); i++) {
//      calc sample postion instead of heapsize=512 pos
//			if ( detected(i,0) > 0 )
//				detected(i,0) = detected(i,0)*512.0/44100.0;
		if (PITCH_CORRECT) {
			if ((exercise[i]>0)&&(detected(i,1)>0)) {
				curPitch = pitch2hertz(detected(i,1));
				desired = pitch2hertz( exercise[2*(i+1)] );
				//cout<<desired<<endl;
				detected(i,1) = desired / curPitch;
				for (j=3; j<detected.getCols(); j = j+2) {
					if ( detected(i,j) > 0 ) {
						desired = pitch2hertz( detected(i,j) );
						detected(i,j) = desired / curPitch;
					}
				}
			} else {
				detected(i,1) = 1;
				for (j=2; j<detected.getCols(); j++) {
					detected(i,j) = 0;
				}
			}
		} else {
			if (detected(i,1)>0) {
                curPitch = pitch2hertz(detected(i,1));
				desired = curPitch;
			    detected(i,1) = desired / curPitch;
                for (j=3; j<detected.getCols(); j = j+2) {
                    if ( detected(i,j) > 0 ) {
                        desired = pitch2hertz( detected(i,j) );
                        detected(i,j) = desired / curPitch;
                    }
                }
			}
		}
	}
}

void Analyze::initHarms() {
	int i;
	// set up initial amplitudes
	for (i=0; i<detected.getRows(); i++) {
		if ( detected(i,0) >=0 ) {
				detected(i,2) = 1.0;
		}
	}
}

void Analyze::addHarmsHokey() {
	int i,j;
	mrs_real curPitch;

	// do hokey lower octave + upper tenth, for testing
	for (i=0; i<detected.getRows(); i++) {
		if ( detected(i,0)>=0 ) {
			curPitch = detected(i,1);
		cout<<curPitch;
			if ( detected(i,0) >= 0 ) {
				detected(i,3) = 0.5 * curPitch;
			cout<<" "<<curPitch - 12;
				detected(i,4) = 0.2;
				detected(i,5) = 5/2 * curPitch; // P8 + M3
			cout<<" "<<curPitch + 16;
				detected(i,6) = 0.5;
			} else {
				for (j=3; j<detected.getCols(); j++) {
					detected(i,j)=0;
				}
			}
			cout<<endl;
		}
	}
}

void Analyze::addHarmsSmooth() {
	int i;
// int j;
	mrs_real curPitch, nextPitch;
	int curMul, nextMul;
	mrs_real curHarm, nextHarm;
	int curNote;
//int nextNote;

	for (i=0; i<detected.getRows(); i++) {
		if ( detected(i,0)>=0 ) {
			//curPitch = detected(i,1);
			//nextPitch = detected(i+1,1);
			curPitch = pitch2hertz( detected(i,1) );
			nextPitch = pitch2hertz( detected(i+1,1) );
			curNote = int(round(curPitch));
		cout<<curNote<<" ";
			curMul = 1;
			nextMul = 1;
			while ( (curMul<20)&&(nextMul<20) ) {
				curHarm = curPitch * curMul;
				nextHarm = nextPitch * nextMul;
				if ( fabs((curHarm/nextHarm)-1.0) < 0.02) { // notes are close
					cout<<curHarm<<" "<<nextHarm<<" ";
					cout<<curMul<<" "<<nextMul<<" ";
					break;
				}
				if (curHarm < nextHarm)
					curMul++;
				else
					nextMul++;
			}
			while ( curHarm >= 2*curPitch) {
				curHarm /= 2;
			}
			cout<<hertz2pitch(curHarm);

			cout<<endl;
		}
	}
}

// 0 4 7: CEG
// 7 11 2: GBD
// 5 9 0 : FAC
void Analyze::addHarmsBasic() {
	int i;
	mrs_real curPitch; // in midi
	int curNote;

	for (i=0; i<detected.getRows(); i++) {
		if (( detected(i,0)>=0 )&&(detected(i,1)>0)) {
			curPitch = detected(i,1);
			curNote = int(round(curPitch));
			curNote = curNote % 12;
			if ( (curNote==0)||(curNote==4)) {
				detected(i,3) = 48;
				detected(i,4) = 0.5;  // amplitude
				detected(i,5) = 52;
				detected(i,6) = 0.2;
				detected(i,7) = 55;
				detected(i,8) = 0.2;
				//cout<<" 48 52 55"<<endl;
			}
			if ( (curNote==7)||(curNote==11)||(curNote==2)) {
      	detected(i,3) = 55;
        detected(i,4) = 0.5;  // amplitude
        detected(i,5) = 59;
        detected(i,6) = 0.2;
        detected(i,7) = 62;
        detected(i,8) = 0.2;
				//cout<<" 55 59 62"<<endl;
			}
			if ( (curNote==5)||(curNote==9)) {
      	detected(i,3) = 43;
        detected(i,4) = 0.5;  // amplitude
        detected(i,5) = 57;
        detected(i,6) = 0.2;
        detected(i,7) = 60;
        detected(i,8) = 0.2;
				//cout<<" 53 57 60"<<endl;
			}
		}
	}
}

