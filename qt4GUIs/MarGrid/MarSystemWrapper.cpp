#include "MarSystemWrapper.h"

using namespace std;
using namespace Marsyas;

MarSystemWrapper::MarSystemWrapper(MarSystem* msys)
{

  msys_ = msys;
  running_ = false;
  guard_ = false;
  probing_ = false;
  pause_ = true;
  empty_ = false;
  

  vec_.create(512);
  
  
}


void MarSystemWrapper::probe(int state)
{
  /* 
  if (state == Qt::Checked) 
    {
      cout << "** PROBING *** " << endl;
      updctrl("Fanout/mixer/Series/sbr/mrs_bool/probe", true);
      updctrl("Fanout/mixer/Series/nbr/mrs_bool/probe", true);
      probing_ = true;
    }
  else
    {
      
      updctrl("Fanout/mixer/Series/sbr/mrs_bool/probe", false);
      updctrl("Fanout/mixer/Series/nbr/mrs_bool/probe", false);
      probing_ = false;
    }
  */ 
  
}




MarControlValue MarSystemWrapper::getctrl(string cname)
{
  MarControlValue v;
  v = msys_->getctrl(cname);
  
  cout << "getctrl ::" << cname << v << endl;
  return v;
}


void MarSystemWrapper::updctrl(QString cname, MarControlValue value) 
{

  cout << "Updctrl called" << endl;
  cout << "cname = " << cname.toStdString() << " -- " ;  
  cout << "value = " << value << endl;
  cur_cname = cname;
  cur_value = value;
  
  cnames_.push_back(cname);
  cvalues_.push_back(value);
  
  if (!running_) 
    {
      msys_->updctrl(cname.toStdString(), value);
      emit ctrlChanged(cname, value);
    }
  else 
    {
      guard_ = true;
      emit ctrlChanged(cname, value);
    }
  

  

}



void MarSystemWrapper::step() 
{
  cout << "STEPPING " << endl;
  
  
  running_ = true;
  
  msys_->tick();
  if (guard_ == true) 
    {
      
      vector<QString>::iterator  vsi;
      vector<MarControlValue>::iterator vvi;
      
      for (vsi = cnames_.begin(), vvi = cvalues_.begin(); 
	   vsi != cnames_.end();
	   ++vsi, ++vvi)
	{
	  msys_->updctrl(vsi->toStdString(), *vvi);
	}
      
      cnames_.clear();
      cvalues_.clear();
      guard_ = false;
    }
  
  
  
  
  if (probing_) 
    {
      msys_->updctrl("Fanout/mixer/Series/sbr/mrs_bool/probe", true);
      msys_->updctrl("Fanout/mixer/Series/nbr/mrs_bool/probe", true);
      
      vec_ = msys_->getctrl("Fanout/mixer/Series/sbr/mrs_realvec/input1").toVec();
      
      emit ctrlChanged("Fanout/mixer/Series/sbr/mrs_realvec/input1", vec_);
      
      vec_ = msys_->getctrl("Fanout/mixer/Series/nbr/mrs_realvec/input1").toVec();
      emit ctrlChanged("Fanout/mixer/Series/nbr/mrs_realvec/input1", vec_);
      
      
    }
  
  
  
}

void MarSystemWrapper::pause()
{
  pause_ = true;
}



void MarSystemWrapper::play()
{
  pause_ = false;
}



void MarSystemWrapper::run() 
{
  cout << "RUNNING " << endl;
  
  while(1)
    {
      running_ = true;

      

      if (guard_ == true)
	{
	  
	  vector<QString>::iterator  vsi;
	  vector<MarControlValue>::iterator vvi;
	  
	  for (vsi = cnames_.begin(), vvi = cvalues_.begin(); 
	       vsi != cnames_.end();
	       ++vsi, ++vvi)
	    {
	      cout << "in run " << vsi->toStdString() << "-" << *vvi << endl;
	      
	      msys_->updctrl(vsi->toStdString(), *vvi);
	    }
	  
	  cnames_.clear();
	  cvalues_.clear();
	  guard_ = false;
	}
      
      if (!pause_)
	{
	  msys_->tick();	
	  empty_ = false;
	}
      
      
      if (empty_ == false) 
	{	 
	  if (msys_->getctrl("mrs_bool/notEmpty").toBool() == false) 
	    {
	      cout << "PAUSING FOR EMPTY " << endl;
	      empty_ = true;
	      pause();
	    }
	  
	  
	}
      
      
      
      /* if (probing_) 
	{
	  msys_->updctrl("Fanout/mixer/Series/sbr/mrs_bool/probe", true);
	  msys_->updctrl("Fanout/mixer/Series/nbr/mrs_bool/probe", true);
	  
	  vec_ = msys_->getctrl("Fanout/mixer/Series/sbr/mrs_realvec/input1").toVec();
	  
	  emit ctrlChanged("Fanout/mixer/Series/sbr/mrs_realvec/input1", vec_);

	  vec_ = msys_->getctrl("Fanout/mixer/Series/nbr/mrs_realvec/input1").toVec();
	  emit ctrlChanged("Fanout/mixer/Series/nbr/mrs_realvec/input1", vec_);
	  
	  
	}
      */ 
      
      

    }
  

  cout << "FINISHED RUNNING" << endl;
  
}
