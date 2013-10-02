
#ifndef ABSTRACTMARCLUSTERINGMODEL_H_
#define ABSTRACTMARCLUSTERINGMODEL_H_

#include <QObject>

#include <marsyas/realvec.h>

class AbstractMarClusteringModel : public QObject
{
  Q_OBJECT

public:
  AbstractMarClusteringModel() {}
  virtual ~AbstractMarClusteringModel() {}

public slots:
  virtual void train(Marsyas::realvec& data) = 0;
  virtual void predict(Marsyas::realvec& data) = 0;
  virtual void reset() = 0;
};

#endif // ABSTRACTMARCLUSTERINGMODEL_H_