#include "MarSpinBox.h"
	
MarSpinBox::MarSpinBox(MarControlPtr cname, QWidget *parent)
        : QSpinBox(parent)
{
  mycname = cname;
  connect(this, SIGNAL(valueChanged(int)), this, SLOT(reemitvalueChanged(int)));
}
    
void MarSpinBox::reemitvalueChanged(int val)
{
  float fval = val / 100.0;
  MarControlPtr myfval((mrs_real)fval);
  emit valueChanged(mycname, myfval);
}

	
void MarSpinBox::setValue(MarControlPtr cname)
{
  if (mycname->getName() == cname->getName())
    {
      int val = floor(cname->to<mrs_real>() * 100.0 + 0.5);
      blockSignals(true);
      QSpinBox::setValue(val);
      blockSignals(false);
    }
}
