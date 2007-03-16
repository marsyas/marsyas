#include <iostream>
#include <fstream>
#include <math.h>
#include "analyze.h"

Analyze::Analyze() {
//	exercise = (int*) malloc (16*sizeof(int));
//	exercise = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
}

Analyze::~Analyze() {
	delete exercise;
}

void Analyze::metroDurations() {
	int SIZE=18;
	float SAMPLES_PER_SECOND = 261.625565;  // works with pitch-to-pratt.py
	int TEMPO=120/60;

// pitch, dur
	int exer[] = {0, 4, 60, 2, 62, 1, 64, 1, 65, 2, 67, 2, 69, 3, 71, 1, 72, 4, 0, 0 };
	int i;
	int samp;

	cout<<"metronome Durations:"<<endl;
	samp=0;
	int deltasamp;
	int pitch;
	for (i=0; i<=SIZE/2; i++) {
		deltasamp = SAMPLES_PER_SECOND/TEMPO * exer[2*i+1];
		samp += deltasamp;
		pitch = exer[2*i];
		cout<<samp << "   "<<deltasamp<<"   "<<pitch<<endl;
	}
}

void Analyze::calcDurations() {
	cout<<"detected Durations:"<<endl;

	ifstream inFile;
	inFile.open("notepitches.txt");
	float curPitch;
	//int sample=0;
	float prevPitch=0;
	int prevSamp=0;
	float pitchList[5000];  // something big enough; ugly hack
	float avg1, avg2;
	float variance1;
	float variance2;

	int i,j,k;

	// load pitches into memory
	i=0;
	while (inFile>>curPitch) {
		pitchList[i]=curPitch;
		i++;
	}
	inFile.close();
	int maxSamps = i;
/*
	cout<<"---------original:"<<endl;
	for (i=0; i<maxSamps; i++) {
		cout<<pitchList[i]<<endl;
	}
*/

	// smooth out 0s.
	i=0;
	while (i<maxSamps) {
		if (pitchList[i]==0) {
			j=i;
			while (true) {
				if (j>=maxSamps) break;
				if (pitchList[j]>0) {
					if (i==0)
						prevPitch=0;
						else
						prevPitch=pitchList[i-1];
					for (k=i; k<j; k++)	{
						pitchList[k]=prevPitch;
					}
					break;
				}
				j++;
			}
			i=j;
		}
		i++;
	}

/*
	cout<<"---------processed:"<<endl;
	for (i=0; i<maxSamps; i++) {
		cout<<pitchList[i]<<endl;
	}
*/
	
	ofstream file;
	file.open("detected.txt");
	//int pitch;
	int next=0;
	float AVERAGE_OVER = 5.0;
	prevSamp=0;
	for (i=AVERAGE_OVER; i<maxSamps-AVERAGE_OVER; i++) {
		avg1=0.0;
		avg2=0.0;
		variance1=0.0;
		variance2=0.0;
		for (j=0; j<AVERAGE_OVER; j++) {
			avg1 += pitchList[i-j-1];
			avg2 += pitchList[i+j];
			if (j < (AVERAGE_OVER-1) )
				variance1+= fabs(pitchList[i-j-1] - pitchList[i-j-2]);
			if (j < (AVERAGE_OVER-1) )
				variance2+= fabs(pitchList[i+j] - pitchList[i+j+1]);
		}
		avg1 = avg1/AVERAGE_OVER;
		avg2 = avg2/AVERAGE_OVER;
		variance1 = variance1 / (AVERAGE_OVER-1);
		variance2 = variance2 / (AVERAGE_OVER-1);

		if ((fabs(avg1-avg2) > 0.5)&&(fabs(pitchList[i]-pitchList[prevSamp])>0.5)) {
			//if ( (variance1>0) || (variance2>0)) // HACK: works in real life.
			if (i>prevSamp+4*(AVERAGE_OVER)) next=i;
		}
		if (next>0) {
			if ((variance1<0.4)&&(variance2<0.4)) {
				//prevSamp = int(i+AVERAGE_OVER);
				prevSamp = int(i);
//				cout<<pitchList[ prevSamp-1]<<endl;
				cout<<"---------------------------"<<endl;
				cout<<i<<"   "<<pitchList[ prevSamp ]<<" was a new pitch"<<endl;
				file<<i<<" "<<73<<endl;
//				cout<<pitchList[ prevSamp+1]<<endl;
				next=0;
			}
		}
		cout<<i<<" "<<pitchList[i];
		cout<<"   "<<avg1<<" "<<avg2<<"   "<<i - prevSamp<<"   "<<"   "<<variance1<<" "<<variance2<<endl;
	//	if (pitchList[i]>61) exit(0);
//zz

	}
	file.close();
}

void Analyze::getPitches(string filename) {
  MarSystemManager mng;
  MarSystem* pnet = mng.create("Series", "pnet");

  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
	pnet->addMarSystem(mng.create("ShiftInput", "sfi"));
  pnet->updctrl("SoundFileSource/src/mrs_string/filename", filename);
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
	
	ofstream file;
	file.open("notepitches.txt");
   for (mrs_natural i=1; i<data.getSize();i+=2)
			if ( data(i)>0 )
				file<<hertz2pitch( data(i) )<<endl;
			else
				file<<0<<endl;
	file.close();
	//zz

	//cout << data ;
	delete pnet;
}

void Analyze::writePitches(string filename) {
	string command;
	command = "python2.4 praat-to-pitch.py ";
	command.append(filename);
	command.append(" 120");  // tempo
	cout<<"DOING: "<<command.c_str()<<endl;
	system(command.c_str());
}

void Analyze::calcNotes(){
	string command;
// please don't look at this code.  I feel embarrassed for having
// written it, and it will be rewritten next weekend.
	float notepitch;
	int note;
	int tatum;
	float sumPitch;
	float avgPitch;
	float pitchError;
	int expected_pitch[8] = {60,62,64,65,67,69,71,72};
	int expected_duration[8] = {2,1,1,2,2,3,1,4};
	note=0;
	ifstream inFile;
	inFile.open("notepitches.txt");
//	ofstream outFile("toperl.txt",ios::out);
	sumPitch=0;
	tatum=0;
	while (inFile >> notepitch) {
// do whatever I want with the note data.
		tatum++;

// I feel so dirty... AND RIGHTLY SO!
		if ((note==0) && (tatum==4)) { tatum=0; } else
		if (notepitch>0) {
		//if (tatum > (2*2-1)) {  // intro beats.  DOESN'T WORK!
			sumPitch += notepitch;
		//	cout<<notepitch<<"   "<<note<<"  "<<tatum<<endl;
			if (tatum>=expected_duration[note]) {
				avgPitch = (sumPitch / tatum);
				pitchError = avgPitch - expected_pitch[note];
	//			cout<<note<<"  "<<pitchError;
	//			cout<<"   "<<avgPitch<<"  "<<expected_pitch[note]<<endl;
//				outFile<<pitchError<<endl;
				//emit nextNoteError(pitchError,1);
				sumPitch=0;
				tatum=0;
				note++;
				if (note>=8) break;
			}
		}
	}
	inFile.close();
//	outFile.close();
//	command = "perl color-aud-output.pl toperl.txt ";
//	system(command.c_str());
}


