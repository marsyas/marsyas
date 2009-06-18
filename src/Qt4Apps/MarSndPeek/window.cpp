#include <QtGui>
#include <QLCDNumber>

#include "glwidget.h"
#include "window.h"

Window::Window(string inAudioFileName)
{

  glWidget = new GLWidget(inAudioFileName);

  // Create the x,y,z sliders
  xSlider = createRotationSlider();
  ySlider = createRotationSlider();
  zSlider = createRotationSlider();

  // The y-scale slider
  yScaleSlider = createSlider(0,1000,10,100,50);

  // Create the x,y,z sliders for the testing rotation angles for the animation
  test_xSlider = createSlider(0,100,1,10,10);
  test_ySlider = createSlider(0,100,1,10,10);
  test_zSlider = createSlider(0,100,1,10,10);

// 	// Create the x,y,z sliders for the endinging rotation angles for the animation
//     end_xSlider = createSlider();
//     end_ySlider = createSlider();
//     end_zSlider = createSlider();

// 	// Create a slider to control the speed of the animation
//     speedSlider = createTimerSlider();

// 	// Create a button to start the animation
// 	go_button  = new QPushButton(tr("Go!"));

// 	// Connect a click signal on the go button to a slot to start the rotation time
//     connect(go_button, SIGNAL(clicked()), glWidget, SLOT(startTimerRotate()));

// 	// The current time index of the animation
// 	currentTimeLCD = new QLCDNumber(3);
//     currentTimeLCD->setSegmentStyle(QLCDNumber::Filled);
// 	connect(glWidget, SIGNAL(timerChanged(int)), currentTimeLCD, SLOT(display(int)));

 	// Connect up the x,y,z sliders with slots to set the rotation values
     connect(xSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setXRotation(int)));
     connect(glWidget, SIGNAL(xRotationChanged(int)), xSlider, SLOT(setValue(int)));
     connect(ySlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setYRotation(int)));
     connect(glWidget, SIGNAL(yRotationChanged(int)), ySlider, SLOT(setValue(int)));
     connect(zSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setZRotation(int)));
     connect(glWidget, SIGNAL(zRotationChanged(int)), zSlider, SLOT(setValue(int)));

	 // Scale sliders
     connect(yScaleSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setYScale(int)));

	 // Connect up the test x,y,z rotation sliders
     connect(test_xSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setTestX(int)));
     connect(test_ySlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setTestY(int)));
     connect(test_zSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setTestZ(int)));

// 	// Connect up the end x,y,z rotation sliders
//     connect(end_xSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setEndXRotation(int)));
//     connect(glWidget, SIGNAL(end_xRotationChanged(int)), end_xSlider, SLOT(setValue(int)));
//     connect(end_ySlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setEndYRotation(int)));
//     connect(glWidget, SIGNAL(end_yRotationChanged(int)), end_ySlider, SLOT(setValue(int)));
//     connect(end_zSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setEndZRotation(int)));
//     connect(glWidget, SIGNAL(end_zRotationChanged(int)), end_zSlider, SLOT(setValue(int)));

// 	// Connect up the animation speed slider
//     connect(speedSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setRotationSpeed(int)));
//     connect(glWidget, SIGNAL(rotationSpeedChanged(int)), speedSlider, SLOT(setValue(int)));

	// A main layout to hold everything
    QHBoxLayout *layout = new QHBoxLayout;

	// The OpenGL window and the sliders to move it interactively
    QVBoxLayout *gl_layout = new QVBoxLayout;
    gl_layout->addWidget(glWidget);

	QHBoxLayout *x_slider_layout = new QHBoxLayout;
 	QLabel *x_slider_label = new QLabel(("X"));
 	x_slider_layout->addWidget(x_slider_label);
	x_slider_layout->addWidget(xSlider);
	gl_layout->addLayout(x_slider_layout);
	
	QHBoxLayout *y_slider_layout = new QHBoxLayout;
 	QLabel *y_slider_label = new QLabel(("Y"));
 	y_slider_layout->addWidget(y_slider_label);
	y_slider_layout->addWidget(ySlider);
	gl_layout->addLayout(y_slider_layout);

	QHBoxLayout *z_slider_layout = new QHBoxLayout;
 	QLabel *z_slider_label = new QLabel(("Z"));
 	z_slider_layout->addWidget(z_slider_label);
	z_slider_layout->addWidget(zSlider);
	gl_layout->addLayout(z_slider_layout);

	layout->addLayout(gl_layout);

 	// All the controls on the right side of the window
	QVBoxLayout *controls_layout = new QVBoxLayout;
 	layout->addLayout(controls_layout);

 	// The scaling sliders
	QVBoxLayout *scale_layout = new QVBoxLayout;
 	QLabel *scale_label = new QLabel(("Scale"));
 	scale_layout->addWidget(scale_label);
 	scale_layout->addWidget(yScaleSlider);
 	controls_layout->addLayout(scale_layout);

 	// The test coordinates for the animation
	QVBoxLayout *test_layout = new QVBoxLayout;
 	QLabel *test_label = new QLabel(("Test"));
 	test_layout->addWidget(test_label);
 	test_layout->addWidget(test_xSlider);
 	test_layout->addWidget(test_ySlider);
 	test_layout->addWidget(test_zSlider);
 	controls_layout->addLayout(test_layout);

// 	// The end coordinates for the animation
//     QVBoxLayout *end_layout = new QVBoxLayout;
// 	QLabel *end_label = new QLabel(("End"));
// 	end_layout->addWidget(end_label);
// 	end_layout->addWidget(end_xSlider);
// 	end_layout->addWidget(end_ySlider);
// 	end_layout->addWidget(end_zSlider);
// 	controls_layout->addLayout(end_layout);

// 	// Controls for the animation
//     QVBoxLayout *buttons_layout = new QVBoxLayout;
// 	QLabel *speed_label = new QLabel(("Animation Speed"));
// 	buttons_layout->addWidget(speed_label);
// 	buttons_layout->addWidget(speedSlider);
// 	buttons_layout->addWidget(go_button);
// 	buttons_layout->addWidget(currentTimeLCD);
// 	controls_layout->addLayout(buttons_layout);

	// Set the layout for this widget to the layout we just created
    setLayout(layout);

 	// Set some nice defaults for all the sliders
	xSlider->setValue(0 * 16);
	ySlider->setValue(0 * 16);
	zSlider->setValue(0 * 16);

    yScaleSlider->setValue(350);
	
     test_xSlider->setValue(0);
     test_ySlider->setValue(50);
     test_zSlider->setValue(0);

//     end_xSlider->setValue(100 * 16);
//     end_ySlider->setValue(200 * 16);
//     end_zSlider->setValue(300 * 16);

// 	speedSlider->setValue(10);

    setWindowTitle(tr("MarSndPeek"));


}

//
// A handy function to create a generic slider for the x,y,z positions
//
QSlider *Window::createRotationSlider()
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 360 * 16);
    slider->setSingleStep(16);
    slider->setPageStep(15 * 16);
    slider->setTickInterval(15 * 16);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

QSlider *Window::createSlider(int range_start, int range_end, int single_step, int page_step, int tick_interval)
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(range_start, range_end);
    slider->setSingleStep(single_step);
    slider->setPageStep(page_step);
    slider->setTickInterval(tick_interval);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

// //
// // Another handy function to create a slider for the animation speed slider
// //
// QSlider *Window::createTimerSlider()
// {
//     QSlider *slider = new QSlider(Qt::Horizontal);
//     slider->setRange(0, 100);
//     slider->setSingleStep(1);
//     slider->setPageStep(10);
//     slider->setTickInterval(10);
//     slider->setTickPosition(QSlider::TicksRight);
//     return slider;
// }
