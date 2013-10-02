#include <marsyas/system/MarSystemManager.h>
#include "math.h"
using namespace Marsyas;
using namespace std;

int main(int argc, char *argv[])
{
  realvec foo(5);
  foo(0) = 1;
  foo(1) = 2;
  foo(2) = 3;
  foo(3) = 4;
  foo(4) = 5;
  /*
  	realvec bar(1);
  	bar(0) = 3;
  	foo.appendRealvec(bar);
  	cout<<foo;
  	cout<<bar;
  */
  realvec bar = foo.getSubVector(2, 2);
  cout<<bar;
  bar.~realvec();
}

