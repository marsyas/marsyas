
#include "MarSystemManager.h"

#include <iostream> 
#include <string>

using namespace std;
using namespace Marsyas;

int 
main(int argc, const char **argv)
{
  string name = argv[1];
  mrs_natural channel(atoi(argv[2]));
  mrs_natural window(atoi(argv[3]));
  mrs_real gain(atof(argv[4]));


  MarSystemManager mng;
  MarSystem* src = mng.create("SoundFileSource", "src");
  MarSystem* erb = mng.create("ERB","ERBfilterBank");
  MarSystem* dest = mng.create("AudioSink", "dest");
  



  
  src->updctrl("mrs_natural/inSamples", window);
  src->updctrl("mrs_string/filename", name);


  // This core dumps. Need to check it out. 
  erb->setctrl("mrs_natural/inObservations", src->getctrl("mrs_natural/onObservations"));
  
  cout << *src << endl;
  
  cout << src->getctrl("mrs_natural/onObservations") << endl;
  cout << src->getctrl("mrs_natural/onSamples") << endl;  
  erb->updctrl("mrs_natural/inObservations", 1);
  

  erb->updctrl("mrs_natural/inSamples", src->getctrl("mrs_natural/onSamples"));
  erb->updctrl("mrs_real/israte",src->getctrl("mrs_real/osrate"));
  erb->updctrl("mrs_natural/numChannels",64);
  erb->updctrl("mrs_real/lowFreq",100.0f);
  
  dest->updctrl("mrs_natural/inObservations", src->getctrl("mrs_natural/onObservations"));
  dest->updctrl("mrs_natural/inSamples", src->getctrl("mrs_natural/onSamples"));
  dest->updctrl("mrs_real/israte", src->getctrl("mrs_real/osrate"));
  dest->updctrl("mrs_natural/nChannels", 1);
  dest->updctrl("mrs_bool/initAudio", true);


  
  realvec src_in, dest_in;
  realvec src_out, erb_out, dest_out;
  
  src_in.create(src->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), src->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  src_out.create(src->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), src->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  
  erb_out.create(erb->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), erb->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  
  dest_in.create(dest->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), dest->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  dest_out.create(dest->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), dest->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  
  while (src->getctrl("mrs_bool/notEmpty")->to<mrs_bool>()){
    src->process(src_in, src_out);
    erb->process(src_out, erb_out);
    
    for (mrs_natural i = 0; i < erb->getctrl("mrs_natural/onSamples")->to<mrs_natural>(); i++){
      dest_in(i) = gain*erb_out(channel,i);
    }
    dest->process(dest_in, dest_out);
  }
  
  return 0;
}


