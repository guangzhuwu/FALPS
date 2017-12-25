#include "BackBoneNet.h"
#include "Node.h"
namespace
{
/************************************************************************/
/*   用于路由器何时将包送到了目的node的定时事件 						*/
/************************************************************************/
class ReachDstNodeTimer : public TimerOnceHandler
{
public:
	ReachDstNodeTimer()
	{
	}
	void setPkt(Packet* pkt)
	{
		dst_ = Node::get(pkt->dstAddr());
		pkt_ = pkt;
	}
protected:
	virtual void expire(Event*)
	{
		DBG(cout << "Node[" << dst_->addr() << "] recved a packet from node["
				<< pkt_->srcAddr() << "] at " << NOW() << endl;);
		//查看队列中是否还有其他包需要处理
		BackboneNet::instance().processOutGate(pkt_->dstAddr());
		//将本包投递给目的节点
		dst_->onRecvd(pkt_);
	}
protected:
	Node* dst_;
	Packet* pkt_;
};

/************************************************************************/
/*   用于发送节点相连的路由器何时将该包投递到了接收节点相连的路由器定时事件
/************************************************************************/
class ReachOutGateTimer : public TimerOnceHandler
{
public:
	ReachOutGateTimer()
	{
	}
	void setPkt(Packet* pkt)
	{
		pkt_ = pkt;
	}
protected:
	virtual void expire(Event*)
	{
		//引发目的节点所在路由器做传输处理
		BackboneNet::instance().transPkt(pkt_);
	}
	Packet* pkt_;
};
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void BackboneNet::initialize(int nodeCount, int routerCount, time_s_t rdelay,
	time_s_t ndelay)
{
	static bool flag = false;
	if (flag)
		return;
	flag = true;

	nodeCount_ = nodeCount;
	routerCount_ = routerCount;
	meanDelayBetweenRouter_ = rdelay;
	meanDelayBetweenNodeBackbone_ = ndelay;

	nodeBelongToWitchRouterVec_.resize(nodeCount_);
	routerDelayVec_.resize(routerCount_);
	outGate_que_.resize(nodeCount_);
	inGate_que_.resize(nodeCount_);
	link_delay_.resize(nodeCount_);
	link_in_.resize(nodeCount_);
	link_out_.resize(nodeCount_);
	last_leave_outGate_time_.resize(nodeCount_);
	last_leave_inQue_time_.resize(nodeCount_, -1);
	is_outGate_sched_.resize(nodeCount_, false);
	is_inGate_sched_.resize(nodeCount_, false);

	initToplogy();
}

void BackboneNet::initToplogy()
{
	//随机将某个节点分配给某个路由器
	for (size_t i = 0; i < nodeCount_; i++)
	{
		addr_t n = static_cast<addr_t>(Rand01() * routerCount_);
		nodeBelongToWitchRouterVec_[i] = n;
	}
	//根据路由器间的平均meandelay计算delay=[0.5-1.5)*meandelay
	for (size_t i = 0; i < routerCount_; i++)
	{
		double d = Rand01() + 0.5;
		double delay = meanDelayBetweenRouter_* d;
		routerDelayVec_[i] = delay;
	}
	//根据不同网络链接类型所占的比例，随机分配给一个节点一种网络链接类型
	//链接类型将决定该节点的上行和下行带宽
	double range = ADSL_256 + ADSL_512 + ADSL_1500 + SDSL_512 + DS1;
	for (size_t i = 0; i < nodeCount_; i++)
	{
		string protocal=PROTOCAL;
		string::size_type pos=protocal.find_first_of('/');
		if (pos!=string::npos)
			protocal=protocal.substr(0,pos);
		if (protocal=="None")
			protocal="Node";

		new_class(protocal);//node
		double d = Rand01() + 0.5;
		link_delay_[i] = d * meanDelayBetweenNodeBackbone_;
		d = range * Rand01();
		if (d <= ADSL_256)
		{
			link_in_[i] = ADSL_256_UPLOAD_RATE;
			link_out_[i] = ADSL_256_DOWNLOAD_RATE;
		}
		else if (d <= ADSL_256 + ADSL_512)
		{
			link_in_[i] = ADSL_512_UPLOAD_RATE;
			link_out_[i] = ADSL_512_DOWNLOAD_RATE;
		}
		else if (d <= ADSL_256 + ADSL_512 + ADSL_1500)
		{
			link_in_[i] = ADSL_1500_UPLOAD_RATE;
			link_out_[i] = ADSL_1500_DOWNLOAD_RATE;
		}
		else if (d <= ADSL_256 + ADSL_512 + ADSL_1500 + SDSL_512)
		{
			link_in_[i] = SDSL_512_UPLOAD_RATE;
			link_out_[i] = SDSL_512_DOWNLOAD_RATE;
		}
		else
		{
			link_in_[i] = DS1_UPLOAD_RATE;
			link_out_[i] = DS1_DOWNLOAD_RATE;
		}
	}
	//如果总节点数不太大，那么预先计算出各个节点所在路由器在骨干网中的延迟
	//并存入一个matrix，这可以提高查询速度。如果节点数目过大，则不再预先计算,
	//因为这非常耗费内存(n^2).
	if (nodeCount_ <= MATRIX_SIZE)
	{
		delayMatrix_.resize(nodeCount_);
		for (size_t i = 0; i < nodeCount_; i++)
			delayMatrix_[i].resize(nodeCount_);

		for (size_t i = 0; i < nodeCount_; i++)
			for (size_t j = 0; j < nodeCount_; j++)
				delayMatrix_[i][j] = static_cast<time_s_t>(getBackBoneDelay_(i,
															j));
	}

	if (Node::size()>0)
		Node::get(0)->construct();
}

time_s_t BackboneNet::getBackBoneDelay_(addr_t n1, addr_t n2)
{
	size_t d1 = nodeBelongToWitchRouterVec_[n1];
	size_t d2 = nodeBelongToWitchRouterVec_[n2];
	if (d1 == d2)
		return 0;
	if (d1 > d2)
		swap(d1, d2);
	double delay = 0.0;
	for (size_t i = d1; i < d2; i++)
		delay += routerDelayVec_[i];
	return delay;
}

void BackboneNet::recv(Packet* pkt)
{
	DBG(cout << "Backbone recvd fragment[" << pkt->srcAddr() << "--->"
			<< pkt->dstAddr() << "] with id " << pkt->fragmentID()
			<< " fragmentCount " << pkt->fragmentCount() << " at time "
			<< NOW() << endl;);

	//这一片段什么时候离开BackboneNet的输入门
	time_s_t t = getBackBoneDelay(pkt->srcAddr(), pkt->dstAddr()) +
		getDelay(pkt->srcAddr()) +
		NOW();
	//ReachOutGateTimer* timer=new ReachOutGateTimer;
	//timer->setPkt(pkt);
	//timer->sched(t-NOW(),timer);
	//return;
	if (!inGate_que_[pkt->srcAddr()].empty())
	{
		if (inGate_que_[pkt->srcAddr()].back().leave_inQue_time_ < t)
		{
			//说明当sched被执行后可以正确的处理本片段
			InGatePendingPkt tmp;
			tmp.leave_inQue_time_ = t;
			tmp.pkt_ = pkt;
			inGate_que_[pkt->srcAddr()].push(tmp);
		}
		else
		{
			//说明当sched被执行后，已经失去处理本片段的机会
			ReachOutGateTimer* timer = new ReachOutGateTimer;
			timer->setPkt(pkt);
			timer->sched(t - NOW());
		}
	}
	else
	{
		if (is_inGate_sched_[pkt->srcAddr()])
		{
			//虽然sched过，但是inGate_que_为空
			if (last_leave_inQue_time_[pkt->srcAddr()] < t)
			{
				//说明当sched被执行后可以正确的处理本片段
				InGatePendingPkt tmp;
				tmp.leave_inQue_time_ = t;
				tmp.pkt_ = pkt;
				inGate_que_[pkt->srcAddr()].push(tmp);
			}
			else
			{
				ReachOutGateTimer* timer = new ReachOutGateTimer;
				timer->setPkt(pkt);
				timer->sched(t - NOW());
			}
		}
		else
		{
			ReachOutGateTimer* timer = new ReachOutGateTimer;
			timer->setPkt(pkt);
			timer->sched(t - NOW());
			last_leave_inQue_time_[pkt->srcAddr()] = t;
			is_inGate_sched_[pkt->srcAddr()] = true;
		}
	}
}

void BackboneNet::transPkt(Packet* pkt)
{

	DBG(cout << "Backbone trans fragment[" << pkt->srcAddr() << " ---> "
			<< pkt->dstAddr() << "] with id " << pkt->fragmentID()
			<< " fragmentCount " << pkt->fragmentCount() << " at time "
			<< NOW() << endl;);
	//对其后继片段处理

	if (!inGate_que_[pkt->srcAddr()].empty())
	{
		Packet* p = inGate_que_[pkt->srcAddr()].front().pkt_;
		time_s_t t = inGate_que_[pkt->srcAddr()].front().leave_inQue_time_;
		inGate_que_[pkt->srcAddr()].pop();

		last_leave_inQue_time_[pkt->srcAddr()] = t;

		ReachOutGateTimer* timer = new ReachOutGateTimer;
		timer->setPkt(p);
		timer->sched(t - NOW());
	}
	else
		is_inGate_sched_[pkt->srcAddr()] = false;

	//首先查看此前队列队尾包何时将被发出护着何时已经被发出，进而计算本包插入
	//队列后作为队尾何时将被发出。
	addr_t dst = pkt->dstAddr();
	if (last_leave_outGate_time_[dst] <= NOW())
		last_leave_outGate_time_[dst] = NOW() +pkt->fragmentBytes(pkt->fragmentID()-1) /link_out_[dst];
	else
		last_leave_outGate_time_[dst] += pkt->fragmentBytes(pkt->fragmentID()-1) /link_out_[dst];

	//如果这个fragment仅仅是一个完整数据包的中间片，则并不真的插入队列，
	//而仅仅做销毁处理。只有本fragment是最后一个片段时候才根据队列有没有
	//sched过而作出插入队列或sched的决定。
	if (pkt->fragmentID() == pkt->fragmentCount())
	{
		//没有sched事件，则本pkt将被sched
		if (!is_outGate_sched_[dst])
		{
			is_outGate_sched_[dst] = true;
			ReachDstNodeTimer* timer = new ReachDstNodeTimer;
			timer->setPkt(pkt);
			timer->sched(last_leave_outGate_time_[dst] - NOW() + getDelay(dst));
			DBG(cout << "Backbone send out packet[" << pkt->srcAddr()
					<< "--->" << pkt->dstAddr() << "] at time " << NOW()
					<< endl;);
		}
		else//否则这个包被插入队列，等待发送
		{
			OutGatePendingPkt pendingpkt;
			pendingpkt.pkt_ = pkt;
			pendingpkt.leave_outQue_time_ = last_leave_outGate_time_[dst];
			outGate_que_[dst].push(pendingpkt);
		}
	}
	else
		delete pkt;
}

void BackboneNet::processOutGate(addr_t addr)
{
	if (outGate_que_[addr].empty())
	{
		is_outGate_sched_[addr] = false;
	}
	else
	{
		is_outGate_sched_[addr] = true;
		OutGatePendingPkt pendingpkt = outGate_que_[addr].front();

		ReachDstNodeTimer* timer = new ReachDstNodeTimer;
		timer->setPkt(pendingpkt.pkt_);
		timer->sched(pendingpkt.leave_outQue_time_ -
				NOW() +
				getDelay(pendingpkt.pkt_->dstAddr()));

		outGate_que_[addr].pop();

		DBG(cout << "Backbone send out packet[" << pendingpkt.pkt_->srcAddr()
				<< "--->" << pendingpkt.pkt_->dstAddr() << "] at time "
				<< NOW() << endl;);
	}
}

void BackboneNet::printNetInfo(int n)
{
	double alf=8.0 / 1024.0 / 1024.0;
	if (n == -1)
		n = static_cast<int>(Node::size());
	n = min(n, (int) Node::size());
	cout << "printNetInfo:" << endl;
	cout << "-------------" << endl;
	for (int i = 0; i < n; i++)
	{
		cout << "node[" << i << "] downband(mbps):"
			<< link_out_[i] *alf  << "  upband(mbps):"
			<< link_in_[i] *alf<< "  delay(s):"
			<< getDelay(i) << "\n";
	}
	time_s_t maxdelay = 0;
	for (int i = 0; i < n; i++)
	{
		for (int j = i; j < n; j++)
			maxdelay = max(maxdelay, (getBackBoneDelay(i, j)));
	}
	cout << "max backbone delay: " << maxdelay << endl;
	cout << "-------------" << endl;
}