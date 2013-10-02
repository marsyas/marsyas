/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include <marsyas/sched/Scheduler.h>
#include <marsyas/sched/EvExpr.h>
#include <marsyas/sched/TmTimerManager.h>

using namespace std;
//using namespace Marsyas;

namespace Marsyas //lmartins: hack: should work without this [?][!]
{

Scheduler::Scheduler()
{
  timers_=NULL;
  timers_count_=0;
}

Scheduler::~Scheduler()
{
  removeAll();
}

void
Scheduler::tick()
{
  for (int i=0; i<timers_count_; ++i) { timers_[i]->tick(); }
}

bool
Scheduler::eventPending()
{
  for (int i=0; i<timers_count_; ++i) {
    if (timers_[i]->eventPending()) { return true; }
  }
  return false;
}

void
Scheduler::addTimer(TmTimer* t)
{
  if (t == NULL) return;
  // look for schedulers with same name to ensure only one of each name
  if (findTimer(t->getPrefix())!=NULL) {
    MRSWARN("Scheduler::addTimer(TmTimer)  refusing to add timer with name already in use");
  }
  else
    appendTimer(t);
}

void
Scheduler::addTimer(std::string class_name, std::string identifier)
{
  // look for schedulers with same name to ensure only one of each name
  if (findTimer(class_name+"/"+identifier)!=NULL) {
    MRSWARN("Scheduler::addTimer(\""+class_name+"\",\""+identifier+"\")  refusing to add timer with name already in use");
  }
  else {
    addTimer(TmTimerManager::getInstance()->make(class_name,identifier));
  }
}

void
Scheduler::addTimer(std::string class_name, std::string identifier, std::vector<TmParam> params)
{
  // look for schedulers with same name to ensure only one of each name
  if (findTimer(class_name+"/"+identifier)!=NULL) {
    MRSWARN("Scheduler::addTimer(\""+class_name+"\",\""+identifier+"\",TmParams)  refusing to add timer with name already in use");
  }
  else {
    addTimer(TmTimerManager::getInstance()->make(class_name,identifier,params));
  }
}

void
Scheduler::split_cname(std::string cname, std::string* head, std::string* tail)
{
  bool second=false;
  for (size_t i=0; i<cname.length(); ++i) {
    if (cname[i]=='/') {
      if (!second) {
//                scheduler_type = cname.substr(0,i);
        second=true;
      }
      else {
        *head = cname.substr(0,i);
        *tail = cname.substr(i+1,cname.length());
        break;
      }
    }
  }
}

void
Scheduler::updtimer(std::string tmr_id, TmControlValue value)
{
  mrs_string timer_ident="";
  mrs_string timer_control="";
  split_cname(tmr_id,&timer_ident,&timer_control);
  TmTimer* s = findTimer(timer_ident);
  if (s==NULL) {
    MRSWARN("Scheduler::updtimer(std::string,TmControlValue)  no timer: "+timer_ident);
  }
  else {
    s->updtimer(timer_control,value);
  }
}
void
Scheduler::updtimer(std::string tmr_id, TmParam& param)
{
  TmTimer* s = findTimer(tmr_id);
  if (s==NULL) {
    MRSWARN("Scheduler::updtimer(std::string,TmControlValue)  no timer: "+tmr_id);
  }
  else {
    s->updtimer(param);
  }
}

void
Scheduler::updtimer(std::string tmr_id, std::vector<TmParam> params)
{
  mrs_string timer_ident="";
  mrs_string timer_control="";
  split_cname(tmr_id,&timer_ident,&timer_control);
  TmTimer* s = findTimer(timer_ident);
  if (s==NULL) {
    MRSWARN("Scheduler::updtimer(std::string,TmControlValue)  no timer: "+timer_ident);
  }
  else {
    s->updtimer(params);
  }
}

void
Scheduler::appendTimer(TmTimer* s)
{
  timers_ = (TmTimer**)realloc(timers_,sizeof(TmTimer*)*(timers_count_+1));
  timers_[timers_count_] = s;
  timers_count_ = timers_count_ + 1;
}

TmTimer*
Scheduler::findTimer(std::string name)
{
  for (int i=0; i<timers_count_; ++i) {
    TmTimer* s = timers_[i];
    if (s->getPrefix()==name) {
      return s;
    }
  }
  return NULL;
}

bool
Scheduler::removeTimer(std::string name)
{
  for (int i=0; i<timers_count_; ++i) {
    if (timers_[i]->getPrefix()==name) {
      delete(timers_[i]);
      for (int j=i+1; j<timers_count_; j++) {
        timers_[j-1]=timers_[j];
        timers_[j]=NULL;
      }
      timers_=(TmTimer**)realloc(timers_,sizeof(TmTimer*)*timers_count_);
      return true;
    }
  }
  return false;
}

void
Scheduler::removeAll()
{
  if (timers_count_>0) {
    for (int i=0; i<timers_count_; ++i) {
      delete timers_[i];
    }
    free(timers_);
    timers_=NULL;
    timers_count_=0;
  }
}

void
Scheduler::post(std::string time, std::string tmname, Repeat r, EvEvent* me)
{
  TmTimer* s = findTimer(tmname);
  if (s!=NULL) {
    if (me!=NULL) {
      // EvExpr supports querying of the scheduler environment in expressions
      EvExpr* e=dynamic_cast<EvExpr*>(me);
      if (e!=NULL) {
        MRSWARN("Scheduler::post(mrs_string time, mrs_string tmname, Repeat r, EvEvent* me) : setScheduler is not working yet");
        e->getExpression()->setScheduler(this);
      }
      s->post(time,r,me);
    }
    else {
      MRSWARN("Scheduler::post(mrs_string,mrs_string,Repeat,EvEvent*)  NULL event");
    }
  }
  else { MRSWARN("Scheduler::post(mrs_string,mrs_string,Repeat,EvEvent*)  unknown timer name: "+tmname); }
}

void
Scheduler::post(TmTime t, Repeat r, EvEvent* me)
{
  // pass the buck
  post(t.getTime(),t.getTimeName(),r,me);
}

void
Scheduler::post(std::string event_time, Repeat rep, EvEvent* me)
{
  if (timers_[0]!=NULL) {
    post(event_time,timers_[0]->getPrefix(),rep,me);
  }
}

void
Scheduler::post(std::string event_time, EvEvent* me)
{
  if (timers_[0]!=NULL) {
    post(event_time,Repeat(),me);
  }
}

mrs_natural
Scheduler::getTime(std::string timer)
{
  TmTimer* s = findTimer(timer);
  if (s!=NULL)
    return s->getTime();
  MRSWARN("Scheduler::getTime(string)  unknown timer '"+timer+"'");
  return 0;
}

//ostream&
//Marsyas::operator<< (ostream& o, Scheduler& sys) {
////    sys.put(o);
//    return o;
//}

}//namespace Marsyas
