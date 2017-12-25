/********************************************************************
  文件:   d:\Simulator\FEPeerSim\FEPeerSim\SmartPtr.h
  日期: 18:1:2007   13:59
  作者: 武广柱
  说明: SmartPtr

  Copyright (C) 2007 - All Rights Reserved
*********************************************************************/
#ifndef _SMARTPTR_H_
#define _SMARTPTR_H_
#include <cassert>
#include<cstdlib>

template <class T>
class SmartPtr
{
public:
	SmartPtr(int = 0) : _ptr(NULL), _count(NULL)
	{
	}

	SmartPtr(T* ptr) : _ptr(ptr), _count(NULL)
	{
		if (ptr)
		{
			_count = new unsigned int;
			*_count = 1;
		}
	}

	SmartPtr(const SmartPtr& rhs) : _ptr(NULL), _count(NULL)
	{
		*this = rhs;
	}

	template <class U>
	SmartPtr(const SmartPtr<U>& rhs) : _ptr(NULL), _count(NULL)
	{
		*this = rhs;
	}

	template <class U>
	SmartPtr(U* rhs) : _ptr(NULL), _count(NULL)
	{
		SmartPtr<U> tmp(rhs);
		*this = tmp;
	}

	~SmartPtr()
	{
		clear();
	}

	void clear()
	{
		if (_count && --(*_count) == 0)
		{
			delete _ptr; 
			_ptr = NULL; 
			delete _count; 
			_count = NULL;
		}
	}

	T* __get_Ptr() const
	{
		return _ptr;
	}

	unsigned int* __get_Count()const
	{
		return _count;
	}

	SmartPtr<T>& operator=(const SmartPtr<T>& rhs)
	{
		clear();
		_ptr = rhs._ptr;
		if ((_count = rhs._count) != NULL)
			++(*_count);
		return *this;
	}

	template <class U>
	SmartPtr<T>& operator=(const SmartPtr<U>& rhs)
	{
		assert(dynamic_cast<T*>(rhs.__get_Ptr()) != NULL);
		clear();
		_ptr = (T *) rhs.__get_Ptr();
		if (_count = rhs.__get_Count())
			++(*_count);
		return *this;
	}

	SmartPtr<T>& operator=(T* ptr)
	{
		SmartPtr tmp(ptr);
		*this = tmp;
		return *this;
	}

	template <class U>
	SmartPtr<T>& operator=(U* ptr)
	{
		assert(dynamic_cast<T*>(ptr) != NULL);
		SmartPtr<T> tmp((T*) ptr);
		*this = tmp;
		return *this;
	}

	T& operator*() const
	{
		assert(_ptr != NULL); return *_ptr;
	}

	T* operator->() const
	{
		assert(_ptr != NULL); return _ptr;
	}

	operator T*() const
	{
		return _ptr;
	}

	operator T*()
	{
		return _ptr;
	}

	template <class U>
	operator U*()
	{
		assert(dynamic_cast<T*>(_ptr));return (U *) _ptr;
	}

	bool operator!() const
	{
		return (!_ptr);
	} 

	bool operator==(int) const
	{
		return (_ptr == NULL);
	}

	bool operator!=(int) const
	{
		return (_ptr != NULL);
	}
protected:
	T* _ptr;
	unsigned int* _count;
};

#endif // SMARTPTR_H

