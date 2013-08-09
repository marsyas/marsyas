#ifndef CLASSIFIER_H
#define CLASSIFIER_H

class Classifier
{
public:
  virtual ~Classifier();
  virtual void classify();
  virtual void train();
};

#endif /* CLASSIFIER_H */

