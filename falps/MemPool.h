/********************************************************************
文件:   d:\MyProjects\FEPeerSim\FEPeerSim\MemPool.h 
日期: 2006/11/11
作者: 武广柱 wuguangzhu@gmail.com

说明: ReuseBase,简单的mempool,最大支持255字节的对象
*********************************************************************/

#ifndef __MEMREUSE_H__
#define __MEMREUSE_H__
#include "Utility.h"
#include <vector>
#include <iostream>
using namespace std;
#ifndef FALPS_ASSERT
#ifdef _DEBUG
#define FALPS_ASSERT(condition,action) \
  do{\
  if (!(condition)){\
  action;\
  cout<<__FILE__<<" Line: "<<__LINE__<<endl;\
  abort();\
  }\
  }while(0)

#define  DBG(x) x
#else
#define FALPS_ASSERT(condition,str)
#define  DBG(x)
#endif//_DEBUG
#endif//FALPS_ASSERT

class ReuseBaseClear;
class  ReuseBase
{
	class PtrPtrVec
	{
	public:
		PtrPtrVec()
		{
			memset(buf_,0,sizeof(buf_));
			memset(size_map_,0,sizeof(size_map_));
			memset(capacity_map_,0,sizeof(capacity_map_));
		}
		void push_back(unsigned char which,void* p);
		void* pop_back(unsigned char which)
		{
			if (size_map_[which]==0)
				return NULL;
			--size_map_[which];
			return buf_[which][size_map_[which]];
		}
		void clear();
		int getSize(unsigned char which)
		{
			return size_map_[which];
		}
		int getCapacity(unsigned char which)
		{
			return capacity_map_[which];
		}
	private:
		void** buf_[0xff]; 
		int size_map_[0xff];
		int capacity_map_[0xff];
	};
	friend class ReuseBaseClear;
public:
	static void* operator new(size_t);
	static void operator delete(void*);
	//DEBUG_NEW support
	static void* operator new(size_t n,const char*,int )
	{
		return ReuseBase::operator new(n);
	}
	static void operator delete(void* p,const char*,int)
	{
		ReuseBase::operator delete(p);
	}
private:
	static void clearall();
private:
	DBG(static size_t reused_;
	static size_t alloc_;
	static size_t deleted_;)
		DBG(static void  info();)
private:
	static PtrPtrVec mem_;
};

#endif//__MEMREUSE_H__
