#include "StrmBuffer.h"
vector<bitset<StrmBuffer::PIECE_MAP_SIZE> > StrmBuffer::s_gloableBufferMap(5000);

StrmBuffer::StrmBuffer()
:m_allDownPieceCnt(0)
,m_erasedPieceBecauseOfBufferFull(-1)
,m_app(NULL)
{
}

//������һ����Ԫ�����뵽buffer��
bool StrmBuffer::insert(int id)
{
	OutputAssert(id<PIECE_MAP_SIZE);
	//buffer���ˣ�ɾ��һ������������ȫ��buffermap
	//ֱ��һ��buffermapͨ�����ں��ȡ����ȫ��buffermap
	if (exist(id))
		return false;
	if (++m_allDownPieceCnt>(BUFFER_TIME)/PLAY_TIME_PER_PIECE)
		++m_erasedPieceBecauseOfBufferFull;
	m_recvdThisCycle.insert(id);
	return true;
}

//�Ƿ��������
bool StrmBuffer::exist(sequence_id_t id)
{
	OutputAssert(m_app);
	OutputAssert(id>=0);
	OutputAssert(id<PIECE_MAP_SIZE);
	//�Ѿ���ɾ��
	if (id<=m_erasedPieceBecauseOfBufferFull)
		return false;
	//��ȫ��map�鿴
	if (s_gloableBufferMap[m_app->addr()].at(id)==true)
		return true;
	//�鿴��cycle���ǲ����յ�����һ��Ƭ��
	return m_recvdThisCycle.find(id)!=m_recvdThisCycle.end();
}

bool StrmBuffer::exist(addr_t ip,sequence_id_t id)
{
	//��ȫ��map�鿴
	OutputAssert(id<PIECE_MAP_SIZE);
	return s_gloableBufferMap[ip].at(id);
}

//�����id��ʼ�����ж��ٸ�������Ƭ�ο��Բ���
sequence_id_t StrmBuffer::continuousNum(sequence_id_t id)
{
	OutputAssert(id<PIECE_MAP_SIZE);
	int rst=0;
	while(exist(id+1+rst))
		++rst;
	return rst;
}

void StrmBuffer::updataGloableMap()
{
	OutputAssert(m_app);
	//����Ѿ����ڴ�ɾ���˵�
	for (int i=m_erasedPieceBecauseOfBufferFull;i>=0;--i)
	{
		if (s_gloableBufferMap[m_app->addr()].at(i)==true)
			s_gloableBufferMap[m_app->addr()].set(i,false);
		else
			break;
	}
	//��Ǹ����ص�
	while(!m_recvdThisCycle.empty())
	{
		s_gloableBufferMap[m_app->addr()].set(*(m_recvdThisCycle.begin()),true);
		m_recvdThisCycle.erase(m_recvdThisCycle.begin());
	}
}

void StrmBuffer::setAll()
{
	OutputAssert(m_app);
	s_gloableBufferMap[m_app->addr()].set();
}
void StrmBuffer::clearAll()
{
	OutputAssert(m_app);
	s_gloableBufferMap[m_app->addr()].reset();
}