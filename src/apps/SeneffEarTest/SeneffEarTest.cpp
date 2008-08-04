
#include "MarSystemManager.h"

#include <string> 
#include <iostream>

using namespace std;
using namespace Marsyas;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char **argv)
{
	string name = argv[1];
	mrs_natural stage(atoi(argv[2]));
	mrs_natural channel(atoi(argv[3]));
	mrs_natural window(atoi(argv[4]));
	mrs_real gain(atoi(argv[5]));

	MarSystemManager mng;
	MarSystem* src = mng.create("SoundFileSource", "src");
	MarSystem* seneffEar = mng.create("SeneffEar","seneffEar");
	MarSystem* dest = mng.create("AudioSink", "dest");

	/*seneffEar->setctrl("mrs_natural/inSamples", mrs_natural(10));
	seneffEar->setctrl("mrs_natural/inObservations", mrs_natural(1));
	seneffEar->setctrl("mrs_real/israte", mrs_real(44100.0f));
	seneffEar->update();

	realvec in, out;
	in.create(seneffEar->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), seneffEar->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
	out.create(seneffEar->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), seneffEar->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

	in(0) = 1.0f;

	seneffEar->process(in, out);

	ofstream log("log");
	log << out << endl;
	log.close();*/

	src->updctrl("mrs_natural/inSamples", window);
	src->updctrl("mrs_string/filename", name);

	seneffEar->updctrl("mrs_natural/inObservations", src->getctrl("mrs_natural/onObservations"));
	seneffEar->updctrl("mrs_natural/inSamples", src->getctrl("mrs_natural/onSamples"));
	seneffEar->updctrl("mrs_real/israte",src->getctrl("mrs_real/osrate"));
	seneffEar->updctrl("mrs_natural/stage", stage);

	dest->updctrl("mrs_natural/inObservations", src->getctrl("mrs_natural/onObservations"));
	dest->updctrl("mrs_natural/inSamples", src->getctrl("mrs_natural/onSamples"));
	dest->updctrl("mrs_real/israte", src->getctrl("mrs_real/osrate"));
	dest->updctrl("mrs_natural/nChannels", 1);

	realvec src_in, dest_in;
	realvec src_out, seneffEar_out, dest_out;

	src_in.create(src->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), src->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
	src_out.create(src->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), src->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

	seneffEar_out.create(seneffEar->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), seneffEar->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

	dest_in.create(dest->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), dest->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
	dest_out.create(dest->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), dest->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

	while (src->getctrl("mrs_bool/notEmpty")->to<mrs_bool>()){
		src->process(src_in, src_out);
		seneffEar->process(src_out, seneffEar_out);
		for (mrs_natural i = 0; i < seneffEar->getctrl("mrs_natural/onSamples")->to<mrs_natural>(); i++){
			dest_in(i) = gain*seneffEar_out(channel,i);
		}
		dest->process(dest_in, dest_out);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
