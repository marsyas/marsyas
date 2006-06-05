
#include "MarSystemManager.h"

#include <string> 
#include <iostream> 
using namespace std;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char **argv)
{
	string name = argv[1];
	natural stage(atoi(argv[2]));
	natural channel(atoi(argv[3]));
	natural window(atoi(argv[4]));
	real gain(atoi(argv[5]));

	MarSystemManager mng;
	MarSystem* src = mng.create("SoundFileSource", "src");
	MarSystem* seneffEar = mng.create("SeneffEar","seneffEar");
	MarSystem* dest = mng.create("AudioSink", "dest");

	/*seneffEar->setctrl("natural/inSamples", natural(10));
	seneffEar->setctrl("natural/inObservations", natural(1));
	seneffEar->setctrl("real/israte", real(44100.0f));
	seneffEar->update();

	realvec in, out;
	in.create(seneffEar->getctrl("natural/inObservations").toNatural(), seneffEar->getctrl("natural/inSamples").toNatural());
	out.create(seneffEar->getctrl("natural/onObservations").toNatural(), seneffEar->getctrl("natural/onSamples").toNatural());

	in(0) = 1.0f;

	seneffEar->process(in, out);

	ofstream log("log");
	log << out << endl;
	log.close();*/

	src->updctrl("natural/inSamples", window);
	src->updctrl("string/filename", name);

	seneffEar->updctrl("natural/inObservations", src->getctrl("natural/onObservations"));
	seneffEar->updctrl("natural/inSamples", src->getctrl("natural/onSamples"));
	seneffEar->updctrl("real/israte",src->getctrl("real/osrate"));
	seneffEar->updctrl("natural/stage", stage);

	dest->updctrl("natural/inObservations", src->getctrl("natural/onObservations"));
	dest->updctrl("natural/inSamples", src->getctrl("natural/onSamples"));
	dest->updctrl("real/israte", src->getctrl("real/osrate"));
	dest->updctrl("natural/nChannels", 1);

	realvec src_in, dest_in;
	realvec src_out, seneffEar_out, dest_out;

	src_in.create(src->getctrl("natural/inObservations").toNatural(), src->getctrl("natural/inSamples").toNatural());
	src_out.create(src->getctrl("natural/onObservations").toNatural(), src->getctrl("natural/onSamples").toNatural());

	seneffEar_out.create(seneffEar->getctrl("natural/onObservations").toNatural(), seneffEar->getctrl("natural/onSamples").toNatural());

	dest_in.create(dest->getctrl("natural/inObservations").toNatural(), dest->getctrl("natural/inSamples").toNatural());
	dest_out.create(dest->getctrl("natural/onObservations").toNatural(), dest->getctrl("natural/onSamples").toNatural());

	while (src->getctrl("bool/notEmpty").toBool()){
		src->process(src_in, src_out);
		seneffEar->process(src_out, seneffEar_out);
		for (natural i = 0; i < seneffEar->getctrl("natural/onSamples").toNatural(); i++){
			dest_in(i) = gain*seneffEar_out(channel,i);
		}
		dest->process(dest_in, dest_out);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
