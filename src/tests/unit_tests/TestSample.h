// TestSample.h

//
// A sample skeleton for a unit test
//
//
// (c) 2008 - sness@sness.net - GPL
//

//
// Usage instructions:
//
// 1) Copy this file to something like TestWekaSource.h
//
// 2) Add this test to the CMakeLists.txt file in the unit test directory (src/tests/unit_tests/CMakeLists.txt)
//
// 3) Add an entry for this class to the generator script (scripts/buildscripts/generate-unit-test-cpp-files.sh)
//
// 4) Run the generator to generate a xxx_runner.cpp file for this test
//
// 5) Run CMake to generate the tests 
//    
//      cd ~/marsyas-build-dir
//      ccmake /path/to/marsyas/marsyas/src
// 
// 6) Run the tests
//
//      make test
//
// 7) Enjoy.
//
//
// Check out documentation for cxxtest at: http://cxxtest.sourceforge.net/
//
// *NB* : Due to strangeness in cxxtestgen.py, you have to put the whole
// function definition of all tests on the same line like this:
//
//    void test_pass(void)
//
// This is contrary to the Marsyas code conventions.
//

#include <cxxtest/TestSuite.h>

#include <cstdio>
#include "Collection.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"

// The class that you want to test, for example "WekaSource"
#include "Sample.h"

#include <vector>

using namespace std;
using namespace Marsyas;

class Sample_runner : public CxxTest::TestSuite
{
public:
  realvec in,out;
  MarSystemManager mng;
  Sample *wsrc;

  void
  setUp()
  {
	wsrc = new Sample("wsrc");
  }

  void test_pass(void) 
  {
	TS_ASSERT( 1 + 1 > 1 );
  }



};

