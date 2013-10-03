// TestWekaSource.h

//
// The WekaSource class loads a Weka .arff file.

//
// (c) 2008 - sness@sness.net - GPL
//

#include <cxxtest/TestSuite.h>
#include <cstdio>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/marsystems/WekaSource.h>
#include <marsyas/CommandLineOptions.h>

#include <vector>

using namespace std;
using namespace Marsyas;

class WekaSource_runner : public CxxTest::TestSuite
{
public:
  realvec in,out;

  void
  setUp()
  {
  }
  // "destructor"
  void tearDown()
  {
  }

  void test_pass(void)
  {
    MarSystemManager mng;
    MarSystem* net = mng.create("Series/net");
    net->addMarSystem(mng.create("WekaSource/wsrc"));
    net->updControl("WekaSource/wsrc/mrs_string/filename", "files/tiny.arff");


    net->tick();
    mrs_realvec processedData = net->getControl("mrs_realvec/processedData")->to<mrs_realvec>();
    TS_TRACE("Checking WekaSource reading data line 1");
    TS_ASSERT_EQUALS(processedData(0,0), 0.056167);
    net->tick();
    processedData = net->getControl("mrs_realvec/processedData")->to<mrs_realvec>();
    TS_TRACE("Checking WekaSource reading data line 2");
    TS_ASSERT_EQUALS(processedData(0,0), 0.082136);

    delete net;
  }

  void test_memory_fold(void)
  {
    TS_TRACE("Checking memory leak in wekasource fold");
    MarSystemManager mng;
    MarSystem* net = mng.create("Series/net");
    net->addMarSystem(mng.create("WekaSource/wsrc"));
    // control updates MUST be in this order to trigger the double free!
    net->updControl("WekaSource/wsrc/mrs_string/validationMode", "kFold,NS,10");
    net->updControl("WekaSource/wsrc/mrs_string/filename", "files/normal.arff");

    net->tick();

    delete net;
    TS_TRACE("done test for memory leak in wekasource fold");
  }

};

