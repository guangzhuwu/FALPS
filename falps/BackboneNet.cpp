#include "BackBoneNet.h"
#include "Node.h"
namespace
{
/************************************************************************/
/*   ����·������ʱ�����͵���Ŀ��node�Ķ�ʱ�¼� 						*/
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
		//�鿴�������Ƿ�����������Ҫ����
		BackboneNet::instance().processOutGate(pkt_->dstAddr());
		//������Ͷ�ݸ�Ŀ�Ľڵ�
		dst_->onRecvd(pkt_);
	}
protected:
	Node* dst_;
	Packet* pkt_;
};

/************************************************************************/
/*   ���ڷ��ͽڵ�������·������ʱ���ð�Ͷ�ݵ��˽��սڵ�������·������ʱ�¼�
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
		//����Ŀ�Ľڵ�����·���������䴦��
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
	//�����ĳ���ڵ�����ĳ��·����
	for (size_t i = 0; i < nodeCount_; i++)
	{
		addr_t n = static_cast<addr_t>(Rand01() * routerCount_);
		nodeBelongToWitchRouterVec_[i] = n;
	}
	//����·�������ƽ��meandelay����delay=[0.5-1.5)*meandelay
	for (size_t i = 0; i < routerCount_; i++)
	{
		double d = Rand01() + 0.5;
		double delay = meanDelayBetweenRouter_* d;
		routerDelayVec_[i] = delay;
	}
	//���ݲ�ͬ��������������ռ�ı�������������һ���ڵ�һ��������������
	//�������ͽ������ýڵ�����к����д���
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
	//����ܽڵ�����̫����ôԤ�ȼ���������ڵ�����·�����ڹǸ����е��ӳ�
	//������һ��matrix���������߲�ѯ�ٶȡ�����ڵ���Ŀ��������Ԥ�ȼ���,
	//��Ϊ��ǳ��ķ��ڴ�(n^2).
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

	//��һƬ��ʲôʱ���뿪BackboneNet��������
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
			//˵����sched��ִ�к������ȷ�Ĵ���Ƭ��
			InGatePendingPkt tmp;
			tmp.leave_inQue_time_ = t;
			tmp.pkt_ = pkt;
			inGate_que_[pkt->srcAddr()].push(tmp);
		}
		else
		{
			//˵����sched��ִ�к��Ѿ�ʧȥ����Ƭ�εĻ���
			ReachOutGateTimer* timer = new ReachOutGateTimer;
			timer->setPkt(pkt);
			timer->sched(t - NOW());
		}
	}
	else
	{
		if (is_inGate_sched_[pkt->srcAddr()])
		{
			//��Ȼsched��������inGate_que_Ϊ��
			if (last_leave_inQue_time_[pkt->srcAddr()] < t)
			{
				//˵����sched��ִ�к������ȷ�Ĵ���Ƭ��
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
	//������Ƭ�δ���

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

	//���Ȳ鿴��ǰ���ж�β����ʱ�����������ź�ʱ�Ѿ����������������㱾������
	//���к���Ϊ��β��ʱ����������
	addr_t dst = pkt->dstAddr();
	if (last_leave_outGate_time_[dst] <= NOW())
		last_leave_outGate_time_[dst] = NOW() +pkt->fragmentBytes(pkt->fragmentID()-1) /link_out_[dst];
	else
		last_leave_outGate_time_[dst] += pkt->fragmentBytes(pkt->fragmentID()-1) /link_out_[dst];

	//������fragment������һ���������ݰ����м�Ƭ���򲢲���Ĳ�����У�
	//�����������ٴ���ֻ�б�fragment�����һ��Ƭ��ʱ��Ÿ��ݶ�����û��
	//sched��������������л�sched�ľ�����
	if (pkt->fragmentID() == pkt->fragmentCount())
	{
		//û��sched�¼�����pkt����sched
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
		else//�����������������У��ȴ�����
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