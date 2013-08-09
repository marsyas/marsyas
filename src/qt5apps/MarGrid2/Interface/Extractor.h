#ifndef EXTRACTOR_H
#define EXTRACTOR_H


class Extractor
{
public:
  virtual ~Extractor();

  virtual void setup();
  virtual void extract();
};


#endif /* EXTRACTOR_H */

