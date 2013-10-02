/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include <marsyas/common_source.h>
#include "Talk.h"

using namespace std;
using namespace Marsyas;

Talk::Talk()
{
}


Talk::~Talk()
{
  // delete src_;
}

void
Talk::init(Communicator *in_com)
{
  communicator_ = in_com;
  dest_ = new AudioSink("dest");
}


struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

void
Talk::cmd_colorgram(mrs_string collection, mrs_string system, unsigned int start, unsigned int end, unsigned int winSize)
{
  // FIXME All the code to this function is commented out, so none of the parameters are used.
  (void) collection;
  (void) system;
  (void) start;
  (void) end;
  (void) winSize;

  // fmatrix projectMatrix;
//   fvec statVector;


//   mrs_string projectStr1 = MARSYAS_MFDIR;
//   projectStr1 += "/projectfiles/";
//   projectStr1 += collection;
//   projectStr1 += system;
//   projectStr1 += "prj.mf";
//   projectMatrix.read(projectStr1);

//   mrs_string projectStr2 = MARSYAS_MFDIR;
//   projectStr2 += "/projectfiles/";
//   projectStr2 += collection;
//   projectStr2 += system;
//   projectStr2 += "sta.mf";
//   statVector.read(projectStr2);

//   cerr << "Colorgram called with " << projectStr1 << endl;
//   cerr << "Colorgram called with " << projectStr2 << endl;

//   mrs_string extractorstr = system;

//   int iwinSize = winSize;
//   if (winSize != DEFAULT_WIN_SIZE)
//     {
//       start = (unsigned int)(start * ((float)winSize / DEFAULT_WIN_SIZE));
//       end = (unsigned int) (end * ((float)winSize/ DEFAULT_WIN_SIZE));
//       winSize = DEFAULT_WIN_SIZE;
//     }
//   src_->initWindow(winSize);
//   cerr << "Src winSize = " << src_->winSize() << endl;


//   Spectral spectral(src_);
//   SpectralSegm spectralsegm(src_, 10);
//   MemMFCC mfcc(src_);

//   FeatExtractor mfccExtractor(src_, &mfcc);
//   FeatExtractor spectralExtractor(src_, &spectral);
//   FeatExtractor segmExtractor(src_, &spectralsegm);
//   SfxExtractor sfxExtractor(src_);

//   FeatMatrix mfccRes(src_->iterations(), mfcc.outSize());
//   FeatMatrix spectralRes(src_->iterations(), spectral.outSize());
//   FeatMatrix spectralSegmRes(src_->iterations(), spectralsegm.outSize());
//   FeatMatrix sfxRes(1, 2);


//   map<const char *, FeatMatrix *, ltstr> results;
//   results["FFT"] = &spectralRes;
//   results["FFT_SEGM"] = &spectralSegmRes;
//   results["MFCC"] = &mfccRes;
//   results["SFX"] = &sfxRes;


//   map<const char *, Extractor *, ltstr> extractors;
//   extractors["FFT"] = &spectralExtractor;
//   extractors["FFT_SEGM"] = &segmExtractor;
//   extractors["MFCC"] = &mfccExtractor;
//   extractors["SFX"] = &sfxExtractor;


//   map<const char *, Extractor *, ltstr>::iterator cur;
//   const char *ch = extractorstr.c_str();
//   cur = extractors.find(ch);

//   if (cur == extractors.end())
//     {
//       cerr << "Extractor " << extractorstr << " is not supported\n" << endl;
//       return;
//     }
//   else
//     {
//       extractors[extractorstr.c_str()]->extract(*(results[extractorstr.c_str()]));
//     }

//   (*(results[extractorstr.c_str()])) *= projectMatrix;
//   unsigned int rows;
//   rows =   (*(results[extractorstr.c_str()])).rows();
//   unsigned int cols;
//   cols =   (*(results[extractorstr.c_str()])).cols();
//   mrs_string name;
//   name = (*(results[extractorstr.c_str()])).name();


//   fvec pc1(rows);
//   fvec pc2(rows);
//   fvec pc3(rows);

//   pc1 = (*(results[extractorstr.c_str()])).col(cols-1);
//   pc2 = (*(results[extractorstr.c_str()])).col(cols-2);
//   pc3 = (*(results[extractorstr.c_str()])).col(cols-3);

//   pc1.renorm(statVector(0), statVector(1), 0.5f, 0.2f);
//   pc2.renorm(statVector(2), statVector(3), 0.5f, 0.2f);
//   pc3.renorm(statVector(4), statVector(5), 0.5f, 0.2f);

//   int scale;
//   scale = iwinSize / winSize;
//   fvec rvec(rows / scale);
//   fvec gvec(rows / scale);
//   fvec bvec(rows / scale);

//   unsigned int i;
//   unsigned int skip = 0;

//   for (i=0; i < rows / scale; ++i)
//     {
//       rvec(i) = pc1(i * scale + skip);
//       gvec(i) = pc2(i * scale + skip);
//       bvec(i) = pc3(i * scale + skip);
//     }
//   ColorPlot cplot;
//   cplot.init(name, rvec, gvec, bvec);

//   cerr << "Colorgram called with " << projectStr1 << endl;
//   cerr << "Colorgram called with " << projectStr2 << endl;
//   cplot.send(communicator_);
}


void
Talk::cmd_fullfft(unsigned int winSize)
{
  // FIXME The code to this function is gone, winSize is unused.
  (void) winSize;

//   src_->initWindow(winSize, winSize, 0, 0);
//   unsigned int iterations  = src_->iterations();
//   fvec win(winSize);
//   fvec fftmag(winSize/2);
//   fvec fullmag((winSize/2)* iterations);
//   MagFFT mag(winSize);
//   unsigned int i,j;
//   for (i=0; i < iterations; ++i)
//     {
//       src_->get(win, i);
//       mag.process(win, fftmag);
//       for (j=0; j < winSize/2; j++)
// 	fullmag(i * (winSize/2) + j) = fftmag(j);
//     }
//   fullmag.send(communicator_);
}


void
Talk::cmd_fft(unsigned int start, unsigned int winSize)
{
  // FIXME Missing code (again).
  (void) start;
  (void) winSize;
//   fvec win(winSize);
//   fvec fftmag(winSize/2);
//   src_->initWindow(winSize, winSize, 0, 0);
//   MagFFT mag(winSize);
//   src_->get(win, start);
//   mag.process(win, fftmag);
//   fftmag.send(communicator_);
}



void Talk::cmd_play(mrs_natural start, mrs_natural end, mrs_natural lineSize)
{
  communicator_->send_message("From Server: Play command received\n");



  src_->updControl("mrs_natural/pos", (mrs_natural)start * lineSize);
  src_->updControl("mrs_natural/inSamples", lineSize);



  Series *series = new Series("playbacknet");
  series->addMarSystem(src_);
  series->addMarSystem(dest_);


  series->updControl("AudioSink/dest/mrs_natural/nChannels",
                     series->getctrl("SoundFileSource/src/mrs_natural/nChannels")->to<mrs_natural>());
  for (int i=0; i < end-start; ++i)
  {
    series->tick();
    // communicator_->send_message("tick\n");
  }

}


void
Talk::cmd_load(mrs_string fname, mrs_natural lineSize)
{
  cout << "cmd_load called" << endl;

  src_ = new SoundFileSource("src");
  src_->updControl("mrs_string/filename", fname);
  fname_ = fname;
  src_->updControl("mrs_natural/inSamples", lineSize);
  AbsMax* absmax = new AbsMax("absmax");

  Series *series = new Series("plot");
  series->addMarSystem(src_);
  series->addMarSystem(absmax);


  mrs_natural hops = src_->getctrl("mrs_natural/size")->to<mrs_natural>() * src_->getctrl("mrs_natural/nChannels")->to<mrs_natural>() / src_->getctrl("mrs_natural/inSamples")->to<mrs_natural>() + 1;


  Accumulator* acc = new Accumulator("acc");
  acc->updControl("mrs_natural/nTimes", hops);
  acc->addMarSystem(series);



  realvec in(acc->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
             acc->getctrl("mrs_natural/inSamples")->to<mrs_natural>());

  realvec out(acc->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
              acc->getctrl("mrs_natural/onSamples")->to<mrs_natural>());



  acc->process(in,out);

  out.send(communicator_);



//   Util util;
//   fname_ = fname;
//   src_ = util.sfopen(fname, MRS_SF_READ);
//   if (src_ == NULL)
//     cout << "src_ = NULL" << endl;

//   if (src_ != NULL)			// File exists
//     {
//       src_->initWindow(lineSize, lineSize, 0, 0);
//       PlotExtractor pextractor(src_, src_->winSize());
//       fvec res(src_->iterations());
//       pextractor.extract(0, src_->iterations(), res);
//       res.send(communicator_);
//     }
//   else
//     {
//       fvec res(0);
//       res.send(communicator_);
//     }
}

void
Talk::cmd_extract(mrs_string systemName, mrs_string fileName, TimeLine tline)
{
  // FIXME See above.
  (void) systemName;
  (void) fileName;
  (void) tline;
//   cerr << "cmd_extract::systemName " << systemName << endl;
//   cerr << "cmd_extract::fileName   " << fileName << endl;

//   FileName fromFile(fname_);
//   mrs_string fromName = fromFile.name();
//   mrs_string extractorstr = systemName;

//   src_->initWindow(512, 512, 0, 0);

//   /* Special window sizes for specific extractors */
//   if (extractorstr == "LPC")
//     src_->initWindow(512, 128, 0, 0);
//   if (extractorstr == "BEAT")
//     src_->initWindow(65536, 65536/2,0,0);
//   if (extractorstr == "DWTC")
//     src_->initWindow(16384, 512, 0, 0);
//   if (extractorstr == "MPITCH")
//     src_->initWindow(512, 512, 0, 0);


//   /* Extractor dictionary contains all the available
//      extractors for feature calculation */
//   ExtractorDictionary dict(src_);

//   /* find appropriate extractor and feature matrix result */
//   Extractor *extr = dict.getExtractor(extractorstr);
//   FeatMatrix *featMatrix = dict.getFeatMatrix(extractorstr);

//   /* do the feature calculation */
//   extr->extract(*featMatrix);

//   /* append the feature matrix to the output file for the collection */
//   mrs_string name = fromName.substr(0, fromName.size()-3);
//   featMatrix->setName(name);
//   imatrix labels(featMatrix->rows(), 2);
//   ivec labelSizes(2);
//   labelSizes(0) = 0;
//   labelSizes(1) = 0;
//   int r;

//   for (r=0; r < labels.rows(); ++r)
//     {
//       labels(r,0) = 0;
//       labels(r,1) = 1;
//     }
//   featMatrix->setLabels(labels, labelSizes);



//   featMatrix->write(fileName);
//   src_->initWindow(512, 512, 0, 0);
}




void
Talk::cmd_segment(mrs_string systemName, unsigned int memSize, unsigned int numPeaks, unsigned int peakSpacing, unsigned int start, unsigned int end, unsigned int winSize)
{
  // FIXME Unused parameters
  (void) memSize;
  (void) numPeaks;
  (void) peakSpacing;
  (void) start;
  (void) end;
  (void) winSize;

  TimeLine tline;

  mrs_natural hops = src_->getctrl("mrs_natural/size")->to<mrs_natural>() * src_->getctrl("mrs_natural/nChannels")->to<mrs_natural>() / src_->getctrl("mrs_natural/inSamples")->to<mrs_natural>() + 1;

  if(!strcmp(systemName.c_str(), "REG"))
    tline.regular(100, hops);

  realvec peaks(hops);


  tline.send(communicator_);
  peaks.send(communicator_);


//   tline.print(stdout);

//   cerr << "cmd_segment::systemName " << systemName << endl;
//   cerr << "cmd_segment::memSize " << memSize << endl;
//   cerr << "cmd_segment::numPeaks " << numPeaks << endl;
//   cerr << "cmd_segment::peakSpacing " << peakSpacing << endl;
//   cerr << "cmd_segment::start " << start << endl;
//   cerr << "cmd_segment::end " << end << endl;
//   cerr << "cmd_segment::winSize " << winSize << endl;






//   mrs_string extractorstr = systemName;
//   mrs_string rextractorstr = systemName;
//   if (!strcmp(rextractorstr.c_str(), "REG"))
//       extractorstr = "FFT_SEGM";

//   if (winSize != DEFAULT_WIN_SIZE)
//     {
//       start = (unsigned int)(start * ((float)winSize / DEFAULT_WIN_SIZE));
//       end = (unsigned int) (end * ((float)winSize/ DEFAULT_WIN_SIZE));
//       winSize = DEFAULT_WIN_SIZE;
//     }
//   src_->initWindow(winSize);
//   cerr << "Src winSize = " << src_->winSize() << endl;


//   Spectral spectral(src_);
//   SpectralSegm spectralsegm(src_,10);
//   MemMFCC mfcc(src_);

//   FeatExtractor mfccExtractor(src_, &mfcc);
//   FeatExtractor spectralExtractor(src_, &spectral);
//   FeatExtractor segmExtractor(src_, &spectralsegm);
//   SfxExtractor sfxExtractor(src_);

//   FeatMatrix mfccRes(src_->iterations(), mfcc.outSize());
//   FeatMatrix spectralRes(src_->iterations(), spectral.outSize());
//   FeatMatrix spectralSegmRes(src_->iterations(), spectralsegm.outSize());
//   FeatMatrix sfxRes(1, 2);


//   map<const char *, FeatMatrix *, ltstr> results;
//   results["FFT"] = &spectralRes;
//   results["FFT_SEGM"] = &spectralSegmRes;
//   results["MFCC"] = &mfccRes;
//   results["SFX"] = &sfxRes;


//   map<const char *, Extractor *, ltstr> extractors;
//   extractors["FFT"] = &spectralExtractor;
//   extractors["FFT_SEGM"] = &segmExtractor;
//   extractors["MFCC"] = &mfccExtractor;
//   extractors["SFX"] = &sfxExtractor;


//   map<const char *, Extractor *, ltstr>::iterator cur;
//   const char *ch = extractorstr.c_str();
//   cur = extractors.find(ch);

//   if (cur == extractors.end())
//     {
//       cerr << "Extractor " << extractorstr << " is not supported\n" << endl;
//       return;
//     }
//   else
//     {
//       extractors[extractorstr.c_str()]->extract(*(results[extractorstr.c_str()]));
//     }
//   TimeLine tline;
//   SegmentorSortedPeaks segmentor;
//   segmentor.init(numPeaks, peakSpacing);
//   fvec res((*(results[extractorstr.c_str()])).rows());
//   fvec peaks((*(results[extractorstr.c_str()])).rows());
//   segmentor.segment(*(results[extractorstr.c_str()]), res);
//   segmentor.peaks(*(results[extractorstr.c_str()]), peaks);


}


void
Talk::cmd_merge(mrs_string systemName, unsigned int pstart, unsigned int pend,
                unsigned int start, unsigned int end, unsigned int winSize)
{
  // FIXME See above (missing function body)
  (void) systemName;
  (void) pstart;
  (void) pend;
  (void) start;
  (void) end;
  (void) winSize;

//   mrs_string extractorstr = systemName;
//   if (winSize != DEFAULT_WIN_SIZE)
//     {
//       pstart = (unsigned int)(start * ((float)winSize / DEFAULT_WIN_SIZE));
//       pend = (unsigned int) (end * ((float)winSize/ DEFAULT_WIN_SIZE));
//       winSize = DEFAULT_WIN_SIZE;
//     }


//   if (winSize != DEFAULT_WIN_SIZE)
//     {
//       start = (unsigned int)(start * ((float)winSize / DEFAULT_WIN_SIZE));
//       end = (unsigned int) (end * ((float)winSize/ DEFAULT_WIN_SIZE));
//       winSize = DEFAULT_WIN_SIZE;
//     }

//   src_->initWindow(winSize);
//   cerr << "Src winSize = " << src_->winSize() << endl;

//   Spectral spectral(src_);
//   SpectralSegm spectralsegm(src_, 10);
//   MemMFCC mfcc(src_);

//   FeatExtractor mfccExtractor(src_, &mfcc);
//   FeatExtractor spectralExtractor(src_, &spectral);
//   FeatExtractor segmExtractor(src_, &spectralsegm);
//   SfxExtractor sfxExtractor(src_);

//   FeatMatrix mfccRes(end-start, mfcc.outSize());
//   FeatMatrix spectralRes(end-start, spectral.outSize());
//   FeatMatrix spectralSegmRes(end-start, spectralsegm.outSize());
//   FeatMatrix sfxRes(1, 2);


//   map<const char *, FeatMatrix *, ltstr> results;

//   results["FFT"] = &spectralRes;
//   results["FFT_SEGM"] = &spectralSegmRes;
//   results["MFCC"] = &mfccRes;
//   results["SFX"] = &sfxRes;


//   FeatMatrix pmfccRes(pend-pstart, mfcc.outSize());
//   FeatMatrix pspectralRes(pend-pstart, spectral.outSize());
//   FeatMatrix pspectralSegmRes(pend-pstart, spectralsegm.outSize());
//   FeatMatrix psfxRes(1, 2);


//   map<const char *, FeatMatrix *, ltstr> presults;
//   presults["FFT"] = &pspectralRes;
//   presults["FFT_SEGM"] = &pspectralSegmRes;
//   presults["MFCC"] = &pmfccRes;
//   presults["SFX"] = &psfxRes;


//   map<const char *, Extractor *, ltstr> extractors;
//   extractors["FFT"] = &spectralExtractor;
//   extractors["FFT_SEGM"] = &segmExtractor;
//   extractors["MFCC"] = &mfccExtractor;
//   extractors["SFX"] = &sfxExtractor;


//   map<const char *, Extractor *, ltstr>::iterator cur;
//   const char *ch = extractorstr.c_str();
//   cur = extractors.find(ch);

//   if (cur == extractors.end())
//     {
//       cerr << "Extractor " << extractorstr << " is not supported\n" << endl;
//       return;
//     }
//   else
//     {
//       extractors[extractorstr.c_str()]->extract(pstart, pend, *(presults[extractorstr.c_str()]));
//       extractors[extractorstr.c_str()]->extract(start, end, *(results[extractorstr.c_str()]));
//     }

//   cout << "MEANS " << endl;

//   fvec m1 = (*(presults[extractorstr.c_str()])).meanRow();
//   fvec m2 = (*(results[extractorstr.c_str()])).meanRow();
//   fvec v1 = (*(presults[extractorstr.c_str()])).varRow();
//   fvec v2 = (*(results[extractorstr.c_str()])).varRow();
//   fvec d1 = m1;
//   fvec v12 = v1;
//   fvec v21 = v2;


//   d1 -= m2;
//   d1.sqr();
//   d1 *= ((v1 + v2)/(v1*v2));


//   v12 /= v2;
//   v21 /= v1;

//   fvec kl1 = d1;
//   kl1 += v12;
//   kl1 += v21;
//   int i;

//   float KL2 = 0.0;

//   for (i=0; i < kl1.size(); ++i)
//     {
//       KL2 += kl1(i);
//     }
//   KL2 /= kl1.size();

//   cout << "KL2 = " << KL2 << endl;



}


void
Talk::cmd_classify(mrs_string systemName, mrs_string classifierName, unsigned int start, unsigned int end, unsigned int winSize)
{
  // FIXME See above.
  (void) systemName;
  (void) classifierName;
  (void) start;
  (void) end;
  (void) winSize;

//   Gaussian classifier;
//   classifier.read(classifierName);


//   if (winSize != DEFAULT_WIN_SIZE)
//     {
//       start = (unsigned int)(start * ((float)winSize / DEFAULT_WIN_SIZE));
//       end = (unsigned int) (end * ((float)winSize/ DEFAULT_WIN_SIZE));
//       winSize = DEFAULT_WIN_SIZE;
//     }

//   mrs_string extractorstr = systemName;

//   if (winSize != DEFAULT_WIN_SIZE)
//     {
//       start = (unsigned int)(start * ((float)winSize / DEFAULT_WIN_SIZE));
//       end = (unsigned int) (end * ((float)winSize/ DEFAULT_WIN_SIZE));
//       winSize = DEFAULT_WIN_SIZE;
//     }
//   src_->initWindow(winSize);
//   cerr << "Src winSize = " << src_->winSize() << endl;


//   Spectral spectral(src_);
//   SpectralSegm spectralsegm(src_, 10);
//   MemMFCC mfcc(src_);

//   FeatExtractor mfccExtractor(src_, &mfcc);
//   FeatExtractor spectralExtractor(src_, &spectral);
//   FeatExtractor segmExtractor(src_, &spectralsegm);
//   SfxExtractor sfxExtractor(src_);

//   FeatMatrix mfccRes(end-start, mfcc.outSize());
//   FeatMatrix spectralRes(end-start, spectral.outSize());
//   FeatMatrix spectralSegmRes(end-start, spectralsegm.outSize());
//   FeatMatrix sfxRes(1, 2);


//   map<const char *, FeatMatrix *, ltstr> results;
//   results["FFT"] = &spectralRes;
//   results["FFT_SEGM"] = &spectralSegmRes;
//   results["MFCC"] = &mfccRes;
//   results["SFX"] = &sfxRes;


//   map<const char *, Extractor *, ltstr> extractors;
//   extractors["FFT"] = &spectralExtractor;
//   extractors["FFT_SEGM"] = &segmExtractor;
//   extractors["MFCC"] = &mfccExtractor;
//   extractors["SFX"] = &sfxExtractor;


//   map<const char *, Extractor *, ltstr>::iterator cur;
//   const char *ch = extractorstr.c_str();
//   cur = extractors.find(ch);

//   if (cur == extractors.end())
//     {
//       cerr << "Extractor " << extractorstr << " is not supported\n" << endl;
//       return;
//     }
//   else
//     {
//       extractors[extractorstr.c_str()]->extract(start, end, *(results[extractorstr.c_str()]));
//     }

//   float conf;

//   unsigned int classId = classifier.estimateClass(*(results[extractorstr.c_str()]),&conf);
//   mrs_string className = classifier.className(classId);
//   cerr << "Estimated className = " << className << endl;
//   cerr << "Estimated classId = " << classId << endl;



//   communicator_->send_message(className + "\n");

//   static char *buf = new char[256];
//   sprintf(buf, "%d\n", classId);
//   mrs_string classIdmessage;
//   classIdmessage = buf;

//   communicator_->send_message(classIdmessage);
//   cerr << "Finished sending message\n" << endl;
}



void
Talk::process(char *message)
{
  printf("Talk process called\n");
  cerr << "Message = " << message << endl;
  char *word;
  char *filename;
  mrs_string fname;
  unsigned int lineSize;
  int start, end, win_size;
  word = strtok(message, SEPCHARS);
  if (!strcmp(word,"load"))
  {
#ifdef MARSYAS_WIN32
    filename = _strdup(strtok(NULL,SEPCHARS));
#else
    filename = strdup(strtok(NULL,SEPCHARS));
#endif
    lineSize = atoi(strtok(NULL, SEPCHARS));
    fname = filename;
    cerr << "load command: Filename = " << filename <<  endl;
    cerr << "load command: LineSize = " << lineSize << endl;
    communicator_->send_message("From Server: Load command received. Preparing plot\n");
    cmd_load(fname, lineSize);



  }
  else if (!strcmp(word,"play"))
  {
    start = atoi(strtok(NULL, SEPCHARS));
    end   = atoi(strtok(NULL, SEPCHARS));
    lineSize = atoi(strtok(NULL, SEPCHARS));
    cerr << "play command. start = " << start << " end = " << end << endl;
    cmd_play(start, end, lineSize);
  }
  else if (!strcmp(word,"merge"))
  {
    mrs_string systemName = strtok(NULL, SEPCHARS);
    unsigned int pstart = atoi(strtok(NULL, SEPCHARS));
    unsigned int pend = atoi(strtok(NULL, SEPCHARS));
    unsigned int start = atoi(strtok(NULL, SEPCHARS));
    unsigned int end = atoi(strtok(NULL, SEPCHARS));
    unsigned int win_size = atoi(strtok(NULL, SEPCHARS));
    cerr << "Merge command " << endl;
    cerr << "System = " << systemName << " pstart = " << pstart <<
         " pend = " << pend << " start = " << start << " end = " << end
         << " win_size = " << win_size << endl;
    communicator_->send_message("From Server; merge command received\n");
    cmd_merge(systemName, pstart, pend, start, end, win_size);
  }


  else if (!strcmp(word,"classify"))
  {
    /* mrs_string systemName = strtok(NULL, SEPCHARS);
    mrs_string classifierName = MARSYAS_MFDIR;
    classifierName += "/classifiers/";
    classifierName += strtok(NULL, SEPCHARS);
    classifierName += systemName;
    classifierName += ".mcf";
    start = atoi(strtok(NULL, SEPCHARS));
    end   = atoi(strtok(NULL, SEPCHARS));
    win_size = atoi(strtok(NULL, SEPCHARS));
    cerr << "Classify command. " << endl;
    cerr << "System = " << systemName << " Classifier = " << classifierName << " start = " << start << " end = " << end << endl;
    communicator_->send_message("From Server: classify command received\n");
    cmd_classify(systemName, classifierName, start, end, win_size);
    */
  }
  else if (!strcmp(word,"fft"))
  {
    int start = atoi(strtok(NULL, SEPCHARS));
    int winSize  = atoi(strtok(NULL, SEPCHARS));
    communicator_->send_message("From Server: fft command received\n");
    cmd_fft(start, winSize);
  }
  else if (!strcmp(word, "fullfft"))
  {
    int winSize = atoi(strtok(NULL, SEPCHARS));
    communicator_->send_message("From Server: fullfft command received\n");
    cmd_fullfft(winSize);
  }
  else if (!strcmp(word,"segment"))
  {
    int mask_size = 11;
    int winSize;

    realvec mask(mask_size);
    for (int i=0; i<mask_size; ++i)
      mask(i) = (mrs_real)atof(strtok(NULL, SEPCHARS));
    cerr << "Segment command." << endl;
    mrs_string systemName = strtok(NULL, SEPCHARS);
    int memSize = atoi(strtok(NULL, SEPCHARS));
    int numPeaks = atoi(strtok(NULL, SEPCHARS));
    int peakSpacing  = atoi(strtok(NULL, SEPCHARS));
    start = atoi(strtok(NULL, SEPCHARS));
    end   = atoi(strtok(NULL, SEPCHARS));
    winSize = atoi(strtok(NULL, SEPCHARS));
    communicator_->send_message("From Server: segment command received\n");
    cmd_segment(systemName, memSize, numPeaks, peakSpacing, start, end, winSize);
  }
  else if (!strcmp(word,"extract"))
  {
    cerr << "Extract command." << endl;
    mrs_string systemName = strtok(NULL, SEPCHARS);
    mrs_string fileName = strtok(NULL, SEPCHARS);
    TimeLine tline;
    tline.receive(communicator_);

    communicator_->send_message("From Server: extract command received\n");

    cmd_extract(systemName, fileName, tline);
  }

  else if (!strcmp(word,"snoop"))
  {
    cerr << "Snoop command." << endl;
    //  these aren't used?  -gp
    //int feat_num;
    //feat_num = atoi(strtok(NULL, SEPCHARS));
    communicator_->send_message("Howdy. You just send a snoop command\n");
  }
  else if (!strcmp(word,"colorgram"))
  {
    cerr << "Colorgram command." << endl;
    mrs_string collection = strtok(NULL, SEPCHARS);
    mrs_string system = strtok(NULL, SEPCHARS);
    start = atoi(strtok(NULL, SEPCHARS));
    end   = atoi(strtok(NULL, SEPCHARS));
    win_size = atoi(strtok(NULL, SEPCHARS));
    communicator_->send_message("From server: colorgram command received.\n");
    cmd_colorgram(collection, system, start, end, win_size);
  }
  else
    communicator_->send_message("Howdy. Unknown command\n");
  cerr << "Talk::Process is done" << endl;
}



