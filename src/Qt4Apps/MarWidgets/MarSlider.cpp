#include "MarSlider.h"
	
MarSlider::MarSlider(MarControlPtr cname, QWidget *parent)
        : QSlider(Qt::Horizontal, parent)
{
  mycname = cname;
  connect(this, SIGNAL(valueChanged(int)), this, SLOT(reemitvalueChanged(int)));
  setMinimumSize(50,100);
  pfval = 0;
  
}


    
void MarSlider::reemitvalueChanged(int val)
{
  float fval = val / 100.0;
  MarControlPtr myfval((mrs_real)fval);
  emit valueChanged(mycname, myfval);
}



void MarSlider::setValue(MarControlPtr cname)
{
  if (mycname->getName() == cname->getName())
    {
      int val = floor(cname->to<mrs_real>() * 100.0 + 0.5);  
      blockSignals(true);
      QSlider::setValue(val);
      blockSignals(false);
    }
}

