#include "StrmBuffer.h"
vector<bitset<StrmBuffer::PIECE_MAP_SIZE> > StrmBuffer::s_gloableBufferMap(5000);

StrmBuffer::StrmBuffer()
:m_allDownPieceCnt(0)
,m_erasedPieceBecauseOfBufferFull(-1)
,m_app(NULL)
{
}

//下载完一个单元，插入到buffer中
bool StrmBuffer::insert(int id)
{
	OutputAssert(id<PIECE_MAP_SIZE);
	//buffer满了，删除一个，但不更新全局buffermap
	//直到一个buffermap通告周期后采取更新全局buffermap
	if (exist(id))
		return false;
	if (++m_allDownPieceCnt>(BUFFER_TIME)/PLAY_TIME_PER_PIECE)
		++m_erasedPieceBecauseOfBufferFull;
	m_recvdThisCycle.insert(id);
	return true;
}

//是否有这个块
bool StrmBuffer::exist(sequence_id_t id)
{
	OutputAssert(m_app);
	OutputAssert(id>=0);
	OutputAssert(id<PIECE_MAP_SIZE);
	//已经被删除
	if (id<=m_erasedPieceBecauseOfBufferFull)
		return false;
	//从全局map查看
	if (s_gloableBufferMap[m_app->addr()].at(id)==true)
		return true;
	//查看本cycle内是不是收到了这一个片段
	return m_recvdThisCycle.find(id)!=m_recvdThisCycle.end();
}

bool StrmBuffer::exist(addr_t ip,sequence_id_t id)
{
	//从全局map查看
	OutputAssert(id<PIECE_MAP_SIZE);
	return s_gloableBufferMap[ip].at(id);
}

//从这个id开始，还有多少个连续的片段可以播放
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
	//标记已经从内存删除了的
	for (int i=m_erasedPieceBecauseOfBufferFull;i>=0;--i)
	{
		if (s_gloableBufferMap[m_app->addr()].at(i)==true)
			s_gloableBufferMap[m_app->addr()].set(i,false);
		else
			break;
	}
	//标记刚下载的
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