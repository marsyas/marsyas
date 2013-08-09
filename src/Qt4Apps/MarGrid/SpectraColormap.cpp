#include "SpectraColormap.h"

int SpectraColormap::getDepth() const {
  return SPECTRACOLORMAP_SIZE;
}

unsigned char SpectraColormap::getRed(int index) const {
  if ( index > 0 && index < SPECTRACOLORMAP_SIZE ) {
    return map[index][COLORMAP_RED];
  }
  return 0;
}

unsigned char SpectraColormap::getGreen(int index) const {
  if ( index > 0 && index < SPECTRACOLORMAP_SIZE ) {
    return map[index][COLORMAP_GREEN];
  }
  return 0;
}

unsigned char SpectraColormap::getBlue(int index) const {
  if ( index > 0 && index < SPECTRACOLORMAP_SIZE ) {
    return map[index][COLORMAP_BLUE];
  }
  return 0;
}

/*
 * SpectraColor Map extracted from Matlab, basic colors used
 * in spectragraph output me thinks
 */
const unsigned char SpectraColormap::map[SPECTRACOLORMAP_SIZE][3] = {
  {   0,	  0,	143 },
  {   0,	  0,	159 },
  {   0,	  0,	175 },
  {   0,	  0,	191 },
  {   0,	  0,	207 },
  {   0,	  0,	223 },
  {   0,	  0,	239 },
  {   0,	  0,	255 },
  {   0,	 16,	255 },
  {   0,	 32,	255 },
  {   0,	 48,	255 },
  {   0,	 64,	255 },
  {   0,	 80,	255 },
  {   0,	 96,	255 },
  {   0,	112,	255 },
  {   0,	128,	255 },
  {   0,	143,	255 },
  {   0,	159,	255 },
  {   0,	175,	255 },
  {   0,	191,	255 },
  {   0,	207,	255 },
  {   0,	223,	255 },
  {   0,	239,	255 },
  {   0,	255,	255 },
  {  16,	255,	239 },
  {  32,	255,	223 },
  {  48,	255,	207 },
  {  64,	255,	191 },
  {  80,	255,	175 },
  {  96,	255,	159 },
  { 112,	255,	143 },
  { 128,	255,	128 },
  { 143,	255,	112 },
  { 159,	255,	 96 },
  { 175,	255,	 80 },
  { 191,	255,	 64 },
  { 207,	255,	 48 },
  { 223,	255,	 32 },
  { 239,	255,	 16 },
  { 255,	255,	  0 },
  { 255,	239,	  0 },
  { 255,	223,	  0 },
  { 255,	207,	  0 },
  { 255,	191,	  0 },
  { 255,	175,	  0 },
  { 255,	159,	  0 },
  { 255,	143,	  0 },
  { 255,	128,	  0 },
  { 255,	112,	  0 },
  { 255,	 96,	  0 },
  { 255,	 80,	  0 },
  { 255,	 64,	  0 },
  { 255,	 48,	  0 },
  { 255,	 32,	  0 },
  { 255,	 16,	  0 },
  { 255,	  0,	  0 },
  { 239,	  0,	  0 },
  { 223,	  0,	  0 },
  { 207,	  0,	  0 },
  { 191,	  0,	  0 },
  { 175,	  0,	  0 },
  { 159,	  0,	  0 },
  { 143,	  0,	  0 },
  { 128,	  0,	  0 }
};


