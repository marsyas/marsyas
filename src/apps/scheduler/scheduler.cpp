#include <stdio.h>
#include <marsyas/system/MarSystemManager.h>
#include "EvExpr.h"
#include "Expr.h"

using namespace std;
using namespace Marsyas;

void sched1()
{
  MarSystemManager mng;

  MarSystem* fanin = mng.create("Fanin", "fanin");
  fanin->addMarSystem(mng.create("SineSource", "src1"));
  fanin->addMarSystem(mng.create("SineSource", "src2"));
  fanin->updctrl("SineSource/src1/mrs_real/frequency",3000.0);
  fanin->updctrl("SineSource/src2/mrs_real/frequency",1000.0);

  MarSystem* series = mng.create("Series", "series");
  series->addMarSystem(fanin);

  series->addMarSystem(mng.create("AudioSink", "dest"));
  series->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

#if 0

  // using aliases makes this a little more readable, see the next bit
  EvExpr* e = new EvExpr(series,
                         Ex((std::string)
                            "Fanin/fanin/SineSource/src1/mrs_real/frequency << 120. + 3000. * R.rand(),"
                            +"Fanin/fanin/SineSource/src2/mrs_real/frequency << 120. + 800. * R.rand(),"
                            +"'src1='+Fanin/fanin/SineSource/src1/mrs_real/frequency+"
                            +"' src2='+Fanin/fanin/SineSource/src2/mrs_real/frequency+'\n'>>Stream.op"),
                         Rp("true"));

#else

  EvExpr* e = new EvExpr(series,
                         Ex((std::string)
                            "Fanin/fanin/SineSource/src1/mrs_real/frequency >> @freq1,"
                            +"Fanin/fanin/SineSource/src2/mrs_real/frequency >> @freq2",
                            (std::string)
                            "freq1 << 120. + 3000. * R.rand(),"
                            +"freq2 << 120. + 800. * R.rand(),"
                            +"'src1=' + freq1 + ' src2=' + freq2 + '\n' >> Stream.op"),
                         Rp("true","'0.25s'"));

#endif

//  e->set_repeat(Repeat("0.25s"));

  series->updctrl(TmTime("TmSampleCount/Virtual","0s"), e);

  //while (2) { series->tick(); }
  for (int i=1; i<100; i++) series->tick();
  delete series;
}
void sched2(std::string s)
{
  if (s=="") {
    std::cout << "Save the following lines to a file then rerun using that file name" << std::endl;
    std::cout << "#ExInit:" << std::endl
              << "Fanin/fanin/SineSource/src1/mrs_real/frequency >> @freq1," << std::endl
              << "Fanin/fanin/SineSource/src2/mrs_real/frequency >> @freq2" << std::endl
              << std::endl
              << "#ExExpr:" << std::endl
              << "freq1 << 120. + 3000. * R.rand()," << std::endl
              << "freq2 << 120. + 800. * R.rand()," << std::endl
              << "'src1=' + freq1 + ' src2=' + freq2 + '\\n' >> Stream.op" << std::endl
              << std::endl
              << "#RpExpr:" << std::endl
              << "true" << std::endl
              << std::endl
              << "#RpRate:" << std::endl
              << "'0.25s'" << std::endl;
    return;
  }

  MarSystemManager mng;

  MarSystem* fanin = mng.create("Fanin", "fanin");
  fanin->addMarSystem(mng.create("SineSource", "src1"));
  fanin->addMarSystem(mng.create("SineSource", "src2"));
  fanin->updctrl("SineSource/src1/mrs_real/frequency",3000.0);
  fanin->updctrl("SineSource/src2/mrs_real/frequency",1000.0);

  MarSystem* series = mng.create("Series", "series");
  series->addMarSystem(fanin);

  series->addMarSystem(mng.create("AudioSink", "dest"));
  series->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

  EvExpr* e = new EvExpr(series,ExFile(s));


  /** Save these following lines into a file, then rerun with that file as a parameter
  #ExInit:

  Fanin/fanin/SineSource/src1/mrs_real/frequency >> @freq1,
  Fanin/fanin/SineSource/src2/mrs_real/frequency >> @freq2

  #ExExpr:

  freq1 << 120. + 3000. * R.rand(),
  freq2 << 120. + 800. * R.rand(),
  'src1=' + freq1 + ' src2=' + freq2 + '\n' >> Stream.op

  #RpExpr:

  true

  #RpRate:

  '0.25s'
  *******/

//  e->set_repeat(Repeat("0.25s"));

  series->updctrl(TmTime("TmSampleCount/Virtual","0s"), e);

  //while (2) { series->tick(); }
  for (int i=1; i<100; i++) series->tick();
  delete series;
}

void sched3()
{
  MarSystemManager mng;

  MarSystem* fanin = mng.create("Fanin", "fanin");
  fanin->addMarSystem(mng.create("SineSource", "src1"));
  fanin->addMarSystem(mng.create("SineSource", "src2"));
  fanin->updctrl("SineSource/src1/mrs_real/frequency",400.0);  // A
  fanin->updctrl("SineSource/src2/mrs_real/frequency",554.37); // C#
  fanin->updctrl("SineSource/src3/mrs_real/frequency",659.26); // E

  MarSystem* series = mng.create("Series", "series");
  series->addMarSystem(fanin);

  series->addMarSystem(mng.create("AudioSink", "dest"));
  series->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

  EvExpr* e =
    new EvExpr(series,
               Ex("tmr<<Timer.cur","tmr.prefix + ' ' + tmr.time + '\n' >> Stream.op"),
               Rp("true"));

  e->set_repeat(Repeat("0.25s"));

  series->updctrl(TmTime("TmSampleCount/Virtual","0s"), e);

//  while (2) { series->tick(); }
  for (int i=1; i<100; i++) series->tick();

  delete series;
}

void sched4()
{
  MarSystemManager mng;

  MarSystem* fanin = mng.create("Fanin", "fanin");
  fanin->addMarSystem(mng.create("SineSource", "src1"));
  fanin->addMarSystem(mng.create("SineSource", "src2"));
  fanin->updctrl("SineSource/src1/mrs_real/frequency",3000.0);
  fanin->updctrl("SineSource/src2/mrs_real/frequency",1000.0);

  MarSystem* series = mng.create("Series", "series");
  series->addMarSystem(fanin);

  series->addMarSystem(mng.create("AudioSink", "dest"));
  series->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

  EvExpr* e = new EvExpr(series,
                         Ex((std::string)
                            "Fanin/fanin/SineSource/src1/mrs_real/frequency >> @freq1,"
                            +"Fanin/fanin/SineSource/src2/mrs_real/frequency >> @freq2,"
                            //  D#      F       G       G#      A#       C        D
                            +"just<<[ 309.38, 352.00, 391.11, 412.50, 469.33,  528.00,  586.67,"
                            +"  618.75, 704.00, 782.22, 825.00, 938.67, 1056.00, 1173.33,"
                            +" 1237.50], notes<<just.len",
                            "a<<notes.rand, b<<notes.rand, freq1 << just[a], freq2 << just[b]"),
                         Rp("true","'0.25s'"));

  series->updctrl(TmTime("TmSampleCount/Virtual","0s"), e);

  //while (2) { series->tick(); }
  for (int i=1; i<100; i++) series->tick();
  delete series;
}

#include "stdlib.h"

void sched5()
{
  MarSystemManager mng;

  MarSystem* fanin = mng.create("Fanin", "fanin");
  fanin->addMarSystem(mng.create("SineSource", "src1"));
  fanin->addMarSystem(mng.create("SineSource", "src2"));
  fanin->updctrl("SineSource/src1/mrs_real/frequency",0.0);
  fanin->updctrl("SineSource/src2/mrs_real/frequency",0.0);

  MarSystem* series = mng.create("Series", "series");
  series->addMarSystem(fanin);

  series->addMarSystem(mng.create("AudioSink", "dest"));
  series->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

  series->updctrl("inSamples",128);

  /*
  B1  493.88
  C1  554.37
  D1  622.25
  E1  659.26
  F1  739.99
  G1  830.61
  A2  880.00
  B2  987.77
  C2 1108.73
  D2 1244.51
  E2 1318.51
  F2 1479.98
  G2 1661.22
  A3 1760.00
  B3 1975.53
  */

  /* wacky wacky timing, oh dear */
  EvExpr* e1 = new EvExpr(series,
                          Ex((std::string)
                             "Fanin/fanin/SineSource/src1/mrs_real/frequency >> @freq1,"
                             //  D#      F       G       G#      A#       C        D
                             +"c<<1.0, h<<(c /2)+'s', q<<(c /4)+'s', e<<(c /8)+'s', s<<(c /16)+'s',"
                             +"t1<<[    q,       q,       q,       e,       e,       q,       q,       h,       e,       e,       e,       e,e,s,s,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,h,q,q,q,q,q,q,h],"
                             +"n1<<[ 987.77, 1661.22, 1661.22, 1479.98, 1318.51, 1479.98, 1760.00, 1760.00, 1975.53, 1661.22, 1760.00, 1479.98,"
                             +"      987.77, 1479.98, 1661.22, 1760.00, 1479.98, 1661.22, 1318.51, 1479.98, 1244.51,"
                             +"     1318.51,  987.77,  830.61,  659.26,  830.61,  987.77, 1318.51, 1661.22,"
                             +"      880.00, 1661.22, 1479.98, 1318.51, 1479.98, 1661.22, 1760.00, 1479.98,"
                             +"     1760.00,  987.77, 1661.22, 1108.73, 1479.98,  739.99, 1244.51, 1318.51 ],"
                             +"n<<0, num<<n1.len",
                             "freq1 << n1[n], n <<+ 1"),
                          Rp("n<num","t1[n]"));

  series->updctrl(TmTime("TmSampleCount/Virtual","0.5s"), e1);

  EvExpr* e2 = new EvExpr(series,
                          Ex((std::string)
                             "Fanin/fanin/SineSource/src2/mrs_real/frequency >> @freq2,"
                             //  D#      F       G       G#      A#       C        D
                             +"c<<1.0, w32<<(1.5*c)+'s', w<<c+'s', hd<<(1.5*c)+'s', h<<(0.5*c)+'s', q<<(0.25*c)+'s', e<<(0.125*c)+'s', s<<(0.0625*c)+'s',"
                             +"t2<<[    q  ,  q  ,    q  ,  w32,    h  , w32 ,    h  ,    q  ,  q  ,    q  ,  q  ,    q  ,  q  ,    h   ],"
                             +"n2<<[ 659.26, 1.00, 880.00, 100.00, 987.77, 1.00, 987.77, 659.26, 0.00, 880.00, 0.00, 493.88, 0.00, 659.26 ],"
                             +"n<<0, num<<n2.len",
                             "freq2 << n2[n], n <<+ 1"),
                          Rp("n<num","t2[n]"));

  series->updctrl(TmTime("TmSampleCount/Virtual","0.5s"), e2);
  //while (2) { series->tick(); }
  for (int i=1; i<400; i++) series->tick();
  delete series;
}
/*
void robot1()
{
  MarSystemManager mng;

  MarSystem* pnet = mng.create("Series", "pnet");
printf("4\n");
  pnet->addMarSystem(mng.create("DeviBot","dbot"));

printf("5\n");
  EvExpr* e =
    new EvExpr(pnet,
        Ex((std::string)
          +"DeviBot/dbot/mrs_string/arm >> @arm"
          +"DeviBot/dbot/mrs_natural/velocity >> @vel"
          +"DeviBot/dbot/mrs_bool/strike >> @strike",
           (std::string)
           "vel << 50, arm << 'Ga', strike << true, Stream.opn << strike"),
      Rp("true"));
printf("6\n");

  e->set_repeat(Repeat("0.25s"));
printf("7\n");

  pnet->updctrl(TmTime("TmSampleCount/Virtual","0s"), e);
printf("8\n");
//  while (2) { series->tick(); }
  for (int i=1;i<100;i++) pnet->tick();

  delete pnet;
}


*/

void usage() {
  printf("Scheduler Test-o-rama\n");
  printf("Usage: ./scheduler [test [options]]\n");
  printf("Test:\n");
  printf("  1         ~ EvExpr : Double fun-time random sine waves\n");
  printf("  2 [fname] ~ EvExpr : Double fun-time random sine waves, expression read from file\n");
  printf("  3         ~ EvExpr : Read and print the Timer's time\n");
  printf("  4         ~ EvExpr : Double random tones, Eb scale in Just Intonation, expr uses a list\n");
  printf("  5         ~ EvExpr : Poor rendition of Bach's Gavotte & Rondeau, Partitia #3\n");
}
bool match(const char* s1, const char* s2) {
  if (s1==NULL&&s2==NULL) return true;
  if (s1==NULL||s2==NULL) return false;
  int i=0;
  while(s1[i]==s2[i]) {
    if (s1[i]=='\0') return true;
    i++;
  }
  return false;
}
int main(int argc, char** argv) {
  switch(argc) {
  case 2:
    if (match(argv[1],"1")) { sched1(); break; }
    if (match(argv[1],"2")) { sched2(""); break; }
    if (match(argv[1],"3")) { sched3(); break; }
    if (match(argv[1],"4")) { sched4(); break; }
    if (match(argv[1],"5")) { sched5(); break; }
//        if (match(argv[1],"3")) robot1();
    usage(); break;
  case 3:
    if (match(argv[1],"2")) { sched2(argv[2]); break; }
    usage(); break;
  default:
    usage(); break;
  }
  return 0;
}

