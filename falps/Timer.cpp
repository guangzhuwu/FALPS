#include "Timer.h"



//≤‚ ‘TimerHandler EventQue Scheduler
#if 0
class coutTime:public TimerHandler
{
protected:
  virtual void expire(Event *)
  {
	cout<<NOW()<<endl;  
	if (NOW()>=100)
	{
	  force_cancel();
	}
	else
	{
	  this->resched(0.1);
	  this->resched(0.1);
	}


  }
};
void main()
{   
  Scheduler::createEventQue("EventQueMap");
  coutTime t;
  t.resched(0);
  t.resched(0);
  Scheduler::instance()->run();
  system("pause");
}
#endif