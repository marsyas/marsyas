#include "MarLpcWindow.h"
#include <marsyas/CommandLineOptions.h>

#include <QApplication>
#include <math.h>
#include <iostream>
#include <string>

using namespace std;
using namespace Marsyas;
using namespace MarsyasQt;

mrs_natural inputOscPort_;
mrs_natural outputOscPort_;

mrs_string inputOscHost_;
mrs_string outputOscHost_;

mrs_bool noShow_;
string fileName_;

CommandLineOptions cmd_options;

void
initOptions()
{
  cmd_options.addBoolOption("noShow", "n", false);
  cmd_options.addNaturalOption("inputOscPort", "i", 1);
  cmd_options.addStringOption("inputOscHost", "I", EMPTYSTRING);
  cmd_options.addNaturalOption("outputOscPort", "o", 2);
  cmd_options.addStringOption("outputOscHost", "O", EMPTYSTRING);
  cmd_options.addStringOption("fileName", "f", EMPTYSTRING);
}


void
loadOptions()
{
  noShow_ = cmd_options.getBoolOption("noShow");
  inputOscPort_ = cmd_options.getNaturalOption("inputOscPort");
  inputOscHost_ = cmd_options.getStringOption("inputOscHost");
  outputOscPort_ = cmd_options.getNaturalOption("outputOscPort");
  outputOscHost_ = cmd_options.getStringOption("outputOscHost");
  fileName_ = cmd_options.getStringOption("fileName");
}


int main(int argc, char **argv)
{

  initOptions();
  cmd_options.readOptions(argc, (const char **) argv);
  loadOptions();

  QApplication* app = new QApplication(argc, argv);

  MarLpcWindow* win = new MarLpcWindow();

  if(!noShow_)
    win->show();

#if 0
  if(inputOscHost_ == EMPTYSTRING)
    inputOscHostAddress_ = QHostAddress::LocalHost;
  else
    inputOscHostAddress_ = QString().fromStdString(inputOscHost_);


  if(outputOscHost_ == EMPTYSTRING)
    outputOscHostAddress_ = QHostAddress::LocalHost;
  else
    outputOscHostAddress_ = QString().fromStdString(outputOscHost_);

  OscMapper* oscMapper = new OscMapper(inputOscHostAddress_, inputOscPort_, outputOscHostAddress_, outputOscPort_, app, win->getMarSystemQtWrapper ());

  oscMapper->registerInputQtSlot (win->m_frequencyPoleSlider, "/frequencyPoleSlider", QVariant::Int);
  oscMapper->registerInputQtSlot (win->m_amplitudePoleSlider, "/amplitudePoleSlider", QVariant::Int);
  oscMapper->registerInputQtSlot (win->m_tiltSlider, "/tiltSlider", QVariant::Int);
  oscMapper->registerInputQtSlot (win->m_breathinessSlider, "/breathinessSlider", QVariant::Int);

  oscMapper->registerOutputQtSlot (win->m_breathinessSlider, "/breathinessSlider", QVariant::Int);

  // sending Osc messages is done through registration of the Marsyas Control in the MarSystemQtWrapper
  MarSystemQtWrapper *mwr = win->getMarSystemQtWrapper ();
  MarControlPtr formantOrderPtr = mwr->getctrl("mrs_natural/formantOrder");
  mwr->trackctrl(formantOrderPtr);
#endif
  if(fileName_ != EMPTYSTRING)
    win->play(QString::fromStdString(fileName_));

  return app->exec();
}
