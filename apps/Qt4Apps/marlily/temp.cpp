#include "MarSystemManager.h"
using namespace Marsyas;

realvec makereal()
{
	realvec foo(1);
	foo(0)=1;
	return foo;
}

int main(int argc, char *argv[])
{
	realvec foo = makereal();
	std::cout<<foo;
}

