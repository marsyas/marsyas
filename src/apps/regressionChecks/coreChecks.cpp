// core/fundamental  Marsyas checks
#include "coreChecks.h"

// possibly useful as a basis for new tests
void
core_null()
{
	cout<<"Null test passed succesfully!"<<endl;
}

void
core_isClose(string infile1, string infile2)
{
	MarSystem* pnet = mng.create("Series", "pnet");

	MarSystem* invnet = mng.create("Series", "invnet");
	invnet->addMarSystem(mng.create("SoundFileSource", "src2"));
	invnet->updctrl("SoundFileSource/src2/mrs_string/filename", infile2);
	invnet->addMarSystem(mng.create("Negative", "neg"));

	MarSystem* fanout = mng.create("Fanout", "fanout");
	fanout->addMarSystem(mng.create("SoundFileSource", "src1"));
	fanout->updctrl("SoundFileSource/src1/mrs_string/filename", infile1);
	fanout->addMarSystem(invnet);

	pnet->addMarSystem(fanout);
	pnet->addMarSystem(mng.create("Sum", "sum"));
	pnet->linkControl("mrs_bool/notEmpty",
	                  "Fanout/fanout/SoundFileSource/src1/mrs_bool/notEmpty");

	mrs_natural i;
	mrs_natural samples =
	    pnet->getctrl("mrs_natural/inSamples")->to<mrs_natural>();
	while ( pnet->getctrl("mrs_bool/notEmpty")->to<mrs_bool>() )
	{
		pnet->tick();
		const realvec& processedData =
		    pnet->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
		for (i=0; i<samples; i++)
		{
			//  useful for tweaking CLOSE_ENOUGH
			//cout<<processedData(i)<<" ";
			if ( processedData(i) > CLOSE_ENOUGH )
			{
				exit(1);
			}
		}
	}
}


void
core_audiodevices()
{
#ifdef MARSYAS_AUDIOIO
	std::cout << "Testing audio devices" << endl;

	RtAudio *audio = 0;
	RtAudioDeviceInfo info;
	try
	{
		audio = new RtAudio();
	}
	catch (RtError &error)
	{
		error.printMessage();
		exit(EXIT_FAILURE);
	}

	int devices = audio->getDeviceCount();
	std::cout << "\nFound " << devices << " device(s) ...\n";

	for (int i=1; i<=devices; i++)
	{
		try
		{
			info = audio->getDeviceInfo(i);
		}
		catch (RtError &error)
		{
			error.printMessage();
			break;
		}

		std::cout << "\nDevice Name = " << info.name << '\n';
		if (info.probed == false)
			std::cout << "Probe Status = UNsuccessful\n";
		else
		{
			std::cout << "Probe Status = Successful\n";
			std::cout << "Output Channels = " << info.outputChannels << '\n';
			std::cout << "Input Channels = " << info.inputChannels << '\n';
			std::cout << "Duplex Channels = " << info.duplexChannels << '\n';
			if (info.isDefault) std::cout << "This is the default device.\n";
			else std::cout << "This is NOT the default device.\n";
			if ( info.nativeFormats == 0 )
				std::cout << "No natively supported data formats(?)!";
			else
			{
				std::cout << "Natively supported data formats:\n";
				if ( info.nativeFormats & RTAUDIO_SINT8 )
					std::cout << "  8-bit int\n";
				if ( info.nativeFormats & RTAUDIO_SINT16 )
					std::cout << "  16-bit int\n";
				if ( info.nativeFormats & RTAUDIO_SINT24 )
					std::cout << "  24-bit int\n";
				if ( info.nativeFormats & RTAUDIO_SINT32 )
					std::cout << "  32-bit int\n";
				if ( info.nativeFormats & RTAUDIO_FLOAT32 )
					std::cout << "  32-bit float\n";
				if ( info.nativeFormats & RTAUDIO_FLOAT64 )
					std::cout << "  64-bit float\n";
			}
			if ( info.sampleRates.size() < 1 )
				std::cout << "No supported sample rates found!";
			else
			{
				std::cout << "Supported sample rates = ";
				for (unsigned int j=0; j<info.sampleRates.size(); j++)
					std::cout << info.sampleRates[j] << " ";
			}
			std::cout << std::endl;
		}
	}
	std::cout << std::endl;

	delete audio;
#endif 
}

void
core_realvec()
{
	realvec test;
	test.create(0);
	test.setval(1);
	for (mrs_natural i=0; i<14; i++)
	{
		test.stretchWrite(i,1);
		cout<<test;
	}
	realvec foo(5);
	foo.setval(2);
	realvec baz;
	baz = foo+test;
	cout<<baz;
}


