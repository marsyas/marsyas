#include <marsyas/system/MarSystemManager.h>
#include <marsyas/marsystems/SimilarityMatrix.h>
#include <marsyas/marsystems/Metric2.h>
#include <marsyas/marsystems/DTWWD.h>
#include <marsyas/marsystems/DTW.h>
#include <marsyas/marsystems/AveragingPattern.h>
#include <marsyas/marsystems/Delta.h>
#include <marsyas/marsystems/Chroma.h>
#include <marsyas/Collection.h>
#include <vector>

using namespace std;
using namespace Marsyas;

#define MAX_TEMPLATES 10
#define ACC_INPUT 8000
#define ACC_TEMPLATE 1000
#define BIN 8
#define NITERATION 15
#define NTEMPLATES 30

void recognize(string sfName, string hName, string tpName, string cnName, string szName, string outName)
{
  mrs_natural i, j, k, l, m;
  mrs_natural wsize, obs, totalCount;
  mrs_natural inputsize, maxind, outsize/*, prevNum*/;
  mrs_real msecondsPerFrame, sfrq;
  //mrs_natural startX, startY, endX, endY;
  MarSystemManager mng;
  MarSystem* netInp = mng.create("Series","netInp");
  MarSystem* netInp2 = mng.create("Series","netInp2");
  MarSystem* accInp = mng.create("Accumulator","accInp");
  MarSystem* accInp2 = mng.create("Accumulator", "accInp2");
  MarSystem* Inp = mng.create("Series","Inp");
  MarSystem* Inp2 = mng.create("Series", "Inp2");
  MarSystem* sim = mng.create("SimilarityMatrix", "sim");
  MarSystem* sim2 = mng.create("SimilarityMatrix", "sim2");
  MarSystem* met = mng.create("Metrix2", "met");
  MarSystem* dtw = mng.create("DTWWD", "dtw");
  MarSystem* met2 = mng.create("Metrix2", "met2");
  MarSystem* dtw2 = mng.create("DTW", "dtw");
  MarSystem* ap = mng.create("AveragingPattern","ap");
  //MarSystem* total = mng.create("Series","total");
  MarSystem* total2 = mng.create("Series", "total2");
  //MarSystem* total3 = mng.create("Series", "total3");
  //MarSystem* total4 = mng.create("Series", "total4");
  //MarSystem* ann = mng.create("Annotator","ann");
  MarSystem* ann2 = mng.create("Annotator", "ann2");
  //MarSystem* ann3 = mng.create("Annotator", "ann3");
  //MarSystem* ann4 = mng.create("Annotator", "ann4");
  //MarSystem* wks = mng.create("WekaSink","wks");
  MarSystem* wks2 = mng.create("WekaSink", "wks2");
  //MarSystem* wks3 = mng.create("WekaSink", "wks3");
  //MarSystem* wks4 = mng.create("WekaSink", "wks4");
  MarSystem* crm = mng.create("Chroma", "crm");
  MarSystem* dlt = mng.create("Delta", "dlt");
  Collection templates, countsclc, inputs, hfiles, sizesclc;
  ostringstream oss;
  realvec sizes, b, tmpcounts, counts, genres, tmpsizes, formin, delta, map;
  realvec dataInp, dataTpl, beginPos, endPos, tmpFeatures, tmpbegin, tmpend;
  realvec featuresInp, featuresTpl, /*outFeatures,*/ outFeatures2, /*outFeatures3, outFeatures4, tmpvec,*/ tmpvec2/*, tmpvec3, tmpvec4*/;
  realvec simInput, simOutput, algOutput, segments, tmpsimin, tmpsimout, tmpalgout;
  mrs_bool b_begin;
  vector<string> strVec;

  inputs.read(sfName);
  templates.read(tpName);
  hfiles.read(hName);
  countsclc.read(cnName);
  sizesclc.read(szName);
  outsize = 0;
  strVec.resize(templates.size());


  /*** load template patterns ***/

  oss.str("");
  l=0;
  for(i=0; i< (mrs_natural)countsclc.size(); ++i) {
    tmpcounts.read(countsclc.entry(i));
    for(j=1; j<(mrs_natural)tmpcounts.getSize(); j++) {
      if(tmpcounts(j) > 0) {
        l++;
      }
    }
  }
  sizes.create(l+1);
  genres.create(l);
  //outFeatures.create(l+1,1);
  outFeatures2.create(templates.getNumLabels(),1);
  //outFeatures3.create(l*l,1);
  //outFeatures4.create(l,1);
  //tmpvec.create(outFeatures.getRows()+1,1);
  tmpvec2.create(outFeatures2.getRows()+1,1);
  //tmpvec3.create(outFeatures3.getRows()+1,1);
  //tmpvec4.create(outFeatures4.getRows()+1,1);
  tmpFeatures.create(1);
  l=0;
  for(i=0; i<(mrs_natural)countsclc.size(); ++i) {
    tmpcounts.read(countsclc.entry(i));
    tmpsizes.read(sizesclc.entry(i));
    for(j=1; j<tmpcounts.getSize(); j++) {
      if(tmpcounts(j) > 0) {
        l++;
        sizes(l) = tmpsizes(j);
      }
    }
  }
  featuresTpl.create(sizes.getSize()*BIN,sizes.maxval());
  for(i=0; i<(mrs_natural)templates.size(); ++i) {
    if(templates.labelName(i) != countsclc.labelName(i)) {
      cerr << "Error: templates and counts don't match!" << endl;
      exit(-1);
    }
  }
  l=0;
  for(i=0; i<(mrs_natural)templates.size(); ++i) {
    strVec[i] = templates.labelName(i);
    tmpcounts.read(countsclc.entry(i));
    dataTpl.read(templates.entry(i));
    tmpsizes.read(sizesclc.entry(i));
    for(j=1; j<tmpcounts.getSize(); j++) {
      if(tmpcounts(j) > 0) {
        genres(l) = i;
        //sizes(l+1) = tmpsizes(j);
        outsize += tmpsizes(j);
        oss << templates.labelName(i) << j << ",";
        for(k=0; k<BIN; k++) {
          for(m=0; m<sizes(l+1); m++) {
            featuresTpl(k+BIN*l,m) = dataTpl(k+BIN*(j-1),m);
          }
        }
        l++;
      }
    }
  }

  /*** set controls to input series ***/

  netInp->addMarSystem(mng.create("SoundFileSource","inpsrc"));
  netInp->addMarSystem(mng.create("Windowing","ham"));
  netInp->addMarSystem(mng.create("Spectrum","spc"));
  netInp->addMarSystem(mng.create("PowerSpectrum","psc"));
  //netInp->addMarSystem(mng.create("MFCC","mfcc"));
  accInp->addMarSystem(netInp);
  accInp->updControl("mrs_natural/nTimes",ACC_INPUT);
  Inp->addMarSystem(accInp);

  netInp2->addMarSystem(mng.create("SoundFileSource","inpsrc2"));
  netInp2->addMarSystem(mng.create("Windowing","ham2"));
  netInp2->addMarSystem(mng.create("Spectrum","spc2"));
  netInp2->addMarSystem(mng.create("PowerSpectrum","psc"));
  netInp2->addMarSystem(crm);
  netInp2->addMarSystem(dlt);
  accInp2->addMarSystem(netInp2);
  accInp2->updControl("mrs_natural/nTimes",ACC_INPUT);
  Inp2->addMarSystem(accInp2);

  /*** set conttols to rhythm map ***/

  sim->updControl("mrs_natural/calcCovMatrix",2);
  sim->updControl("mrs_string/normalize", "MeanStd");
  sim->addMarSystem(met);
  sim2->updControl("mrs_natural/calcCovMatrix",2);
  sim2->updControl("mrs_string/normalize", "MeanStd");
  sim2->addMarSystem(met2);
  met->updControl("mrs_string/metric","euclideanDistance");
  met2->updControl("mrs_string/metric", "euclideanDistance");
  dtw->updControl("mrs_string/lastPos","lowest");
  dtw->updControl("mrs_string/startPos","lowest");
  dtw->updControl("mrs_string/localPath","diagonal");
  dtw->updControl("mrs_bool/weight",false);
  dtw->updControl("mrs_string/mode","OnePass");
  dtw2->updControl("mrs_string/lastPos", "end");
  dtw2->updControl("mrs_string/startPos", "zero");
  dtw2->updControl("mrs_string/localPath", "diagonal");
  dtw2->updControl("mrs_bool/weight", false);
  dtw2->updControl("mrs_string/mode", "normal");

  /*** create wekasink series ***/

  //total->addMarSystem(ann);
  //total->addMarSystem(wks);
  total2->addMarSystem(ann2);
  total2->addMarSystem(wks2);
  //total3->addMarSystem(ann3);
  //total3->addMarSystem(wks3);
  //total4->addMarSystem(ann4);
  //total4->addMarSystem(wks4);

  /*** update control of wekasink series ***/
  /*
  total->updControl("mrs_natural/inSamples",1);
  total->updControl("mrs_natural/inObservations",outFeatures.getRows());
  total->updControl("WekaSink/wks/mrs_string/labelNames",inputs.getLabelNames());
  total->updControl("WekaSink/wks/mrs_natural/nLabels",inputs.getNumLabels());
  total->updControl("WekaSink/wks/mrs_natural/downsample",1);
  total->updControl("WekaSink/wks/mrs_string/inObsNames",oss.str());
  total->updControl("WekaSink/wks/mrs_string/filename",outName);
  */
  total2->updControl("mrs_natural/inSamples",1);
  total2->updControl("mrs_natural/inObservations",outFeatures2.getRows());
  total2->updControl("WekaSink/wks2/mrs_string/labelNames",sfName);
  total2->updControl("WekaSink/wks2/mrs_natural/nLabels",1);
  total2->updControl("WekaSink/wks2/mrs_natural/downsample",1);
  total2->updControl("WekaSink/wks2/mrs_string/inObsNames",oss.str());
  //oss.str("");
  //oss << outName << "_add.arff";
  //total2->updControl("WekaSink/wks2/mrs_string/filename",oss.str());
  total2->updControl("WekaSink/wks2/mrs_string/filename",outName);
  /*
  total3->updControl("mrs_natural/inSamples",1);
  total3->updControl("mrs_natural/inObservations",outFeatures3.getRows());
  total3->updControl("WekaSink/wks3/mrs_string/labelNames",inputs.getLabelNames());
  total3->updControl("WekaSink/wks3/mrs_natural/nLabels",inputs.getNumLabels());
  total3->updControl("WekaSink/wks3/mrs_natural/downsample",1);
  total3->updControl("WekaSink/wks3/mrs_string/inObsNames",oss.str());
  oss.str("");
  oss << outName << "_LSA.arff";
  total3->updControl("WekaSink/wks3/mrs_string/filename",oss.str());

  total4->updControl("mrs_natural/inSamples",1);
  total4->updControl("mrs_natural/inObservations",outFeatures4.getRows());
  total4->updControl("WekaSink/wks4/mrs_string/labelNames",inputs.getLabelNames());
  total4->updControl("WekaSink/wks4/mrs_natural/nLabels",inputs.getNumLabels());
  total4->updControl("WekaSink/wks4/mrs_natural/downsample",1);
  total4->updControl("WekaSink/wks4/mrs_string/inObsNames",oss.str());
  oss.str("");
  oss << outName << "_distance.arff";
  total4->updControl("WekaSink/wks4/mrs_string/filename",oss.str());
  */


  /*** process ***/

  featuresInp.create(BIN,ACC_INPUT);
  b.create(BIN+2);
  simInput.create(BIN+featuresTpl.getRows(),ACC_INPUT);
  simOutput.create(outsize,ACC_INPUT);
  tmpsimin.create(BIN*2,ACC_INPUT);
  tmpsimout.create(ACC_INPUT,ACC_INPUT);
  algOutput.create(3*ACC_INPUT,2);
  tmpalgout.create(3*ACC_INPUT,2);
  delta.create(1,ACC_INPUT);
  tmpsizes.create(2);
  segments.create(1,1);
  tmpbegin.create(1,1);
  tmpend.create(1,1);
  for(l=0; l<(mrs_natural)inputs.size(); l++) {
    cout << "Now processing: " << inputs.entry(l) << endl;

    /*** calculate input spectrogram ***/

    netInp->updControl("SoundFileSource/inpsrc/mrs_string/filename",inputs.entry(l));
    netInp2->updControl("SoundFileSource/inpsrc2/mrs_string/filename",hfiles.entry(l));
    inputsize = netInp->getctrl("SoundFileSource/inpsrc/mrs_natural/size")->to<mrs_natural>();
    wsize = netInp->getctrl("Windowing/ham/mrs_natural/size")->to<mrs_natural>();
    inputsize /= wsize;
    accInp->updControl("mrs_natural/nTimes",inputsize);
    accInp2->updControl("mrs_natural/nTimes",inputsize);
    Inp->tick();
    dataInp = Inp->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    /*** calculate input feature vector of input ***/

    featuresInp.stretch(BIN,dataInp.getCols());
    for(i=0; i<featuresInp.getRows(); ++i) {
      for(j=0; j<featuresInp.getCols(); j++) {
        featuresInp(i,j) = 0;
      }
    }
    sfrq = netInp->getctrl("SoundFileSource/inpsrc/mrs_real/osrate")->to<mrs_real>();
    obs = netInp->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
    for(i=0; i<BIN+2; ++i) {
      b(i) = wsize*700/sfrq*(pow(10, (log10(1.0+sfrq/1400))*i/(BIN+1))-1);
    }
    for(j=0; j<BIN; j++) {
      for(k=0; k<obs; k++) {
        if(b(j) < k && k < b(j+1)) {
          for(i=0; i<dataInp.getCols(); ++i) {
            featuresInp(j,i) += dataInp(k,i)*(k-b(j))/(b(j+1)-b(j));
          }
        } else if(b(j+1) <= k && k <= b(j+2)) {
          for(i=0; i<dataInp.getCols(); ++i) {
            featuresInp(j,i) += dataInp(k,i)*(b(j+2)-k)/(b(j+2)-b(j+1));
          }
        }
      }
      for(i=0; i<featuresInp.getCols(); ++i) {
        featuresInp(j,i) /= (b(j+2)-b(j))/2;
        featuresInp(j,i) = log(100000*featuresInp(j,i)+1);
      }
    }
    dataInp.stretch(0,0);

    // calculate input chroma delta
    crm->setctrl("mrs_real/samplingFreq", sfrq);
    crm->setctrl("mrs_natural/lowOctNum", 0);
    crm->setctrl("mrs_natural/highOctNum", 8);
    crm->setctrl("mrs_natural/inObservations", wsize/2);
    crm->update();

    // NOTE: The following are deprecated controls of Delta.
    // In order to use this code, revert to git ID:
    // [649bb95e41339d1dd984ddfea7a43bf178cd3627]
    dlt->setctrl("mrs_bool/sum", true);
    dlt->setctrl("mrs_bool/absolute", true);
    dlt->setctrl("mrs_bool/normalize", true);
    dlt->setctrl("mrs_natural/normSize", 40);
    dlt->update();

    Inp2->tick();
    delta = Inp2->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    /*** calculate input of SimilarityMatrix ***/
    simInput.stretch(featuresInp.getRows()+featuresTpl.getRows(),featuresInp.getCols());
    for(i=0; i<featuresInp.getCols(); ++i) {
      for(j=0; j<featuresInp.getRows(); j++) {
        simInput(j,i) = featuresInp(j,i);
      }
    }
    for(i=0; i<featuresTpl.getCols(); ++i) {
      for(j=0; j<featuresTpl.getRows(); j++) {
        simInput(j+featuresInp.getRows(),i) = featuresTpl(j,i);
      }
    }

    /*** update control of rhythm map ***/
    sizes(0) = featuresInp.getCols();
    sim->updControl("mrs_realvec/sizes",sizes);
    sim->updControl("mrs_natural/inSamples",simInput.getCols());
    sim->updControl("mrs_natural/inObservations",simInput.getRows());
    simOutput.stretch(outsize,sizes(0));
    dtw->updControl("mrs_realvec/sizes",sizes);
    dtw->setctrl("mrs_realvec/delta", delta);
    dtw->setctrl("mrs_real/deltaWeight", 10.0);
    dtw->updControl("mrs_natural/inSamples",simOutput.getCols());
    dtw->updControl("mrs_natural/inObservations",simOutput.getRows());
    algOutput.stretch(3*sizes(0),2);
    ap->updControl("mrs_realvec/sizes",sizes);
    ap->updControl("mrs_natural/inSamples",simInput.getCols());
    ap->updControl("mrs_natural/inObservations",simInput.getRows());

    /*** calculate the alignment ***/
    sim->process(simInput,simOutput);
    dtw->process(simOutput,algOutput);
    ap->updControl("mrs_realvec/alignment",algOutput);
    ap->process(simInput,simInput);

    /*** out the counts features vector ***/
    tmpFeatures.stretch(ap->getctrl("mrs_realvec/counts")->to<mrs_realvec>()(0)-1);
    counts = ap->getctrl("mrs_realvec/counts")->to<mrs_realvec>();
    //oss.str(""); oss << inputs.entry(l) << "_counts.dat";
    //counts.write(oss.str());
    for(i=1; i<counts.getSize(); ++i) {
      counts(i) = counts(i)/counts(0);
    }
    counts(0) = 0;
    counts.maxval(&maxind);
    //cout << strVec[genres(maxind)] << endl;
    /*
    for(i=0; i<counts.getSize(); ++i){
      outFeatures(i,0) = counts(i);
    }
    */
    for(i=0; i<outFeatures2.getRows(); ++i) {
      outFeatures2(i,0) = 0;
    }

    for(i=0; i<counts.getSize(); ++i) {
      outFeatures2(genres(i),0) += counts(i);
    }
    /*for(i=0; i<outFeatures3.getRows(); ++i){
      for(j=0; j<outFeatures3.getCols(); j++){
    outFeatures3(i,j) = 0.0;
      }
    }

    b_begin = false;
    startX = algOutput(m,0);
    startY = algOutput(m,1);
    k = 0;
    prevNum = 1000;
    for(i=m; i<algOutput.getRows(); ++i){
      for(j=0; j<beginPos.getSize(); j++){
    if(algOutput(i,1) == beginPos(j)){
    if(!b_begin){
      b_begin = true;
      m = i;
      startX = algOutput(i,0);
      startY = algOutput(i,1);
    }
    break;
    } else if(algOutput(i,1) == endPos(j)-1){
    if(b_begin){
      endX = algOutput(i,0);
      endY = algOutput(i,1);
      if(prevNum != 1000){
        //tmpFeatures(k-1) = templatesDistance(map(j),prevNum);
        outFeatures3(prevNum*(sizes.getSize()-1)+map(j),0)++;
      }
      prevNum = map(j);
      k++;
      b_begin = false;
      break;
    }
    }
      }
    }
    for(i=0; i<outFeatures3.getRows(); ++i){
      outFeatures3(i,0) /= tmpFeatures.getSize();
    }
    */

    // update control
    //total->updControl("Annotator/ann/mrs_natural/label",inputs.labelNum(inputs.labelEntry(l)));
    total2->updControl("Annotator/ann2/mrs_natural/label", 0);
    total2->process(outFeatures2,tmpvec2);

    // out segment data

    tmpbegin.stretch(sizes.getSize()-1);
    tmpend.stretch(sizes.getSize()-1);
    tmpbegin(0) = 0;
    for(i=1; i<sizes.getSize()-1; ++i) {
      tmpbegin(i) = sizes(i) + tmpbegin(i-1);
    }
    for(i=0; i<sizes.getSize()-1; ++i) {
      tmpend(i) = tmpbegin(i) + sizes(i+1);
    }
    m=0; i=0;
    while(m==0) {
      if(algOutput(i,0) >= 0 && algOutput(i,1) >= 0) {
        m = i;
      }
      ++i ;
    }

    totalCount = 1;
    for(i=0; i<tmpbegin.getSize(); ++i) {
      if(tmpbegin(i) <= algOutput(m,1) && algOutput(m,1) < tmpend(i)) {
        k = i;
        break;
      }
    }
    b_begin = true;
    for(i=m; i<algOutput.getRows(); ++i) {
      for(j=0; j<tmpbegin.getSize(); j++) {
        if(algOutput(i,1) == tmpbegin(j)) {
          if(!b_begin) {
            b_begin = true;
            totalCount++;
          }
          break;
        } else if(algOutput(i,1) == tmpend(j)-1) {
          b_begin = false;
          break;
        }
      }
    }

    segments.stretch(totalCount,3);
    msecondsPerFrame = (mrs_real)wsize/(mrs_real)sfrq*1000.0;
    segments(0,0) = 0.0; segments(0,2) = k+1;
    b_begin = true;
    k = 0;
    for(i=m; i<algOutput.getRows(); ++i) {
      for(j=0; j<tmpbegin.getSize(); j++) {
        if(algOutput(i,1) == tmpbegin(j)) {
          if(!b_begin) {
            b_begin = true;
            segments(k,1) = (mrs_real)algOutput(i,0)*msecondsPerFrame;
            //segments(k,1) = algOutput(i,0);
            k++;
            segments(k,0) = (mrs_real)algOutput(i,0)*msecondsPerFrame;
            //segments(k,0) = algOutput(i,0);
            segments(k,2) = genres(j)+1;
          }
          break;
        } else if(algOutput(i,1) == tmpend(j)-1) {
          b_begin = false;
          break;
        }
      }
    }
    segments(k,1) = (mrs_real)algOutput(algOutput.getRows()-1,0)*msecondsPerFrame;
    //segments(k,1) = algOutput(algOutput.getRows()-1,0);
    //oss.str(""); oss << sfName << "_genres.txt";
    oss.str(""); oss << inputs.entry(l) << "_segments.txt";
    segments.write(oss.str());
    /*
    for(j=0; j<outFeatures4.getRows(); j++){
      outFeatures4(j,0) = 0.0;
    }
    for(i=0; i<segments.getRows(); ++i){
      for(k=0; k<genres.getSize(); k++){
    tmpsizes(0) = segments(i,1) - segments(i,0);
    tmpsizes(1) = sizes(k+1);
    tmpsimin.stretch(BIN*2,tmpsizes.maxval());
    sim2->updControl("mrs_realvec/sizes",tmpsizes);
    sim2->update();
    sim2->updControl("mrs_natural/inSamples",tmpsimin.getCols());
    sim2->updControl("mrs_natural/inObservations",tmpsimin.getRows());
    tmpsimout.stretch(tmpsizes(1),tmpsizes(0));
    dtw2->updControl("mrs_realvec/sizes",tmpsizes);
    dtw2->update();
    dtw2->updControl("mrs_natural/inSamples",tmpsimout.getCols());
    dtw2->updControl("mrs_natural/inObservations",tmpsimout.getRows());
    tmpalgout.stretch(tmpsizes(0)+tmpsizes(1),2);
    for(j=0; j<tmpsizes(0); j++){
    for(m=0; m<BIN; m++){
      tmpsimin(m,j) = featuresInp(m,j+segments(i,0));
    }
    }
    for(j=tmpsizes(0); j<tmpsizes.maxval(); j++){
    for(m=0; m<BIN; m++){
      tmpsimin(m,j) = 0.0;
    }
    }
    for(j=0; j<tmpsizes(1); j++){
    for(m=0; m<BIN; m++){
      tmpsimin(m+BIN,j) = featuresTpl(m+k*BIN,j);
    }
    }
    for(j=tmpsizes(1); j<tmpsizes.maxval(); j++){
    for(m=0; m<BIN; m++){
      tmpsimin(m+BIN,j) = 0.0;
    }
    }
    sim2->process(tmpsimin,tmpsimout);
    dtw2->process(tmpsimout,tmpalgout);
    if(tmpsizes(0) > 0 && dtw2->getctrl("mrs_real/totalDistance")->to<mrs_real>()> 0){
    outFeatures4(k,0) += dtw2->getctrl("mrs_real/totalDistance")->to<mrs_real>()/tmpsizes(0);
    }
      }
    }
    if(segments.getRows() > 0){
      for(j=0; j<outFeatures4.getRows(); j++){
    outFeatures4(j,0) /= segments.getRows();
      }
    }
    total4->updControl("Annotator/ann4/mrs_natural/label", inputs.labelNum(inputs.labelEntry(l)));
    total4->process(outFeatures4,tmpvec4);*/
  }

  /*** delete memory ***/

  delete sim;
  delete sim2;
  delete dtw;
  delete dtw2;
  delete ap;
  delete Inp;
  delete Inp2;
  //delete total;
  delete total2;
  //delete total3;
  //delete total4;

}

int main(int argc, const char **argv)
{
  string fileName;
  string hName;
  string templates;
  string cnName;
  string szName;
  string outName;
  if (argc<2)
  {
    cout<<"Please enter filename."<<endl;
    exit(1);
  }
  else
  {
    fileName = argv[1];
    hName = argv[2];
    templates = argv[3];
    cnName = argv[4];
    szName = argv[5];
    outName = argv[6];
  }
  cout << "Processing file " << fileName << endl;

  recognize(fileName, hName, templates, cnName, szName, outName);
  exit(0);
}
