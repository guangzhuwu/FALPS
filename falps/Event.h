/********************************************************************
  �ļ�:   d:\Simulator\FEPeerSim\FEPeerSim\Event.h
  ����: 18:1:2007   13:57
  ����: �����
  ˵��: 

  Copyright (C) 2007 - All Rights Reserved
*********************************************************************/
#ifndef _EVENT_H_
#define _EVENT_H_
#include "Utility.h"
#include "Handler.h"
#include "MemPool.h"
#include <cassert>
#include <iostream>
#include <string>
using namespace std;
class Event;

struct EventTimeKey
{
	time_s_t time_;//ʱ�䣨millisecond��
	long long uid_;//ΨһID
};
const EventTimeKey EVENTTIMEKEY_NULL ={0, 0};//NULL�¼�
const EventTimeKey EVENTTIMEKEY_INVALID ={-1, -1};//�Ƿ��¼�

inline bool ValidEventTimeKey(const EventTimeKey& key){return key.uid_ != -1;
}

class Event : public ReuseBase, public NonCopyble
{
public:
	Event()
	{
		handler_ = 0;
		need_delete_ = false;
		time_key_ = EVENTTIMEKEY_INVALID;
	}
	EventTimeKey time_key_;
	Handler* handler_;
	bool need_delete_;//ִ�к��Ƿ���Ҫɾ���������Ƿ���new���ģ�
};

//�Ƚ�������,Ϊʲô��ֱ���ú������÷º�����map��������ĵ��������Ծ��Ƿº�����
class EventTimeKeyCompareLess
{
public:
	bool EventTimeKeyCompareLess::operator ()(const EventTimeKey& a,
		const EventTimeKey& b)const
	{
		if (a.time_ < b.time_)
		{
			return true;
		}
		else if (a.time_ > b.time_)
		{
			return false;
		}
		else
			return a.uid_ < b.uid_;
	}
};

#endif//_EVENT_H_