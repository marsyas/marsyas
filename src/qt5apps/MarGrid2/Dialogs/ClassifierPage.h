#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <QWidget>
#include "PreferencesDialog.h"

class ClassifierPage : public PreferencesPage
{
public:
  ClassifierPage(QWidget *parent=0);
  void update() {}
};

#endif /* CLASSIFIER_H */
