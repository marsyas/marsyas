
#include "MarSystemManager.h"

#include <iostream> 
#include <string>
using namespace std;

int 
main(int argc, const char **argv)
{
  string name = argv[1];
  natural channel(atoi(argv[2]));
  natural window(atoi(argv[3]));
  real gain(atoi(argv[4]));
  
  MarSystemManager mng;
  MarSystem* src = mng.create("SoundFileSource", "src");
  MarSystem* erb = mng.create("ERB","ERBfilterBank");
  MarSystem* dest = mng.create("AudioSink", "dest");
  
  src->updctrl("natural/inSamples", window);
  src->updctrl("string/filename", name);
  
  erb->updctrl("natural/inObservations", src->getctrl("natural/onObservations"));
  erb->updctrl("natural/inSamples", src->getctrl("natural/onSamples"));
  erb->updctrl("real/israte",src->getctrl("real/osrate"));
  erb->updctrl("natural/numChannels",24);
  erb->updctrl("real/lowFreq",100.0f);
  
  dest->updctrl("natural/inObservations", src->getctrl("natural/onObservations"));
  dest->updctrl("natural/inSamples", src->getctrl("natural/onSamples"));
  dest->updctrl("real/israte", src->getctrl("real/osrate"));
  dest->updctrl("natural/nChannels", 1);
  
  realvec src_in, dest_in;
  realvec src_out, erb_out, dest_out;
  
  src_in.create(src->getctrl("natural/inObservations").toNatural(), src->getctrl("natural/inSamples").toNatural());
  src_out.create(src->getctrl("natural/onObservations").toNatural(), src->getctrl("natural/onSamples").toNatural());
  
  erb_out.create(erb->getctrl("natural/onObservations").toNatural(), erb->getctrl("natural/onSamples").toNatural());
  
  dest_in.create(dest->getctrl("natural/inObservations").toNatural(), dest->getctrl("natural/inSamples").toNatural());
  dest_out.create(dest->getctrl("natural/onObservations").toNatural(), dest->getctrl("natural/onSamples").toNatural());
  
  while (src->getctrl("bool/notEmpty").toBool()){
    src->process(src_in, src_out);
    erb->process(src_out, erb_out);
    for (natural i = 0; i < erb->getctrl("natural/onSamples").toNatural(); i++){
      dest_in(i) = gain*erb_out(channel,i);
    }
    dest->process(dest_in, dest_out);
  }
  
  return 0;
}


