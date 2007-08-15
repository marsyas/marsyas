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

#include "SchedulerManager.h"
#include "TimerFactory.h"
/*
//#include "TmTimer.h"
#include "TmGetTime.h"
#include "TmSampleCount.h"
#include "TmKbdTap.h"
#include "TmMIDITap.h"
#include "TmPeakTap.h"
*/
using namespace std;
using namespace Marsyas;

SchedulerManager::SchedulerManager()
{
    name_ = "SchedulerManager";
    type_ = "SchedulerManager";
}

SchedulerManager::~SchedulerManager()
{
}







/*
TmTimer*
SchedulerManager::makeTimer(std::string class_name, std::string identifier)
{
    if (class_name=="TmGetTime") { return new TmGetTime(identifier); }
    if (class_name=="TmSampleCount") { return new TmSampleCount(identifier); }
    if (class_name=="TmKbdTap") { return new TmKbdTap(identifier); }
    if (class_name=="TmMIDITap") { return new TmMIDITap(identifier); }
    if (class_name=="TmPeakTap") { return new TmPeakTap(identifier); }
    MRSWARN("SchedulerManager::makeTimer(string,string)  Unknown timer name requested");
    return NULL;
}
*/
TmTimer*
SchedulerManager::makeTimer(std::string class_name, std::string identifier)
{
    TimerFactory* f = TimerFactory::getInstance();
    return f->make(class_name,identifier);
/*
    if (class_name=="TmGetTime") { return new TmGetTime(identifier); }
    if (class_name=="TmSampleCount") { return new TmSampleCount(identifier); }
    if (class_name=="TmKbdTap") { return new TmKbdTap(identifier); }
    if (class_name=="TmMIDITap") { return new TmMIDITap(identifier); }
    if (class_name=="TmPeakTap") { return new TmPeakTap(identifier); }
    MRSWARN("SchedulerManager::makeTimer(string,string)  Unknown timer name requested");
    return NULL;
*/
}

