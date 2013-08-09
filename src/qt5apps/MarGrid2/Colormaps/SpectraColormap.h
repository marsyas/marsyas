#ifndef SPECTRACOLORMAP_H
#define SPECTRACOLORMAP_H

#define SPECTRACOLORMAP_SIZE	64

#include "Colormap.h"

class SpectraColormap : public Colormap
{
public:
  int getDepth() const;
  unsigned char getRed(int index) const;
  unsigned char getGreen(int index) const;
  unsigned char getBlue(int index) const;

private:
  static const unsigned char map[SPECTRACOLORMAP_SIZE][3];
};

#endif /* SPECTRACOLORMAP_H */

