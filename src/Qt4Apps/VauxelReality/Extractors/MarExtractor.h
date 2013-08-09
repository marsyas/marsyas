
#ifndef MAREXTRACTOR_H_
#define MAREXTRACTOR_H_

#include "AbstractMarExtractor.h"

class MarExtractor : public AbstractMarExtractor
{
  Q_OBJECT

public:
  MarExtractor();
  ~MarExtractor();

  void setup();

public slots:
  void extract(Marsyas::realvec& dest);
};

#endif /* MAREXTRACTOR_H_ */
