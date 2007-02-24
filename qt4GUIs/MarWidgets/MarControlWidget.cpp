#include "MarControlWidget.h"
	
MarControlWidget::MarControlWidget(MarControlPtr cname, QString widget_type, 
				   MarSystemQtWrapper* mwr, QWidget *parent = 0):QWidget(parent)
{

  mycname = cname;

  qRegisterMetaType<MarControlPtr>("MarControlPtr");  
  
  if (widget_type == "slider")
    {
      QLabel *cnameLabel = new QLabel(cname->getName().c_str(), this);
      MarSlider* slider = new MarSlider(mycname, this);
      slider->setFixedSize(150,20);
      
      connect(slider, SIGNAL(valueChanged(MarControlPtr, MarControlPtr)), 
	      mwr,    SLOT(updctrl(MarControlPtr,MarControlPtr)));

      connect(mwr, SIGNAL(ctrlChanged(MarControlPtr)),
	      slider, SLOT(setValue(MarControlPtr)));
			  
      QVBoxLayout *layout = new QVBoxLayout;
      layout->addWidget(slider,0, Qt::AlignLeft);
      layout->addWidget(cnameLabel, 0, Qt::AlignLeft);


      this->setLayout(layout);
    }

  if (widget_type == "spinbox")
    {

      QLabel *cnameLabel = new QLabel(cname->getName().c_str(), this);
      MarSpinBox* spinbox = new MarSpinBox(mycname,this);
      

      // spinbox->setFixedSize(150,20);
      connect(spinbox, SIGNAL(valueChanged(MarControlPtr, MarControlPtr)), 
	      mwr,    SLOT(updctrl(MarControlPtr,MarControlPtr)));
      
      connect(mwr, SIGNAL(ctrlChanged(MarControlPtr)),
	      spinbox, SLOT(setValue(MarControlPtr)));      

      QVBoxLayout *layout = new QVBoxLayout;
      layout->addWidget(spinbox,0, Qt::AlignLeft);
      layout->addWidget(cnameLabel, 0, Qt::AlignLeft);
      this->setLayout(layout);
    }

   if (widget_type == "lineedit")
    {
      QLabel *cnameLabel = new QLabel(cname->getName().c_str(), this);
      // QSpinBox* spinbox = new QSpinBox(cname.toStdString(),this);
      MarLineEdit* lineedit = new MarLineEdit(mycname,this);
      
      // lineedit->setFixedSize(150,20);

      connect(lineedit, SIGNAL(valueChanged(MarControlPtr, MarControlPtr)), 
	      mwr,    SLOT(updctrl(MarControlPtr,MarControlPtr)));
      
      connect(mwr, SIGNAL(ctrlChanged(MarControlPtr)),
	      lineedit, SLOT(setValue(MarControlPtr)));      

      QVBoxLayout *layout = new QVBoxLayout;
      layout->addWidget(lineedit,0, Qt::AlignLeft);
      layout->addWidget(cnameLabel, 0, Qt::AlignLeft);
      this->setLayout(layout);
    }


  /* if (widget_type == "gl")
    {
      QLabel *cnameLabel = new QLabel(cname, this);
      MarGLWidget* glw = new MarGLWidget(mycname,this);
      
      // connect(glw, SIGNAL(valueChanged(QString, MarControlValue)), 
      // mwr,    SLOT(updctrl(QString,MarControlValue)));
      
      connect(mwr, SIGNAL(ctrlChanged(QString,MarControlValue)),
	      glw, SLOT(setValue(QString,MarControlValue)));      
      
      QVBoxLayout *layout = new QVBoxLayout;
      layout->addWidget(glw,0, Qt::AlignLeft);
      layout->addWidget(cnameLabel, 0, Qt::AlignLeft);
      this->setLayout(layout);
    }
  
 */ 

}
    



void MarControlWidget::valueChanged(MarControlPtr cname, MarControlPtr val)
{
  cout << "valueChanged" << endl;
  
}


