#include "SpeedMeter.h"

ContributeMeter& ContributeMeter::operator+=(size_B_i_t bytes) {
	invalid_=false;
	while (history_.size())
	{
		if (history_.front().timestamp+timeWindow_<NOW())
			history_.pop_front();
		else
			break;
	}
	if (timestart_<0)
		timestart_=NOW();
	history_.push_back(_pair(NOW(),bytes));
	totalBytes_+=bytes;
	return *this;
}

//获取速度(bytes/s)
band_Bps_t ContributeMeter::getSpeed() const
{
	if (invalid_)
	{
		return -1;
	}
	if(lastCalculateTime_+0.001>NOW())
		return lastCalculateSpeed_;
	else
	{
		lastCalculateTime_=NOW();
		while (history_.size())
		{
			if (history_.front().timestamp+timeWindow_<NOW())
				history_.pop_front();
			else
				break;
		}
		size_B_f_t bytes=0;
		for (iterator itr(history_.begin());itr!=history_.end();++itr)
			bytes+=itr->bytes;
		//time_s_t window=min(NOW()-timestart_,timeWindow_);
		//if (0.001>window)
		//	window=1;
		time_s_t window=timeWindow_;

		lastCalculateSpeed_=bytes/window;
		return lastCalculateSpeed_;
	}
}
size_B_i_t ContributeMeter::getCount() const
{
	if (invalid_)
	{
		return 0;
	}
	if(lastCalculateTime_+0.001>NOW())
		return lastCalculateSpeed_;
	else
	{
		lastCalculateTime_=NOW();
		while (history_.size())
		{
			if (history_.front().timestamp+timeWindow_<NOW())
				history_.pop_front();
			else
				break;
		}
		size_B_i_t bytes=0;
		for (iterator itr(history_.begin());itr!=history_.end();++itr)
			bytes+=itr->bytes;
		lastCalculateSpeed_=bytes;
		return lastCalculateSpeed_;
	}
}

//////////////////////////////////////////////////////////////////////////
//获取速度(bytes/s)
band_Bps_t AbilityMeter::getSpeed() const 
{
	for (iterator itr(history_.begin());itr!=history_.end();)
	{
		if (NOW()>itr->deadlineTime&&itr->stopTime<0)
		{
			lost_++;
			history_.erase(itr++);
		}
		else if (itr->startTime+timeWindow_<NOW())
			history_.erase(itr++);
		else ++itr;
	}

	band_Bps_t speed=0;
	for (iterator itr(history_.begin());itr!=history_.end();++itr)
		speed+=itr->bytes/(itr->stopTime-itr->startTime+0.000001);
	if (history_.size())
		speed/=history_.size();
	if (speed)
		lastSpeed_=speed;
	//cout<<request_<<"---------------------"<<lost_<<"---------------------"<<lastSpeed_<<endl;
	return lastSpeed_;
}

void AbilityMeter::start(id_t id,time_s_t t){
	for (iterator itr(history_.begin());itr!=history_.end();)
	{
		if (NOW()>itr->deadlineTime&&itr->stopTime<0)
		{
			lost_++;
			history_.erase(itr++);
		}
		else if (itr->startTime+timeWindow_<NOW())
			history_.erase(itr++);
		else ++itr;
	}
	_five tmp;
	tmp.id=id;
	tmp.startTime=NOW();
	tmp.stopTime=-1;
	tmp.bytes=0;
	tmp.deadlineTime=t+NOW();
	history_.push_back(tmp);
	++request_;
}

void AbilityMeter::stop(id_t id,size_B_i_t bytes)
{
	for (iterator itr(history_.begin());itr!=history_.end();++itr)
	{
		if (itr->id==id&&itr->stopTime==-1)
		{
			itr->bytes=bytes;
			itr->stopTime=NOW();
			break;
		}
	}
}