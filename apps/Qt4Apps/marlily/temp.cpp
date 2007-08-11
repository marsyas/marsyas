#include "MarSystemManager.h"
#include "math.h"
using namespace Marsyas;
using namespace std;

int main(int argc, char *argv[])
{
	mrs_real bar;
	bar = NAN;
	bar = samples2hertz(bar,44100.0);
	cout<<bar<<endl;
}

