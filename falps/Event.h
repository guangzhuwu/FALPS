/********************************************************************
  文件:   d:\Simulator\FEPeerSim\FEPeerSim\Event.h
  日期: 18:1:2007   13:57
  作者: 武广柱
  说明: 

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
	time_s_t time_;//时间（millisecond）
	long long uid_;//唯一ID
};
const EventTimeKey EVENTTIMEKEY_NULL ={0, 0};//NULL事件
const EventTimeKey EVENTTIMEKEY_INVALID ={-1, -1};//非法事件

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
	bool need_delete_;//执行后是否需要删除操作（是否是new出的）
};

//比较排序用,为什么不直接用函数而用仿函数？map排序操作的第三个参试就是仿函数！
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