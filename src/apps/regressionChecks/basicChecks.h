#ifndef REGCHECK_BASIC
#define REGCHECK_BASIC

// Basic Audio Processing checks
#include "common-reg.h"

// TODO: move
void basic_vibrato(string infile, string outfile);

void basic_delay(string infile, string outfile);

// Produces "settings that are not supported in Marsyas" .au files.  :(
void basic_downsample(string infile, string outfile);

void basic_negative(string infile, string outfile);

void basic_shifter(string infile, string outfile);

void basic_shiftInput(string infile, string outfile);

void basic_windowing(string infile, string outfile);

#endif
