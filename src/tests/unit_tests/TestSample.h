// TestSample.h

//
// A sample skeleton for a unit test
//
// (c) 2008 - sness@sness.net - GPL
//

// Usage instructions:
//
// 1) Copy this file to something like TestFooBar.h in the unit test folder
//    src/tests/unit_tests. Make sure the file name starts with "Test" and
//    ends with ".h".
//
//    Replace "FooBar" in this file with the MarSystem you want to test and
//    implement the tests. For more information: see the already implemented
//    unit tests in this folder and check out the documentation for cxxtest at
//    http://cxxtest.sourceforge.net/
//
//    Note: the CxxTest tools require that the whole function definition of a
//    test is on one line, like this:
//        void test_pass(void)
//    This is contrary to the Marsyas code conventions.
//
// 2) Run the test runner generator script
//        scripts/generate-unit-test-cpp-files.py
//    This will generate a test runner source file like FooBar_runner.cpp.
//
// 3) Add an entry for this generated runner to
//    src/tests/unit_tests/CMakeLists.txt
//
// 4) Run CMake to generate the tests.
//    (Enable the MARSYAS_TESTS option if not on already.)
//
//        cd path/to/marsyas-build-dir
//        ccmake path/to/marsyas/marsyas/src
//
// 6) Compile and run the tests as follows
//        make test
//    If everything passes: make a cheering sound.
//    If not: you'll find the testing log at
//       path/to/marsyas-build-dir/Testing
//
// TODO: use a Cmake plugin to do step 2 automatically (e.g. after a step like 3)?

#include <cxxtest/TestSuite.h>

#include <cstdio>
#include "Collection.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"

// The class that you want to test, for example "FooBar"
#include "FooBar.h"

#include <vector>

using namespace std;
using namespace Marsyas;

class FooBar_runner : public CxxTest::TestSuite
{
public:
  realvec in,out;
  MarSystemManager mng;
  FooBar *foobar;

  void
  setUp()
  {
	foobar = new FooBar("foobar");
  }

  void test_pass(void)
  {
	TS_ASSERT( 1 + 1 > 1 );
  }



};

