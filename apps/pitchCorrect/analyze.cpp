//  TEMPORARY -- these functions will soon be implemented as
//  MarSystems.

#include <iostream>
#include <fstream>
#include <math.h>
#include "analyze.h"

Analyze::Analyze() {
	median_radius = 10.0;
	new_note_midi = 0.6;
	pitch_certainty_div = 500;

// for display
	IMPULSE_HIGHT = 75;
// not used right now
	// tries to filter out octave errors
	LOWEST_NOTE = 59;
//	HIGHEST_NOTE = 99;
	HIGHEST_NOTE = 74;
}

Analyze::~Analyze() {
	pitchList.~realvec();
	notes.~realvec();
/*
	delete exercise;
	detected.~realvec();
*/
}

void
Analyze::setOptions(mrs_real getRadius, mrs_real getNewNote, mrs_real getCertantyDiv)
{
	median_radius = getRadius;
	new_note_midi = getNewNote;
	pitch_certainty_div = getCertantyDiv;
}

void Analyze::loadData(string inputFilename) {
	int length = inputFilename.length();
	string extension = inputFilename.substr(length-4,length);
	if ( extension == ".wav" ) {
		cout<<"Loading audio file... ";
		getPitchesFromAudio(inputFilename);
	} else {
		cout<<"Loading text (realvec) file... ";
		getPitchesFromRealvecText(inputFilename);
	}
	cout<<"done."<<endl;
	outputFilename = inputFilename;
	outputFilename.erase( outputFilename.length()-4, outputFilename.length() );
}

void Analyze::clearPitches() {
	pitchList.~realvec();
}

// yuck.  I know this is bad.
float fretToPitch( int fret ) {
	// Ma - F# 
	if ((fret > 0) && (fret < 5))
	  {
	    //Silence
			return 0.0;
	  }

	// Tivra Ma - G
	else if ((fret > 5) && (fret < 10))
	  {
	    return hertz2pitch(195.998);  
	  }
	
	// Pa - G# 
	else if ((fret > 10) && (fret < 15))
	  {
	    return hertz2pitch(207.652);  
	  }

	// Komal Dha - A
	else if ((fret > 15) && (fret < 20))
	  {
	    return hertz2pitch(220.000);  
	  }
	
	// Dha - A# 
	else if ((fret > 20) && (fret < 24))
	  {
	    return hertz2pitch(233.082);  
	  }

	// Komal Ni - B
	else if ((fret > 24) && (fret < 28))
	  {
	    return hertz2pitch(246.942);  
	  }
	
	// Ni - C 
	else if ((fret > 28) && (fret < 36))
	  {
	    return hertz2pitch(261.626);  
	  }
	
	// Sa - C#
	else if ((fret > 35) && (fret < 42))
	  {
	    return hertz2pitch(277.183);    
	  }

	// Re - D# 
	else if ((fret > 41) && (fret < 50))
	  {
	    return hertz2pitch(311.127);    
	  }

	// Komal Ga - E 
	else if ((fret > 49) && (fret < 58))
	  {
	    return hertz2pitch(329.628);     
	  }
	
	// Ga - F 
	else if ((fret > 57) && (fret < 66))
	  {
	    return hertz2pitch(349.228);    
	  }

	// Ma - F# 
	else if ((fret > 65) && (fret < 75))
	  {
	    return hertz2pitch(369.994);    
	  }

	// Tivra Ma - G 
	else if ((fret > 74) && (fret < 83))
	  {
	    return hertz2pitch(391.995);    
	  }
	
	// Pa - G# 
	else if ((fret > 82) && (fret < 91))
	  {
	    return hertz2pitch(415.305);    
	  }
	
	// Dha - A# 
	else if ((fret > 90) && (fret < 99))
	  {
	    return hertz2pitch(466.164);    
	  }

	// Komal Ni - B 
	else if ((fret > 98) && (fret < 106))
	  {
	    return hertz2pitch(493.883);    
	  }
	
	// Ni - C
	else if ((fret > 105) && (fret < 112))
	  {
	    return hertz2pitch(523.251);    
	  }

	// Sa - C#
	else if ((fret > 111) && (fret < 117))
	  {
	    return hertz2pitch(554.4);    
	  }
	
	// Re - D#
	else if ((fret > 116) && (fret < 122))
	  {
	    return hertz2pitch(622.3);    
	  }

	// Ga - F
	else if ((fret > 121) && (fret < 125))
	  {
	    return hertz2pitch(698.5);    
	  }
	
	// Ma - F#
	else if ((fret > 124) && (fret < 128))
	  {
	    return hertz2pitch(740.0);    
	  }
	return 0.0;
}

void Analyze::realvecFileToPlain(string textFilename) {
	realvec blah;
	ifstream infile;	
	infile.open( textFilename.c_str() );
	infile >> blah;
	infile.close();

	ofstream file;
	string filename = outputFilename;
	filename.append(".other.txt");
	cout<<"Writing realvec to "<<filename<<endl;
	file.open( filename.c_str() );
	for (int i=0;i< blah.getSize(); i++) {
		if (blah(i) != 0) {
			file<<blah(i);
			file<<"\t";
			file<<IMPULSE_HIGHT<<endl;
		}
	}
	file.close();
}

void Analyze::getPitchesFromRealvecText(string textFilename) {
	pitchList.~realvec();
	pitchList.read( textFilename);

	for (int i=0; i<pitchList.getSize(); i++) {
		pitchList(i) = fretToPitch( pitchList(i) );
		if (pitchList(i) == 0)
			pitchList(i) = 51;
// we can trust the fret data
/*
		if (pitchList(i) < LOWEST_NOTE)
			pitchList(i) = pitchList(i)+12;
		if (pitchList(i) > HIGHEST_NOTE)
			pitchList(i) = pitchList(i)-12;
*/
	}
}

void Analyze::getPitchesFromAudio(string audioFilename) {
  MarSystemManager mng;
  MarSystem* pnet = mng.create("Series", "pnet");

  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
	pnet->addMarSystem(mng.create("ShiftInput", "sfi"));
  pnet->updctrl("SoundFileSource/src/mrs_string/filename", audioFilename);
  pnet->addMarSystem(mng.create("PitchPraat", "pitch")); 
  pnet->addMarSystem(mng.create("RealvecSink", "rvSink")); 

  mrs_real lowPitch = 59;
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
	int i;
	mrs_real maxConf=0.0;
	for (i=0; i<data.getSize(); i=i+2) {
		if (maxConf < data(i))
			maxConf = data(i);
	}

	pitchList.allocate( data.getSize()/2 );
	for (i=0; i<pitchList.getSize(); i++) {
		if ( data(2*i+1) > 0 )
			if (data(2*i) > maxConf/pitch_certainty_div)
				pitchList(i) = hertz2pitch( data(2*i+1) );
			else
				pitchList(i) = 0.0;
		else
			pitchList(i) = 0.0;
	}
/*
	for (i=0; i<pitchList.getSize(); i++) {
		if (pitchList(i) < LOWEST_NOTE)
			pitchList(i) = pitchList(i)+12;
		if (pitchList(i) > HIGHEST_NOTE)
			pitchList(i) = pitchList(i)-12;
	}
*/
	delete pnet;
}

void Analyze::getExpectedNotes(string expectedNotesFilename) {
	ifstream infile;	
	infile.open( expectedNotesFilename.c_str() );
/*
	infile >> exerLength;
	exercise = (int*) malloc(exerLength*sizeof(int));

	int i=0;
	while ( infile>>exercise[i] ) {
		i++;
	}
*/
	infile.close();
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

void Analyze::writePitches() {
//	int i;
	string filename = outputFilename;
	filename.append(".pitches.txt");
	cout<<"Writing pitches to "<<filename<<endl;
	pitchList.writeText(filename);
/*
	ofstream file;
	file.open( filename.c_str() );
	for (i=0; i<pitchList.getSize(); i++)
		file<<pitchList(i)<<endl;
//		file<<fmod(pitchList(i),12)<<endl;
	file.close();
*/
}

void Analyze::writeOnsets() {
	int i;
	ofstream file;
	string filename = outputFilename;
//	filename = "onsets.txt";
	filename.append(".onsets.txt");
	cout<<"Writing onsets to "<<filename<<endl;
	file.open( filename.c_str() );
  for (i=0; i<onsets.getSize(); i++) {
    file<<onsets(i);
		file<<"\t";
		file<<IMPULSE_HIGHT;
		file<<endl;
  }
	file.close();
}

void Analyze::writeNotes() {
	int i;
	ofstream file;
	string filename = outputFilename;
//	filename = "onsets.txt";
	filename.append(".notes.txt");
	cout<<"Writing notes to "<<filename<<endl;
	file.open( filename.c_str() );
  for (i=0; i<onsets.getSize(); i++) {
    file<<onsets(i);
		file<<"\t";
		if (notes(i)>0)
			file<<notes(i);
		else
			file<<'0';
		file<<endl;
  }
	file.close();
}

void Analyze::calcOnsets() {
	onsets.allocate(1);
	onsets.stretchWrite(0, 0.0);
	int i;

	float median;
	float prevNote=0.0;
	int durIndex=1;
	int prevSamp=0;
	for (i=median_radius; i<pitchList.getSize()-median_radius; i++) {
		median = findMedian(i-median_radius, 2*median_radius, pitchList);
		if ( fabs(median-prevNote) > new_note_midi ) {
			if (i>prevSamp+median_radius) {
//				cout<<"---: "<<i<<" "<<prevNote<<" "<<median<<endl;
				prevNote = median;
				prevSamp = i;
				onsets.stretchWrite( durIndex, i);
				durIndex++;
			}
			else {
				prevNote = median;
				prevSamp = i;
				onsets(durIndex) = i;
			}
		}
	}
	onsets.stretchWrite(durIndex, pitchList.getSize() );
	onsets.stretch(durIndex+1);
}

void Analyze::calcNotes(){
	notes.allocate( onsets.getSize() );
	notes.stretchWrite(0, 0.0);
  int i, j;

  //first pass of average pitch
  int start, len;
	mrs_real pitch;
  for (i=0; i<onsets.getSize(); i++) {
    start = onsets(i);
    len = onsets(i+1) - start;
    pitch = findMedian(start, len, pitchList);
		notes.stretchWrite(i, pitch);
  }

	int totalFrames;
	int correctFrames;
	float percentageCorrect;
	float correctSum;
	// second pass: percentage of close pitches
	// TODO: check array boundaries
  for (i=0; i<onsets.getSize()-1; i++) {
		totalFrames = 0;
		correctFrames = 0;
		correctSum = 0.0;
		for (j=onsets(i); j<onsets(i+1); j++) {
			if ( fabs( pitchList(j) - notes(i) < 0.1 )) {
				correctFrames++;
				correctSum += notes(i);
			}
			totalFrames++;
		}
		percentageCorrect = (float) correctFrames / totalFrames;
		if (percentageCorrect < 0.8) {
			notes(i) = IMPULSE_HIGHT;
/*
			if (i>0)
				notes(i) = notes(i-1);
			else
				notes(i) = 0;
*/
		} else {
			notes(i) = round( correctSum / correctFrames );
		}
//zz
		//cout<<percentageCorrect<<endl;
	}

// FIXME*2: we're doing this a different way for the sitar; need to
//    figure out if the sitar-method works for violin as well.
//   FIXME: implement this.  So far there's been no need because
//            the onset detection has been sucking so hard.  :(
/*
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
*/
}


