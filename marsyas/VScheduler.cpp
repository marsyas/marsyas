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

*/

#include "VScheduler.h"

using namespace std;
//using namespace Marsyas;

namespace Marsyas //lmartins: hack: should work without this [?][!]
{

VScheduler::VScheduler() {
    schedulers=NULL;
    schedulers_count=0;
}
VScheduler::~VScheduler() { removeAll(); }
void VScheduler::tick() {
  for (int i=0;i<schedulers_count;i++) { schedulers[i]->tick(); }
}
bool VScheduler::eventPending() {
  for (int i=0;i<schedulers_count;i++) {
    if (schedulers[i]->eventPending()) { return true; }
  }
  return false;
}

void VScheduler::addTimer(TmTimer* t) {
    // look for schedulers with same name to ensure only one of each name
    for (int i=0;i<schedulers_count;i++) {
        if (schedulers[i]->getTimerName()==t->getName()) { return; }
    }
    schedulers = (Scheduler**)realloc(schedulers,sizeof(Scheduler*)*(schedulers_count+1));
    schedulers[schedulers_count] = new Scheduler(t);
    schedulers_count = schedulers_count + 1;
}

bool VScheduler::removeTimer(string name) {
    for (int i=0; i<schedulers_count;i++) {
        if (schedulers[i]->getTimerName()==name) {
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
void VScheduler::removeAll() {
  if (schedulers_count>0) {
    for (int i=0;i<schedulers_count;i++) { delete schedulers[i]; }
    free(schedulers);
  }
}
void
VScheduler::post(string time, string tmname, Repeat r, MarEvent* me)
{
    for (int i=0;i<schedulers_count;i++) {
        if (schedulers[i]->getTimerName()==tmname) {
            schedulers[i]->post(time,r,me);
            return;
        }
    }
}
void
VScheduler::post(TmTime t, Repeat r, MarEvent* me)
{
    // pass the buck
    post(t.getTime(),t.getTimeName(),r,me);
}

void VScheduler::post(string event_time, Repeat rep, MarEvent* me) {
    if (schedulers[0]!=NULL) {
        schedulers[0]->post(event_time,rep,me);
    }
}
void VScheduler::post(string event_time, MarEvent* me) {
    if (schedulers[0]!=NULL) {
        schedulers[0]->post(event_time,me);
    }
}
void VScheduler::post(ScheduledEvent* e) {
    if (schedulers[0]!=NULL) {
        schedulers[0]->post(e);
    }
}

//ostream&
//Marsyas::operator<< (ostream& o, Scheduler& sys) {
////    sys.put(o);
//    return o;
//}

}//namespace Marsyas

