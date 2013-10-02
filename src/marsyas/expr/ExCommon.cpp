#include <marsyas/expr/ExCommon.h>

using namespace Marsyas;

unsigned int
ex_string_to_typeid(std::string tp)
{
  if (tp=="mrs_unit") return ExT_mrs_unit;
  if (tp=="mrs_bool") return ExT_mrs_bool;
  if (tp=="mrs_natural") return ExT_mrs_natural;
  if (tp=="mrs_real") return ExT_mrs_real;
  if (tp=="mrs_string") return ExT_mrs_string;
  if (tp=="mrs_timer") return ExT_mrs_timer;
  if (tp=="mrs_scheduler") return ExT_mrs_scheduler;
  return 0;
}

std::string
ex_typeid_to_string(unsigned int tp)
{
  if (tp==ExT_mrs_unit) return "mrs_unit";
  if (tp==ExT_mrs_bool) return "mrs_bool";
  if (tp==ExT_mrs_natural) return "mrs_natural";
  if (tp==ExT_mrs_real) return "mrs_real";
  if (tp==ExT_mrs_string) return "mrs_string";
  if (tp==ExT_mrs_timer) return "mrs_timer";
  if (tp==ExT_mrs_scheduler) return "mrs_scheduler";
  return 0;
}

std::string
Marsyas::dtos(double d)
{
  std::ostringstream oss;
  oss << d;
  return oss.str();

}

std::string
Marsyas::dtos(float d)
{
  std::ostringstream oss;
  oss << d;
  return oss.str();
}

std::string
Marsyas::ltos(mrs_natural l)
{
  std::ostringstream oss;
  oss << l;
  return oss.str();
}

std::string
Marsyas::btos(mrs_bool b)
{
  return (b) ? "true" : "false";
}

mrs_natural
Marsyas::stol(std::string n)
{
  long num=0; unsigned int i=0; bool neg=false;
  if (n[0]=='-') { neg=true; i=1; }
  for (; i<n.length(); ++i) {
    num = (num*10) + (n[i] - '0');
  }
  return (neg) ? -num : num;
}

