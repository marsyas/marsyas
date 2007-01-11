#include <stdio.h>
#include "MarSystemManager.h"
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
      +"Fanin/fanin/SineSource/src2/mrs_real/frequency >> @freq2 "
      ,(std::string)
       "freq1 << 120. + 3000. * R.rand(),"
      +"freq2 << 120. + 800. * R.rand(),"
      +"'src1=' + freq1 + ' src2=' + freq2 + '\n' >> Stream.op"),
    Rp("true"));
#endif

  e->set_repeat(Repeat("0.25s"));

  series->updctrl(TmTime("TmSampleCount/Virtual","0s"), e);

  //while (2) { series->tick(); }
  for (int i=1;i<100;i++) series->tick();
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
              << "true" << std::endl;
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


/** Save these following lines into a file "__SchedulerTest_3__"
#ExInit:

Fanin/fanin/SineSource/src1/mrs_real/frequency >> @freq1,
Fanin/fanin/SineSource/src2/mrs_real/frequency >> @freq2

#ExExpr:

freq1 << 120. + 3000. * R.rand(),
freq2 << 120. + 800. * R.rand(),
'src1=' + freq1 + ' src2=' + freq2 + '\n' >> Stream.op

#RpExpr:

true
*******/

  e->set_repeat(Repeat("0.25s"));

  series->updctrl(TmTime("TmSampleCount/Virtual","0s"), e);

  //while (2) { series->tick(); }
  for (int i=1;i<100;i++) series->tick();
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
    for (int i=1;i<100;i++) series->tick();

  delete series;
}

void usage() {
    printf("Scheduler Test-o-rama\n");
    printf("Usage: ./scheduler [test [options]]\n");
    printf("Test:\n");
    printf("  1         ~ EvExpr : Double fun-time random sine waves\n");
    printf("  2 [fname] ~ EvExpr : Double fun-time random sine waves, expression read from file\n");
    printf("  3         ~ EvExpr : Read and print the Timer's time\n");
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
        usage(); break;
    case 3:
        if (match(argv[1],"2")) { sched2(argv[2]); break; }
        usage(); break;
    default:
        usage(); break;
    }
    return 0;
}

