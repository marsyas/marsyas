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

/**
   \class Scheduler
   \brief Scheduler schedules things
   \author inb@cs.uvic.ca
*/

#include "VScheduler.h"
#include "EvExpr.h"

using namespace std;
//using namespace Marsyas;

namespace Marsyas //lmartins: hack: should work without this [?][!]
{

VScheduler::VScheduler()
{
    schedulers=NULL;
    schedulers_count=0;
}
VScheduler::~VScheduler() { removeAll(); }
void VScheduler::tick()
{
    for (int i=0;i<schedulers_count;i++) { schedulers[i]->tick(); }
}
bool VScheduler::eventPending()
{
    for (int i=0;i<schedulers_count;i++) {
        if (schedulers[i]->eventPending()) { return true; }
    }
    return false;
}

void VScheduler::addTimer(TmTimer* t) {
    if (t == NULL) return;
    // look for schedulers with same name to ensure only one of each name
    if (findScheduler(t->getPrefix())!=NULL)
        MRSWARN("VScheduler::addTimer(TmTimer)  refusing to add timer with name already in use");
    appendScheduler(new Scheduler(t));
}
void
VScheduler::addTimer(string class_name, string given_name)
{
    // look for schedulers with same name to ensure only one of each name
    if (findScheduler(class_name+"/"+given_name)!=NULL)
        MRSWARN("VScheduler::addTimer(string,string)  refusing to add timer with name already in use");
    appendScheduler(new Scheduler(class_name,given_name));
}

void
VScheduler::split_cname(std::string cname, std::string* head, std::string* tail)
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
VScheduler::updtimer(std::string cname, TmControlValue value)
{
    string timer_ident="";
    string timer_control="";
    split_cname(cname,&timer_ident,&timer_control);
    Scheduler* s = findScheduler(timer_ident);
    if (s==NULL) {
        MRSWARN("VScheduler::updtimer(std::string,TmControlValue)  no timer: "+timer_ident);
    } else {
        s->updtimer(timer_control,value);
    }
}

void
VScheduler::appendScheduler(Scheduler* s)
{
    schedulers = (Scheduler**)realloc(schedulers,sizeof(Scheduler*)*(schedulers_count+1));
    schedulers[schedulers_count] = s;
    schedulers_count = schedulers_count + 1;
}

Scheduler*
VScheduler::findScheduler(std::string name)
{
    for (int i=0;i<schedulers_count;i++) {
        Scheduler* s = schedulers[i];
        if (s->getPrefix()==name) {
            return s;
        }
    }
    return NULL;
}

bool VScheduler::removeTimer(string name)
{
    for (int i=0; i<schedulers_count;i++) {
        if (schedulers[i]->getPrefix()==name) {
            delete(schedulers[i]);
            for (int j=i+1;j<schedulers_count;j++) {
                schedulers[j-1]=schedulers[j];
                schedulers[j]=NULL;
            }
            schedulers=(Scheduler**)realloc(schedulers,sizeof(Scheduler*)*schedulers_count);
            return true;
        }
    }
    return false;
}
void VScheduler::removeAll()
{
    if (schedulers_count>0) {
        for (int i=0;i<schedulers_count;i++) { delete schedulers[i]; }
        free(schedulers);
        schedulers=NULL;
        schedulers_count=0;
    }
}
void
VScheduler::post(string time, string tmname, Repeat r, MarEvent* me)
{
    Scheduler* s = findScheduler(tmname);
    if (s!=NULL) {
        if (me!=NULL) {
            // EvExpr supports querying of the scheduler environment in expressions
            EvExpr* e=dynamic_cast<EvExpr*>(me);
            if (e!=NULL) { e->getExpression()->setVScheduler(this); }
            s->post(time,r,me);
        }
        else MRSWARN("VScheduler::post(string,string,Repeat,MarEvent*)  NULL event");
    }
    else { MRSWARN("VScheduler::post(string,string,Repeat,MarEvent*)  unknown timer name: "+tmname); }
}
void
VScheduler::post(TmTime t, Repeat r, MarEvent* me)
{
    // pass the buck
    post(t.getTime(),t.getTimeName(),r,me);
}

void VScheduler::post(string event_time, Repeat rep, MarEvent* me) {
    if (schedulers[0]!=NULL) {
        post(event_time,schedulers[0]->getPrefix(),rep,me);
    }
}
void VScheduler::post(string event_time, MarEvent* me) {
    if (schedulers[0]!=NULL) {
        post(event_time,Repeat(),me);
    }
}
mrs_natural VScheduler::getTime(std::string timer) {
    Scheduler* s = findScheduler(timer);
    if (s!=NULL) return s->getTime();
    MRSWARN("VScheduler::getTime(string)  unknown timer '"+timer+"'");
    return 0;
}

//ostream&
//Marsyas::operator<< (ostream& o, Scheduler& sys) {
////    sys.put(o);
//    return o;
//}

}//namespace Marsyas

