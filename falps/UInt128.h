#ifndef __UINT128_H__
#define __UINT128_H__
#include "falps.h"

#if ('\001\002\003\004'>>24==4)
#define  _BIG_ENDIAN_
#elif('\001\002\003\004'>>24==1)
#define  _LITTLE_ENDIAN_
#else
#error "unknown  _BIG_ENDIAN_ or _LITTLE_ENDIAN_"
#endif
#ifndef uint8
typedef unsigned char uint8;
#endif

//FIXME:这个类还有很多bug

class HashID
{
public:
	enum{
		IDSIZE=128
	};

	explicit HashID();
	explicit HashID(uint32 value);
	explicit HashID(const uint8* valueBE);

	HashID(const HashID& value, uint32 numBits=128);

	static HashID Hash(const void* what,uint32 len); 

	~HashID()
	{
	};

	const uint8* GetData() const
	{
		return (uint8*)m_data;
	}
	uint8* GetDataPtr() const
	{
		return (uint8*)m_data;
	}

	HashID MaskBits(uint32 m);

	/** Bit at level 0 being most significant. */
	uint32 GetBitNumber(uint32 bit) const;
	int CompareTo(const HashID& other) const;
	int CompareTo(uint32 value) const;

	string ToHexString(void) const;
	string ToBinaryString(bool trim=false) const;
	void ToByteArray(uint8* b) const;

	uint32 Get32BitChunk(int val) const
	{
		return m_data[val];
	}
	uint32 GetBigestNoneZero();
	HashID& SetValue(const HashID& value);
	HashID& SetValue(uint32 value);
	HashID& SetValue(const uint8* valueBE);

	HashID& SetValueRandom(void);
	//  HashID& setValueGUID(void);

	HashID& SetBitNumber(uint32 bit, uint32 value);
	HashID& ShiftLeft(uint32 bits);

	HashID& Add(const HashID& value);
	HashID& Add(uint32 value);
	HashID& Subtract(const HashID& value);
	HashID& Subtract(uint32 value);

	HashID& XOR(const HashID& value);
	HashID& XOR(const uint8* valueBE); 
	
	bool InIntervalOO(const HashID& a,const HashID& b);
	bool InIntervalOC(const HashID& a, const HashID& b);

	void operator+(const HashID& value)
	{
		Add(value);
	}
	void operator-(const HashID& value)
	{
		Subtract(value);
	}
	void operator=(const HashID& value)
	{
		SetValue(value);
	}
	bool operator<(const HashID& value) const
	{
		return (CompareTo(value)<0);
	}
	bool operator>(const HashID& value) const
	{
		return (CompareTo(value)>0);
	}
	bool operator<=(const HashID& value) const
	{
		return (CompareTo(value)<=0);
	}
	bool operator>=(const HashID& value) const
	{
		return (CompareTo(value)>=0);
	}
	bool operator==(const HashID& value) const
	{
		return (CompareTo(value)==0);
	}
	bool operator!=(const HashID& value) const
	{
		return (CompareTo(value)!=0);
	}

	void operator+(uint32 value)
	{
		Add(value);
	}
	void operator-(uint32 value)
	{
		Subtract(value);
	}
	void operator=(uint32 value)
	{
		SetValue(value);
	}
	bool operator<(uint32 value) const
	{
		return (CompareTo(value)<0);
	}
	bool operator>(uint32 value) const
	{
		return (CompareTo(value)>0);
	}
	bool operator<=(uint32 value) const
	{
		return (CompareTo(value)<=0);
	}
	bool operator>=(uint32 value) const
	{
		return (CompareTo(value)>=0);
	}
	bool operator==(uint32 value) const
	{
		return (CompareTo(value)==0);
	}
	bool operator!=(uint32 value) const
	{
		return (CompareTo(value)!=0);
	}

private:

	uint32 m_data[4];
};
inline HashID operator^(const HashID&id1, const HashID&id2)
{
	HashID tmp(id1);
	tmp.XOR(id2);
	return tmp;
}

#endif
