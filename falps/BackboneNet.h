/********************************************************************
  文件:   d:\Simulator\FEPeerSim\FEPeerSim\BackboneNet.h
  日期: 18:1:2007   13:56
  作者: 武广柱
  说明: 对骨干网的抽象。这里假设骨干网的带宽无限大，而仅仅考虑
	  传播延迟。也就是对于一个数据包来说，transmitionDelay=0； 
	  delay=transmitionDelay+propagationDelay=propagationDelay

  Copyright (C) 2007 - All Rights Reserved
*********************************************************************/
#ifndef _OMP2PSIM_BACKBONENET_
#define _OMP2PSIM_BACKBONENET_
#include "Utility.h"
#include "Packet.h"
#include "Timer.h"
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <deque>
#include <queue>
using namespace std;

class BackboneNet : public NonCopyble
{
public:
	//***********************************************************************
	// 功能:  根据节点数和路由器数等初始化BockboneNet
	// 输入:  (节点数，路由器数，路由器间的平均延迟，节点与路由器间平均延迟)
	// 输出:  void
	//***********************************************************************
	void initialize(int nodeCount, int routerCount, time_s_t routeDelay,
		time_s_t nodeDelay);

	//***********************************************************************
	// 功能:  获取节点到骨干网的带宽，也就是节点的上行带宽  
	// 输入:  节点地址  
	// 输出:  带宽，Byte/s  
	//***********************************************************************
	band_Bps_t getNodeToBackboneBw(addr_t addr);

	//***********************************************************************
	// 功能:  获取骨干网到节点的带宽，也就是节点的下行带宽  
	// 输入:  节点地址  
	// 输出:  带宽，Byte/s  
	//***********************************************************************
	band_Bps_t getBackboneToNodeBw(addr_t addr);

	//***********************************************************************
	// 功能:  获取节点到骨干网的延迟  
	// 输入:  节点地址 
	// 输出:  延迟，s  
	//***********************************************************************
	time_s_t getDelay(addr_t addr);


	//***********************************************************************
	// 功能:  获取两节点之间的网络延迟  
	// 输入:  两节点地址 
	// 输出:  延迟，s  
	//***********************************************************************
	time_s_t getDelay(addr_t addr1, addr_t addr2);


	//***********************************************************************
	// 功能:  骨干网收到节点发来的数据包，recv将计算何时本包被送到接收方路由
	//  	  器队列  
	// 输入:  数据packet  
	// 输出:  void  
	//***********************************************************************
	void recv(Packet* pkt); 

	//***********************************************************************
	// 功能:  recv一个数据包后，经过延迟本包送到了接收方路由器。该函数处理何  
	//  	  该包将被从路由器发送队列发送出去
	// 输入:  数据包  
	// 输出:  void  
	//***********************************************************************
	void transPkt(Packet* pkt); 

	//***********************************************************************
	// 功能:  获取两个节点所在的路由器在骨干网络中的延迟  
	// 输入:  (节点地址，节点地址)  
	// 输出:  延迟,s  
	//***********************************************************************
	time_s_t getBackBoneDelay(addr_t n1, addr_t n2);

	//***********************************************************************
	// 功能:  数据包到达目的节点所在路由器后，数据包将被放到输出队列，本函数
	//  	  处理输出队列  
	// 输入:  目的节点地址  
	// 输出:  void 
	//***********************************************************************
	void processOutGate(addr_t pktDst);

	//***********************************************************************
	// 功能:  设置节点的上行带宽
	// 输入:  addr:节点地址；bw:带宽
	// 输出:  void
	//***********************************************************************
	void setUpBand(addr_t addr, rate_Bps_t bw);

	//***********************************************************************
	// 功能:  设置节点的下行带宽
	// 输入:  addr:节点地址；bw:带宽
	// 输出:  void
	//***********************************************************************
	void setDownBand(addr_t addr, rate_Bps_t bw);

	//***********************************************************************
	// 功能:  设置节点带宽，包括下行带宽和上行带宽
	// 输入:  addr:节点地址；bw:带宽
	// 输出:  void
	//***********************************************************************
	void setBand(addr_t addr, rate_Bps_t bw);

	//***********************************************************************
	// 功能:  打印节点消息
	// 输入:  n:打印0-n个节点信息,-1表示全部打印
	// 输出:  void
	//***********************************************************************
	void printNetInfo(int n = -1);


public:
	static BackboneNet& instance()
	{
		static BackboneNet instance_;
		return instance_;
	}
protected:   
	//***********************************************************************
	// 功能:  初始化网络拓扑  
	// 输入:  void  
	// 输出:  void  
	//***********************************************************************
	void initToplogy();

	//***********************************************************************
	// 功能:  获取两个节点所在的路由器在骨干网络中的延迟,保护成员函数  
	// 输入:  (节点地址，节点地址)  
	// 输出:  延迟,s  
	//***********************************************************************
	time_s_t getBackBoneDelay_(addr_t n1, addr_t n2);

protected:   
	BackboneNet()
	{
	}
	~BackboneNet()
	{
	}

protected: 
	size_t nodeCount_;//仿真节点个数
	size_t routerCount_;//骨干网中路由节点数,用来模拟节点间的延迟

	time_s_t meanDelayBetweenRouter_;//路由器间的平均延迟（s）
	time_s_t meanDelayBetweenNodeBackbone_;//node到backbonenet间的平均延迟
	vector<addr_t> nodeBelongToWitchRouterVec_;//node属哪个路由器，模拟延迟
	vector<time_s_t> routerDelayVec_;//用于计算路由器间的delay  
	//如果节点数小于MATRIX_SIZE就用matrix存储节点间的delay,从而获得速度。
	//否则因内存消耗太大，采用临时计算的方式
	enum
	{
		MATRIX_SIZE = 1024 * 2
	};
	vector<vector<time_s_t> > delayMatrix_;

	vector<band_Bps_t> link_in_;//节点到骨干网的带宽
	vector<band_Bps_t> link_out_;//骨干网到节点的带宽
	vector<time_s_t> link_delay_;//骨干网和节点间的延迟

	struct OutGatePendingPkt
	{
		time_s_t leave_outQue_time_;//本pending包何时将被从发送队列中发送出去
		Packet* pkt_;//要发送的数据包指针
	};
	vector<queue<OutGatePendingPkt> > outGate_que_;//和节点链接的路由器的发送队列
	vector<time_s_t> last_leave_outGate_time_;//队列中最后一个包何时(要/已)被发送出去
	vector<bool> is_outGate_sched_;//发送队列是否对队首包做了sched（将引发目的节点recv

	struct InGatePendingPkt
	{
		time_s_t leave_inQue_time_;
		Packet* pkt_;//要发送的数据包指针
	};
	vector<queue<InGatePendingPkt> > inGate_que_;
	vector<time_s_t> last_leave_inQue_time_;
	vector<bool> is_inGate_sched_;
};

inline band_Bps_t BackboneNet::getNodeToBackboneBw(addr_t addr)
{
	return link_in_[addr];
}

inline  band_Bps_t BackboneNet::getBackboneToNodeBw(addr_t addr)
{
	return link_out_[addr];
}

inline   time_s_t BackboneNet::getDelay(addr_t addr)
{
	return link_delay_[addr];
}

inline time_s_t BackboneNet::getDelay(addr_t addr1, addr_t addr2)
{
	return getDelay(addr1) + getDelay(addr2) + 
		getBackBoneDelay(addr1,addr2);
}

inline  void BackboneNet::setUpBand(addr_t addr, rate_Bps_t bw)
{
	link_in_[addr] = bw;
}
inline  void BackboneNet::setDownBand(addr_t addr, rate_Bps_t bw)
{
	link_out_[addr] = bw;
}


inline  void BackboneNet::setBand(addr_t addr, rate_Bps_t bw)
{
	setDownBand(addr, bw);
	setUpBand(addr, bw);
}

inline  time_s_t BackboneNet::getBackBoneDelay(addr_t n1, addr_t n2)
{
	if (nodeCount_ <= MATRIX_SIZE)
		return delayMatrix_[n1][n2];
	return getBackBoneDelay_(n1, n2);
}

#endif//_OMP2PSIM_BACKBONENET_
