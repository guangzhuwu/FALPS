/********************************************************************
  文件:   d:\Simulator\FEPeerSim\FEPeerSim\Packet.h
  日期: 18:1:2007   13:58
  作者: 武广柱
  说明: Packet

  Copyright (C) 2007 - All Rights Reserved
*********************************************************************/
#ifndef _PACKET_H_
#define _PACKET_H_

#include "Utility.h"
#include "MemPool.h"
#include <iostream>
#include <cassert>
using namespace std;

class Data : public ReuseBase
{
public:
	Data()
	{
	};
	virtual ~Data()
	{
	};
};
class Packet : public ReuseBase
{
	friend class Node;
public:
	Packet();
	static Packet* newPacket(int kind, Data* data = NULL);
	size_B_i_t length()
	{
		return nbytes_;
	}
	void length(size_B_i_t nbyte);
	size_B_i_t lengthWithOverhead()
	{
		return nbytes_with_overhead_;
	}
	
	static size_B_i_t lengthWithOverhead(size_B_i_t len)
	{
		size_t fragmentNum= max(len/(size_B_i_t) MSS, (size_B_i_t)1);
		size_t lastFragmentSize_=len%MSS+PROTOCOL_OVERHEAD;
		return (fragmentNum-1)*FRAGMENT_LEN+lastFragmentSize_;
	}

	Data*& data()
	{
		return data_;
	}
	pkt_id_t id()
	{
		return id_;
	}
	addr_t& srcAddr()
	{
		return source_;
	}
	addr_t& dstAddr()
	{
		return dest_;
	}
	size_t& fragmentCount()
	{
		return fragment_num_;
	}
	size_B_i_t fragmentBytes(size_t frameID)
	{
		assert(frameID<fragment_size_);
		if (frameID==(fragment_size_-1))
			return last_fragment_size_;
		else
			return fragment_size_;
	}
	size_t& fragmentID()
	{
		return fragment_id_;
	}
	int& type()
	{
		return kind_;
	}
	~Packet();
	bool isSelfMsg()
	{
		return is_self_msg_;
	}
	void setSelfMsg()
	{
		is_self_msg_ = true;
	}

protected:
	Data* data_;
	size_B_i_t nbytes_;
	addr_t source_;
	addr_t dest_;
	size_t fragment_id_;
	size_t fragment_num_;
	int kind_;
	size_B_i_t fragment_size_;
	size_B_i_t last_fragment_size_;
	size_B_i_t nbytes_with_overhead_;
	bool is_self_msg_;
private:
	static pkt_id_t id_seed_;
	pkt_id_t id_;

};

inline Packet* Packet::newPacket(int kind, Data* data)
{
	Packet* pkt = new Packet;
	pkt->data_ = data;
	pkt->kind_ = kind;
	return pkt;
}

inline  void Packet::length(size_B_i_t nbyte)
{
	nbytes_ = nbyte;
	fragment_num_ = max(nbytes_ / (size_B_i_t) MSS, (size_B_i_t) 1);
	fragment_id_ = 0;
	fragment_size_ = (fragment_num_ == 1) ?nbytes_ +PROTOCOL_OVERHEAD :FRAGMENT_LEN;
	last_fragment_size_=nbytes_%MSS+PROTOCOL_OVERHEAD;
	nbytes_with_overhead_=(fragment_num_-1)*FRAGMENT_LEN+last_fragment_size_;
}

#endif//_PACKET_H_