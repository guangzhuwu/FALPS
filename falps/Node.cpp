#include "Node.h"
id_t Node::node_id_seed_;
set<id_t> Node::s_sended_callback_types_;

REGIST_OUT_CLASS(Node);

class ReachBackboneTimer : public TimerOnceHandler
{
public:
	ReachBackboneTimer()
	{
	}
	void setNodePkt(Node* n, Packet* pkt)
	{
		node_ = n;
		pkt_ = pkt;
	}
protected:
	virtual void expire(Event*)
	{
		BackboneNet::instance().recv(pkt_);
		node_->setLinkIdle();
		if (pkt_->fragmentID() == pkt_->fragmentCount()
			&&Node::s_sended_callback_types_.find(pkt_->type())!=Node::s_sended_callback_types_.end()
			)
			node_->onSended(pkt_->type());
		node_->transHelp();
	}
	Packet* pkt_;
	Node* node_;
};

class ReachNodeTimer : public TimerOnceHandler
{
public:
	ReachNodeTimer()
	{
	}
	void setDstPkt(addr_t dst, Packet* pkt)
	{
		dst_ = Node::get(dst);pkt_ = pkt;
	}
protected:
	virtual void expire(Event*)
	{
		dst_->onRecvd(pkt_);
	}
	Node* dst_;
	Packet* pkt_;
};

Node::Node()
{
	addr_ = node_id_seed_++;
	is_link_busy_ = false;
	next_idle_time_ = NOW();
	app_ = NULL;
	setActive();
}
Node::~Node()
{
};

bool Node::attachApp(Application* ptr)
{
	app_ = ptr;
	ptr->attachNode(this);
	return true;
}

void Node::unattachApp()
{
	app_ = NULL;
}

void Node::onRecvd(Packet* pkt)
{
	FALPS_ASSERT(pkt->dstAddr() == addr_,
		cout << "packet's dst(" << pkt->dstAddr() << ")is not the node("
		<< addr_ << ").There is  a bug in code!" << endl;);
	if (!isActive())
	{
		delete pkt;
		return;
	}
	if (app_)
		app_->onRecvd(pkt);
	else
		delete pkt;
}

void Node::registerSendedCallback(id_t type)
{
	s_sended_callback_types_.insert(type);
}

void Node::onSended(id_t t)
{
	if (app_)
		app_->onSended(t);
}

void Node::sendTo(Packet* pkt)
{
	if (pkt->dstAddr() >= Node::size())
	{
		delete pkt;
		return;
	}
#if 0
	if (pkt->length() < MSS)
	{
		ReachNodeTimer* timer = new ReachNodeTimer;
		timer->setDstPkt(pkt->dstAddr(), pkt);
		BackboneNet& net = BackboneNet::instance();
		time_s_t t = net.getDelay(addr_) +
			net.getDelay(pkt->dstAddr()) +
			net.getBackBoneDelay(addr_,
			pkt->dstAddr());
		t += (pkt->length() + PROTOCOL_OVERHEAD) /
			net.getNodeToBackboneBw(addr_);
		t += (pkt->length() + PROTOCOL_OVERHEAD) /
			net.getNodeToBackboneBw(pkt->dstAddr());
		timer->sched(t);
		DBG(cout << "Node(upload band:"
			<< BackboneNet::instance().getNodeToBackboneBw(addr_)
			<< ")trans fragment[" << pkt->srcAddr() << "-->"
			<< pkt->dstAddr() << "]with id " << pkt->fragmentID()
			<< " at " << NOW() << endl;);  
	}
	else
#endif
	{
		send_pkt_list_.push(pkt);
		next_idle_time_ = max(next_idle_time_, NOW()) +
			pkt->lengthWithOverhead()/BackboneNet::instance().getNodeToBackboneBw(addr_);
		transHelp();
	}

}
void Node::transHelp()
{
	//本node有待发送的包
	if (is_link_busy_)
		return;
	if (!send_pkt_list_.empty())
	{
		is_link_busy_ = true;
		Packet* pkt = send_pkt_list_.front();
		pkt->fragmentID()++;
		Packet* packcpy = new Packet;
		*packcpy = *pkt;
		time_s_t delaytm = packcpy->fragmentBytes(packcpy->fragmentID()-1) /
			BackboneNet::instance().getNodeToBackboneBw(addr_);
		if (pkt->fragmentID() == pkt->fragmentCount())
		{
			pkt->data() = NULL;
			delete pkt;
			pkt = NULL;
			send_pkt_list_.pop();
		}
		else
		{
			packcpy->data() = NULL;
		}
		ReachBackboneTimer* timer = new ReachBackboneTimer;
		timer->setNodePkt(this, packcpy);
		timer->sched(delaytm);
		DBG(cout << "Node(upload band:"
			<< BackboneNet::instance().getNodeToBackboneBw(addr_)
			<< ")trans fragment[" << packcpy->srcAddr() << "-->"
			<< packcpy->dstAddr() << "]with id " << packcpy->fragmentID()
			<< " at " << NOW() << endl;);
	}
}

void Node::sendTo(size_B_i_t nbytes, addr_t dst, Packet* pkt)
{
	if (!pkt)
		pkt = new Packet;
	addr_t adr =addr();
	pkt->length(nbytes);
	pkt->srcAddr() = adr;
	pkt->dstAddr() = dst;
	if (pkt->dstAddr() == pkt->srcAddr())//loopback
		onRecvd(pkt);
	else
		sendTo(pkt);
}

void Node::onFindSuccess(const HashID& id,const list<addr_t>& l)
{
	app_->onFindSuccess(id,l);
}
void Node::onFindFailed(const HashID& id)
{
	app_->onFindFailed(id);
}
void Node::onStoreFailed(const HashID& id)
{
	app_->onStoreFailed(id);
}
void Node::onStoreSuccess(const HashID& id)
{
	app_->onStoreSuccess(id);
}