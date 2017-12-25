/********************************************************************
  文件:   d:\MyProjects\FEPeerSim\FEPeerSim\Scheduler.h 
  日期: 2006/11/10
  作者: 武广柱 wuguangzhu@gmail.com
  
  说明: Scheduler
*********************************************************************/
#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_
#include "EventQue.h"
#include "Utility.h"
#include <string>
#include <iostream>
using namespace std;

class Scheduler
{
public:
	void run();
	void schedule(Handler* h, Event* e, double delay);
	void cancel(Event* p);
	void insert(Event* p);
	Event* lookup(EventTimeKey timeKey);
	Event* frontEvent();
	void pop_front();
	double clock() const;
	void reset();
	static Scheduler& instance();
	virtual ~Scheduler()
	{
	}

protected:
	void dispatch(Event* p);
	void dispatch(Event* p, double t);

protected:
	Scheduler() : clock_(0.0), halted_(false)
	{
	}

protected:
	time_s_t clock_;
	bool halted_;
	static id_t uid_seed_;
};

inline time_s_t NOW()
{
	return Scheduler::instance().clock();
}

inline void StartSimulation()
{
	Scheduler::instance().run();
}

inline  void Scheduler::schedule(Handler* h, Event* e, double delay)
{
	FALPS_ASSERT(h != 0,
		cout << "Scheduler: attempt to schedule an event with a NULL handler"
					<< endl);
	FALPS_ASSERT(delay >= 0.0,
		cout << "Scheduler::schedule: scheduling event with negative delay"
					<< endl);
	FALPS_ASSERT(uid_seed_ >= 0,
		cout << "Scheduler: UID space exhausted!" << endl);

	e->time_key_.uid_ = uid_seed_++;
	e->handler_ = h;
	e->time_key_.time_ = clock_ + delay;
	insert(e);
}

inline void Scheduler::cancel(Event* p)
{
	EventQue::instance().remove(p);
}

inline void Scheduler::insert(Event* p)
{
	EventQue::instance().insert(p);
}

inline Event* Scheduler::lookup(EventTimeKey timeKey)
{
	return EventQue::instance().find(timeKey);
}

inline Event* Scheduler::frontEvent()
{
	return EventQue::instance().frontEvent();
}

inline void Scheduler::pop_front()
{
	return EventQue::instance().pop_front();
}

inline double Scheduler::clock() const
{
	return (clock_);
}

inline void Scheduler::reset()
{
	EventQue::instance().clear();clock_ = 0;halted_ = false;uid_seed_ = 0;
}

inline Scheduler& Scheduler::instance()
{
	static Scheduler instance_;
	return instance_;
}

inline void Scheduler::dispatch(Event* p)
{
	dispatch(p, p->time_key_.time_);
} 

inline void Scheduler::dispatch(Event* p, double t)
{
	FALPS_ASSERT(t >= clock_,
		cout << "scheduler going backwards in time from " << clock_ << " to "
					<< t << "\n");
	clock_ = t;
	p->time_key_.uid_ = -p->time_key_.uid_; 
	p->handler_->handle(p);
	//Event::Execute(p);
} 

#endif /* _SCHEDULER_H_ */
