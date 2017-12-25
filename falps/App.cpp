#include "App.h"
app_id_t Application::app_id_seed_;

REGIST_OUT_CLASS(Application);

Application::Application()
{
	node_ = NULL;
	id_ = app_id_seed_++;
}

Application::~Application()
{
}

void Application::start()
{
}

void Application::stop()
{
}

void Application::sendSelfMsg(time_s_t delay, Packet* pkt )
{
	if (pkt == NULL)
		pkt = new Packet;
	pkt->setSelfMsg();
	SelfMsgTimer* timer = new SelfMsgTimer(this, pkt);
	timer->sched(delay);
}

addr_t Application::addr()
{
	FALPS_ASSERT(node_ != NULL,
		cout << "Application has not attached to node!" << endl;);
	return node_->addr();
}

void Application::sendTo(size_B_i_t nbytes, addr_t dst, Packet* pkt)
{
	node_->sendTo(nbytes,dst,pkt);
}

void Application::onRecvd(Packet* pkt)
{
	cout << "Application recv a packet! time: " << NOW() << endl;
	delete pkt;
}
void Application::onSended(pkt_type_t t)
{
	cout << "Application Sended a packet! time: " << NOW() << endl;
}

void Application::attachNode(Node* nodePtr)
{
	if (node_ != nodePtr)
	{
		node_ = nodePtr;
		nodePtr->attachApp(this);
	}
}

bool Application::isUplinkIdle()
{
	if (node_)
		return node_->isUplinkIdle();
	return false;
}

time_s_t Application::getNextUplinkIdleTime()
{
	if (node_)
		return node_->getNextUplinkIdleTime();
	return NOW();
}


void Application::unattachNode()
{
	if (node_)
		node_->unattachApp();
	node_ = NULL;
}
void Application::find(const HashID& id)
{
	node_->find(id);
}
void Application::store(const HashID& id)
{
	node_->store(id);
}

rate_Bps_t Application::getUplinkBandwidth()
{
	assert(node_);
	return BackboneNet::instance().getNodeToBackboneBw(addr());
}
rate_Bps_t Application::getDownlinkBandwidth()
{
	assert(node_);
	return BackboneNet::instance().getBackboneToNodeBw(addr());
}