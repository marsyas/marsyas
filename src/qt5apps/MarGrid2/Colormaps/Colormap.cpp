#include "Colormap.h"
#include "SpectraColormap.h"
#include "GreyScaleColormap.h"

Colormap* Colormap::factory(Colormaps colormap) {
  switch ( colormap ) {
  case Spectra:
    return new SpectraColormap;
  case GreyScale:
  default:
    return new GreyScaleColormap;
  };
}

