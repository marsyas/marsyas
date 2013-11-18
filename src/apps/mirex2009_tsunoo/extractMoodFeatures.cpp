#include <marsyas/system/MarSystemManager.h>
#include <marsyas/marsystems/Chroma.h>
#include <marsyas/marsystems/ChromaScale.h>
#include <marsyas/Collection.h>

using namespace std;
using namespace Marsyas;

void recognize(string sfName, string outName)
{
  MarSystemManager mng;

  mrs_realvec out, tmpvec;
  mrs_natural inputsize, wsize;
  mrs_real samplingFreq;
  Collection inputs;

  MarSystem* all = mng.create("Series", "all");
  MarSystem* acc = mng.create("Accumulator", "acc");
  MarSystem* net = mng.create("Series", "net");
  MarSystem* total = mng.create("Series", "total");
  MarSystem* src = mng.create("SoundFileSource", "src");
  MarSystem* spc = mng.create("Spectrum", "spc");
  MarSystem* pws = mng.create("PowerSpectrum", "pws");
  MarSystem* crm = mng.create("Chroma","crm");
  MarSystem* csl = mng.create("ChromaScale","csl");
  MarSystem* ff = mng.create("Fanout", "ff");
  MarSystem* ss = mng.create("Fanout", "ss");
  MarSystem* mn = mng.create("Mean", "mn");
  MarSystem* std = mng.create("StandardDeviation", "std");

  MarSystem* mfccnet = mng.create("Series", "mfccnet");

  MarSystem* ant = mng.create("Annotator", "ant");
  MarSystem* wks = mng.create("WekaSink", "wks");
  MarSystem* wksnet = mng.create("Series", "wksnet");

  inputs.read(sfName);

  mfccnet->addMarSystem(mng.create("Spectrum","spc2"));
  mfccnet->addMarSystem(mng.create("PowerSpectrum","psc2"));
  mfccnet->addMarSystem(mng.create("MFCC", "mfcc"));

  total->addMarSystem(src);
  total->addMarSystem(ff);
  net->addMarSystem(spc);
  net->addMarSystem(pws);
  net->addMarSystem(crm);
  net->addMarSystem(csl);
  ff->addMarSystem(mfccnet);
  ff->addMarSystem(net);
  acc->addMarSystem(total);
  all->addMarSystem(acc);
  ss->addMarSystem(mn);
  ss->addMarSystem(std);
  all->addMarSystem(ss);

  wksnet->addMarSystem(ant);
  wksnet->addMarSystem(wks);

  wksnet->updControl("mrs_natural/inSamples", 1);
  wksnet->updControl("mrs_natural/inObservations", all->getctrl("mrs_natural/onObservations")->to<mrs_natural>());
  wksnet->updControl("WekaSink/wks/mrs_string/labelNames", inputs.getLabelNames()); // change to just sfName
  wksnet->updControl("WekaSink/wks/mrs_natural/nLabels", (mrs_natural)inputs.getNumLabels());  // change to just 1
  wksnet->updControl("WekaSink/wks/mrs_natural/downsample", 1);
  wksnet->updControl("WekaSink/wks/mrs_string/filename", outName);
  tmpvec.create(all->getctrl("mrs_natural/onObservations")->to<mrs_natural>()+1,1);

  for(mrs_natural i=0; i<inputs.size(); ++i) {
    cout << "Now processing: " << inputs.entry(i) << endl;

    src->updControl("mrs_string/filename", inputs.entry(i));
    //spc->updControl("mrs_natural/inSamples", 1024);
    inputsize = src->getctrl("mrs_natural/size")->to<mrs_natural>();
    wsize = src->getctrl("mrs_natural/onSamples")->to<mrs_natural>();
    inputsize /= wsize;
    acc->updControl("mrs_natural/nTimes", inputsize);
    samplingFreq = src->getctrl("mrs_real/israte")->to<mrs_real>();
    all->update();
    crm->setctrl("mrs_real/samplingFreq", samplingFreq);
    crm->setctrl("mrs_natural/lowOctNum", 2);
    crm->setctrl("mrs_natural/highOctNum", 5);
    crm->setctrl("mrs_natural/inObservations", wsize/2);
    crm->update();
    csl->setctrl("mrs_natural/inObservations", 12);
    csl->update();

    all->tick();

    out = all->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    wksnet->updControl("Annotator/ant/mrs_natural/label", (mrs_natural)inputs.labelNum(inputs.labelEntry(i)));  // change to just 0
    wksnet->updControl("WekaSink/wks/mrs_string/currentlyPlaying", inputs.entry(i));
    wksnet->process(out, tmpvec);

  }

  delete all;
  delete wksnet;

}

int main(int argc, const char **argv)
{
  string fileName;
  string outName;

  if(argc < 2)
  {
    cout << "Please enter filename." << endl;
    exit(1);
  }
  else
  {
    fileName = argv[1];
    outName = argv[2];
  }
  cout << "Processing file " << fileName << endl;

  recognize(fileName,outName);
  exit(0);
}

