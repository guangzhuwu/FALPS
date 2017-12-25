#include "Scheduler.h"

id_t Scheduler::uid_seed_ = 0;

void Scheduler::run()
{
	//static bool runflag = false;
	//if (runflag)
	//	return;
	//runflag = true;

	//while (true)
	//{
	//	// let others run
	//	while (anyready())
	//		yield();

	//	// time is going to move forward.

	//	// everyone else is quiet.
	//	// must be time for the next event.
	//	// run events for next time in the queue
	//	if (!advance())
	//		break;
	//}

	Event* p;
	while (!halted_ && (p = frontEvent()) != NULL)
	{
		dispatch(p, p->time_key_.time_);
		if (p->need_delete_)
			delete p->handler_;
		pop_front();
	}
}
//bool Scheduler::advance()
//{
//
//
//	// Remove the events for the current time and advance the
//	// time *before* executing the events, so that the events
//	// can correctly call now() and add_event().
//
//	Event* eqe=frontEvent();
//	if (eqe== NULL)
//	{
//		cout<<"queue empty"<<endl;
//		return false;
//	}
//	pop_front();
//    dispatch(eqe);
//	if (frontEvent()== NULL)
//	{
//		cout<<"queue empty"<<endl;
//		return false;
//	}
//	return true;
//}
