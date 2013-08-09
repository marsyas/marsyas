#include "ColourMapDisplay.h"

ColourMapDisplay::ColourMapDisplay(QWidget *parent)
  : QWidget(parent)
{
  startPos = 0;
  renderArea = new SquareRenderArea(this);
  colourKey = new QGroupBox(this);
  QHBoxLayout *layout = new  QHBoxLayout;
  layout->addWidget(renderArea);
  layout->addWidget(colourKey);
  setLayout(layout);

  /*
  * index - genre - color
  * 0 - blues - Qt::blue
  * 1 - classical - Qt::darkRed
  * 2 - country - Qt::green
  * 3 - disco - PURPLE
  * 4 - hiphop - Qt::yellow
  * 5 - jazz - Qt::darkGreen
  * 6 - metal - BROWN
  * 7 - reggae - PINK
  * 8 - rock - ORANGE
  * 9 - pop - Qt::gray
  */
  colourKey->setTitle("Colour Key");
  QVBoxLayout *vbox = new QVBoxLayout;
  QString label = "<font color='%1'>%2</font>";
  QColor * colour = new QColor(Qt::darkRed);
  vbox->addWidget(new QLabel(label.arg(colour->name(), "Classical")));
  delete colour;
  colour = new QColor(Qt::blue);
  vbox->addWidget(new QLabel(label.arg(colour->name(),"Blues")));
  delete colour;
  colour = new QColor(Qt::green);
  vbox->addWidget(new QLabel(label.arg(colour->name(),"Country")));
  delete colour;
  colour = new QColor(PURPLE);
  vbox->addWidget(new QLabel(label.arg(colour->name(),"Disco")));
  delete colour;
  colour = new QColor(Qt::yellow);
  vbox->addWidget(new QLabel(label.arg(colour->name(),"HipHop")));
  delete colour;
  colour = new QColor(Qt::darkGreen);
  vbox->addWidget(new QLabel(label.arg(colour->name(),"Jazz")));
  delete colour;
  colour = new QColor(BROWN);
  vbox->addWidget(new QLabel(label.arg(colour->name(),"Metal")));
  delete colour;
  colour = new QColor(PINK);
  vbox->addWidget(new QLabel(label.arg(colour->name(),"Reggae")));
  delete colour;
  colour = new QColor(ORANGE);
  vbox->addWidget(new QLabel(label.arg(colour->name(),"Rock")));
  delete colour;
  colour = new QColor(Qt::gray);
  vbox->addWidget(new QLabel(label.arg(colour->name(),"Pop")));


  colourKey->setLayout(vbox);
  colourKey->setStyleSheet("QGroupBox{background-color: black; color: white;}");
  colourKey->show();
  renderArea->show();

}
ColourMapDisplay::~ColourMapDisplay()
{
}
void ColourMapDisplay::updateSquare(int * genreDensity, int numGenres)
{
  renderArea->updateSquare(genreDensity, numGenres);
  startPos = 1;
}