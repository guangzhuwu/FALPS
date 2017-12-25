#ifndef __SPEEDMETER_H__
#define __SPEEDMETER_H__

#include "Falps.h"
#include "ParamDef.h"
#include <deque>
#include <list>
using namespace std;

//#########################################################################
//ContributeMeter
//���������ã���һ��������resolutionʱ�䴰���ڵ��ٶȹ��㡣
//��ӳ�����resolutinʱ���ڣ������ڵ�Ա��ڵ�Ĺ��׻򱾽ڵ�����Ĺ��ס�
//��ˣ����ٶȲ���Ϊ�����ײ�����
class ContributeMeter
{
	struct _pair 
	{
		time_s_t timestamp;
		size_B_i_t bytes;
		_pair(time_s_t t,size_B_i_t b):timestamp(t),bytes(b){}
	};
	typedef std::deque<_pair> history_type;
	typedef history_type::iterator iterator;
public:

	ContributeMeter(time_s_t timewindow=4)
		:timeWindow_(timewindow)
		,timestart_(-1)
		,totalBytes_(0)
		,lastCalculateTime_(INT_MIN)
		,invalid_(true)
	{
	}

	//��ȡ�ٶ�(bytes/s)
	band_Bps_t getSpeed() const ;
	size_B_i_t getCount() const ;

	size_B_i_t getTotalContribution() const { return totalBytes_;}
	ContributeMeter& operator+=(size_B_i_t bytes);

private:
	mutable history_type   history_;   
	mutable time_s_t       timeWindow_;               
	mutable size_B_i_t     totalBytes_;
	mutable time_s_t       timestart_;
	mutable time_s_t       lastCalculateTime_;
	mutable band_Bps_t     lastCalculateSpeed_;
	mutable bool           invalid_;
};

inline std::ostream& operator<<(std::ostream &o, const ContributeMeter &speed) {
	return o << speed.getSpeed();
}

//#########################################################################
//AbilityMeter
//�������������������ݴ�С�ʹӷ��������յ���������ʱ��ı�ֵ
//Ҳͳ���˶����ʣ����߾ܾ������ʣ�
class AbilityMeter
{
	struct _five 
	{
		id_t id;
		time_s_t startTime;
		time_s_t stopTime;
		size_B_i_t bytes;
		time_s_t deadlineTime;
	};
	typedef std::list<_five> history_type;
	typedef history_type::iterator iterator;
public:
	AbilityMeter(time_s_t timewindow=10)
		:timeWindow_(timewindow)
		,timestart_(-1)
		,lastSpeed_(MEDIA_BPS)
		,lost_(0)
		,request_(0)
	{
	}

	//��ȡ�ٶ�(bytes/s)
	band_Bps_t getSpeed() const ;
	size_t getTotalLost() const { return lost_;}
	size_t getTotalRequest()const{ return request_;}
	void start(id_t id,time_s_t deadlineTime);
	void stop(id_t id,size_B_i_t bytes);
private:
	mutable history_type history_;   
	mutable time_s_t     timeWindow_;               
	mutable time_s_t     timestart_;
	mutable size_t       lost_;
	mutable size_t       request_;
	mutable band_Bps_t   lastSpeed_;
};

inline std::ostream& operator<<(std::ostream &o, const AbilityMeter &speed) {
	return o << speed.getSpeed();
}

#endif
