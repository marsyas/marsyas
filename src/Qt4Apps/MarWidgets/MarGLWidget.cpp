#include "MarGLWidget.h"

using namespace Marsyas;

	
MarGLWidget::MarGLWidget(QString cname, QWidget *parent)
        : GLWidget(parent)
{
  
  mycname = cname;
  connect(this, SIGNAL(waveformChanged(realvec)), this, 
	  SLOT(reemitvalueChanged(realvec)));

  setMinimumSize(150,100);
  this->show();
}

    
void 
MarGLWidget::reemitvalueChanged(realvec val)
{
  emit valueChanged(mycname, val);
}


void 
MarGLWidget::setValue(QString cname, MarControlValue fval)
{
  if (mycname == cname)  
    {
      if (fval.getType() == mar_vec)
	GLWidget::setWaveform(fval.toVec());
    }
}


	
