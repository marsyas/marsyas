#ifndef COLOR_H
#define COLOR_H

#include <QWidget>
#include "PreferencesDialog.h"

class ColorPage : public PreferencesPage
{
public:
  ColorPage(QWidget *parent=0);
  void update() {}
};

#endif /* COLOR_H */
