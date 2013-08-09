#include "GreyScaleColormap.h"

int GreyScaleColormap::getDepth() const {
  return GREYSCALECOLORMAP_SIZE;
}

unsigned char GreyScaleColormap::getRed(int index) const {
  if ( index > 0 && index < GREYSCALECOLORMAP_SIZE ) {
    return index;
  }
  return 0;
}

unsigned char GreyScaleColormap::getGreen(int index) const {
  if ( index > 0 && index < GREYSCALECOLORMAP_SIZE ) {
    return index;
  }
  return 0;
}

unsigned char GreyScaleColormap::getBlue(int index) const {
  if ( index > 0 && index < GREYSCALECOLORMAP_SIZE ) {
    return index;
  }
  return 0;
}

