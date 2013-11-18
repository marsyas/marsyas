#include <marsyas/system/MarSystemManager.h>
#include <marsyas/marsystems/ConstQFiltering.h>
#include <marsyas/marsystems/MatchBassModel.h>
#include <marsyas/Collection.h>
#include <vector>

using namespace std;
using namespace Marsyas;

#define NINITIALIZE 5
#define NITERATION 10
#define NTEMPLATES 20
#define QVALUE 40
#define CHANNELS 50
#define LOWFREQ 41.2
#define HIGHFREQ 1318.5
#define ANALYZELOWFREQ 82.4
#define ANALYZEHIGHFREQ 330.0
#define NDEVISION 16

void recognize(string sfName, string segName, string tempName, string outName)
{
  MarSystemManager mng;

  mrs_natural j, k, tmpint1, tmpint2/*, maxnum*/;
  //mrs_real tmpreal, max, samplingFreq;
  realvec omegaHat, vHat, sigmaHat, sumW;
  realvec freq, time, W, seg, tmpvec, /*tmpvec2, */tmpvec3, /*tmpvec4,*/ tmpObs;
  realvec templates, u, d, K;
  realvec temporg, genres, /*hist, hist2, */hist3/*, hist4*/, distance;
  mrs_natural width;
  MarSystem *net = mng.create("Series", "net");
  MarSystem *src = mng.create("SoundFileSource", "src");
  MarSystem *pws = mng.create("PowerSpectrum", "pws");
  MarSystem *wlt = mng.create("ConstQFiltering","wlt");
  MarSystem *met = mng.create("Metric", "met");
  MarSystem *mbm = mng.create("MatchBassModel","mbm");
  //MarSystem *total = mng.create("Series", "total");
  //MarSystem *total2 = mng.create("Series", "total2");
  MarSystem *total3 = mng.create("Series", "total3");
  //MarSystem *total4 = mng.create("Series", "total4");
  //MarSystem *ann = mng.create("Annotator", "ann");
  //MarSystem *ann2 = mng.create("Annotator", "ann2");
  MarSystem *ann3 = mng.create("Annotator", "ann3");
  //MarSystem *ann4 = mng.create("Annotator", "ann4");
  //MarSystem *wks = mng.create("WekaSink", "wks");
  //MarSystem *wks2 = mng.create("WekaSink", "wks2");
  MarSystem *wks3 = mng.create("WekaSink", "wks3");
  //MarSystem *wks4 = mng.create("WekaSink", "wks4");
  ostringstream oss;
  Collection inputs, segfiles, tempfiles;
  vector<realvec> dVec, kVec;

  inputs.read(sfName);
  segfiles.read(segName);
  W.create(256,256);
  dVec.resize(inputs.size());
  kVec.resize(inputs.size());
  tempfiles.read(tempName);

  oss.str("");
  tmpint1 = tmpint2 = 0;
  for(mrs_natural i=0; i<tempfiles.size(); ++i) {
    tmpvec.read(tempfiles.entry(i));
    tmpint1 += tmpvec.getCols();
  }
  tmpvec.read(tempfiles.entry(0));
  temporg.create(tmpvec.getRows(), tmpint1);
  genres.create(tmpint1/NDEVISION);
  for(mrs_natural i=0; i<tempfiles.size(); ++i) {
    tmpvec.read(tempfiles.entry(i));
    tmpint1 = tmpint2;
    tmpint2 = tmpint1 + tmpvec.getCols();
    for(j=0; j<tmpvec.getRows(); j++) {
      for(k=0; k<tmpvec.getCols(); k++) {
        temporg(j, tmpint1+k) = tmpvec(j, k);
      }
    }
    for(j=tmpint1/NDEVISION; j<tmpint2/NDEVISION; j++) {
      genres(j) = i;
    }
    oss << tempfiles.labelName(i) << ",";
  }
  //hist.create(tempfiles.size(),1);
  //hist2.create(genres.getSize(),1);
  hist3.create(NTEMPLATES*tempfiles.size(),1);
  //hist4.create(NTEMPLATES*tempfiles.size()*2,1);
  //tmpvec.create(tempfiles.size()+1, 1);
  //tmpvec2.create(hist2.getRows()+1, 1);
  tmpvec3.create(hist3.getRows()+1, 1);
  //tmpvec4.create(hist4.getRows()+1, 1);

  /*** set controls to input series ***/

  net->addMarSystem(src);
  net->addMarSystem(wlt);
  net->addMarSystem(pws);
  pws->updControl("mrs_string/spectrumType", "magnitude");
  wlt->setctrl("mrs_natural/channels", CHANNELS);
  wlt->setctrl("mrs_real/qValue", QVALUE);
  wlt->setctrl("mrs_real/lowFreq", LOWFREQ);
  wlt->setctrl("mrs_real/highFreq", HIGHFREQ);
  wlt->update();
  freq = wlt->getctrl("mrs_realvec/freq")->to<mrs_realvec>();

  /*** create wekasink series ***/

  //total->addMarSystem(ann);
  //total->addMarSystem(wks);
  //total2->addMarSystem(ann2);
  //total2->addMarSystem(wks2);
  total3->addMarSystem(ann3);
  total3->addMarSystem(wks3);
  //total4->addMarSystem(ann4);
  //total4->addMarSystem(wks4);

  /*** update control of wekasink series ***/
  /*
  total->updControl("mrs_natural/inSamples", 1);
  total->updControl("mrs_natural/inObservations", hist.getRows());
  total->updControl("WekaSink/wks/mrs_string/labelNames",inputs.getLabelNames());
  total->updControl("WekaSink/wks/mrs_natural/nLabels",inputs.getNumLabels());
  total->updControl("WekaSink/wks/mrs_natural/downsample",1);
  total->updControl("WekaSink/wks/mrs_string/inObsNames",oss.str());
  total->updControl("WekaSink/wks/mrs_string/filename",outName);

  total2->updControl("mrs_natural/inSamples", 1);
  total2->updControl("mrs_natural/inObservations", hist2.getRows());
  total2->updControl("WekaSink/wks2/mrs_string/labelNames",inputs.getLabelNames());
  total2->updControl("WekaSink/wks2/mrs_natural/nLabels",inputs.getNumLabels());
  total2->updControl("WekaSink/wks2/mrs_natural/downsample",1);
  total2->updControl("WekaSink/wks2/mrs_string/inObsNames",oss.str());
  oss.str("");
  oss << outName << "_all.arff";
  total2->updControl("WekaSink/wks2/mrs_string/filename",oss.str());
  */
  total3->updControl("mrs_natural/inSamples", 1);
  total3->updControl("mrs_natural/inObservations", hist3.getRows());
  total3->updControl("WekaSink/wks3/mrs_string/labelNames",inputs.getLabelNames());
  total3->updControl("WekaSink/wks3/mrs_natural/nLabels", (mrs_natural)inputs.getNumLabels());
  total3->updControl("WekaSink/wks3/mrs_natural/downsample",1);
  total3->updControl("WekaSink/wks3/mrs_string/inObsNames",oss.str());
  //oss.str("");
  //oss << outName << "_distance.arff";
  //total3->updControl("WekaSink/wks3/mrs_string/filename",oss.str());
  total3->updControl("WekaSink/wks3/mrs_string/filename",outName);
  /*
  total4->updControl("mrs_natural/inSamples", 1);
  total4->updControl("mrs_natural/inObservations", hist4.getRows());
  total4->updControl("WekaSink/wks4/mrs_string/labelNames",inputs.getLabelNames());
  total4->updControl("WekaSink/wks4/mrs_natural/nLabels",inputs.getNumLabels());
  total4->updControl("WekaSink/wks4/mrs_natural/downsample",1);
  total4->updControl("WekaSink/wks4/mrs_string/inObsNames",oss.str());
  oss.str("");
  oss << outName << "_distanceStd.arff";
  total4->updControl("WekaSink/wks4/mrs_string/filename",oss.str());
  */
  /*** set controls to bass map (matching) ***/

  mbm->setctrl("mrs_natural/nTemplates", (mrs_natural)(NTEMPLATES*tempfiles.size()));
  mbm->setctrl("mrs_natural/nDevision", NDEVISION);
  mbm->setctrl("mrs_real/lowFreq", ANALYZELOWFREQ);
  mbm->setctrl("mrs_real/highFreq", ANALYZEHIGHFREQ);
  mbm->setctrl("mrs_realvec/freq", freq);
  mbm->setctrl("mrs_string/normalize", "none");
  mbm->addMarSystem(met);
  met->updControl("mrs_string/metric", "euclideanDistance");
  mbm->update();

  /**** update paramater **/

  templates = temporg;
  mbm->setctrl("mrs_realvec/templates", templates);
  mbm->update();
  for(mrs_natural m=0; m<inputs.size(); m++) {
    // load input file
    src->setctrl("mrs_string/filename", inputs.entry(m));
    src->update();
    width = src->getctrl("mrs_natural/size")->to<mrs_natural>();
    j=1;
    while(true) {
      if(j < width) {
        j *= 2;
      } else {
        break;
      }
    }
    width = j;
    net->updControl("mrs_natural/inSamples",width);
    wlt->setctrl("mrs_natural/inSamples",width);
    wlt->setctrl("mrs_natural/width",width/256);
    wlt->update();
    freq = wlt->getctrl("mrs_realvec/freq")->to<mrs_realvec>();
    time = wlt->getctrl("mrs_realvec/time")->to<mrs_realvec>();
    seg.read(segfiles.entry(m));
    net->tick();
    W = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    mbm->setctrl("mrs_realvec/freq", freq);
    mbm->setctrl("mrs_realvec/segmentation", seg);
    mbm->setctrl("mrs_realvec/time", time);
    mbm->setctrl("mrs_realvec/intervals", dVec[m]);
    mbm->setctrl("mrs_realvec/selections", kVec[m]);
    mbm->update();
    mbm->process(W,W);
    dVec[m] = mbm->getctrl("mrs_realvec/intervals")->to<mrs_realvec>();
    kVec[m] = mbm->getctrl("mrs_realvec/selections")->to<mrs_realvec>();
    cout << "Analyzing file:" << inputs.entry(m) << endl;
    distance = mbm->getctrl("mrs_realvec/distance")->to<mrs_realvec>();
    /*for(i=0; i<hist.getRows(); ++i){
      hist(i,0) = 0;
    }
    for(i=0; i<hist2.getRows(); ++i){
      hist2(i,0) = 0;
    }
    tmpint1 = 0;
    for(i=0; i<kVec[m].getSize(); ++i){
      hist(genres(kVec[m](i)),0)++;
      tmpint1++;
      hist2(kVec[m](i),0)++;
    }
    for(i=0; i<hist.getRows(); ++i){
      hist(i,0) /= tmpint1;
    }
    for(i=0; i<hist2.getRows(); ++i){
      hist2(i,0) /= tmpint1;
    }
    max = 0.0;
    maxnum = 0;
    for(i=0; i<hist.getRows(); ++i){
      if(max < hist(i,0)){
        max = hist(i,0);
        maxnum = i;
      }
    }
    */
    for(mrs_natural i=0; i<hist3.getRows(); ++i) {
      distance.getRow(i,tmpObs);
      hist3(i,0) = tmpObs.mean();
      //hist4(i,0) = tmpObs.mean();
      //hist4(i+hist3.getRows(), 0) = tmpObs.std();
    }
    //cout << tempfiles.labelName(maxnum) << endl;


    /*** update control ***/

    //total->updControl("Annotator/ann/mrs_natural/label", inputs.labelNum(inputs.labelEntry(m)));
    //total2->updControl("Annotator/ann2/mrs_natural/label", inputs.labelNum(inputs.labelEntry(m)));
    total3->updControl("Annotator/ann3/mrs_natural/label", (mrs_natural)inputs.labelNum(inputs.labelEntry(m)));
    //total4->updControl("Annotator/ann4/mrs_natural/label", inputs.labelNum(inputs.labelEntry(m)));

    //total->process(hist, tmpvec);
    //total2->process(hist2, tmpvec2);
    total3->process(hist3, tmpvec3);
    //total4->process(hist4, tmpvec4);
  }

  delete net;
  delete mbm;
  //delete total;
  //delete total2;
  delete total3;
  //delete total4;
}

int main(int argc, const char **argv)
{
  string fileName;
  string segfile;
  string outName;
  string tempFile;
  if (argc<2)
  {
    cout<<"Please enter filename."<<endl;
    exit(1);
  }
  else
  {
    fileName = argv[1];
    segfile = argv[2];
    tempFile = argv[3];
    outName = argv[4];
  }
  cout << "Processing file " << fileName << endl;

  recognize(fileName, segfile, tempFile, outName);
  exit(0);
}
