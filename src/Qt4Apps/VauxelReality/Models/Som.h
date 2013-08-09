
#ifndef SOM_H_
#define SOM_H_

#include "AbstractMarClusteringModel.h"

class Som : public AbstractMarClusteringModel
{
  Q_OBJECT

public:
  Som();
  ~Som();

public slots:
  void train(Marsyas::realvec& data);
  void predict(Marsyas::realvec& data);
  void reset();
};

#endif