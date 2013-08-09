#ifndef GREYSCALECOLORMAP_H
#define GREYSCALECOLORMAP_H

#define GREYSCALECOLORMAP_SIZE 256

#include "Colormap.h"

class GreyScaleColormap : public Colormap
{
public:
  int getDepth() const;
  unsigned char getRed(int index) const;
  unsigned char getGreen(int index) const;
  unsigned char getBlue(int index) const;
};

#endif /* GREYSCALECOLORMAP_H */

