#include "IndexServer.h"
#include "VodMsg.h"

REGIST_OUT_CLASS(IndexServerApp);

void IndexServerApp::onRecvd(Packet* pkt)
{
	OutputAssert(pkt->type()==MSG_PEER_REQUEST);
	FindNeightborResponseMsg* data=new FindNeightborResponseMsg(m_peers);
	sendTo(CONTROL_MSG_LEN,pkt->srcAddr(),
		Packet::newPacket(MSG_PEER_RESPONSE,data));
	m_peers.push_back(pkt->srcAddr());
	if (m_peers.size()>20)
		m_peers.pop_front();
}