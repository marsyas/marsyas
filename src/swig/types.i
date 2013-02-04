%{
#include <marsyas/common_header.h>

using namespace Marsyas;

%}

%include "std_string.i"

%apply(int) { mrs_natural };
%apply(bool) { mrs_bool };
%apply(double) { mrs_real };
%apply(std::string) { mrs_string };
