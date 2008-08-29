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

#include "Scheduler.h"
#include "EvExpr.h"
#include "TimerFactory.h"

using namespace std;
//using namespace Marsyas;

namespace Marsyas //lmartins: hack: should work without this [?][!]
{

Scheduler::Scheduler()
{
    timers=NULL;
    timers_count=0;
}
Scheduler::~Scheduler() { removeAll(); }
void Scheduler::tick()
{
    for (int i=0;i<timers_count;i++) { timers[i]->tick(); }
}
bool Scheduler::eventPending()
{
    for (int i=0;i<timers_count;i++) {
        if (timers[i]->eventPending()) { return true; }
    }
    return false;
}

void Scheduler::addTimer(TmTimer* t) {
    if (t == NULL) return;
    // look for schedulers with same name to ensure only one of each name
	if (findTimer(t->getPrefix())!=NULL){
        MRSWARN("Scheduler::addTimer(TmTimer)  refusing to add timer with name already in use");
	}
    else
        appendTimer(t);
}
void
Scheduler::addTimer(string class_name, string given_name)
{
    // look for schedulers with same name to ensure only one of each name
	if (findTimer(class_name+"/"+given_name)!=NULL){
            MRSWARN("Scheduler::addTimer(string,string)  refusing to add timer with name already in use");
	}
        else {
            addTimer(TimerFactory::getInstance()->make(class_name,given_name));
        }
//    appendTimer(new Scheduler(class_name,given_name));
    
}

void
Scheduler::split_cname(std::string cname, std::string* head, std::string* tail)
{
    bool second=false;
    for (unsigned int i=0;i<cname.length();i++) {
        if (cname[i]=='/') {
            if (!second) {
//                scheduler_type = cname.substr(0,i);
                second=true;
            } else {
                *head = cname.substr(0,i);
                *tail = cname.substr(i+1,cname.length());
                break;
            }
        }
    }
}

void
Scheduler::updtimer(std::string cname, TmControlValue value)
{
    string timer_ident="";
    string timer_control="";
    split_cname(cname,&timer_ident,&timer_control);
    TmTimer* s = findTimer(timer_ident);
    if (s==NULL) {
        MRSWARN("Scheduler::updtimer(std::string,TmControlValue)  no timer: "+timer_ident);
    } else {
        s->updtimer(timer_control,value);
    }
}

void
Scheduler::appendTimer(TmTimer* s)
{
    timers = (TmTimer**)realloc(timers,sizeof(TmTimer*)*(timers_count+1));
    timers[timers_count] = s;
    timers_count = timers_count + 1;
}

TmTimer*
Scheduler::findTimer(std::string name)
{
    for (int i=0;i<timers_count;i++) {
        TmTimer* s = timers[i];
        if (s->getPrefix()==name) {
            return s;
        }
    }
    return NULL;
}

bool Scheduler::removeTimer(string name)
{
    for (int i=0; i<timers_count;i++) {
        if (timers[i]->getPrefix()==name) {
            delete(timers[i]);
            for (int j=i+1;j<timers_count;j++) {
                timers[j-1]=timers[j];
                timers[j]=NULL;
            }
            timers=(TmTimer**)realloc(timers,sizeof(TmTimer*)*timers_count);
            return true;
        }
    }
    return false;
}
void Scheduler::removeAll()
{
    if (timers_count>0) {
        for (int i=0;i<timers_count;i++) { delete timers[i]; }
        free(timers);
        timers=NULL;
        timers_count=0;
    }
}
void
Scheduler::post(string time, string tmname, Repeat r, MarEvent* me)
{
    TmTimer* s = findTimer(tmname);
    if (s!=NULL) {
        if (me!=NULL) {
            // EvExpr supports querying of the scheduler environment in expressions
            EvExpr* e=dynamic_cast<EvExpr*>(me);
            if (e!=NULL) {
                MRSWARN("Scheduler::post(string time, string tmname, Repeat r, MarEvent* me) : setScheduler is not working yet");
                e->getExpression()->setScheduler(this);
            }
            s->post(time,r,me);
        }
        else MRSWARN("Scheduler::post(string,string,Repeat,MarEvent*)  NULL event");
    }
    else { MRSWARN("Scheduler::post(string,string,Repeat,MarEvent*)  unknown timer name: "+tmname); }
}
void
Scheduler::post(TmTime t, Repeat r, MarEvent* me)
{
    // pass the buck
    post(t.getTime(),t.getTimeName(),r,me);
}

void Scheduler::post(string event_time, Repeat rep, MarEvent* me) {
    if (timers[0]!=NULL) {
        post(event_time,timers[0]->getPrefix(),rep,me);
    }
}
void Scheduler::post(string event_time, MarEvent* me) {
    if (timers[0]!=NULL) {
        post(event_time,Repeat(),me);
    }
}
mrs_natural Scheduler::getTime(std::string timer) {
    TmTimer* s = findTimer(timer);
    if (s!=NULL) return s->getTime();
    MRSWARN("Scheduler::getTime(string)  unknown timer '"+timer+"'");
    return 0;
}

//ostream&
//Marsyas::operator<< (ostream& o, Scheduler& sys) {
////    sys.put(o);
//    return o;
//}

}//namespace Marsyas

