/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/************************************************************************/
/* MarControlPtr implementation                                         */
/************************************************************************/

#include <marsyas/system/MarControl.h>
#include <marsyas/system/MarControlValue.h>
#include <marsyas/system/MarSystem.h>

#include <cstddef>

using std::ostringstream;
using std::vector;
using std::pair;
using std::size_t;
using std::string;

namespace Marsyas {


#ifdef TRACECONTROLS
std::set<MarControl*> MarControlPtr::controlTracer;

void
MarControlPtr::printControlTracer()
{
  std::set<MarControl*>::iterator it;
  if (MarControlPtr::controlTracer.size() > 0)
  {
    cout << "#############################################################" << endl;
    cout << "++ Existing MarControls: " << MarControlPtr::controlTracer.size() << endl;
    for (it=MarControlPtr::controlTracer.begin(); it!=MarControlPtr::controlTracer.end(); it++)
    {
      cout << (*it)->getMarSystem()->getPrefix() << (*it)->getName()
           << " | ref.count: " << (*it)->getRefCount() << endl;
    }
    cout << "#############################################################" << endl;
  }
}
#endif

MarControlPtr::MarControlPtr()
{
  control_ = NULL;
}

MarControlPtr::~MarControlPtr()
{
  if (control_)
  {
    TRACE_REMCONTROL;
    control_->unref();
    control_ = NULL;
  }
}

/************************************************************************/
/* MarControl implementation                                            */
/************************************************************************/


WAS_INLINE void MarControl::ref()
{
  refCount_++;
}
WAS_INLINE void MarControl::unref()
{
  if (--refCount_ <= 0)
    delete this;
}
int MarControl::getRefCount() const
{
  return refCount_;
}


MarControl*
MarControl::clone()
{
  return new MarControl(*this);
}

void
MarControl::setMarSystem(MarSystem* msys)
{
  msys_ = msys;
}

MarSystem*
MarControl::getMarSystem() const
{
  return msys_;
}

void
MarControl::setName(const std::string & cname)
{
  cname_ = cname;
  string::size_type separator = cname.find('/');
  if (separator != string::npos)
    id_ = cname.substr( separator + 1 );
  else
    id_.clear();
}

void
MarControl::setState(bool state)
{
  state_ = state;
}

bool
MarControl::hasState() const
{
  return state_;
}

mrs_string
MarControl::getType() const
{
  return value_->getType();
}

std::string MarControl::path() const
{
  string path;
  const MarSystem *system = getMarSystem();
  if (system)
  {
    path += system->path();
  }
  path += id_;
  return path;
}

void
MarControl::callMarSystemUpdate()
{
  if (state_ && msys_)
  {
    MarSystem* msys = msys_;
    msys->update(this);
    return;
  }
}

bool
MarControl::linkTo(MarControlPtr ctrl, bool update)
{
  if (ctrl.isInvalid())
  {
    ostringstream oss;
    oss << "MarControl::linkTo() - Linking to an invalid control ";
    oss << "(" << ctrl->cname_ << " with " << cname_ << ").";
    MRSWARN(oss.str());
    return false;
  }

  //check if these controls are already linked
  //(i.e. they own the same MarControlValue)
  if(value_ == ctrl->value_)
  {
    return true;//already linked! :-)
  }

  if (ctrl->value_->type_ != value_->type_)
  {
    ostringstream oss;
    oss << "MarControl::linkTo() - Linking controls of different types ";
    oss << "(" << ctrl->cname_ << " with " << cname_ << ").";
    MRSWARN(oss.str());
    return false;
  }

  //unlink this control (but keeping all links to it)
  //before linking it again to the passed control
  this->unlinkFromTarget();

  //store a pointer to the (soon to be old) MarControlValue object,
  //so we can later delete it from memory
  MarControlValue* oldvalue = value_;
  //and get a pointer to the new value
  MarControlValue* newvalue = ctrl->value_;

  //get all the links of our current MarControlValue so we can also
  //re-link them to the passed ctrl
  vector<pair<MarControl*, MarControl*> >::iterator lit;
  for(lit=oldvalue->links_.begin(); lit!=oldvalue->links_.end(); ++lit)
  {
    //make each linked control now point to the "passed" MarControlValue
    lit->first->value_ = newvalue;

    // check if this is the root link
    if(lit->first == lit->second)
    {
      //make it "link to" the passed control
      newvalue->links_.push_back(pair<MarControl*, MarControl*>(lit->first, ctrl()));
    }
    else //if not a root link, just copy the table entry unchanged into the new MarControlValue
      newvalue->links_.push_back(*lit);
  }

  //old MarControlValue can and should now be safely deleted from memory
  delete oldvalue;

  //check if it's needed to call update()
  if(update)
    value_->callMarSystemsUpdate();//newvalue->callMarSystemsUpdate();

  return true;
}


mrs_string
MarControl::to_string() const
{
  if(!this)
  {
    MRSERR("MarControl::to() - trying to get a value from a NULL MarControl! Returning invalid value...");
    return "";
  }
  const MarControlValueT<mrs_string> *ptr = dynamic_cast<const MarControlValueT<mrs_string>*>(value_);
  if(ptr)
  {
    return ptr->get();
  }
  else
  {
    MRSERR("MarControl::to() -  Incompatible type requested - " << "expected " << value_->getType() << " for control  " << this->getName()) ;
    return "";
  }
}

mrs_natural
MarControl::to_natural() const
{
  if(!this)
  {
    MRSERR("MarControl::to() - trying to get a value from a NULL MarControl! Returning invalid value...");
    return 0;
  }
  const MarControlValueT<mrs_natural> *ptr = dynamic_cast<const MarControlValueT<mrs_natural>*>(value_);
  if(ptr)
  {
    return ptr->get();
  }
  else
  {
    MRSERR("MarControl::to() -  Incompatible type requested - " << "expected " << value_->getType() << " for control  " << this->getName()) ;
    return 0;
  }
}


mrs_bool
MarControl::to_bool() const
{
  if(!this)
  {
    MRSERR("MarControl::to() - trying to get a value from a NULL MarControl! Returning invalid value...");
    return 0;
  }
  const MarControlValueT<mrs_bool> *ptr = dynamic_cast<const MarControlValueT<mrs_bool>*>(value_);
  if(ptr)
  {
    return ptr->get();
  }
  else
  {
    MRSERR("MarControl::to() -  Incompatible type requested - " << "expected " << value_->getType() << " for control  " << this->getName()) ;
    return 0;
  }
}

mrs_real
MarControl::to_real() const
{
  if(!this)
  {
    MRSERR("MarControl::to() - trying to get a value from a NULL MarControl! Returning invalid value...");
    return 0.0;
  }
  const MarControlValueT<mrs_real> *ptr = dynamic_cast<const MarControlValueT<mrs_real>*>(value_);
  if(ptr)
  {
    return ptr->get();
  }
  else
  {
    MRSERR("MarControl::to() -  Incompatible type requested - " << "expected " << value_->getType() << " for control  " << this->getName()) ;
    return 0.0;
  }
}

mrs_realvec
MarControl::to_realvec() const
{
  if(!this)
  {
    MRSERR("MarControl::to() - trying to get a value from a NULL MarControl! Returning invalid value...");
    realvec s;
    return s;
  }
  const MarControlValueT<mrs_realvec> *ptr = dynamic_cast<const MarControlValueT<mrs_realvec>*>(value_);
  if(ptr)
  {
    return ptr->get();
  }
  else
  {
    MRSERR("MarControl::to() -  Incompatible type requested - " << "expected " << value_->getType() << " for control  " << this->getName()) ;
    realvec s;
    return s;
  }
}



void
MarControl::unlinkFromAll()
{
  //first unlink this control from all the controls to which
  //it links to
  this->unlinkFromTarget();

  //get a list of all the controls that target this controls...
  vector<pair<MarControl*, MarControl*> >::iterator lit;
  vector<MarControl*> linkedControls;
  for(lit=value_->links_.begin(); lit!=value_->links_.end(); ++lit)
  {
    if(lit->second == this && lit->first != lit->second)
      linkedControls.push_back(lit->first);
  }
  //... and now unlink them all from this
  for(mrs_natural i=0; i < (mrs_natural)linkedControls.size(); ++i)
    linkedControls[i]->unlinkFromTarget();
}

void
MarControl::unlinkFromTarget()
{
  vector<pair<MarControl*, MarControl*> >::iterator lit;

  //check if this MarControl is linked
  //(i.e. more than one MarControl linking
  //to the MarControlValue).
  //if not, no point doing unlink - just return.
  if(value_->links_.size() <= 1)
    return;

  MarControlValue* oldvalue = value_;
  MarControlValue* newvalue = oldvalue->clone();

  vector<pair<MarControl*, MarControl*> >* inSet = new vector<pair<MarControl*, MarControl*> >;
  vector<pair<MarControl*, MarControl*> >* outSet = new vector<pair<MarControl*, MarControl*> >;

  size_t toProcess = oldvalue->links_.size();
  bool* processed = new bool[toProcess];
  for(size_t i=0; i < toProcess; ++i)
    processed[i] = false;

  //iterate over all the links
  MarControl* oldRootLink = NULL;
  //vector<pair<MarControl*, MarControl*> >::iterator lit;
  lit = oldvalue->links_.begin();
  mrs_natural idx = 0;
  while(toProcess > 0)
  {
    //avoid processing processed links
    if(!processed[idx])
    {
      //check if this is the old root link and send it to the outSet (also as a root link)
      if(lit->first == lit->second)
      {
        oldRootLink = lit->first;
        outSet->push_back(*lit);
        toProcess --;
        processed[idx]=true;
      }
      //check if this is this same control (i.e. the control to unlink at)
      //and set it as the root link of the inSet
      else if(lit->first == this)
      {
        lit->first->value_ = newvalue; //"this" is already locked for writing
        inSet->push_back(pair<MarControl*, MarControl*>(lit->first, lit->first));
        toProcess--;
        processed[idx]=true;
      }
      //check if this is directly linked to the old root link and if so,
      //send it to the outSet
      else if(lit->second == oldRootLink)
      {
        outSet->push_back(*lit);
        toProcess --;
        processed[idx]=true;
      }
      //if this control links directly to the control we want to unlink, send it to the inSet
      else if(lit->second == this)
      {
        lit->first->value_ = newvalue;
        inSet->push_back(*lit);
        toProcess--;
        processed[idx]=true;
      }
      //This control is not directly connected to any root link, so we have
      //to check to which set belongs the control it links to and assign it to
      //the same set
      else
      {
        bool found = false;
        vector<pair<MarControl*, MarControl*> >::iterator sit;
        //do a copy of the inSet because we are using iterators
        //and they become invalid if we change the size of the inSet object
        //inside the for loop below
        vector<pair<MarControl*, MarControl*> > inSet2 = *inSet;
        //start by searching in the inSet...
        for(sit = inSet2.begin(); sit != inSet2.end(); ++sit)
        {
          if(lit->second == sit->first)
          {
            lit->first->value_ = newvalue;
            inSet->push_back(*lit);
            toProcess--;
            processed[idx]=true;
            found = true;
          }
        }
        //if not found there, look for it in the outSet...
        if(!found)
        {
          vector<pair<MarControl*, MarControl*> > outSet2 = *outSet;
          for(sit = outSet2.begin(); sit != outSet2.end(); ++sit)
          {
            if(lit->second == sit->first)
            {
              outSet->push_back(*lit);
              toProcess--;
              processed[idx]=true;
            }
          }
        }
      }
    }

    //iterate until all links were processed!
    if(lit!=oldvalue->links_.end())
    {
      lit++;
      idx++;
    }
    else
    {
      lit=oldvalue->links_.begin();
      idx = 0;
    }
  }

  delete [] processed;

  //set the two unlinked tables
  oldvalue->links_ = *outSet;
  newvalue->links_ = *inSet;

  delete inSet;
  delete outSet;

  //check if newValue has in fact any control
  //linking to it
  if(newvalue->links_.size() == 0)
    delete newvalue;
}

bool
MarControl::isLinked() const
{
  //if there is only one link (i.e. this control itself),
  //it means that there are no other linked controls
  // => return false (i.e. 0)
  if(value_->links_.size()-1 == 0)
    return false;
  else
    return true;
}

vector<pair<MarControlPtr, MarControlPtr> >
MarControl::getLinks()
{
  vector<pair<MarControlPtr, MarControlPtr> > res;
  vector<pair<MarControl*, MarControl*> >::const_iterator lit;
  for(lit=value_->links_.begin(); lit != value_->links_.end(); ++lit)
  {
    res.push_back(pair<MarControlPtr, MarControlPtr>(MarControlPtr(lit->first),MarControlPtr(lit->second)));
  }
  return res;
}



WAS_INLINE MarControlPtr::MarControlPtr(MarControl control)
{
  control_ = new MarControl(control);
  control_->ref();
  TRACE_ADDCONTROL;
}

WAS_INLINE MarControlPtr::MarControlPtr(MarControlValue *value)
{
  control_ = new MarControl(value);
  control_->ref();
  TRACE_ADDCONTROL;
}

WAS_INLINE MarControlPtr::MarControlPtr(int ne)
{
  control_ = new MarControl((mrs_natural)ne);
  control_->ref();
  TRACE_ADDCONTROL;
}

WAS_INLINE MarControlPtr::MarControlPtr(unsigned int ne)
{
  control_ = new MarControl((mrs_natural)ne);
  control_->ref();
  TRACE_ADDCONTROL;
}

WAS_INLINE MarControlPtr::MarControlPtr(float ne)
{
  control_ = new MarControl(ne);
  control_->ref();
  TRACE_ADDCONTROL;
}


WAS_INLINE MarControlPtr::MarControlPtr(mrs_natural ne)
{
  control_ = new MarControl(ne);
  control_->ref();
  TRACE_ADDCONTROL;
}

WAS_INLINE MarControlPtr::MarControlPtr(double re)
{
  control_ = new MarControl(re);
  control_->ref();
  TRACE_ADDCONTROL;
}

WAS_INLINE MarControlPtr::MarControlPtr(const char *c)
{
  control_ = new MarControl(std::string(c));
  control_->ref();
  TRACE_ADDCONTROL;
}

WAS_INLINE MarControlPtr::MarControlPtr(std::string st)
{
  control_ = new MarControl(st);
  control_->ref();
  TRACE_ADDCONTROL;
}

WAS_INLINE MarControlPtr::MarControlPtr(mrs_bool be)
{
  control_ = new MarControl(be);
  control_->ref();
  TRACE_ADDCONTROL;
}

WAS_INLINE MarControlPtr::MarControlPtr(realvec ve)
{
  control_ = new MarControl(ve);
  control_->ref();
  TRACE_ADDCONTROL;
}

WAS_INLINE std::ostream& operator<<(std::ostream& os, const MarControlPtr& ctrl)
{
  return (os << (*ctrl.control_));
}

WAS_INLINE bool operator==(const MarControlPtr& v1, const MarControlPtr& v2)
{
  return (*v1.control_) == (*v2.control_);
}

WAS_INLINE bool operator!=(const MarControlPtr& v1, const MarControlPtr& v2)
{
  return (*v1.control_) != (*v2.control_);
}

WAS_INLINE mrs_real operator+(const MarControlPtr& v1, const mrs_real& v2)
{
  return (*v1.control_)+v2;
}

WAS_INLINE mrs_real operator+(const mrs_real& v1, const MarControlPtr& v2)
{
  return v1+(*v2.control_);
}

WAS_INLINE mrs_real operator-(const MarControlPtr& v1, const mrs_real& v2)
{
  return (*v1.control_)-v2;
}

WAS_INLINE mrs_real operator-(const mrs_real& v1, const MarControlPtr& v2)
{
  return v1-(*v2.control_);
}

WAS_INLINE mrs_real operator*(const MarControlPtr& v1, const mrs_real& v2)
{
  return (*v1.control_)*v2;
}
WAS_INLINE mrs_real operator*(const mrs_real& v1, const MarControlPtr& v2)
{
  return v1*(*v2.control_);
}

WAS_INLINE mrs_real operator/(const MarControlPtr& v1, const mrs_real& v2)
{
  return (*v1.control_)/v2;
}

WAS_INLINE mrs_real operator/(const mrs_real& v1, const MarControlPtr& v2)
{
  return v1/(*v2.control_);
}

WAS_INLINE MarControlPtr operator+(const MarControlPtr& v1, const MarControlPtr& v2)
{
  return (*v1.control_)+(*v2.control_);
}

WAS_INLINE MarControlPtr operator-(const MarControlPtr& v1, const MarControlPtr& v2)
{
  return (*v1.control_)-(*v2.control_);
}

WAS_INLINE MarControlPtr operator*(const MarControlPtr& v1, const MarControlPtr& v2)
{
  return (*v1.control_)*(*v2.control_);
}

WAS_INLINE MarControlPtr operator/(const MarControlPtr& v1, const MarControlPtr& v2)
{
  return (*v1.control_)/(*v2.control_);
}

bool operator<(const MarControlPtr& v1, const MarControlPtr& v2)
{
  return v1.control_ < v2.control_;
}

//WAS_INLINE
MarControlPtr::MarControlPtr(const MarControlPtr& a) //mutexes? [?]
{
  control_ = a.control_;
  if (control_)
  {
    control_->ref();
    TRACE_ADDCONTROL;
  }
}

WAS_INLINE
MarControlPtr::MarControlPtr(MarControl *control)//mutexes? [?]
{
  control_ = control;
  if (control_)
  {
    control_->ref();
    TRACE_ADDCONTROL;
  }
}

WAS_INLINE
MarControlPtr&
MarControlPtr::operator=(const MarControlPtr& a)//mutexes? [?]
{
  if (control_)
  {
    TRACE_REMCONTROL;
    control_->unref();
  }
  control_ = a.control_;
  if (control_)
  {
    control_->ref();
    TRACE_ADDCONTROL;
  }
  return *this;
}

WAS_INLINE
bool
MarControlPtr::isInvalid() const
{
  return (control_== NULL);
}

WAS_INLINE
bool
MarControlPtr::isEqual(const MarControlPtr& p)
{
  return (control_ == p.control_);
}



// default constructor
MarControl::MarControl() :
  refCount_(0),
  value_(NULL),
  msys_(NULL),
  state_(false),
  is_public_(false)
{


}


WAS_INLINE
MarControl::MarControl(const MarControl& a):
  refCount_(0),
  value_(a.value_->clone()),
  msys_(a.msys_),
  cname_(a.cname_),
  id_(a.id_),
  desc_(a.desc_),
  state_(a.state_),
  is_public_(a.is_public_)
{
  value_->links_.push_back(std::pair<MarControl*, MarControl*>(this, this));
}

WAS_INLINE
MarControl::MarControl(MarControlValue *value, std::string cname, MarSystem* msys, bool state):
  refCount_(0),
  value_(value->clone()),
  msys_(msys),
  state_(state),
  is_public_(false)
{
  setName(cname);
  value_->links_.push_back(std::pair<MarControl*, MarControl*>(this, this));
}

WAS_INLINE
MarControl::MarControl(double re, std::string cname, MarSystem* msys, bool state):
  refCount_(0),
  value_(new MarControlValueT<mrs_real>(re)),
  msys_(msys),
  state_(state),
  is_public_(false)
{
  setName(cname);
  value_->links_.push_back(std::pair<MarControl*, MarControl*>(this, this));
}

WAS_INLINE
MarControl::MarControl(float re, std::string cname, MarSystem* msys, bool state):
  refCount_(0),
  value_(new MarControlValueT<mrs_real>(re)),
  msys_(msys),
  state_(state),
  is_public_(false)
{
  setName(cname);
  value_->links_.push_back(std::pair<MarControl*, MarControl*>(this, this));
}


WAS_INLINE
MarControl::MarControl(mrs_natural ne, std::string cname, MarSystem* msys, bool state):
  refCount_(0),
  value_(new MarControlValueT<mrs_natural>(ne)),
  msys_(msys),
  state_(state),
  is_public_(false)
{
  setName(cname);
  value_->links_.push_back(std::pair<MarControl*, MarControl*>(this, this));
}

WAS_INLINE
MarControl::MarControl(std::string st, std::string cname, MarSystem* msys, bool state):
  refCount_(0),
  value_(new MarControlValueT<std::string>(st)),
  msys_(msys),
  state_(state),
  is_public_(false)
{
  setName(cname);
  value_->links_.push_back(std::pair<MarControl*, MarControl*>(this, this));
}

WAS_INLINE
MarControl::MarControl(mrs_bool be, std::string cname, MarSystem* msys, bool state):
  refCount_(0),
  value_(new MarControlValueT<bool>(be)),
  msys_(msys),
  state_(state),
  is_public_(false)
{
  setName(cname);
  value_->links_.push_back(std::pair<MarControl*, MarControl*>(this, this));
}

WAS_INLINE
MarControl::MarControl(realvec& ve, std::string cname, MarSystem* msys, bool state):
  refCount_(0),
  value_(new MarControlValueT<realvec>(ve)),
  msys_(msys),
  state_(state),
  is_public_(false)
{
  setName(cname);
  value_->links_.push_back(std::pair<MarControl*, MarControl*>(this, this));
}

WAS_INLINE
MarControl::~MarControl()
{
  //first unlink this control from everything
  this->unlinkFromAll();
  //now we can safely delete its uniquely owned MarControlValue
  delete value_;
  value_ = NULL;
}

WAS_INLINE
MarControl&
MarControl::operator=(const MarControl& a)
{
  if (this != &a)
    this->setValue(a.value_);

  return *this;
}



WAS_INLINE
bool
MarControl::setValue(MarControlPtr mc, bool update)
{
  if (value_->type_ != mc->value_->type_)
  {
    std::ostringstream sstr;
    sstr << "MarControl::setValue() - Trying to set value of incompatible type "
         << "(expected " << value_->type_ << ", given " << mc->value_->type_ << ")";
    MRSWARN(sstr.str());
    return false;
  }

  if (MarControlPtr(this) == mc)
  {
    return true;
  }

  value_->copyValue(*(mc->value_));

#ifdef MARSYAS_TRACECONTROLS
  value_->setDebugValue();
#endif

  //check if it's needed to call update()
  if(update)
    value_->callMarSystemsUpdate();

  return true;
}

WAS_INLINE
bool
MarControl::setValue(MarControlValue *mcv, bool update)
{
  if (value_->type_ != mcv->type_)
  {
    std::ostringstream sstr;
    sstr << "MarControl::setValue() - Trying to set value of incompatible type "
         << "(expected " << value_->type_ << ", given " << mcv->type_ << ")";
    MRSWARN(sstr.str());
    return false;
  }

  if (mcv->isEqual(value_))
  {
    return true;
  }

  value_->copyValue(*(mcv));

#ifdef MARSYAS_TRACECONTROLS
  value_->setDebugValue();
#endif

  //check if it's needed to call update()
  if(update)
    value_->callMarSystemsUpdate();

  return true;
}

WAS_INLINE
bool
MarControl::setValue(const char *t, bool update)
{
  return this->setValue(std::string(t), update);
}

WAS_INLINE
bool
MarControl::setValue(const int t, bool update)
{
  return this->setValue((mrs_natural)t, update);
}



WAS_INLINE
std::ostream&
operator<<(std::ostream& os, const MarControl& ctrl)
{
  return ctrl.value_->serialize(os);
}

WAS_INLINE
mrs_real
operator+(const MarControl& v1, const mrs_real& v2)
{
  mrs_real r1;
  MarControlValueT<mrs_real> *ptr = dynamic_cast<MarControlValueT<mrs_real>*>(v1.value_);
  if(ptr)
  {
    r1 = ptr->get();
  }
  else
  {
    std::ostringstream sstr;
    sstr << "MarControl::operator + : Trying to get value of incompatible type "
         << "(expected " << v1.getType() << ", given " << typeid(mrs_real).name() << ")";
    MRSWARN(sstr.str());
    return false;
  }
  return r1 + v2;
}

WAS_INLINE
mrs_real
operator+(const mrs_real& v1, const MarControl& v2)
{
  mrs_real r2;
  MarControlValueT<mrs_real> *ptr = dynamic_cast<MarControlValueT<mrs_real>*>(v2.value_);
  if(ptr)
  {
    r2 = ptr->get();
  }
  else
  {
    std::ostringstream sstr;
    sstr << "MarControl::operator + : Trying to get value of incompatible type "
         << "(expected " << v2.getType() << ", given " << typeid(mrs_real).name() << ")";
    MRSWARN(sstr.str());
    return false;
  }
  return v1 + r2;
}

WAS_INLINE
mrs_real
operator-(const MarControl& v1, const mrs_real& v2)
{
  mrs_real r1;
  MarControlValueT<mrs_real> *ptr = dynamic_cast<MarControlValueT<mrs_real>*>(v1.value_);
  if(ptr)
  {
    r1 = ptr->get();
  }
  else
  {
    std::ostringstream sstr;
    sstr << "[MarControl::setValue] Trying to get value of incompatible type "
         << "(expected " << v1.getType() << ", given " << typeid(mrs_real).name() << ")";
    MRSWARN(sstr.str());
    return false;
  }
  return r1 - v2;
}

WAS_INLINE
mrs_real
operator-(const mrs_real& v1, const MarControl& v2)
{
  mrs_real r2;
  MarControlValueT<mrs_real> *ptr = dynamic_cast<MarControlValueT<mrs_real>*>(v2.value_);
  if(ptr)
  {
    r2 = ptr->get();
  }
  else
  {
    std::ostringstream sstr;
    sstr << "[MarControl::setValue] Trying to get value of incompatible type "
         << "(expected " << v2.getType() << ", given " << typeid(mrs_real).name() << ")";
    MRSWARN(sstr.str());
    return false;
  }
  return v1 - r2;
}

WAS_INLINE
mrs_real
operator*(const MarControl& v1, const mrs_real& v2)
{
  mrs_real r1;
  MarControlValueT<mrs_real> *ptr = dynamic_cast<MarControlValueT<mrs_real>*>(v1.value_);
  if(ptr)
  {
    r1 = ptr->get();
  }
  else
  {
    std::ostringstream sstr;
    sstr << "[MarControl::setValue] Trying to get value of incompatible type "
         << "(expected " << v1.getType() << ", given " << typeid(mrs_real).name() << ")";
    MRSWARN(sstr.str());
    return false;
  }
  return r1 * v2;
}

WAS_INLINE
mrs_real
operator*(const mrs_real& v1, const MarControl& v2)
{
  mrs_real r2;
  MarControlValueT<mrs_real> *ptr = dynamic_cast<MarControlValueT<mrs_real>*>(v2.value_);
  if(ptr)
  {
    r2 = ptr->get();
  }
  else
  {
    std::ostringstream sstr;
    sstr << "[MarControl::setValue] Trying to get value of incompatible type "
         << "(expected " << v2.getType() << ", given " << typeid(mrs_real).name() << ")";
    MRSWARN(sstr.str());
    return false;
  }
  return v1 * r2;
}

WAS_INLINE
mrs_real
operator/(const MarControl& v1, const mrs_real& v2)
{
  mrs_real r1;
  MarControlValueT<mrs_real> *ptr = dynamic_cast<MarControlValueT<mrs_real>*>(v1.value_);
  if(ptr)
  {
    r1 = ptr->get();
  }
  else
  {
    std::ostringstream sstr;
    sstr << "[MarControl::setValue] Trying to get value of incompatible type "
         << "(expected " << v1.getType() << ", given " << typeid(mrs_real).name() << ")";
    MRSWARN(sstr.str());
    return false;
  }
  return r1 / v2;
}

WAS_INLINE
mrs_real
operator/(const mrs_real& v1, const MarControl& v2)
{
  mrs_real r2;
  MarControlValueT<mrs_real> *ptr = dynamic_cast<MarControlValueT<mrs_real>*>(v2.value_);
  if(ptr)
  {
    r2 = ptr->get();
  }
  else
  {
    std::ostringstream sstr;
    sstr << "[MarControl::setValue] Trying to get value of incompatible type "
         << "(expected " << v2.getType() << ", given " << typeid(mrs_real).name() << ")";
    MRSWARN(sstr.str());
    return false;
  }
  return v1 / r2;
}

WAS_INLINE
MarControl
operator+(const MarControl& v1, const MarControl& v2)
{
  MarControlValue *val = v1.value_->sum(v2.value_);
  MarControl ret(val);
  delete val;
  return ret;
}

WAS_INLINE
MarControl
operator-(const MarControl& v1, const MarControl& v2)
{
  MarControlValue *val = v1.value_->subtract(v2.value_);
  MarControl ret(val);
  delete val;
  return ret;
}

WAS_INLINE
MarControl
operator*(const MarControl& v1, const MarControl& v2)
{
  MarControlValue *val = v1.value_->multiply(v2.value_);
  MarControl ret(val);
  delete val;
  return ret;
}

WAS_INLINE
MarControl
operator/(const MarControl& v1, const MarControl& v2)
{
  MarControlValue *val = v1.value_->divide(v2.value_);
  MarControl ret(val);
  delete val;
  return ret;
}


WAS_INLINE
bool
MarControl::isTrue()
{
  MarControlValueT<bool> *ptr = dynamic_cast<MarControlValueT<bool>*>(value_);
  if(ptr)
  {
    return ptr->get();
  }
  else
  {
    std::ostringstream sstr;
    sstr << "MarControl::isTrue() - Trying to get use bool-specific method with " << value_->getType();
    MRSWARN(sstr.str());
    return false;
  }
}


} // namespace Marsyas
