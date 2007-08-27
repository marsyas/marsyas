/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
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
\class MarControlAccessor
\brief MarControlAccessor
\author Luis F. Teixeira
\adapted from MarsyasX to Marsyas0.2 by Luis Gustavo Martins - 24.08.2007
\date 21/07/2007
*/

#ifndef __MARCONTROL__
#error Do not include this file directly, include only MarControl.h
#endif

namespace Marsyas
{
	class MarControlAccessor
	{
	private:
		MarControlPtr ctrl_;
		bool update_;
		bool readOnlyAccess_;

	public:
		MarControlAccessor(MarControlPtr ctrl, bool update = true, bool readOnlyAccess = false)
		{
			ctrl_ = ctrl;
			update_ = update;
			readOnlyAccess_ = readOnlyAccess;
			
			#ifdef MARSYAS_QT
			if(readOnlyAccess_)
				rwLock_.lockForRead(); //more efficient: allows multiple readers
			else
				rwLock_.lockForWrite();//only a single writer/reader
			#endif
		}

		~MarControlAccessor()
		{
			#ifdef MARSYAS_QT
			rwLock_.unlock(); 
			#endif

			if(update_)
				ctrl_->callMarSystemUpdate();
		}

		void enableUpdates() {update_ = true;};
		void disableUpdates(){update_ = false;};

		template<class T> T& to()
		{
			if(readOnlyAccess_)
				return const_cast<T&>(ctrl_->to<T>());
			else //this means we have an active write lock...
			{
				#ifdef MARSYAS_QT
				rwLock_.unlock(); //unlock write mutex so to() does not deadlock
				#endif
				
				T& res = const_cast<T&>(ctrl_->to<T>());
				
				#ifdef MARSYAS_QT
				rwLock_.lockForWrite(); //lock again
				#endif
				
				return res; 
			}
		}
	};
}

