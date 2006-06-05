/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/** 
    \class Conversions
    \brief Various conversion functions
*/

#include "Conversions.h"
using namespace std;


real pitch2hertz(real pitch) {
  return real(440.0 * pow(2.0, ((pitch - 69.0) / 12.0)));
}

real hertz2pitch(real hz) {
  return (hz == 0.0) ? (real)0.0 : (real)(69.0 + 12.0 * (log(hz/440.0)/log(2.0)));
}

real samples2hertz(natural samples, real srate) {
    return (samples == 0.0)  ? (real)0.0 : (real) (srate * 1.0) / (samples);
}

natural hertz2samples(real hz, real srate) {
  return (hz == 0.0) ? (natural)0 : (natural) (srate / hz);
}

/* convert a string representing time to number of samples base on the 
   given sample rate. Format "123.456#" where # is the time division.
   Valid time divisions: { h, m, s, ms, us }.
   On a format error, 
   Errors: -1 is returned. ie more than 1 decimal point, invalid time
     division.
*/
natural time2samples(string time, real srate) {
//example times: { "10us", "10ms", "10s", "10m", "10h" }
    if (time=="") { return 0; }
    // calculate time value
    real samples=0;
    int i=0;
    int len=time.length();
    bool decimal_point=false;
    real divisor = 10.0;
    for (i=0;i<len && (time[i]=='.' || (time[i]>='0' && time[i]<='9'));i++) {
        if (decimal_point) {
            if (time[i]=='.') { return -1.0; }
            samples = samples + ((real)(time[i]-'0'))/divisor;
            divisor = divisor * 10.0;
        } else if (time[i]=='.') {
            decimal_point=true;
        } else {
            samples = samples * 10.0 + (time[i]-'0');
        }
    }
    //
    if (i<len) {
        char a=time[i++];
        if (i>=len) {
            if (a=='h') { // hours
                samples= 120.0*samples*srate;
            } else if (a=='m') { // minutes
                samples=  60.0*samples*srate;
            } else if (a=='s') { // seconds
                samples=       samples*srate;
            } else {
                return -1.0;
            }
        } else {
            char b=time[i];
            if ((i+1)>=len) {
                if (a=='u' && b=='s') { // micro-seconds
                    samples= samples/1000000.0*srate;
                } else if (a=='m' && b=='s') { // milli-seconds
                    samples= samples/1000.0*srate;
                } else {
                    return -1.0;
                }
            }
        }
    }
    return (natural)samples;
}
natural time2usecs(string time) {
//example times: { "10us", "10ms", "10s", "10m", "10h" }
    if (time=="") { return 0; }
    // calculate time value
    real usecs=0;
    int i=0;
    int len=time.length();
    bool decimal_point=false;
    real divisor = 10.0;
    for (i=0;i<len && (time[i]=='.' || (time[i]>='0' && time[i]<='9'));i++) {
        if (decimal_point) {
            if (time[i]=='.') { return -1.0; }
            usecs = usecs + ((real)(time[i]-'0'))/divisor;
            divisor = divisor * 10.0;
        } else if (time[i]=='.') {
            decimal_point=true;
        } else {
            usecs = usecs * 10.0 + (time[i]-'0');
        }
    }
    //
    if (i<len) {
        char a=time[i++];
        if (i>=len) {
            if (a=='h') { // hours
                usecs *= 1000.0 * 1000.0 * 60.0 * 60.0;
            } else if (a=='m') { // minutes
                usecs *= 1000.0 * 1000.0 * 60.0;
            } else if (a=='s') { // seconds
                usecs *= 1000.0 * 1000.0;
            } else {
                return -1.0;
            }
        } else {
            char b=time[i];
            if ((i+1)>=len) {
                if (a=='u' && b=='s') { // micro-seconds
                    ;
                } else if (a=='m' && b=='s') { // milli-seconds
                    usecs *= 1000.0;
                } else {
                    return -1.0;
                }
            }
        }
    }
    return (natural)usecs;
}

