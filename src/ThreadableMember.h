/* 
 * $Id$
 *
 * This file is part of Gorgon.
 * Copyright (C) 2005-2006 Sanel Zukan.
 *
 * Gorgon is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Gorgon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __THREADABLEMEMBER_H__
#define __THREADABLEMEMBER_H__

#include <pthread.h>
#include <edelib/Debug.h>

template <typename Object, typename MemberFunc, typename Param>
class ThreadableMember {
	private:
		Object& instance;
		MemberFunc mem_fun;
		Param param;

		struct ThreadArgs {
			Object* object;
			MemberFunc mfunc;
			Param param;
			int*  is_running;

			ThreadArgs(Object& o, MemberFunc m, const Param& p, int* running) :
			object(&o), mfunc(m), param(p), is_running(running) { }
		};

		static void* ThreadFunc(void* p) {
			ThreadArgs *arg = static_cast<ThreadArgs*>(p);
			if(arg->is_running) 
				*(arg->is_running) = 1;	

			((arg->object)->*(arg->mfunc))(arg->param);

			// quickly modify external variable before object destruction
			if(arg->is_running) 
				*(arg->is_running) = 0;	

			delete arg;
			return (void*)0;
		}

	public:
		ThreadableMember(Object& o, MemberFunc m) : instance(o), mem_fun(m)
		{ }

		int Run(const Param& param, pthread_t& id, int* status) {
			ThreadArgs* args = new ThreadArgs(instance, mem_fun, param, status);

			int ret = pthread_create((pthread_t*)&id, 0, ThreadFunc, args);
			if(ret != 0) {
				delete args;
				return 0;
			}

			return 1;
		}
};

template <typename Object, typename MemberFunc, typename Param>
inline int RunInThread(Object& o, MemberFunc m, const Param& p, pthread_t& id, int* running = 0) {
	/*
	 * NOTE: if status is not NULL, it will be modified by started
	 * thread so make sure it is locked in case externaly is modified
	 * during thread execution. It is used to show is thread is still running.
	 */
	ThreadableMember<Object, MemberFunc, Param> tm(o, m);
	return tm.Run(p, id, running);
}

void StopThread(pthread_t& id) {
	if(id != 0) {
		pthread_cancel(id);
		pthread_join(id, NULL);
		id = 0;
	}
}

#endif // !__THREADABLEMEMBER_H__
