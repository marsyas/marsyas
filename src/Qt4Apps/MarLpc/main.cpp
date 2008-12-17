#include <QApplication>
#include <QPushButton>
#include <math.h>
#include <iostream>
#include <string>

#include "MarLpcWindow.h"
#include "CommandLineOptions.h"

#include "OscMapper.h"

mrs_natural inputOscPort_;
mrs_natural outputOscPort_;

mrs_string inputOscHost_;
mrs_string outputOscHost_;

mrs_bool noShow_;
string fileName_;


QHostAddress inputOscHostAddress_ = QHostAddress::LocalHost;
QHostAddress outputOscHostAddress_ = QHostAddress::LocalHost;

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


if(inputOscHost_ == EMPTYSTRING)
inputOscHostAddress_ = QHostAddress::LocalHost;
else
inputOscHostAddress_ = QString().fromStdString(inputOscHost_);


if(outputOscHost_ == EMPTYSTRING)
outputOscHostAddress_ = QHostAddress::LocalHost;
else
outputOscHostAddress_ = QString().fromStdString(outputOscHost_);

	OscMapper* oscMapper = new OscMapper(inputOscHostAddress_, inputOscPort_, outputOscHostAddress_, outputOscPort_, app, win->getMarSystemQtWrapper ());

	oscMapper->registerInputQtSlot (win->frequencyPoleSlider_, "/frequencyPoleSlider", QVariant::Int);
	oscMapper->registerInputQtSlot (win->amplitudePoleSlider_, "/amplitudePoleSlider", QVariant::Int);
	oscMapper->registerInputQtSlot (win->tiltSlider_, "/tiltSlider", QVariant::Int);
	oscMapper->registerInputQtSlot (win->breathinessSlider_, "/breathinessSlider", QVariant::Int);

	oscMapper->registerOutputQtSlot (win->breathinessSlider_, "/breathinessSlider", QVariant::Int);

	// sending Osc messages is done through registration of the Marsyas Control in the MarSystemQtWrapper
	MarSystemQtWrapper *mwr = win->getMarSystemQtWrapper ();
	MarControlPtr formantOrderPtr = mwr->getctrl("mrs_natural/formantOrder");
	mwr->trackctrl(formantOrderPtr);

	if(fileName_ != EMPTYSTRING)
		win->play(fileName_);

	return app->exec();
}
