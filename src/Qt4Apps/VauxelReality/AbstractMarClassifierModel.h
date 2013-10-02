#ifndef ABSTRACTMARCLASSIFIERMODEL_H_
#define ABSTRACTMARCLASSIFIERMODEL_H_

#include <QObject>

#include <marsyas/realvec.h>

class AbstractMarClassifierModel : public QObject
{
  Q_OBJECT

public:
  AbstractMarClassifierModel() {}
  virtual ~AbstractMarClassifierModel() {}

public slots:
  virtual void train(Marsyas::realvec& data) = 0;
  virtual void classify(Marsyas::realvec& data) = 0;
  virtual void reset() = 0;
};

#endif /* ABSTRACTMARCLASSIFIERMODEL_H_ */

