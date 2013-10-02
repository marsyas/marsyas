#ifndef MARSYAS_EASYMAR_H
#define MARSYAS_EASYMAR_H

#include <marsyas/system/MarSystemManager.h>
using namespace Marsyas;

using namespace std;

class Easymar
{

public:
  Easymar();
  ~Easymar();

  static mrs_real addFileSource(MarSystem* net, const string infile);
  static void addDest(MarSystem* net, const string outfile);

private:
};
#endif

