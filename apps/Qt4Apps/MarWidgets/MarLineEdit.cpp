#include "MarLineEdit.h"
	
MarLineEdit::MarLineEdit(MarControlPtr cname, QWidget *parent) 
        : QLineEdit(parent)
{
  mycname = cname;
  connect(this, SIGNAL(editingFinished()), this, 
	  SLOT(reemitvalueChanged()));
}
    
void MarLineEdit::reemitvalueChanged()
{
  MarControlPtr myval(this->text().toStdString());
  emit valueChanged(mycname, myval);
}

	
void MarLineEdit::setValue(MarControlPtr cname)
{
  if (mycname->getName() == cname->getName())
    {
      QString qtest(cname->to<mrs_string>().c_str());
      blockSignals(true);
      QLineEdit::setText(qtest);
      blockSignals(false);
    }
}

	
