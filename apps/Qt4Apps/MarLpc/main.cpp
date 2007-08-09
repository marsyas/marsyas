#include <QApplication>
#include <QPushButton>
#include <math.h>
#include <iostream>
#include <string>

#include "MarLpcWindow.h"
#include "CommandLineOptions.h"

#include "..\OscMapper.h"

#define EMPTYSTRING "MARSYAS_EMPTY"

mrs_natural oscPort_;
mrs_bool noShow_;
string fileName_;

CommandLineOptions cmd_options;

void 
initOptions()
{
	cmd_options.addBoolOption("noShow", "n", false);
	cmd_options.addNaturalOption("oscPort", "o", 1);
	cmd_options.addStringOption("fileName", "f", EMPTYSTRING);
}


void 
loadOptions()
{
	noShow_ = cmd_options.getBoolOption("noShow");
	oscPort_ = cmd_options.getNaturalOption("oscPort");
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

	//OscMapper* oscMapper = new OscMapper(oscPort_, app, win->getMarSystemQtWrapper ());

	//oscMapper->registerQtSlot (win->frequencyPoleSlider_, "/frequencyPoleSlider", QVariant::Int);
	//oscMapper->registerQtSlot (win->amplitudePoleSlider_, "/amplitudePoleSlider", QVariant::Int);
	//oscMapper->registerQtSlot (win->tiltSlider_, "/tiltSlider", QVariant::Int);
	//oscMapper->registerQtSlot (win->breathinessSlider_, "/breathinessSlider", QVariant::Int);

	if(fileName_ != EMPTYSTRING)
		win->play(fileName_);

	return app->exec();
}
