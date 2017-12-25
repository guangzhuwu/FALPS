/********************************************************************
  �ļ�:   d:\Simulator\FEPeerSim\FEPeerSim\BackboneNet.h
  ����: 18:1:2007   13:56
  ����: �����
  ˵��: �ԹǸ����ĳ����������Ǹ����Ĵ������޴󣬶���������
	  �����ӳ١�Ҳ���Ƕ���һ�����ݰ���˵��transmitionDelay=0�� 
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
	// ����:  ���ݽڵ�����·�������ȳ�ʼ��BockboneNet
	// ����:  (�ڵ�����·��������·�������ƽ���ӳ٣��ڵ���·������ƽ���ӳ�)
	// ���:  void
	//***********************************************************************
	void initialize(int nodeCount, int routerCount, time_s_t routeDelay,
		time_s_t nodeDelay);

	//***********************************************************************
	// ����:  ��ȡ�ڵ㵽�Ǹ����Ĵ���Ҳ���ǽڵ�����д���  
	// ����:  �ڵ��ַ  
	// ���:  ����Byte/s  
	//***********************************************************************
	band_Bps_t getNodeToBackboneBw(addr_t addr);

	//***********************************************************************
	// ����:  ��ȡ�Ǹ������ڵ�Ĵ���Ҳ���ǽڵ�����д���  
	// ����:  �ڵ��ַ  
	// ���:  ����Byte/s  
	//***********************************************************************
	band_Bps_t getBackboneToNodeBw(addr_t addr);

	//***********************************************************************
	// ����:  ��ȡ�ڵ㵽�Ǹ������ӳ�  
	// ����:  �ڵ��ַ 
	// ���:  �ӳ٣�s  
	//***********************************************************************
	time_s_t getDelay(addr_t addr);


	//***********************************************************************
	// ����:  ��ȡ���ڵ�֮��������ӳ�  
	// ����:  ���ڵ��ַ 
	// ���:  �ӳ٣�s  
	//***********************************************************************
	time_s_t getDelay(addr_t addr1, addr_t addr2);


	//***********************************************************************
	// ����:  �Ǹ����յ��ڵ㷢�������ݰ���recv�������ʱ�������͵����շ�·��
	//  	  ������  
	// ����:  ����packet  
	// ���:  void  
	//***********************************************************************
	void recv(Packet* pkt); 

	//***********************************************************************
	// ����:  recvһ�����ݰ��󣬾����ӳٱ����͵��˽��շ�·�������ú��������  
	//  	  �ð�������·�������Ͷ��з��ͳ�ȥ
	// ����:  ���ݰ�  
	// ���:  void  
	//***********************************************************************
	void transPkt(Packet* pkt); 

	//***********************************************************************
	// ����:  ��ȡ�����ڵ����ڵ�·�����ڹǸ������е��ӳ�  
	// ����:  (�ڵ��ַ���ڵ��ַ)  
	// ���:  �ӳ�,s  
	//***********************************************************************
	time_s_t getBackBoneDelay(addr_t n1, addr_t n2);

	//***********************************************************************
	// ����:  ���ݰ�����Ŀ�Ľڵ�����·���������ݰ������ŵ�������У�������
	//  	  �����������  
	// ����:  Ŀ�Ľڵ��ַ  
	// ���:  void 
	//***********************************************************************
	void processOutGate(addr_t pktDst);

	//***********************************************************************
	// ����:  ���ýڵ�����д���
	// ����:  addr:�ڵ��ַ��bw:����
	// ���:  void
	//***********************************************************************
	void setUpBand(addr_t addr, rate_Bps_t bw);

	//***********************************************************************
	// ����:  ���ýڵ�����д���
	// ����:  addr:�ڵ��ַ��bw:����
	// ���:  void
	//***********************************************************************
	void setDownBand(addr_t addr, rate_Bps_t bw);

	//***********************************************************************
	// ����:  ���ýڵ�����������д�������д���
	// ����:  addr:�ڵ��ַ��bw:����
	// ���:  void
	//***********************************************************************
	void setBand(addr_t addr, rate_Bps_t bw);

	//***********************************************************************
	// ����:  ��ӡ�ڵ���Ϣ
	// ����:  n:��ӡ0-n���ڵ���Ϣ,-1��ʾȫ����ӡ
	// ���:  void
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
	// ����:  ��ʼ����������  
	// ����:  void  
	// ���:  void  
	//***********************************************************************
	void initToplogy();

	//***********************************************************************
	// ����:  ��ȡ�����ڵ����ڵ�·�����ڹǸ������е��ӳ�,������Ա����  
	// ����:  (�ڵ��ַ���ڵ��ַ)  
	// ���:  �ӳ�,s  
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
	size_t nodeCount_;//����ڵ����
	size_t routerCount_;//�Ǹ�����·�ɽڵ���,����ģ��ڵ����ӳ�

	time_s_t meanDelayBetweenRouter_;//·�������ƽ���ӳ٣�s��
	time_s_t meanDelayBetweenNodeBackbone_;//node��backbonenet���ƽ���ӳ�
	vector<addr_t> nodeBelongToWitchRouterVec_;//node���ĸ�·������ģ���ӳ�
	vector<time_s_t> routerDelayVec_;//���ڼ���·�������delay  
	//����ڵ���С��MATRIX_SIZE����matrix�洢�ڵ���delay,�Ӷ�����ٶȡ�
	//�������ڴ�����̫�󣬲�����ʱ����ķ�ʽ
	enum
	{
		MATRIX_SIZE = 1024 * 2
	};
	vector<vector<time_s_t> > delayMatrix_;

	vector<band_Bps_t> link_in_;//�ڵ㵽�Ǹ����Ĵ���
	vector<band_Bps_t> link_out_;//�Ǹ������ڵ�Ĵ���
	vector<time_s_t> link_delay_;//�Ǹ����ͽڵ����ӳ�

	struct OutGatePendingPkt
	{
		time_s_t leave_outQue_time_;//��pending����ʱ�����ӷ��Ͷ����з��ͳ�ȥ
		Packet* pkt_;//Ҫ���͵����ݰ�ָ��
	};
	vector<queue<OutGatePendingPkt> > outGate_que_;//�ͽڵ����ӵ�·�����ķ��Ͷ���
	vector<time_s_t> last_leave_outGate_time_;//���������һ������ʱ(Ҫ/��)�����ͳ�ȥ
	vector<bool> is_outGate_sched_;//���Ͷ����Ƿ�Զ��װ�����sched��������Ŀ�Ľڵ�recv

	struct InGatePendingPkt
	{
		time_s_t leave_inQue_time_;
		Packet* pkt_;//Ҫ���͵����ݰ�ָ��
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
