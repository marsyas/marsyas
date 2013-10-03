// TestAubioYin.h
//
// The AubioYin class is a fundamental frequency estimator (Cheveigne
// and Kawahara 2002) that is implemented in the Aubio
// (http://aubio.org) framework.  We have ported this code to Marsyas,
// and present here a series of functional tests to ensure that the
// results from the Aubio implementation of YIN is the same as our
// Marsyas implementation.
//
// One note to make is that Aubio uses floats to calculate values
// instead of doubles like Marsyas does.  I have extracted just the
// YIN code from Aubio, have made a separate directory for it, and
// have changed all instances of floats to doubles.  The results from
// this standalone code base.
//
//
// (c) 2009 - sness@sness.net - GPL
//


#include <cxxtest/TestSuite.h>
#include <cstdio>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/marsystems/AubioYin.h>
#include <marsyas/CommandLineOptions.h>

#include <vector>

using namespace std;
using namespace Marsyas;

class AubioYin_runner : public CxxTest::TestSuite
{
public:
  realvec in,out;
  MarSystemManager mng;
  AubioYin *yin;

  void
  setUp()
  {
    yin = new AubioYin("yin");
    yin->updControl("mrs_real/israte", 44100.0);
  }

  // "destructor"
  void tearDown()
  {
    delete yin;
  }


  //
  // Test one single buffer of audio with a 1000Hz tone
  //
  void test_one_window_1000hz(void)
  {
    // Create input and output realvecs
    int length = 512;
    realvec input_realvec;
    in.create(1,length);
    out.create(1,length);

    // Fill up the input realvec with a sine wave
    for (int i = 0; i < length; i++) {
      double d = sin(i * 1000.0 / 44100.0 * (PI*2.0));
      in(0,i) = d;
    }

    // Process the data with the YIN algorithm
    yin->myProcess(in,out);

    // Check to see if the pitch is correct.
    // sness - By correct, we mean that it's the same as output by the
    // Aubio implementation of the yin algorithm.
    double pitch = out(0,0);
    TS_ASSERT_DELTA(pitch,989.011,0.001);
  }

  //
  // Test ten windows of audio with a 1000Hz tone
  //
  void test_ten_windows_1000hz(void)
  {
    int buffer_size = 256;
    int num_buffers = 10;

    // Create a realvec that we will fill
    realvec nbuf;
    nbuf.create(1,buffer_size*num_buffers);

    // Create a realvec that we will process
    realvec ibuf;
    ibuf.create(1,buffer_size);

    // Fill up the realvec with a sine wave
    for (int i = 0; i < buffer_size*num_buffers; i++) {
      double d = sin(i * 1000.0 / 44100.0 * (PI*2.0));
      nbuf(0,i) = d;
    }

    // A series containing a RealvecSource followed by an AubioYin
    MarSystem* net = mng.create("Series", "series");
    net->addMarSystem(mng.create("RealvecSource", "src"));
    net->addMarSystem(mng.create("AubioYin", "yin"));

    net->updControl("mrs_natural/inSamples", buffer_size);
    net->updControl("mrs_real/israte", 44100.0);

    // A realvec to store our answers in
    realvec check(num_buffers,1);

    for (int i=0; i < num_buffers; i++) {
      // Copy over the section of nbuf to ibuf
      int offset = i * buffer_size;
      for (mrs_natural j = 0; j < ibuf.getSize(); j++) {
        ibuf(0,j) = nbuf(0,j+offset);
      }

      // Copy ibuf to the RealvecSource
      net->updControl("RealvecSource/src/mrs_realvec/data", ibuf);

      // Tick the network once to push the data through the network
      net->tick();

      // Get the output of the network
      realvec r;
      r = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
      check(i,0) = r(0,0);
    }

    // These numbers are what Aubio outputs
    TS_ASSERT_DELTA(check(0,0),989.011,0.001);
    TS_ASSERT_DELTA(check(1,0),989.011,0.001);
    TS_ASSERT_DELTA(check(2,0),989.011,0.001);
    TS_ASSERT_DELTA(check(3,0),989.011,0.001);
    TS_ASSERT_DELTA(check(4,0),989.122,0.001);
    TS_ASSERT_DELTA(check(5,0),989.011,0.001);
    TS_ASSERT_DELTA(check(6,0),989.011,0.001);
    TS_ASSERT_DELTA(check(7,0),989.122,0.001);
    TS_ASSERT_DELTA(check(8,0),989.011,0.001);
    TS_ASSERT_DELTA(check(9,0),989.122,0.001);

    delete net;
  }

  //
  // Test ten windows of audio with a 1000Hz tone followed by a 500Hz tone
  //
  void test_ten_windows_1000hz_500hz(void)
  {
    int buffer_size = 256;
    int num_buffers = 10;

    // Create a realvec that we will fill
    realvec nbuf;
    nbuf.create(1,buffer_size*num_buffers);

    // Create a realvec that we will process
    realvec ibuf;
    ibuf.create(1,buffer_size);

    // Fill up the realvec with a sine wave
    for (int i = 0; i < buffer_size*num_buffers/2; i++) {
      double d = sin(i * 1000.0 / 44100.0 * (PI*2.0));
      nbuf(0,i) = d;
    }

    // Fill up the realvec with a sine wave
    for (int i = (buffer_size*num_buffers)/2; i < buffer_size*num_buffers; i++) {
      double d = sin(i * 500.0 / 44100.0 * (PI*2.0));
      nbuf(0,i) = d;
    }

    // A series containing a RealvecSource followed by an AubioYin
    MarSystem* net = mng.create("Series", "series");
    net->addMarSystem(mng.create("RealvecSource", "src"));
    net->addMarSystem(mng.create("AubioYin", "yin"));

    net->updControl("mrs_natural/inSamples", buffer_size);
    net->updControl("mrs_real/israte", 44100.0);

    // A realvec to store our answers in
    realvec check(num_buffers,1);

    for (int i=0; i < num_buffers; i++) {
      // Copy over the section of nbuf to ibuf
      int offset = i * buffer_size;
      for (mrs_natural j = 0; j < ibuf.getSize(); j++) {
        ibuf(0,j) = nbuf(0,j+offset);
      }

      // Copy ibuf to the RealvecSource
      net->updControl("RealvecSource/src/mrs_realvec/data", ibuf);

      // Tick the network once to push the data through the network
      net->tick();

      // Get the output of the network
      realvec r;
      r = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
      check(i,0) = r(0,0);
    }

    TS_ASSERT_DELTA(check(0,0),989.011,0.001);
    TS_ASSERT_DELTA(check(1,0),989.011,0.001);
    TS_ASSERT_DELTA(check(2,0),989.011,0.001);
    TS_ASSERT_DELTA(check(3,0),989.011,0.001);
    TS_ASSERT_DELTA(check(4,0),989.122,0.001);
    TS_ASSERT_DELTA(check(5,0),497.210,0.001);
    TS_ASSERT_DELTA(check(6,0),497.210,0.001);
    TS_ASSERT_DELTA(check(7,0),497.210,0.001);
    TS_ASSERT_DELTA(check(8,0),497.210,0.001);
    TS_ASSERT_DELTA(check(9,0),497.210,0.001);

    delete net;
  }

  //
  // Test ten windows of audio with 1000Hz tone sweeping smoothly to a
  // 990Hz tone
  //
  void test_ten_windows_1000_990_smooth_hz(void)
  {
    int buffer_size = 256;
    int num_buffers = 10;

    // Create a realvec that we will fill
    realvec nbuf;
    nbuf.create(1,buffer_size*num_buffers);

    // Create a realvec that we will process
    realvec ibuf;
    ibuf.create(1,buffer_size);

    // The starting and ending frequencies for the sine wave sweep
    double start_freq = 1000.0;
    double end_freq = 990.0;
    double freq;
    double pos;

    // Fill up the realvec with a sine wave
    for (int i = 0; i < buffer_size*num_buffers; i++) {
      pos = ((double)i+1) / (buffer_size*num_buffers);
      freq = start_freq + ((end_freq - start_freq) * pos);
      double d = sin(i * freq / 44100.0 * (PI*2.0));
      nbuf(0,i) = d;
    }

    // A series containing a RealvecSource followed by an AubioYin
    MarSystem* net = mng.create("Series", "series");
    net->addMarSystem(mng.create("RealvecSource", "src"));
    net->addMarSystem(mng.create("AubioYin", "yin"));

    net->updControl("mrs_natural/inSamples", buffer_size);
    net->updControl("mrs_real/israte", 44100.0);

    // A realvec to store our answers in
    realvec check(num_buffers,1);

    for (int i=0; i < num_buffers; i++) {
      // Copy over the section of nbuf to ibuf
      int offset = i * buffer_size;
      for (mrs_natural j = 0; j < ibuf.getSize(); j++) {
        ibuf(0,j) = nbuf(0,j+offset);
      }

      // Copy ibuf to the RealvecSource
      net->updControl("RealvecSource/src/mrs_realvec/data", ibuf);

      // Tick the network once to push the data through the network
      net->tick();

      // Get the output of the network
      realvec r;
      r = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
      check(i,0) = r(0,0);
    }

    TS_ASSERT_DELTA(check(0,0),988.346,0.001);
    TS_ASSERT_DELTA(check(1,0),986.357,0.001);
    TS_ASSERT_DELTA(check(2,0),984.375,0.001);
    TS_ASSERT_DELTA(check(3,0),982.401,0.001);
    TS_ASSERT_DELTA(check(4,0),980.327,0.001);
    TS_ASSERT_DELTA(check(5,0),978.478,0.001);
    TS_ASSERT_DELTA(check(6,0),976.528,0.001);
    TS_ASSERT_DELTA(check(7,0),974.586,0.001);
    TS_ASSERT_DELTA(check(8,0),972.651,0.001);
    TS_ASSERT_DELTA(check(9,0),970.831,0.001);

    delete net;
  }

  //
  // Test ten windows of audio with 1000Hz tone sweeping in blocks to a
  // 880Hz tone, with limiting the search range
  //
  void test_ten_windows_1000_880_smooth_hz(void)
  {
    int buffer_size = 256;
    int num_buffers = 10;

    // Create a realvec that we will fill
    realvec nbuf;
    nbuf.create(1,buffer_size*num_buffers);

    // Create a realvec that we will process
    realvec ibuf;
    ibuf.create(1,buffer_size);

    // The starting and ending frequencies for the sine wave sweep
    double start_freq = 1000.0;
    double end_freq = 880.0;
    double freq;
    double pos;

    // Fill up the realvec with a sine wave

    // I'm really suspicious about the lack of phase adjustment;
    // that's probably why yin ends up detecting 768 Hz.
    // but hey, it's a better test than nothing.  -Graham
    for (int i = 0; i < buffer_size*num_buffers; i++) {
      pos = ((double)i+1) / (buffer_size*num_buffers);
      freq = start_freq + ((end_freq - start_freq) * pos);
      double d = sin(i * freq / 44100.0 * (PI*2.0));
      nbuf(0,i) = d;
    }

    // A series containing a RealvecSource followed by an AubioYin
    MarSystem* net = mng.create("Series", "series");
    net->addMarSystem(mng.create("RealvecSource", "src"));
    net->addMarSystem(mng.create("Yin", "yin"));

    net->updControl("mrs_natural/inSamples", buffer_size);
    net->updControl("mrs_real/israte", 44100.0);
    net->updControl("Yin/yin/mrs_real/frequency_min", 500.0);
    net->updControl("Yin/yin/mrs_real/frequency_max", 1500.0);

    // A realvec to store our answers in
    realvec check(num_buffers,1);

    for (int i=0; i < num_buffers; i++) {
      // Copy over the section of nbuf to ibuf
      int offset = i * buffer_size;
      for (mrs_natural j = 0; j < ibuf.getSize(); j++) {
        ibuf(0,j) = nbuf(0,j+offset);
      }

      // Copy ibuf to the RealvecSource
      net->updControl("RealvecSource/src/mrs_realvec/data", ibuf);

      // Tick the network once to push the data through the network
      net->tick();

      // Get the output of the network
      realvec r;
      r = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
      check(i,0) = r(0,0);
    }

    TS_ASSERT_DELTA(check(0,0),981.199,0.001);
    TS_ASSERT_DELTA(check(1,0),957.343,0.001);
    TS_ASSERT_DELTA(check(2,0),934.025,0.001);
    TS_ASSERT_DELTA(check(3,0),910.687,0.001);
    TS_ASSERT_DELTA(check(4,0),886.521,0.001);
    TS_ASSERT_DELTA(check(5,0),863.352,0.001);
    TS_ASSERT_DELTA(check(6,0),839.84,0.001);
    TS_ASSERT_DELTA(check(7,0),815.685,0.001);
    TS_ASSERT_DELTA(check(8,0),792.524,0.001);
    TS_ASSERT_DELTA(check(9,0),768.694,0.001);

    delete net;
  }

};

