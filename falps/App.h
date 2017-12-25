/********************************************************************
  文件:   d:\Simulator\FEPeerSim\FEPeerSim\App.h
  日期: 18:1:2007   13:53
  作者: 武广柱
  说明: 应用抽象

  Copyright (C) 2007 - All Rights Reserved
*********************************************************************/
#ifndef _APP_H_
#define _APP_H_

#include "Node.h"
#include "Timer.h"
#include "Utility.h"
#include <list>
using namespace std;
class Node;
class Application;
class helpClearApp;
class HashID;

typedef vector<Application*> appPtrVec_t;
class Application : public NonCopyble
{
	friend class helpClearApp;
	friend class Node;
	class SelfMsgTimer : public TimerOnceHandler
	{
	public :
		SelfMsgTimer(Application* app, Packet* pkt)
		{
			app_ = app;pkt_ = pkt;
		}
	protected:
		virtual void expire(Event*)
		{
			app_->onRecvd(pkt_);
		}
		Application* app_;
		Packet* pkt_;
	};
public:
	//***********************************************************************
	// 功能:  发送数据包 
	// 输入:  nbytes,数据包大小uint8；dst，目的地址；pkt数据pkt
	// 输出:  void
	//***********************************************************************
	void sendTo(size_B_i_t nbytes, addr_t dst, Packet* pkt = NULL);

	//***********************************************************************
	// 功能:  向自身发送消息,设置后不可撤销
	// 输入:  delay,时间；pkt，自身消息pkt
	// 输出:  void
	//***********************************************************************
	void sendSelfMsg(time_s_t delay, Packet* pkt = NULL);

	//***********************************************************************
	// 功能:  数据包到达目的节点应用层后会自动调用本函数  
	// 输入:  pkt,接收到的数据包指针
	// 输出:  void
	//***********************************************************************
	virtual void onRecvd(Packet* pkt);

	//***********************************************************************
	// 功能:  一个packet发送完毕后将被回调  
	// 输入:  void
	// 输出:  void
	//***********************************************************************
	virtual void onSended(pkt_type_t t);

	//***********************************************************************
	// 功能:  节点应用层开始工作
	// 输入:  void
	// 输出:  void
	//***********************************************************************
	virtual void start();

	//***********************************************************************
	// 功能:  节点应用层停止工作
	// 输入:  void
	// 输出:  void
	//***********************************************************************
	virtual void stop();

	//***********************************************************************
	// 功能:  将某应用绑定到一节点  
	// 输入:  nodePtr,节点指针
	// 输出:  void
	//***********************************************************************
	void attachNode(Node* nodePtr);

	//***********************************************************************
	// 功能:  将某应用重新绑定到某节点
	// 输入:  nodePte,节点指针
	// 输出:  void
	//***********************************************************************
	void reattachNode(Node* nodePtr);

	//***********************************************************************
	// 功能:  解除应用层和节点的绑定关系
	// 输入:  void
	// 输出:  void
	//***********************************************************************
	void unattachNode();

	//***********************************************************************
	// 功能:  返回应用ID
	// 输入:  void
	// 输出:  应用ID
	//***********************************************************************
	app_id_t id();

	//***********************************************************************
	// 功能:  返回绑定的节点的地址
	// 输入:  void
	// 输出:  节点地址
	//***********************************************************************
	addr_t addr();

	Node* node()
	{
		return node_;
	}

	bool isUplinkIdle();
	time_s_t getNextUplinkIdleTime();
	rate_Bps_t getUplinkBandwidth();
	rate_Bps_t getDownlinkBandwidth();

	//////////////////////////////////////////////////////////////////////////
	//DHT相关
	virtual void onFindSuccess(const HashID&,const list<addr_t>&){
	}
	virtual void onFindFailed(const HashID&){}
	virtual void onStoreFailed(const HashID&){}
	virtual void onStoreSuccess(const HashID&){}
	void find(const HashID&);
	void store(const HashID&);

protected:
	Application();
	virtual ~Application();
protected: 
	Node* node_;
	app_id_t id_;
	static app_id_t app_id_seed_;
	REGIST_GLOBLE_OBJECT_LIST_IN_BASECLASS(Application);
	REGIST_IN_CLASS(Application);
};


inline 	void Application::reattachNode(Node* nodePtr)
{
	attachNode(nodePtr);
}

inline 	app_id_t Application::id()
{
	return id_;
}


#endif//_APP_H_