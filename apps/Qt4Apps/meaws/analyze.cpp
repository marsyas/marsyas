#include <iostream>
#include <fstream>
#include <math.h>
#include "analyze.h"

#define MAXMUL 1.2
#define MINMUL 0.8

Analyze::Analyze(string audioFileName, string exerciseFileName) {
//	cout<<"in Analyze"<<endl;
	if ( exerciseFileName != "" ) {
		getExercise(exerciseFileName);
		PITCH_CORRECT=true;
	} else {
		PITCH_CORRECT=false;
	}
	outputFileName = audioFileName;
	outputFileName.erase( outputFileName.length()-4, outputFileName.length());
//	cout<<"calculating pitches"<<endl;
	getPitches(audioFileName);
//	cout<<numPitches<<endl;

	detected = realvec(numPitches,9);  // this is way overkill; we
// probably only need numPitches/100 or so.  But in the worst case
// we'll need O(n), so that's what I'll allocate here, to avoid
// having to allocate more memory later. -gp

//	cout<<"end Analyze"<<endl;
}

Analyze::~Analyze() {
	delete exercise;
	detected.~realvec();
	pitchList.~realvec();
}

void Analyze::calcNothing() {
	int i;
	for (i=0; i<numPitches; i++) {
		detected(i,0) = i;
		detected(i,1) = pitchList(i);
	}
}

double Analyze::getPitchStability() {
	mrs_real median = pitchList.median();
	mrs_real sum = 0;
	mrs_real diff;
	for (int i=0; i<pitchList.getSize(); i++) {
		diff = abs( pitchList(i)-median );
		if (diff<1.0)
			sum += diff;
	}
	//cout<<sum<<endl;
	sum = sum/pitchList.getSize();
	//cout<<sum<<endl;
	return 1.0 - sum;
}

void Analyze::calcDurations() {
	int MEAN_RADIUS = 30.0;
	float prevNote=0.0;
	float median;
	int i;
	int detectedIndex=0;
	int prevSamp=0;
	for (i=MEAN_RADIUS; i<numPitches-MEAN_RADIUS; i++) {
		median = findMedian(i-MEAN_RADIUS, 2*MEAN_RADIUS, pitchList);
		if ( fabs(median-prevNote) > 0.6) {
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
	//cout<<"size: "<<numPitches<<endl;
	pitchList.allocate(numPitches);
	for (int i=0; i<numPitches; i++) {
		if ( data(2*i+1)>0 )
			pitchList(i) = hertz2pitch( data(2*i+1) );
		else
			pitchList(i) = 0;
	}
	delete pnet;
}

realvec *Analyze::retPitches() {
	return &pitchList;
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
	string fileName = outputFileName;
	fileName.append(".pitches.txt");
	file.open(fileName.c_str());
	for (i=0; i<numPitches; i++)
		file<<pitchList(i)<<endl;
//		file<<fmod(pitchList(i),12)<<endl;
	file.close();
}

void Analyze::writeNotes() {
	int i,j;
	string fileName = outputFileName;
	fileName.append(".calcNotes.txt");
	ofstream file;
	file.open( fileName.c_str() );
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
	string fileName = outputFileName;
	fileName.append(".wav.analysis.txt");
	file.open( fileName.c_str() );
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

void Analyze::calcIndividualMultipliers(){
	int i;
	int j;
	mrs_real desired;
	mrs_real curPitch;

	// copy
  realvec orig= realvec(detected.getRows(),detected.getCols());
  for (i=0; i< detected.getRows(); i++)
    for (j=0; j< detected.getCols(); j++)
      orig(i,j) = detected(i,j);

	int pos=0;
	i=0;
	mrs_real lastVal = 0.0;
	mrs_real lastHarm1 = 0.0;
	mrs_real lastHarm2 = 0.0;
	mrs_real lastHarm3 = 0.0;
	// display output
  while (true) {
    while (i >= orig(pos,0)) {
      pos++;
      if (pos >= detected.getRows()) {
  			for (i=0; i<detected.getRows(); i++) {
					if (( detected(i,1) > 0 )&&( detected(i,1) < MAXMUL)&&(detected(i,1)>MINMUL)) {
						detected(i,2) = 1.0;
					} else {
		//				cout<<"Repeat "<<i<<endl;
						detected(i,1) = lastVal;
						detected(i,2) = 1.0;
						detected(i,3) = lastHarm1;
						detected(i,5) = lastHarm2;
						detected(i,7) = lastHarm3;
					}
					lastVal = detected(i,1);
					lastHarm1 = detected(i,3);
					lastHarm2 = detected(i,5);
					lastHarm3 = detected(i,7);
				}
      	writeTemp(detected);
        return;
      }
    }
  
		detected(i,0) = i;  
		curPitch = pitch2hertz( pitchList(i) );
		if (PITCH_CORRECT)
			desired = pitch2hertz( exercise[2*(pos+0)] );
		else
			desired = curPitch;
	//cout<<"expected pitch"<<exercise[2*(pos+0)]<<endl;
		detected(i,1) = desired / curPitch;

		if (pos>0) {
		for (j=3; j<detected.getCols(); j = j+2) {
			if ( orig(pos,j) > 0 ) {
				desired = pitch2hertz( orig(pos,j) );
				detected(i,j) = desired / curPitch;
	//cout<<"  desired: "<<desired;
				detected(i,j+1) = orig(pos,j+1);
	//cout<<" result: "<<detected(i,j)*curPitch;
			}
		}
		//cout<<endl;
		} else {
		for (j=3; j<detected.getCols(); j++) {
			detected(i,j) = 0;
		}
	}

//	cout<<"pos: "<<pos<<"  i: "<<i<<"  pitchList: "<<pitchList(i);
//	cout<<"  "<<desired<<"   "<<curPitch;
//	cout<<"    "<<detected(i,1);
//	cout<<endl;
		i++;
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
				detected(i,1) = desired / curPitch;
			cout<<"orig: "<<curPitch<<"   "<<desired<<"  "<<detected(i,1)<<endl;
				//for (j=3; j<detected.getCols(); j = j+2) {	
				for (j=3; j<4; j = j+2) {	
					if ( detected(i,j) > 0 ) {
						desired = pitch2hertz( detected(i,j) );
		//				detected(i,j) = desired / curPitch;
						detected(i,j) = 0.0;
					}
					detected(i,5)=0;
					detected(i,6)=0;
					detected(i,7)=0;
					detected(i,8)=0;
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

void Analyze::screwJazz() {
	int i;

	for (i=0; i<detected.getRows(); i++ ) {
		if ((i>50)&&(i<100)) {
			detected(i,1) = detected(i,1)*0.94387431268;
		}
		if ((i>300)&&(i<500)) {
			detected(i,1) = detected(i,1)*0.5;
		}
		if ((i>550)&&(i<770)) {
			detected(i,1) = detected(i,1)*1.12246204831; // one tone
		}
		if ((i>1070)&&(i<1200)) {
			detected(i,1) = detected(i,1)*0.89089871814; // one tone
		}
	}
}

void Analyze::makeMinor() {
	int i;
	int curNote;

	for (i=0; i<exerLength; i=i+2) {
		curNote = exercise[i];
		curNote = curNote % 12;
		if ( (curNote==4)||(curNote==9)||(curNote==11)) {
			exercise[i] = exercise[i]-1;
			cout<<"changed to "<<exercise[i]<<endl;
		}
	}
}
//c cis d dis e f fis g gis a ais b
//0 1   2 3   4 5 6   7 8   9 10 11  

// 0 4 7: CEG
// 7 11 2: GBD
// 5 9 0 : FAC
void Analyze::addHarmsBasic() {
	int i;
	mrs_real curPitch; // in midi
	int curNote;

	for (i=0; i<detected.getRows(); i++) {
		if (detected(i,0)>=0 ) {
		//	if (detected(i,1)>0)
		//		curPitch = detected(i,1);
		//	else
				curPitch = exercise[2*i];
		//	cout<<curPitch<<endl;
			curNote = int(round(curPitch));
			curNote = curNote % 12;
			if ( (curNote==0)||(curNote==4)) {
				detected(i,3) = 48;
				detected(i,4) = 0.7;  // amplitude
				detected(i,5) = 52;
				detected(i,6) = 0.5;
				detected(i,7) = 55;
				detected(i,8) = 0.5;
				//cout<<" 48 52 55"<<endl;
			}
			if ( (curNote==7)||(curNote==11)||(curNote==2)) {
      	detected(i,3) = 55;
        detected(i,4) = 0.7;  // amplitude
        detected(i,5) = 59;
        detected(i,6) = 0.5;
        detected(i,7) = 62;
        detected(i,8) = 0.5;
				//cout<<" 55 59 62"<<endl;
			}
			if ( (curNote==5)||(curNote==9)) {
      	detected(i,3) = 43;
        detected(i,4) = 0.7;  // amplitude
        detected(i,5) = 57;
        detected(i,6) = 0.5;
        detected(i,7) = 60;
        detected(i,8) = 0.5;
				//cout<<" 53 57 60"<<endl;
			}
	// check that it matches something
			if ( detected(i,3)==0 ){
				cout<<"NOT FOUND for frame "<<i<<" which is "<<detected(i,1)<<endl;
			}
		}
	}
}

