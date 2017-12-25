/********************************************************************
�ļ�: d:\MyProjects\FEPeerSim\FEPeerSim\Node.h  
����: 2006/10/31
����: �����

˵��: �ڵ����
*********************************************************************/
#ifndef _NODE_H_
#define _NODE_H_

//#include "Base.h"
//#include "Coro.h"

#include "Utility.h"
#include "Event.h"
#include "Factory.h"
#include "Packet.h"
#include "App.h"
#include "BackboneNet.h"
#include "UInt128.h"
#include "md4.h"
#include <vector>
#include <iostream>
#include <string>
#include <queue>
#include <set>
#include <map>
#include <list>
using namespace std;

class Node;
class Application;
class Packet;
class helpClearNode;
class BackboneNet;
class HashID;

typedef vector<Node*> nodePtrVec_t;
class Node
{
	friend class helpClearNode;
	friend class Application;
public:
	//��ĳApp
	bool attachApp(Application* appPtr);
	void unattachApp();
	
	//����Node�ĵ�ַ����id
	addr_t addr();
	node_id_t id();

	//ʱ����صĲ���
	void   setStartTime(time_s_t t);
	time_s_t getStartTime()const;
	void   setEndTime(time_s_t t);
	time_s_t getEndTime()const;

	//�յ���Ϣ����Connection���ã�������Agent ��recv
	virtual void onRecvd(Packet* pkt);
	virtual void onSended(id_t type);
	//ע��һ�����͵İ���������͵İ���������Ϻ��ص�onSended
	void static registerSendedCallback(id_t type);
	//������Ϣ����Agent���ã�������Edeg��sendto
	virtual void sendTo(Packet* pkt);
	virtual void sendTo(size_B_i_t nbytes, addr_t dst, Packet* pkt);
	time_s_t getNextUplinkIdleTime();

	void setActive();
	void setInActive();
	bool isActive();
	bool isUplinkIdle()
	{
		return !is_link_busy_&&send_pkt_list_.empty();
	}
	//Node������
	void setType(int type);
	int type();
	void transHelp();
	void setLinkBusy();
	void setLinkIdle();

	Application* app(){
		return app_;
	}

	//////////////////////////////////////////////////////////////////////////
	//DHT ���
	void onFindSuccess(const HashID&,const list<addr_t>&);
	void onFindFailed(const HashID&);
	void onStoreFailed(const HashID&);
	void onStoreSuccess(const HashID&);
	
	virtual void construct(){}

	virtual void find(const HashID&){}
	virtual void store(const HashID&){}

	virtual void searchLocal(const HashID&, list<addr_t>&){}
	virtual void storeLocal(const HashID&,addr_t ip){}

protected:
	
	//�û���Ȩֱ�ӵ��ù��캯���Լ�delete
	Node();
	virtual ~Node();

public:
	enum
	{
		LAN_NODE,
		MAN_NODE,
		WAN_NODE,
		EDGE_NODE,
		CORE_NODE,
		INVALID
	};//node type
protected:
	double x_;// x position
	double y_;//y position
	int node_type_;//node type
	addr_t addr_;//��Node��ַ(int)
	bool activated_ ;//status
	double starttime_, endtime_;
	queue<Packet*> send_pkt_list_;//��node�����͵�packet
	bool is_link_busy_;
	Application* app_;
	time_s_t next_idle_time_;
public:
	static id_t node_id_seed_;//���ڲ���Node��IP������
	static set<id_t> s_sended_callback_types_;
protected:
	//static nodePtrVec_t global_node_list_;
	REGIST_GLOBLE_OBJECT_LIST_IN_BASECLASS(Node);
	REGIST_IN_CLASS(Node);
};

inline addr_t Node::addr()
{
	return addr_;
}
inline node_id_t Node::id()
{
	return addr_;
}
inline void Node::setStartTime(time_s_t t)
{
	starttime_ = t;
} 
inline time_s_t Node::getStartTime()const
{
	return starttime_;
}
inline void Node::setEndTime(time_s_t t)
{
	endtime_ = t;
}
inline double Node::getEndTime()const
{
	return endtime_;
}

inline time_s_t Node::getNextUplinkIdleTime()
{
	return next_idle_time_ <= NOW() ? NOW() : next_idle_time_;
}

inline void Node::setActive()
{
	activated_ = true;
}
inline void Node::setInActive()
{
	activated_ = false;
}
inline bool Node::isActive()
{
	return activated_;
}
inline void Node::setType(int type)
{
	node_type_ = type;
}
inline int Node::type()
{
	return node_type_;
}

inline void Node::setLinkBusy()
{
	is_link_busy_ = true;
}
inline void Node::setLinkIdle()
{
	is_link_busy_ = false;
}

#endif//_NODE_H_