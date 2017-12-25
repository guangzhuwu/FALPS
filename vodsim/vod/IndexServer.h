#ifndef _P2PSTREAM_INDEXSERVER_H_
#define _P2PSTREAM_INDEXSERVER_H_
//Ë÷Òý·þÎñÆ÷
#include "Falps.h"
class IndexServerApp:public Application
{
public:
	virtual void onRecvd(Packet* pkt);
	virtual void onSended(pkt_type_t t){}
	virtual void start(){}
	virtual void stop(){}
private:
	list<addr_t> m_peers;
	REGIST_IN_CLASS(IndexServerApp);
};

#endif//_P2PSTREAM_INDEXSERVER_H_