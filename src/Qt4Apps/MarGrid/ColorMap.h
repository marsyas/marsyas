#ifndef COLORMAP_H
#define COLORMAP_H

#define COLORMAP_RED	0
#define COLORMAP_GREEN	1
#define COLORMAP_BLUE	2

class Colormap
{
public:
  virtual ~Colormap() {}

  virtual int getDepth() const = 0;
  virtual unsigned char getRed(int index) const = 0;
  virtual unsigned char getGreen(int index) const = 0;
  virtual unsigned char getBlue(int index) const = 0;

  enum Colormaps {GreyScale, Spectra};
  static Colormap* factory(Colormaps colormap);
private:

};

#endif /* COLORMAP_H */

