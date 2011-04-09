// TestWekaSource.h

//
// The WekaSource class loads a Weka .arff file.

//
// (c) 2008 - sness@sness.net - GPL
//

#include <cxxtest/TestSuite.h>
#include <cstdio>
#include "Collection.h"
#include "MarSystemManager.h"
#include "WekaSource.h"
#include "CommandLineOptions.h"

#include <vector>

using namespace std;
using namespace Marsyas;

class WekaSource_runner : public CxxTest::TestSuite
{
public:
  realvec in,out;
  MarSystem *net;

  void
  setUp()
  {
  }
	// "destructor"
	void tearDown()
	{
		delete net;
	}

  void test_pass(void) 
  {
    MarSystemManager mng;
    MarSystem* net = mng.create("Series/net");
    net->addMarSystem(mng.create("WekaSource/wsrc"));
    net->updControl("WekaSource/wsrc/mrs_string/filename", "files/tiny.arff");


    net->tick();
    mrs_realvec processedData = net->getControl("mrs_realvec/processedData")->to<mrs_realvec>();
    cout << processedData << endl;
    TS_TRACE("Checking WekaSource reading data line 1");
    TS_ASSERT_EQUALS(processedData(0,0), 0.056167);
    net->tick();
    processedData = net->getControl("mrs_realvec/processedData")->to<mrs_realvec>();
    TS_TRACE("Checking WekaSource reading data line 2");
    TS_ASSERT_EQUALS(processedData(0,0), 0.082136);
    
  }



};

