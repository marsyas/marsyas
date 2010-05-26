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
  MarSystemManager mng;
  WekaSource *norm;

  void
  setUp()
  {
	norm = new WekaSource("norm");
  }

  void test_pass(void) 
  {
	TS_ASSERT( 1 + 1 > 1 );
  }



};

