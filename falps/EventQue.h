/********************************************************************
  �ļ�:   d:\MyProjects\FEPeerSim\FEPeerSim\EventQue-map.h  
  ����: 2006/11/01
  ����: �����
  
  ˵��: Event Queue,������stl��map�����Ӷ�O(log(n))
*********************************************************************/
#ifndef _EVENTQUE_MAP_H_
#define _EVENTQUE_MAP_H_
#include "Event.h"
#include <map>
using namespace std;


//��std::map����ʵ����һ��EventQue
class EventQue
{
	friend class Scheduler;
public:
	~EventQue()
	{
	};

	void insert(Event* event);
	//�¼������Ƿ�Ϊ��
	bool isEmpty();
	//��ȡ������ʱ��������¼�
	Event* frontEvent();
	//����(ɾ��)������ʱ��������¼�
	void pop_front() ;
	//��ȡ������ʱ��������¼���key
	EventTimeKey frontKey();
	//�Ӷ�����ɾ��ĳ�¼�
	void remove(Event* ev);
	Event* find(EventTimeKey timeId);
	void clear()
	{
		event_map_.clear();
	}
	static EventQue& instance()
	{
		static EventQue instance_;
		return instance_;
	}
protected:
	EventQue()
	{
	};
protected:
	typedef map<EventTimeKey, Event*, EventTimeKeyCompareLess> EventMap_T;
	typedef EventMap_T::iterator Iterator;
	typedef EventMap_T::const_iterator Const_Iterator;
protected:
	EventMap_T event_map_;
};

inline void EventQue::insert(Event* event)
{
	std::pair<EventQue::Iterator, bool> result = event_map_.insert(std::make_pair(event->time_key_,
																	event));
	FALPS_ASSERT(result.second == true,
		cout << "EventQueMap::insert error!" << endl);
}

inline bool EventQue::isEmpty(void)
{
	return event_map_.empty();
}

inline Event* EventQue::frontEvent()
{
	if (isEmpty())
		return NULL;
	return event_map_.begin()->second;
}
inline EventTimeKey EventQue::frontKey()
{
	if (isEmpty())
		return EVENTTIMEKEY_NULL;
	return event_map_.begin()->first;
}
inline void EventQue::pop_front(void)
{
	event_map_.erase(event_map_.begin());
}
inline void EventQue::remove(Event* ev)
{
	event_map_.erase(ev->time_key_);
}

inline Event* EventQue::find(EventTimeKey timeId)
{
	Iterator itr = event_map_.find(timeId);
	if (itr != event_map_.end())
		return itr->second;
	return  NULL;
}

#endif//_EVENTQUE_MAP_H_