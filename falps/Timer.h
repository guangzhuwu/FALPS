/********************************************************************
  文件:   d:\MyProjects\FEPeerSim\FEPeerSim\Timer.h 
  日期: 2006/11/01
  作者: 武广柱
  
  说明: 定时器
*********************************************************************/
#ifndef _TIMER_H_
#define _TIMER_H_
#include "Event.h"
#include "Handler.h"
#include "Scheduler.h"

class TimerHandler : public Handler
{
public:
	enum TimerStatus
	{
		TIMER_IDLE,
		TIMER_PENDING,
		TIMER_HANDLING
	};

	TimerHandler() : status_(TIMER_IDLE)
	{
	}
	void sched(time_s_t delay); 
	void resched(time_s_t delay); 
	void cancel();    
	void forceCancel();
	int status()
	{
		return status_;
	}
	bool isSched()
	{
		return status_ != TIMER_IDLE;
	}

protected:
	virtual void expire(Event*) = 0;
private: 
	virtual void handle(Event*);
	void _sched(time_s_t delay);
	void _cancel();
private:
	int status_;
	Event event_;
};

//一次性的事件定时，插入就不撤销，此时的Handler是new的，执行后会自动删除
//一般用户除非比要不要使用
class TimerOnceHandler : public Handler, public ReuseBase
{
public:
	TimerOnceHandler()
	{
	}
	void sched(time_s_t delay);
	void cancel(){Scheduler::instance().cancel(&event_);}
	virtual void expire(Event*)
	{
	}
private:
	virtual void handle(Event*);
private:
	Event event_;
	TimerOnceHandler* ptr_;
};

//////////////////////////////////////////////////////////////////////////
inline void TimerOnceHandler::handle(Event* e)
{
	expire(e);
}

inline void TimerOnceHandler::sched(time_s_t delay)
{
	this->event_.need_delete_ = true;
	Scheduler::instance().schedule(this, &(this->event_), delay);
};  

//////////////////////////////////////////////////////////////////////////
inline void TimerHandler::sched(time_s_t delay)
{
	if (status_ != TIMER_IDLE)
	{
		fprintf(stderr, "Couldn't schedule timer");
		abort();
	}
	_sched(delay);
	status_ = TIMER_PENDING;
}

inline void TimerHandler::cancel()
{
	if (status_ != TIMER_PENDING)
	{
		cout << "Attempting to cancel timer which is not scheduled\n",
		abort();
	}
	_cancel();
	status_ = TIMER_IDLE;
}

inline void TimerHandler::forceCancel()
{
	if (status_ == TIMER_PENDING)
	{
		_cancel();
		status_ = TIMER_IDLE;
	}
}

inline void TimerHandler::resched(time_s_t delay)
{
	if (status_ == TIMER_PENDING)
		_cancel();
	_sched(delay);
	status_ = TIMER_PENDING;
}

inline void TimerHandler::handle(Event* e)
{
	if (status_ != TIMER_PENDING)   // sanity check
		abort();
	status_ = TIMER_HANDLING;
	expire(e);
	// if it wasn't rescheduled, it's done
	if (status_ == TIMER_HANDLING)
		status_ = TIMER_IDLE;
}

inline void TimerHandler::_sched(time_s_t delay)
{
	Scheduler::instance().schedule(this, &event_, delay);
}
inline  void TimerHandler::_cancel()
{
	Scheduler::instance().cancel(&event_);
}

#endif //_TIMER_H_