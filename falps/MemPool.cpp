#include "MemPool.h"
#include <assert.h>

// Static ReuseBase variables
ReuseBase::PtrPtrVec ReuseBase::mem_;
DBG(
	size_t ReuseBase::reused_;    
size_t ReuseBase::alloc_;
size_t ReuseBase::deleted_;
)

void ReuseBase::PtrPtrVec::push_back(unsigned char which,void* p)
{
	if (capacity_map_[which]<=size_map_[which])
	{
		if (capacity_map_[which]==0)
		{
			capacity_map_[which]=8;
			buf_[which]=(void**)malloc(capacity_map_[which]*sizeof(void**));
		}
		else
		{
			capacity_map_[which]<<=1;
			buf_[which]= (void**)realloc(buf_[which],capacity_map_[which]*sizeof(void**));
		}

	}
	buf_[which][size_map_[which]]=p;
	++size_map_[which];
}
void ReuseBase::PtrPtrVec::clear()
{
	for (size_t i=0;i<0xff;++i)
	{
		if (buf_[i]==NULL)
			continue;
		for(int j=0;j<mem_.size_map_[i];j++)
		{
			unsigned char* ptr=reinterpret_cast<unsigned char*>(buf_[i][j]);
			--ptr;
			free ((void*)ptr);
		}
		free(buf_[i]);
	}
}

void* ReuseBase::operator new(size_t n)
{
	register size_t n4=((n+3)>>2);//将要分配的字节长度转化为4字节为单位的长度
	assert(n4<0xff);//保证一个字节可以表示.对象过大，不可继承使用本ReuseBase
	register void * p;
	{
		p=mem_.pop_back((unsigned char)n4);
	}
	if (p==NULL)
	{
		unsigned char * pl = (unsigned char *) malloc((n4<<2)+ 1);//第一个字节用来保存对象的大小
		*pl =(unsigned char)(n4);//n4一定小于等于0xff,这里的强制转换不会丢失数据
		p = &pl[1];
		DBG(alloc_++;)
	}
	DBG(else reused_++;);
	return ::operator new(n, p);
}
void ReuseBase::operator delete(void* p)
{
	if(!p)
		return;
	register unsigned char len = *((unsigned char*)(p)-1); // 以4字节为单位  
	mem_.push_back(len,p);
	DBG(deleted_++;)
}

DBG(
	//ReuseBase在程序退出时会自动清理
	static class  ReuseBaseClear
{
public:
	~ReuseBaseClear(){	
		ReuseBase::clearall();
	}
}__clearReuseBase__;

void ReuseBase::clearall()
{
	DBG(
		info();
	);
	mem_.clear();
}

void ReuseBase::info()
{
	size_t tot = 0;
	cout << "Printing HeapReuse information" << endl;

	cout << "Reused " << reused_ << " Newalloc " << alloc_
		<< " Deleted " << deleted_ << endl;
	for (unsigned char i = 0; i <0xff; ++i)
	{
		cout << "Size " << i*4<<"  :";
		if (0==mem_.getSize(i))
		{
			cout << " NULL " << endl;
		}
		else
		{
			cout << " capacity " << mem_.getCapacity(i)
				<< " size " << mem_.getSize(i)
				<< " total bytes " << mem_.getSize(i)*i* 4
				<< endl;
			tot += mem_.getSize(i) *i;
		}
	}
	cout << "Total bytes in ReuseBase " << tot * 4<<endl;
	if (deleted_!=alloc_+reused_)
	{
		cout<<"bad!!! new and delete is not equal in your code,check it!"<<endl;
		cout<<"alloc: "<<alloc_+reused_<<endl;
		cout<<"delete: "<<deleted_<<endl;
		assert(0);
	}
	else
		cout<<"good!"<<endl;
	//system("pause");
}

)

