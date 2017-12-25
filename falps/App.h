/********************************************************************
  �ļ�:   d:\Simulator\FEPeerSim\FEPeerSim\App.h
  ����: 18:1:2007   13:53
  ����: �����
  ˵��: Ӧ�ó���

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
	// ����:  �������ݰ� 
	// ����:  nbytes,���ݰ���Сuint8��dst��Ŀ�ĵ�ַ��pkt����pkt
	// ���:  void
	//***********************************************************************
	void sendTo(size_B_i_t nbytes, addr_t dst, Packet* pkt = NULL);

	//***********************************************************************
	// ����:  ����������Ϣ,���ú󲻿ɳ���
	// ����:  delay,ʱ�䣻pkt��������Ϣpkt
	// ���:  void
	//***********************************************************************
	void sendSelfMsg(time_s_t delay, Packet* pkt = NULL);

	//***********************************************************************
	// ����:  ���ݰ�����Ŀ�Ľڵ�Ӧ�ò����Զ����ñ�����  
	// ����:  pkt,���յ������ݰ�ָ��
	// ���:  void
	//***********************************************************************
	virtual void onRecvd(Packet* pkt);

	//***********************************************************************
	// ����:  һ��packet������Ϻ󽫱��ص�  
	// ����:  void
	// ���:  void
	//***********************************************************************
	virtual void onSended(pkt_type_t t);

	//***********************************************************************
	// ����:  �ڵ�Ӧ�ò㿪ʼ����
	// ����:  void
	// ���:  void
	//***********************************************************************
	virtual void start();

	//***********************************************************************
	// ����:  �ڵ�Ӧ�ò�ֹͣ����
	// ����:  void
	// ���:  void
	//***********************************************************************
	virtual void stop();

	//***********************************************************************
	// ����:  ��ĳӦ�ð󶨵�һ�ڵ�  
	// ����:  nodePtr,�ڵ�ָ��
	// ���:  void
	//***********************************************************************
	void attachNode(Node* nodePtr);

	//***********************************************************************
	// ����:  ��ĳӦ�����°󶨵�ĳ�ڵ�
	// ����:  nodePte,�ڵ�ָ��
	// ���:  void
	//***********************************************************************
	void reattachNode(Node* nodePtr);

	//***********************************************************************
	// ����:  ���Ӧ�ò�ͽڵ�İ󶨹�ϵ
	// ����:  void
	// ���:  void
	//***********************************************************************
	void unattachNode();

	//***********************************************************************
	// ����:  ����Ӧ��ID
	// ����:  void
	// ���:  Ӧ��ID
	//***********************************************************************
	app_id_t id();

	//***********************************************************************
	// ����:  ���ذ󶨵Ľڵ�ĵ�ַ
	// ����:  void
	// ���:  �ڵ��ַ
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
	//DHT���
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