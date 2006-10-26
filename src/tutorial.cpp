// This executable contains various tutorial-style functions 
// with somewhat more extensive comments that hopefully 
// help in understanding how to use the Marsyas audio 
// processing software framework. 

#include <cstdio>
#include "MarSystemManager.h" 
#include <string>

using namespace std;
using namespace Marsyas;


// 
// basic approach - similar to Matlab or hand-crafted code 
 
void 
sfplay1(string sfName)
{
  cout << "sfplay1: Playing " << sfName << endl;
  
  // create a soundfilesource for reading samples 
  // from the file 
  MarSystem* src = new SoundFileSource("src");
  src->updctrl("mrs_string/filename", sfName);
  src->updctrl("mrs_natural/inSamples", 2048);
  
  // simple MarSystem a gain control 
  MarSystem* gain = new Gain("gain");
  gain->updctrl("mrs_real/gain", 2.0);
  gain->updctrl("mrs_natural/inSamples", 2048);  
  
  // SoundFileSink writes to a file 
  MarSystem* dest = new SoundFileSink("dest");
  dest->updctrl("mrs_natural/inSamples", 2048);  
  dest->updctrl("mrs_string/filename", "ajay.au");
  
  // write system to output 
  cout << (*src) << endl;
  
  // create two matrices for input/output
  realvec in; 
  realvec out;
  realvec gout;
  
  in.create(src->getctrl("mrs_natural/inObservations").toNatural(), 
	    src->getctrl("mrs_natural/inSamples").toNatural());
  
  out.create(src->getctrl("mrs_natural/onObservations").toNatural(), 
	     src->getctrl("mrs_natural/onSamples").toNatural());
  
  gout.create(src->getctrl("mrs_natural/onObservations").toNatural(), 
	      src->getctrl("mrs_natural/onSamples").toNatural());
  
  // nothing happens until process is called 
  // usually for MarSystems the in matrix is processed 
  // and the result is written in the out matrix. 
  // For SoundFileSources the input is ignored 
  // and the out matrix is updated from 
  // reading the samples 
  // from the file 
  
  gain->updctrl("mrs_real/gain", 0.0);
  
  cout << (*gain) << endl;
  for (int i=0; i < 20; i++)
    {
      // call process seperately for each MarSystem 
      src->process(in,out);
      // multiply window with gain 
      gain->updctrl("mrs_real/gain", 
		    gain->getctrl("mrs_real/gain").toReal() + 0.1);
      gain->process(out, gout);
      dest->process(gout, gout); 	// Just outputs the input to the file 
      // and copies it to the output
    }
  
  
  delete src;
  delete gain;
  delete dest;
  
}



//Pluck Karplus Strong Model Kastro.cpp output to wavfile
void 
Pluck(mrs_real pos, mrs_real fre, mrs_real loz, mrs_real stret, string name)
{
 

  MarSystemManager mng;
  
  MarSystem* series = mng.create("Series", "series");
  
  
  series->addMarSystem(mng.create("Plucked", "src"));
  series->addMarSystem(mng.create("Gain", "gain"));
 
  series->addMarSystem(mng.create("SoundFileSink", "dest"));

  series->update();
  
  

  
  series->updctrl("Gain/gain/mrs_real/gain", 1.0);



  series->updctrl("SoundFileSink/dest/mrs_natural/nChannels", 
  series->getctrl("Plucked/src/mrs_natural/nChannels"));
  series->updctrl("SoundFileSink/dest/mrs_real/israte", 
   series->getctrl("Plucked/src/mrs_real/osrate"));
  series->updctrl("SoundFileSink/dest/mrs_string/filename",name);

  series->updctrl("Plucked/src/mrs_real/frequency",fre);
 series->updctrl("Plucked/src/mrs_real/pluckpos",pos);
 // series->updctrl("Plucked/src/mrs_real/loss",loz);
 //series->updctrl("Plucked/src/mrs_real/stretch",stret);
  
  series->updctrl("mrs_natural/inSamples", 512);
  series->update();
  


  
  cout << (*series) << endl;


 realvec in(series->getctrl("mrs_natural/inObservations").toNatural(), 
	     series->getctrl("mrs_natural/inSamples").toNatural());
  realvec out(series->getctrl("mrs_natural/onObservations").toNatural(), 
	      series->getctrl("mrs_natural/onSamples").toNatural());

  mrs_natural t=0;
  
 
	 for (t = 0; t < 400; t++)
    {
	 series->process(in,out);
	 t++;
		
	}	      

 


  
  //while (series->getctrl("SoundFileSource/src/mrs_bool/notEmpty").toBool())
  //{
  //  series->tick();
  //}
  
  
}


//Pluck Karplus Strong Model Plucked.cpp outputs to DAC
void PluckLive(mrs_real pos, mrs_real fre, mrs_real loz, mrs_real stret)
{
 

  MarSystemManager mng;
  
  MarSystem* series = mng.create("Series", "series");
  
  
  series->addMarSystem(mng.create("Plucked", "src"));
  series->addMarSystem(mng.create("Gain", "gain"));
  series->addMarSystem(mng.create("AudioSink", "dest"));
 

  series->update();
  
  

  
  series->updctrl("Gain/gain/mrs_real/gain", 1.0);

  series->updctrl("AudioSink/dest/mrs_natural/nChannels", 
  series->getctrl("Plucked/src/mrs_natural/nChannels"));
series->updctrl("AudioSink/dest/mrs_real/israte", 
series->getctrl("Plucked/src/mrs_real/osrate"));



  series->updctrl("Plucked/src/mrs_real/frequency",fre);
 series->updctrl("Plucked/src/mrs_real/pluckpos",pos);
 //series->updctrl("Plucked/src/mrs_real/loss",loz);
 //series->updctrl("Plucked/src/mrs_real/stretch",stret);
  
  series->updctrl("mrs_natural/inSamples", 512);
  series->update();
  


  
  cout << (*series) << endl;


 realvec in(series->getctrl("mrs_natural/inObservations").toNatural(), 
	     series->getctrl("mrs_natural/inSamples").toNatural());
  realvec out(series->getctrl("mrs_natural/onObservations").toNatural(), 
	      series->getctrl("mrs_natural/onSamples").toNatural());

  mrs_natural t=0;
  
 
	 for (t = 0; t < 400; t++)
    {
	 series->process(in,out);
	 t++;
		
	}	      

 


  
  //while (series->getctrl("SoundFileSource/src/mrs_bool/notEmpty").toBool())
  //{
  //  series->tick();
  //}
  
  
}




 void 
 wavsfplay(string sfName)
 {
   cout << "wavsfplay1: Playing " << sfName << endl;

   // create a soundfilesource for reading samples 
   // from the file 
   MarSystem* src = new SoundFileSource("src");
   src->updctrl("mrs_string/filename", sfName);
   src->updctrl("mrs_natural/inSamples", 2048);

   // simple MarSystem a gain control 
   MarSystem* gain = new Gain("gain");
   gain->updctrl("mrs_real/gain", 2.0);
   gain->updctrl("mrs_natural/inSamples", 2048);  

   // SoundFileSink writes to a file 
   MarSystem* dest = new SoundFileSink("dest");
   dest->updctrl("mrs_natural/inSamples", 2048);  
   dest->updctrl("mrs_real/israte", src->getctrl("mrs_real/osrate"));
   dest->updctrl("mrs_natural/nChannels", src->getctrl("mrs_natural/nChannels"));
   dest->updctrl("mrs_string/filename", "ajay.wav");

   // write system to output 
   cout << (*src) << endl;

   // create two matrices for input/output
   realvec in; 
   realvec out;
   realvec gout;

   in.create(src->getctrl("mrs_natural/inObservations").toNatural(), 
	     src->getctrl("mrs_natural/inSamples").toNatural());

   out.create(src->getctrl("mrs_natural/onObservations").toNatural(), 
	      src->getctrl("mrs_natural/onSamples").toNatural());

   gout.create(src->getctrl("mrs_natural/onObservations").toNatural(), 
	       src->getctrl("mrs_natural/onSamples").toNatural());

   // nothing happens until process is called 
   // usually for MarSystems the in matrix is processed 
   // and the result is written in the out matrix. 
   // For SoundFileSources the input is ignored 
   // and the out matrix is updated from 
   // reading the samples 
   // from the file 

   gain->updctrl("mrs_real/gain", 1.0);

   while (src->getctrl("mrs_bool/notEmpty").toBool())
     {
       src->process(in,out);
       gain->process(out, gout);
       dest->process(gout, gout); 	
     }


   delete src;
   delete gain;
   delete dest;

 }



 void 
 wavsfplay1(string sfName)
 {
   cout << "wavsfplay1: Playing " << sfName << endl;

   MarSystemManager mng;

   MarSystem* series = mng.create("Series", "series");


   series->addMarSystem(mng.create("SoundFileSource", "src"));
   series->addMarSystem(mng.create("Gain", "gain"));
   series->addMarSystem(mng.create("SoundFileSink", "dest"));

   series->updctrl("SoundFileSource/src/mrs_string/filename", sfName);


   series->updctrl("Gain/gain/mrs_real/gain", 1.0);
   series->updctrl("SoundFileSink/dest/mrs_natural/nChannels", 
		   series->getctrl("SoundFileSource/src/mrs_natural/nChannels"));
   series->updctrl("SoundFileSink/dest/mrs_real/israte", 
		   series->getctrl("SoundFileSource/src/mrs_real/osrate"));

   series->updctrl("mrs_natural/inSamples", 2048);  
   series->updctrl("SoundFileSink/dest/mrs_string/filename", "ajay.wav");


   cout << (*series) << endl;

   while (series->getctrl("SoundFileSource/src/mrs_bool/notEmpty").toBool())
     {
       series->tick();
     }


 }






 // use Composite 
void 
sfplay2(string sfName)
{
  cout << "sfplay2: Playing " << sfName << endl;
  
  // Create a series Composite 
  MarSystem* series = new Series("series");
  
  // create a soundfilesource for reading samples 
  // from the file 
  MarSystem* src = new SoundFileSource("src");
  src->updctrl("mrs_string/filename", sfName);
  src->updctrl("mrs_natural/inSamples", 2048);
  
  // simple MarSystem a gain control 
  MarSystem* gain = new Gain("gain");
  gain->updctrl("mrs_real/gain", 2.0);
  gain->updctrl("mrs_natural/inSamples", 2048);  
  
  // SoundFileSink writes to a file 
  MarSystem* dest = new SoundFileSink("dest");
  dest->updctrl("mrs_natural/inSamples", 2048);  
  dest->updctrl("mrs_string/filename", "ajay.au");
  
  // add MarSystems to series Composite 
  series->addMarSystem(src);
  series->addMarSystem(gain);
  series->addMarSystem(dest);
  
  
  cout << (*series) << endl;
  
  // create two matrices for input/output
  realvec in; 
  realvec out;
  
  in.create(series->getctrl("mrs_natural/inObservations").toNatural(), 
	    series->getctrl("mrs_natural/inSamples").toNatural());
  
  out.create(series->getctrl("mrs_natural/onObservations").toNatural(), 
	     series->getctrl("mrs_natural/onSamples").toNatural());
  
  // nothing happens until process is called 
  // usually for MarSystems the in matrix is processed 
  // and the result is written in the out matrix. 
  // For SoundFileSources the input is ignored 
  // and the out matrix is updated from 
  // reading the samples 
  // from the file 
  
  // when using a Composite you need to call 
  // process only once without having to 
  // provide buffers for the intermediate steps 
  
  // when using a composite controls of internal 
  // MarSystems can be updated using a OSC-inspired 
  // path notation 
  
  series->updctrl("Gain/gain/mrs_real/gain", 0.0);
  
  for (int i=0; i<20; i++)
    {
      series->updctrl("Gain/gain/mrs_real/gain", 
		      series->getctrl("Gain/gain/mrs_real/gain").toReal() + 0.1);      
      series->process(in,out);  
      
    }
  
  // Composite deletes the added MarSystems 
  // so you must not delete them 
  delete series;
}


// take advantage of Composite 
void 
sfplay3(string sfName)
{
  cout << "sfplay3: Playing " << sfName << endl;
  
  // get rid of some stuff taking advantage of 
  // composite 
  
  // Create a series Composite 
  MarSystem* series = new Series("series");
  
  // create a soundfilesource for reading samples 
  // from the file 
  MarSystem* src = new SoundFileSource("src");
  src->updctrl("mrs_string/filename", sfName);
  
  // simple MarSystem a gain control 
  MarSystem* gain = new Gain("gain");
  gain->updctrl("mrs_real/gain", 2.0);
  
  // SoundFileSink writes to a file 
  MarSystem* dest = new SoundFileSink("dest");
  dest->updctrl("mrs_string/filename", "ajay.au");
  
  // add MarSystems to series Composite 
  series->addMarSystem(src);
  series->addMarSystem(gain);
  series->addMarSystem(dest);
  
  // we only need to change window size at the composite level
  series->updctrl("mrs_natural/inSamples", 4096);
  
  cout << (*series) << endl;
  
  // tick method basically calls process 
  // with an empty input buffer and an empty output buffer 
  // because soundfilesource/sink read/write to files 
  // as sideeffects this works 
  
  
  series->updctrl("Gain/gain/mrs_real/gain", 0.0);
  for (int i=0; i<20; i++)
    {
      series->tick();
      series->updctrl("Gain/gain/mrs_real/gain", 
		      series->getctrl("Gain/gain/mrs_real/gain").toReal() + 0.1);
    }
  
  // Composite deletes the added MarSystems 
  // so you must not delete them 
  delete series;
}



 // take advantage of MarSystemManager 
 void 
 sfplay4(string sfName)
 {
   cout << "sfplay4: Playing " << sfName << endl; 

   MarSystemManager mng;

   // Create a series Composite 
   MarSystem* series = mng.create("Series", "series");
   series->addMarSystem(mng.create("SoundFileSource", "src"));
   series->addMarSystem(mng.create("Gain", "gain"));
   series->addMarSystem(mng.create("SoundFileSink", "dest"));

   // only update controls from Composite level 
   series->updctrl("mrs_natural/inSamples", 128);
   series->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
   series->updctrl("SoundFileSink/dest/mrs_string/filename", "ajay.wav"); 
   
   series->linkctrl("mrs_natural/gain", "Gain/gain/mrs_real/gain");
   while (series->getctrl("SoundFileSource/src/mrs_bool/notEmpty").toBool())
     {
       series->updctrl("mrs_natural/gain", 2.0);
       series->tick();
     }
  
  
  delete series;
}



// take advantage of MarSystemManager 
void 
tempotest_sfplay(string sfName)
{
  cout << "Playing " << sfName << endl; 

  MarSystemManager mng;

  // Create a series Composite 
  MarSystem* series = mng.create("Series", "series");
  series->addMarSystem(mng.create("SoundFileSource", "src"));
  series->addMarSystem(mng.create("AudioSink", "dest"));
  
  // only update controls from Composite level 
  series->updctrl("mrs_natural/inSamples", 128);
  series->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  
  while (series->getctrl("SoundFileSource/src/mrs_bool/notEmpty").toBool())
    series->tick();

  delete series;
}



// add some simple feature extraction 
void 
sfplay5(string sfName)
{
  cout << "sfplay5: Playing " << sfName << endl;
  MarSystemManager mng;
  
  // Create a series Composite 
  MarSystem* series = mng.create("Series", "series");
  series->addMarSystem(mng.create("SoundFileSource", "src"));
  series->addMarSystem(mng.create("Gain", "gain"));
  series->addMarSystem(mng.create("SoundFileSink", "dest"));
  // hamming window -> complex spectrum -> power spectrum 
  series->addMarSystem(mng.create("Hamming", "hamming"));
  series->addMarSystem(mng.create("Spectrum", "spk"));
  series->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  series->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType", "power");
  series->addMarSystem(mng.create("Centroid", "centroid"));
		       
  // only update controls from Composite level 
  series->updctrl("mrs_natural/inSamples", 4096);
  series->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  series->updctrl("SoundFileSink/dest/mrs_string/filename", "ajay.au");
  
  cout << (*series) << endl;

  realvec in(series->getctrl("mrs_natural/inObservations").toNatural(), 
	     series->getctrl("mrs_natural/inSamples").toNatural());
  realvec out(series->getctrl("mrs_natural/onObservations").toNatural(), 
	      series->getctrl("mrs_natural/onSamples").toNatural());
  
	     

  // play all the file 
  while (series->getctrl("SoundFileSource/src/mrs_bool/notEmpty").toBool())
    {
      series->process(in,out);
      cout << "centroid = " << out(0,0) << endl;
    }
  
}



// add some simple feature extraction 
void 
sfplay6(string sfName)
{
  cout << "sfplay6: Playing " << sfName << endl;
  MarSystemManager mng;

  // Create a series Composite 
  MarSystem* series = mng.create("Series", "series");
  series->addMarSystem(mng.create("SoundFileSource", "src"));
  series->addMarSystem(mng.create("Gain", "gain"));
  series->addMarSystem(mng.create("SoundFileSink", "dest"));
  // hamming window -> complex spectrum -> power spectrum 
  series->addMarSystem(mng.create("Hamming", "hamming"));
  series->addMarSystem(mng.create("Spectrum", "spk"));
  series->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  series->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType", "power");

  // fanout of two features that both are 
  // calculated on the mangitude spectrum 
  MarSystem* fanout = mng.create("Fanout", "fanout");
  fanout->addMarSystem(mng.create("Centroid", "centroid"));
  fanout->addMarSystem(mng.create("Rolloff", "rolloff"));
  
  // add the fanout to the series network 
  series->addMarSystem(fanout);
		       
  // only update controls from Composite level 
  series->updctrl("mrs_natural/inSamples", 4096);
  series->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  series->updctrl("SoundFileSink/dest/mrs_string/filename", "ajay.au");
  
  cout << (*series) << endl;

  realvec in(series->getctrl("mrs_natural/inObservations").toNatural(), 
	     series->getctrl("mrs_natural/inSamples").toNatural());
  realvec out(series->getctrl("mrs_natural/onObservations").toNatural(), 
	      series->getctrl("mrs_natural/onSamples").toNatural());
  
  // play all the file 
  while (series->getctrl("SoundFileSource/src/mrs_bool/notEmpty").toBool())
    {
      series->process(in,out);
      // output observation 1 and observation 2 
      cout << "features = " << out(0,0) << ", " << out(1,0) << endl;
    }
  
}



// add some simple feature extraction 
// if you understand this function you understand 
// most of feature extraction in Marsyas 
void 
sfplay7(string sfName)
{
  cout << "sfplay7: Playing " << sfName << endl;
  MarSystemManager mng;

  // Full series for everything
  MarSystem* total = mng.create("Series", "total");

  // Accumulator accumulates many calls to tick 
  // and outputs the whole thing as nTimes samples 
  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->updctrl("mrs_natural/nTimes", 100);

  // Create a series Composite 
  MarSystem* series = mng.create("Series", "series");
  series->addMarSystem(mng.create("SoundFileSource", "src"));
  series->addMarSystem(mng.create("Gain", "gain"));
  series->addMarSystem(mng.create("SoundFileSink", "dest"));
  // hamming window -> complex spectrum -> power spectrum 
  series->addMarSystem(mng.create("Hamming", "hamming"));
  series->addMarSystem(mng.create("Spectrum", "spk"));
  series->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  series->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType", "power");

  // fanout of two features that both are 
  // calculated on the mangitude spectrum 
  MarSystem* fanout = mng.create("Fanout", "fanout");
  fanout->addMarSystem(mng.create("Centroid", "centroid"));
  fanout->addMarSystem(mng.create("Rolloff", "rolloff"));
  // add the fanout to the series network 
  series->addMarSystem(fanout);
  
		       
  // only update controls from Composite level 
  series->updctrl("mrs_natural/inSamples", 512);
  series->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  series->updctrl("SoundFileSink/dest/mrs_string/filename", "ajay.au");
  
  cout << (*series) << endl;
  
  // play all the file 
  // while (series->getctrl("SoundFileSource/src/mrs_bool/notEmpty").toBool())
  acc->addMarSystem(series);
  total->addMarSystem(acc);
  
  MarSystem* stats = mng.create("Fanout", "stats");
  stats->addMarSystem(mng.create("Mean", "mn"));
  stats->addMarSystem(mng.create("StandardDeviation", "std"));
  total->addMarSystem(stats);
  
  // always allocate after updating/adding marsystems 
  realvec in(total->getctrl("mrs_natural/inObservations").toNatural(), 
	     total->getctrl("mrs_natural/inSamples").toNatural());
  realvec out(total->getctrl("mrs_natural/onObservations").toNatural(), 
	      total->getctrl("mrs_natural/onSamples").toNatural());
		      


  // only 1 call is needed as accumulate calls internally 
  // multiple times process 
  total->process(in,out);
  out.write("ajay.plot");
}



void test_normMaxMin()
{
  MarSystemManager mng;
  // MarSystem * nrm = mng.create("NormMaxMin", "nrm");
  MarSystem* nrm = mng.create("Normalize", "nrm");

  realvec in;
  realvec out;


  
  in.create((mrs_natural)4,(mrs_natural)3);
  out.create((mrs_natural)4,(mrs_natural)3);
  
  nrm->updctrl("mrs_natural/inSamples", (mrs_natural)3);
  nrm->updctrl("mrs_natural/inObservations", (mrs_natural)4);



  in(0,0) = 5.1;
  in(1,0) = 3.5;
  in(2,0) = 1.4;
  in(3,0) = 0.2;

  in(0,1) = 4.9;
  in(1,1) = 3.0;
  in(2,1) = 1.4;
  in(3,1) = 0.2;

  in(0,2) = 4.7;
  in(1,2) = 3.2;
  in(2,2) = 1.3;
  in(3,2) = 0.2;

  cout << "before processing " << endl;
  
  nrm->process(in,out);
  
  cout << "out = " << out << endl;
}



void 
tempo_test(string fname, int tempo, int rank)
{
  ifstream from(fname.c_str());
  
  string name;
  int itempo;
  float strength;


  vector<string> tempo_map[11][11];
  vector<string> names;
  vector<float>  tempos;
  vector<float>  strengths;
  int count = 0;


  
  
  float min_strength = 1000.0;
  float max_strength = 0.0;
  float min_tempo = 1000.0;
  float max_tempo = 0.0;
  
  while(!from.eof() )
    {
      from >> name;
      from >> itempo;
      from >> strength;
      
      names.push_back(name);
      tempos.push_back(itempo);
      strengths.push_back(strength);

      if (strength > max_strength) 
	max_strength = strength;
      if (strength < min_strength) 
	min_strength = strength;      


      if (itempo > max_tempo) 
	max_tempo = itempo;
      if (itempo < min_tempo) 
	min_tempo = itempo;      

      count++;
    }
  cout << "Min strength = " << min_strength << endl;
  cout << "Max strength = " << max_strength << endl;

  cout << "Min tempo = " << min_tempo << endl;
  cout << "Max temp = " << max_tempo << endl;
   
  
  for (int i=0; i < count; i++)
    {
      strengths[i] = floor(((strengths[i] - min_strength) / (max_strength - min_strength)) * 10.0 + 0.5);

      tempos[i] = floor(((tempos[i] - min_tempo) / (max_tempo - min_tempo)) * 10.0 + 0.5);
      
      /* cout << "Name = " << names[i] << endl;
      cout << "Tempo = " << tempos[i] << endl;
      cout << "Strength = " << strengths[i] << endl;
      */ 
      
      int si = (int)strengths[i];
      int ti = (int)tempos[i];
      
      /* cout << "si = " << si << endl;
      cout << "ti = " << ti << endl;
      */ 
      
      tempo_map[si][ti].push_back(names[i]);
    }

  
  



  

    for (int ti = 0; ti < 11; ti++)
      for (int si = 0; si < 11; si++)
	{
	  cout << "Tempo = " << ti << " - " << "Strength = " << si << endl;
	  vector<string> retrievedFiles = tempo_map[si][ti];
	  
	  vector<string>::iterator vi;
	  
	  for (vi = retrievedFiles.begin(); vi != retrievedFiles.end(); ++vi)
	    {
	      cout << (*vi) << endl;
	    }
	
	  
	
	}
    
    
    tempo = floor(((tempo - min_tempo) / (max_tempo - min_tempo)) * 10.0 + 0.5);
    vector<string> retrievedFiles = tempo_map[rank][tempo];

    if (retrievedFiles.size()  == 0) 
      cout << "No file for these specs" << endl;
    else 
      {
	cout << "Playing " << retrievedFiles[0] << endl;
	
	tempotest_sfplay(retrievedFiles[0]);
      }
    
    
    
    return;
  
  
}


/* vector< vector<string> > tempo_map;
  vector< vector<mrs_real> > strength_map;
  vector< vector<string> >::iterator iter;

  int i;  
  for (i=0; i < 120; i++)
    {
      vector<string> tempo_files;
      tempo_map.push_back(tempo_files);
    }
  


  
  while(!from.eof() )
    {
      vector<string> tempo_files;
      
      from >> name;
      from >> itempo;
      from >> strength;

      cout << "Name = " << name << endl;
      cout << "Tempo = " << itempo << endl;
      cout << "Strength = " << strength << endl;
      
      tempo_map[itempo].push_back(name);
      strength_map[itempo].push_back(strength);
      if (strength > max_strength) 
	max_strength = strength;
      if (strength < min_strength) 
	min_strength = strength;
    }
  
  i =0;
  
  for (iter = tempo_map.begin(); iter != tempo_map.end(); ++iter)  
    {
      cout << "Bin" << " " << i << endl;
      

      
      vector<string>::iterator viter;
      for (viter = iter->begin(); viter != iter->end(); ++viter)        
	{
	  cout << "    " << *viter << endl;
	  tempo_map1[0][itempo] = *viter;
	}
      
      

      i++;
    }
  
  
    
    vector<string> tempo_files = tempo_map[tempo];
    vector<string>::iterator viter;
    cout << "FILES AT " << tempo << " bpm" << endl;
    
    int counter=0;
    
    for (viter = tempo_files.begin(); viter != tempo_files.end(); ++viter)       
    {
    
    cout << counter << ": " << *viter << endl;
      counter++;
      }
    
  cout << endl << endl;
  
  
  tempotest_sfplay(tempo_files[rank]);
  vector<string>::iterator viter;

for (iter = tempo_map.begin(); iter != tempo_map.end(); ++iter)
  {
    cout << "Files for tempo " << iter->first << " = " << endl;
      for (viter = (iter->second).begin(); viter != (iter->second).end(); 
++viter)
	   {
	cout << *viter << endl;
	  }
	}

*/ 


double 
randD(double max)
{
  return max  *  (double)rand() / ((double)(RAND_MAX)+(double)(1.0)) ; 
}  
      
  
void 
test_collection(string sfName)
{
  
  MarSystemManager mng;
  MarSystem* pnet = mng.create("Series", "pnet");
  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet->addMarSystem(mng.create("AudioSink", "dest"));
  
  
  pnet->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  pnet->updctrl("SoundFileSource/src/mrs_bool/shuffle", true);
  cout << "first round" << endl; 


  for (int k=0; k < 4; k++) 
    {
      pnet->updctrl("SoundFileSource/src/mrs_natural/cindex", k);
      cout << pnet->getctrl("SoundFileSource/src/mrs_string/currentlyPlaying").toString() << endl;
      for (int i=0; i <100; i++) 
	{
	  
	pnet->tick();
	}
    }
  cout << "second round" << endl;
  pnet->updctrl("SoundFileSource/src/mrs_string/filename", "music1.mf"); 
  for (int k=0; k < 4; k++) 
    {
      pnet->updctrl("SoundFileSource/src/mrs_natural/cindex", k);
      cout << pnet->getctrl("SoundFileSource/src/mrs_string/currentlyPlaying").toString() << endl;
      for (int i=0; i <100; i++) 
// 	while(pnet->getctrl("SoundFileSource/src/mrs_bool/notEmpty").toBool())
	  {
	    pnet->tick();
	  }
    }
  

  
}



  
void test_SOM(string collectionName) 
{
  MarSystemManager mng;
  
  srand(4);

  MarSystem* extractNet = mng.create("Series", "extractNet");
  extractNet->addMarSystem(mng.create("SoundFileSource", "src"));
  MarSystem* spectimeFanout = mng.create("Fanout", "spectimeFanout");
  spectimeFanout->addMarSystem(mng.create("ZeroCrossings", "zcrs"));
  
  MarSystem* spectralNet = mng.create("Series", "spectralNet");
  spectralNet->addMarSystem(mng.create("Hamming", "ham"));
  spectralNet->addMarSystem(mng.create("Spectrum", "spk"));
  spectralNet->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  MarSystem* featureFanout = mng.create("Fanout", "featureFanout");
  featureFanout->addMarSystem(mng.create("Centroid", "centroid"));
  featureFanout->addMarSystem(mng.create("Rolloff", "rolloff"));
  featureFanout->addMarSystem(mng.create("MFCC", "mfcc"));
  spectralNet->addMarSystem(featureFanout);
  spectimeFanout->addMarSystem(spectralNet);
  extractNet->addMarSystem(spectimeFanout);
  extractNet->addMarSystem(mng.create("Memory", "mem"));

  MarSystem* stats = mng.create("Fanout", "stats");
  stats->addMarSystem(mng.create("Mean", "mn1"));
  stats->addMarSystem(mng.create("StandardDeviation", "std1"));
  extractNet->addMarSystem(stats);
  

  
  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->updctrl("mrs_natural/nTimes", 1200);
  acc->addMarSystem(extractNet);


  MarSystem* total = mng.create("Series", "total");
  total->addMarSystem(acc);
  MarSystem* stats2 = mng.create("Fanout", "stats2");
  stats2->addMarSystem(mng.create("Mean", "mn2"));
  stats2->addMarSystem(mng.create("StandardDeviation", "std2"));  
  total->addMarSystem(stats2);

  total->addMarSystem(mng.create("Annotator", "ann"));
  total->addMarSystem(mng.create("SOM", "som"));
  

  /* 
  total->addMarSystem(mng.create("KNNClassifier", "knn"));
  total->addMarSystem(mng.create("ClassOutputSink", "csink"));
  

  total->updctrl("KNNClassifier/knn/mrs_natural/k",3);
  total->updctrl("KNNClassifier/knn/mrs_natural/nPredictions", 3);
  total->updctrl("KNNClassifier/knn/mrs_string/mode","train");  
  */ 
  

  
  

  // link top-level controls 
  total->linkctrl("mrs_string/filename",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_string/filename");  

  total->linkctrl("mrs_natural/pos",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/pos");  


  total->linkctrl("mrs_string/allfilenames",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_string/allfilenames");  

  total->linkctrl("mrs_natural/numFiles",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/numFiles");  


  total->linkctrl("mrs_bool/notEmpty",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_bool/notEmpty");  
  total->linkctrl("mrs_bool/advance",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_bool/advance");  

  total->linkctrl("mrs_bool/memReset",
		  "Accumulator/acc/Series/extractNet/Memory/mem/mrs_bool/reset");  

  total->linkctrl("mrs_natural/label",
		  "Annotator/ann/mrs_natural/label");



  total->updctrl("Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/inSamples", 512);
  
  
  
  
  total->updctrl("mrs_string/filename", collectionName);

  /* total->updctrl("ClassOutputSink/csink/mrs_string/labelNames", 
		 total->getctrl("mrs_string/allfilenames"));

  total->updctrl("ClassOutputSink/csink/mrs_natural/nLabels", 
		 total->getctrl("mrs_natural/numFiles"));




  total->updctrl("KNNClassifier/knn/mrs_natural/nLabels", 
		 total->getctrl("mrs_natural/numFiles"));
  */ 
  

  
  mrs_natural l=0;

  /* while (total->getctrl("mrs_bool/notEmpty").toBool())
    {
      total->updctrl("mrs_natural/label", l);
      total->tick();
      total->updctrl("mrs_bool/memReset", true);
      total->updctrl("mrs_bool/advance", true);
      l++;
      cerr << "Processed " << l << " files " << endl;
    }
  */ 
  
  /* total->updctrl("KNNClassifier/knn/mrs_bool/done",true);  
  total->updctrl("KNNClassifier/knn/mrs_string/mode", "predict");
  total->updctrl("ClassOutputSink/csink/mrs_string/filename", "similar.mf");
  total->updctrl("ClassOutputSink/csink/mrs_bool/silent", false);
  */ 
  
  //cout << (*total) << endl;




  int trainSize = 2000;
  int grid_width = 10;
  int grid_height = 10;
  int iterations = 20;
  
  

  MarSystem* som = mng.create("SOM", "som");
  som->updctrl("mrs_natural/inObservations", 2);
  som->updctrl("mrs_natural/inSamples", 1);
  som->updctrl("mrs_natural/grid_height", grid_height);
  som->updctrl("mrs_natural/grid_width", grid_width);
  som->updctrl("mrs_string/mode", "train");



  

  realvec train_data(trainSize);
  
  realvec input;
  input.create(2);
  realvec output;
  output.create(3);
  

  // init train data 

  cout << "Initializing training data" << endl;
  
  for (int i=0; i < trainSize; i++) 
    {
      train_data(i) = randD(1.0);
      cout << train_data(i) << endl;
    }

  
  // train map 
  cout << "training " << endl;


  // output initial map 
  realvec som_map;
  som_map.create((mrs_natural)grid_width,(mrs_natural)grid_height);
  
  
  for (int k=0; k < iterations; k++) 
    {
      cout << "Iteration " << k << endl;

      
      for (int i=0; i < trainSize; i++) 
	{
	  input(0) = train_data(i);
	  input(1) = 0;
	  som->process(input, output);
	  som_map((mrs_natural)output(0), (mrs_natural)output(1)) = train_data(i) * 64.0;
	  if (i==100)
	    {
	      ostringstream oss;
	      oss << "map" << k << ".plot";
	      som_map.write(oss.str());	    
	    }
	  
	}

      

    }
  


  cout << "predicting" << endl;


  som->updctrl("mrs_bool/done", true);
  // predict 
  som->updctrl("mrs_string/mode", "predict");
  
  for (int i=0; i < 100; i++) 
    {
      input(0) = train_data(i);
      cout << "input(0) = " << input(0) << endl;
      input(1) = i;
      cout << "input(1) = " << input(1) << endl;
      som->process(input,output);
      som->process(input, output);
      som_map((mrs_natural)output(0), (mrs_natural)output(1)) = train_data(i) * 64.0;
      if (i==99)
	{
	  ostringstream oss;
	  oss << "predictmap" << ".plot";
	  som_map.write(oss.str());	    
	}
    }


  
  

  
  
  
  
  
}


void 
test_scheduler() 
{
  string ipName="/home/gtzan/data/sound/music_speech/music/gravity.au";
  string opName="op.wav";
  
  
  MarSystemManager mng;
  
  // Create a series Composite 
  MarSystem* series = mng.create("Series", "series");
  series->addMarSystem(mng.create("SoundFileSource", "src"));
  series->addMarSystem(mng.create("Gain", "gain"));
  series->addMarSystem(mng.create("SoundFileSink", "dest"));
  
  // basic setup
  series->updctrl("mrs_natural/inSamples", 256);
  series->updctrl("SoundFileSource/src/mrs_string/filename", ipName);
  series->updctrl("SoundFileSink/dest/mrs_string/filename", opName);
  
  // adjust some gain in a demonstrative but otherwise useless way
//   series->updctrl("1.5s","Gain/gain/mrs_real/gain", 0.0);
//   series->updctrl("3.0s","Gain/gain/mrs_real/gain", 1.0);
//   series->updctrl("3.500s","Gain/gain/mrs_real/gain", 0.0);
//  series->updctrl("4s","Gain/gain/mrs_real/gain", 1.0);
  
  // loop for a time, waving hands
  for (int i=0; i<10000; i++) {
    series->tick();
  }
  
  // Composite deletes the added MarSystems
  // so you must not delete them
  delete series;
  

}



void shake(mrs_natural bufferSize, mrs_natural inputSize,string backgroundopt) 
{
 
 
  mrs_real amp;
  cout<<"before MarSystemManager in Shake"<<endl;
  MarSystemManager mng;
   cout<<"before MarSystemManager in Shake"<<endl;
  MarSystem* series = mng.create("Series", "series");
  series->addMarSystem(mng.create("Maraca", "maraca")); 
  series->addMarSystem(mng.create("AudioSink", "dest"));
  series->update();
  
  
  series->updctrl("Gain/gain/mrs_real/gain", 1.0);
  series->updctrl("AudioSink/dest/mrs_natural/bufferSize", 128); 
  series->updctrl("mrs_natural/inSamples", 64);
  series->update();
  
  
  while(1) 

    {  
      amp = (mrs_real) (7 * rand() / (RAND_MAX + 1.0) );
      series->updctrl("Maraca/maraca/mrs_real/frequency",2000.0);
      series->updctrl("Maraca/maraca/mrs_real/amplitude",amp);
     

      series->tick();	
    }//while

  
}





  




int
main(int argc, char **argv)
{
  string sfName;

  /* if (argc == 2) 
    tempo_test(argv[1], 100, 0);
  if (argc == 3)
    tempo_test(argv[1], atoi(argv[2]), 0);
  if (argc == 4) 
    tempo_test(argv[1], atoi(argv[2]), atoi(argv[3]));    
  */ 
  
  // sfplay4(argv[1]);
  // PluckLive(0,100,1.0,0.5);
  // Pluck(0,100,1.0,0.5,"TestPluckedRich0_100hz.wav");


  test_SOM("music.mf");
  // test_collection("music.mf");
  

  // test_scheduler();
  
  


  /* 
  if (argc != 2) 
    {
      cout << "Wrong number of arguments " << endl;
      exit(1);
    }
  else
    sfName = argv[1];
  
  cout << "SoundFile is " << sfName << endl;
  */ 

  // Matlab style with explicit allocation of buffers 
  // and seperate process functions for each MarSystem
  

  // test_normMaxMin();
  
  // sfplay1(sfName);
  
  // Matlab style with explicit allocation of buffers 
  // but using Series composite
  // sfplay2(sfName);

  // get rid of buffers altogether using the tick method 
  // sfplay3(sfName);


  // using MarSystemManager and change the window 
  // size on each loop iteration 
  // sflay4(sfName);
  
  
  

  // add centroid calculation over the whole file 
  // sfplay5(sfName);

  // add centroid and rolloff calculation using Fanout 
  // sfplay6(sfName);
 

  // what if you want to calculate the mean, variances  
  // over 100 windows 
  // sfplay7(sfName);
  
  // wavsfplay1(sfName);
  
}
