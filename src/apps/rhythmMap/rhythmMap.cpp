#include <marsyas/system/MarSystemManager.h>
#include <marsyas/marsystems/SimilarityMatrix.h>
#include <marsyas/marsystems/Metric2.h>
#include <marsyas/marsystems/DTW.h>
#include <marsyas/marsystems/AveragingPattern.h>
#include <marsyas/Collection.h>

using namespace std;
using namespace Marsyas;

#define MAX_TEMPLATES 10
#define ACC_INPUT 8000
#define ACC_TEMPLATE 1000
#define BIN 8
#define NITERATION 15

void recognize(string sfName, string tpName)
{
  mrs_natural  i, j, k, l;
  mrs_natural nsamples, sfrq, obs, outsize;
  mrs_natural wsize = 0;

  mrs_natural maxsize, totalCount, inputsize;
  mrs_real msecondsPerFrame;
  MarSystemManager mng;
  MarSystem* netInp = mng.create("Series", "netInp");
  MarSystem* accInp = mng.create("Accumulator", "accInp");
  MarSystem* wavInp = mng.create("Series", "wavInp");
  MarSystem* wavaccInp = mng.create("Accumulator","wavaccInp");
  MarSystem* wavnetInp = mng.create("Series","wavnetInp");
  MarSystem* wavplOut = mng.create("Parallel","wavplOut");
  MarSystem* wavsrdOut = mng.create("Shredder","wavsrdOut");
  MarSystem* plTpl = mng.create("Parallel","plTpl");
  MarSystem* Inp = mng.create("Series","Inp");
  MarSystem* Tpl = mng.create("Series","Tpl");
  MarSystem* sim = mng.create("SimilarityMatrix", "sim");
  MarSystem* met = mng.create("Metric2", "met");
  MarSystem* dtw = mng.create("DTW", "dtw");
  MarSystem* ap = mng.create("AveragingPattern", "ap");
  Collection templates;
  ostringstream oss, oss2;
  string tmpStr;
  realvec sizes, b, countvector, segments;
  realvec beginPos, endPos, order;
  realvec dataInp, dataTpl, inpspec, allspec, tplspec;
  realvec featuresInp, featuresTpl;
  realvec simInput, simOutput, algOutput, tplOutput;
  mrs_bool b_begin;
  MarSystem* netTpl[MAX_TEMPLATES];
  MarSystem* accTpl[MAX_TEMPLATES];
  MarSystem* srsTpl[MAX_TEMPLATES];
  MarSystem* wavnetOut[MAX_TEMPLATES];

  templates.read(tpName);
  sizes.create(templates.size()+1);
  outsize = 0;

  /*** caluculate templates spectrogram ***/

  if(templates.size() > MAX_TEMPLATES) {
    cerr << "Error: invalied templates size!" << endl;
    exit(-1);
  }
  for(i=0; i< (mrs_natural)templates.size(); ++i) {
    oss.str(""); oss << "net" << i;
    tmpStr = oss.str();
    netTpl[i] = mng.create("Series", tmpStr);
    oss.str(""); oss << "srs" << i;
    srsTpl[i] = mng.create("Series", oss.str());
    oss.str(""); oss << "tplsrc" << i;
    netTpl[i]->addMarSystem(mng.create("SoundFileSource",oss.str()));
    oss2.str(""); oss2 << "SoundFileSource/" << oss.str() << "/mrs_string/filename";
    netTpl[i]->updControl(oss2.str(),templates.entry(i));
    oss2.str(""); oss2 << "SoundFileSource/" << oss.str() << "/mrs_natural/size";
    nsamples = netTpl[i]->getctrl(oss2.str())->to<mrs_natural>();
    oss.str(""); oss << "han" << i;
    netTpl[i]->addMarSystem(mng.create("Windowing",oss.str()));
    oss2.str(""); oss2 << "Windowing/" << oss.str() << "/mrs_natural/onSamples";
    wsize = netTpl[i]->getctrl(oss2.str())->to<mrs_natural>();
    sizes(i+1) = nsamples/wsize;
    oss.str(""); oss << "spc" << i;
    netTpl[i]->addMarSystem(mng.create("Spectrum",oss.str()));
    oss.str(""); oss << "psc" << i;
    netTpl[i]->addMarSystem(mng.create("PowerSpectrum",oss.str()));
    //oss.str(""); oss << "mfcc" << i;
    //netTpl[i]->addMarSystem(mng.create("MFCC",oss.str()));
    oss.str(""); oss << "acc" << i;
    accTpl[i] = mng.create("Accumulator",oss.str());
    accTpl[i]->addMarSystem(netTpl[i]);
    accTpl[i]->updControl("mrs_natural/nTimes",ACC_TEMPLATE);
    srsTpl[i]->addMarSystem(accTpl[i]);
    plTpl->addMarSystem(srsTpl[i]);
    outsize += sizes(i+1);
  }
  Tpl->addMarSystem(plTpl);
  Tpl->tick();
  dataTpl = Tpl->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
  maxsize = sizes.maxval();





  /*** calculate the feature vector of template ***/

  featuresTpl.create(BIN*(templates.size()+1),dataTpl.getCols());
  sfrq = netTpl[0]->getctrl("SoundFileSource/tplsrc0/mrs_real/osrate")->to<mrs_real>();
  obs = netTpl[0]->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
  b.create(BIN+2);
  for(i=0; i<BIN+2; ++i) {
    b(i) = wsize*700/sfrq*(pow(10, (log10(1.0+sfrq/1400))*i/(BIN+1))-1);
  }
  for(j=0; j<BIN; j++) {
    for(l=0; l<(mrs_natural)templates.size(); l++) {
      for(k=0; k<obs; k++) {
        if(b(j) < k && k < b(j+1)) {
          for(i=0; i<dataTpl.getCols(); ++i) {
            featuresTpl(j+l*BIN,i) += dataTpl(k+l*obs,i)*(k-b(j))/(b(j+1)-b(j));
          }
        } else if(b(j+1) <= k && k <= b(j+2)) {
          for(i=0; i<dataTpl.getCols(); ++i) {
            featuresTpl(j+l*BIN,i) += dataTpl(k+l*obs,i)*(b(j+2)-k)/(b(j+2)-b(j+1));
          }
        }
      }
    }
    for(l=0; l<(mrs_natural)templates.size(); l++) {
      for(i=0; i<featuresTpl.getCols(); ++i) {
        featuresTpl(j+l*BIN,i) /= (b(j+2)-b(j))/2;
        featuresTpl(j+l*BIN,i) = log(100000*featuresTpl(j+l*BIN,i)+1);
      }
    }
  }



  /*** set controls to input series ***/

  netInp->addMarSystem(mng.create("SoundFileSource","inpsrc"));
  netInp->addMarSystem(mng.create("Spectrum","spc"));
  netInp->addMarSystem(mng.create("PowerSpectrum","psc"));
  //netInp->addMarSystem(mng.create("MFCC","mfcc"));
  accInp->addMarSystem(netInp);
  accInp->updControl("mrs_natural/nTimes",ACC_INPUT);
  Inp->addMarSystem(accInp);

  /*** set controls to rhythm map ***/

  sim->updControl("mrs_natural/calcCovMatrix",2);
  sim->updControl("mrs_string/normalize", "MeanStd");
  sim->addMarSystem(met);
  met->updControl("mrs_string/metric","euclideanDistance");
  dtw->updControl("mrs_string/lastPos","lowest");
  dtw->updControl("mrs_string/startPos","lowest");
  dtw->updControl("mrs_string/localPath","diagonal");
  dtw->updControl("mrs_bool/weight",false);
  dtw->updControl("mrs_string/mode","OnePass");

  /*** calculate first templates ***/

  tplOutput.create(featuresTpl.getRows(),maxsize);
  for(i=0; i<maxsize; ++i) {
    for(j=0; j<featuresTpl.getRows(); j++) {
      tplOutput(j,i) = featuresTpl(j,i);
    }
  }



  /*** calculate input spectrogram ***/

  netInp->updControl("SoundFileSource/inpsrc/mrs_string/filename",sfName);
  inputsize = netInp->getctrl("SoundFileSource/inpsrc/mrs_natural/size")->to<mrs_natural>();
  inputsize /= wsize;
  accInp->updControl("mrs_natural/nTimes",inputsize);
  Inp->tick();
  dataInp = Inp->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();



  /*** calculate input feature vector of input ***/

  featuresInp.create(BIN,dataInp.getCols());
  sfrq = netInp->getctrl("SoundFileSource/inpsrc/mrs_real/osrate")->to<mrs_real>();
  obs = netInp->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
  b.create(BIN+2);
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


  /*** calculate input of SimilarityMatrix ***/

  simInput.create(featuresInp.getRows()+featuresTpl.getRows(),dataInp.getCols());
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
  simOutput.create(outsize,sizes(0));
  dtw->updControl("mrs_realvec/sizes",sizes);
  dtw->updControl("mrs_natural/inSamples",simOutput.getCols());
  dtw->updControl("mrs_natural/inObservations",simOutput.getRows());
//<<<<<<< .mine
//=======
//
//>>>>>>> .r3676
  algOutput.create(3*sizes(0),2);
  ap->updControl("mrs_realvec/sizes",sizes);
  ap->updControl("mrs_natural/inSamples",simInput.getCols());
  ap->updControl("mrs_natural/inObservations",simInput.getRows());



  /*** iterative learning ***/

  for(l=0; l<NITERATION; l++) {
    sim->process(simInput,simOutput);
    dtw->process(simOutput,algOutput);
    ap->updControl("mrs_realvec/alignment",algOutput);
    ap->process(simInput,simInput);
    MarControlAccessor acs(dtw->getctrl("mrs_real/totalDistance"));
    cout << "ITR#" << l << " " << acs.to<mrs_real>() << endl;
  }

  /*** calculate the segment information ***/

  beginPos.create(sizes.getSize()-1);
  endPos.create(sizes.getSize()-1);
  beginPos(0) = 0;
  for(i=1; i<sizes.getSize()-1; ++i) {
    beginPos(i) = sizes(i) + beginPos(i-1);
  }
  for(i=0; i<sizes.getSize()-1; ++i) {
    endPos(i) = beginPos(i) + sizes(i+1);
  }
  order.create(sizes.getSize()-1);
  l=0; i=0;
  while(l==0) {
    if(algOutput(i,0) >= 0 && algOutput(i,1) >= 0) {
      l = i;
    }
    ++i ;
  }
  k = 1; totalCount = 1;
  for(i=0; i<beginPos.getSize(); ++i) {
    if(beginPos(i) <= algOutput(l,1) && algOutput(l,1) < endPos(i)) {
      order(i) = k;
      k ++;
      break;
    }
  }
  b_begin = true;
  for(i=l; i<algOutput.getRows(); ++i) {
    for(j=0; j<beginPos.getSize(); j++) {
      if(algOutput(i,1) == beginPos(j)) {
        if(!b_begin) {
          if(order(j) == 0) {
            order(j) = k;
            k++;
          }
          b_begin = true;
          totalCount++;
        }
        break;
      } else if(algOutput(i,1) == endPos(j)-1) {
        b_begin = false;
        break;
      }
    }
  }
  segments.create(totalCount,3);
  msecondsPerFrame = (mrs_real)wsize/(mrs_real)sfrq*1000.0;
  segments(0,0) = 0.0; segments(0,2) = 1;
  b_begin = true;
  k = 0;
  for(i=l; i<algOutput.getRows(); ++i) {
    for(j=0; j<beginPos.getSize(); j++) {
      if(algOutput(i,1) == beginPos(j)) {
        if(!b_begin) {
          b_begin = true;
          segments(k,1) = (mrs_real)algOutput(i,0)*msecondsPerFrame;
          k++;
          segments(k,0) = (mrs_real)algOutput(i,0)*msecondsPerFrame;
          segments(k,2) = order(j);
        }
        break;
      } else if(algOutput(i,1) == endPos(j)-1) {
        b_begin = false;
        break;
      }
    }
  }
  segments(k,1) = (mrs_real)algOutput(algOutput.getRows()-1,0)*msecondsPerFrame;
  oss.str(""); oss << sfName << "_segments.txt";
  segments.write(oss.str());
  cout << "Now writing a segmentation file: " << oss.str() << "." << endl;

  /*** create WAV file of template patterns ***/

  wavInp->addMarSystem(wavaccInp);
  wavaccInp->addMarSystem(wavnetInp);
  wavnetInp->addMarSystem(mng.create("SoundFileSource","wavsrc"));
  wavnetInp->updControl("SoundFileSource/wavsrc/mrs_string/filename",sfName);
  wavnetInp->addMarSystem(mng.create("Spectrum","wavspc"));
  wavaccInp->updControl("mrs_natural/nTimes",inputsize);
  wavInp->tick();
  inpspec = wavInp->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
  allspec.create(inpspec.getRows()*sizes.getSize(),inpspec.getCols());
  for(i=0; i<inpspec.getCols(); ++i) {
    for(j=0; j<inpspec.getRows(); j++) {
      allspec(j,i) = inpspec(j,i);
    }
  }
  sizes(0) = inpspec.getCols();
  ap->updControl("mrs_natural/inSamples",allspec.getCols());
  ap->updControl("mrs_natural/inObservations",allspec.getRows());
  ap->updControl("mrs_realvec/sizes",sizes);
  ap->updControl("mrs_realvec/alignmnet",algOutput);
  ap->process(allspec,allspec);
  tplspec.create(inpspec.getRows()*order.maxval(),maxsize);
  for(k=1; k<=order.maxval(); k++) {
    for(l=0; l<order.getSize(); l++) {
      if(order(l) == k) {
        for(i=0; i<maxsize; ++i) {
          for(j=0; j<inpspec.getRows(); j++) {
            tplspec(j+(k-1)*inpspec.getRows(),i) = allspec(j+(l+1)*inpspec.getRows(),i);
          }
        }
        break;
      }
    }
  }
  for(i=0; i<order.maxval(); ++i) {
    oss.str(""); oss << "wavnetOut" << i;
    wavnetOut[i] = mng.create("Series",oss.str());
    wavplOut->addMarSystem(wavnetOut[i]);
    oss.str(""); oss << "ispOut" << i;
    wavnetOut[i]->addMarSystem(mng.create("InvSpectrum",oss.str()));
    oss.str(""); oss << "sfsOut" << i;
    wavnetOut[i]->addMarSystem(mng.create("SoundFileSink",oss.str()));
    oss.str(""); oss << "SoundFileSink/sfsOut" << i << "/mrs_natural/inObservations";
    wavnetOut[i]->updControl(oss.str(),1);
    oss.str(""); oss << "SoundFileSink/sfsOut" << i << "/mrs_natural/inSamples";
    wavnetOut[i]->updControl(oss.str(),inpspec.getRows());
    oss.str(""); oss << "SoundFileSink/sfsOut" << i << "/mrs_real/israte";
    wavnetOut[i]->updControl(oss.str(),sfrq);
    oss2.str(""); oss2 << "SoundFileSink/sfsOut" << i << "/mrs_string/filename";
    oss.str(""); oss << sfName << "_template" << i+1 << ".wav";
    wavnetOut[i]->updControl(oss2.str(),oss.str());
    wavnetOut[i]->updControl("mrs_natural/inObservations",inpspec.getRows());
    wavnetOut[i]->updControl("mrs_natural/inSamples",1);
    cout << "Now writing WAV file: " << oss.str() << "." << endl;
  }
  wavsrdOut->addMarSystem(wavplOut);
  wavsrdOut->updControl("mrs_natural/nTimes",maxsize);
  wavsrdOut->updControl("mrs_natural/inSamples",tplspec.getCols());
  wavsrdOut->updControl("mrs_natural/inObservations",tplspec.getRows());
  wavplOut->updControl("mrs_natural/inSamples",1);
  wavplOut->updControl("mrs_natural/inObservations",tplspec.getRows());
  realvec tmpRealvec(order.maxval(),wsize);
  wavsrdOut->process(tplspec,tmpRealvec);

  /*** delete memory ***/

  delete sim;
  delete dtw;
  delete ap;
  delete Inp;
  delete Tpl;
  delete wavInp;
  delete wavsrdOut;

}

int main(int argc, const char **argv)
{
  string fileName;
  string templates;
  if (argc<2)
  {
    cout<<"Please enter filename."<<endl;
    exit(1);
  }
  else
  {
    fileName = argv[1];
    templates = argv[2];
  }
  cout << "Processing file " << fileName << endl;

  recognize(fileName, templates);
  exit(0);
}
