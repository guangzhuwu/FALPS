#include "UInt128.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


HashID::HashID()
{
	SetValue((uint32)0);
}

HashID::HashID(uint32 value)
{
	SetValue(value);
}

HashID::HashID(const uint8* valueBE)
{
	if (valueBE==NULL)
		SetValue((uint32)0);
	SetValue(valueBE);
}

HashID::HashID(const HashID& value, uint32 numBits)
{
	SetValue(value);
	for (uint32 i=numBits; i<128; ++i)
	{
		SetBitNumber(i,(rand()%2));
	}
}

HashID& HashID::SetValue(const HashID& value)
{
	m_data[0]=value.m_data[0];
	m_data[1]=value.m_data[1];
	m_data[2]=value.m_data[2];
	m_data[3]=value.m_data[3];
	return *this;
}
HashID& HashID::SetValue(uint32 value)
{
	m_data[0]=0;
	m_data[1]=0;
	m_data[2]=0;
	m_data[3]=value;
	return *this;
}

HashID& HashID::SetValue(const uint8* valueBE)
{
#ifdef _BIG_ENDIAN_
	uint8* to=(uint8*)m_data;
	for (int i=0; i<4; ++i)
	{
		valueBE+=4;
		const uint8* p=valueBE;
		for (int j=0; j<4; ++j)
		{
			*to++= *(--p);
		}
	}
#else
	memcpy((char*)m_data,valueBE,16);
#endif
	return *this;
}

HashID& HashID::SetValueRandom(void)
{
	//AutoSeededRandomPool rng;
	//byte randomBytes[16];
	//rng.GenerateBlock(randomBytes, 16);
	//setValueBE( randomBytes );
	uint8* p=(uint8*)m_data;
	for (size_t i=0; i<16; i++)
	{
		*p=RandUint8();
	}
	return *this;
}

uint32 HashID::GetBitNumber(uint32 bit) const
{
	if (bit>127)
	{
		return 0;
	}
	int ulongNum=(bit>>5);//=bit/32
	int shift=31-(bit%32);
	return ((m_data[ulongNum]>>shift)&1);
}

HashID& HashID::SetBitNumber(uint32 bit, uint32 value)
{
	if (bit>=128)
		return *this;
	uint32 iLongNum=bit/32;
	uint32 iShift=31-(bit%32);
	m_data[iLongNum]|=(1<<iShift);
	if (value==0)
	{
		m_data[iLongNum]^=(1<<iShift);
	}
	return *this;
}

HashID& HashID::XOR(const HashID& value)
{
	m_data[0]^=value.m_data[0];
	m_data[1]^=value.m_data[1];
	m_data[2]^=value.m_data[2];
	m_data[3]^=value.m_data[3];
	return *this;
}

HashID& HashID::XOR(const uint8* valueBE)
{
	HashID temp(valueBE);
	return XOR(temp);
}

string HashID::ToHexString(void) const
{
	char buf[33];
	for (int i=0; i<4; ++i)
	{
		char* p=buf+(i<<3);
		sprintf_s(p,33,"%08X",m_data[i]);
	}
	return string(buf);
}

string HashID::ToBinaryString(bool trim) const
{
	char buf[129];
	char* p=buf;
	int b;
	for (int i=0; i<128; ++i)
	{
		b=GetBitNumber(i);
		if ((!trim)||b)
		{
			*p++=b?'1':'0'; 
			trim=false;
		}
	}
	*p='\0';
	return string(buf);
}
//注意，末尾并没有补'\0'
void HashID::ToByteArray(uint8* b) const
{
#ifdef _BIG_ENDIAN_
	const uint8* from=(const uint8*)m_data;
	for (int i=0; i<4; ++i)
	{
		from+=4;
		const uint8* p=from;
		for (int j=0; j<4; ++j)
		{
			*b++= *(--p);
		}
	}
#else
	memcpy(b,(uint8*)m_data,16);
#endif
}

int HashID::CompareTo(const HashID& other) const
{
	for (int i=0; i<4; ++i)
	{
		if (m_data[i]<other.m_data[i])
		{
			return -1;
		}
		if (m_data[i]>other.m_data[i])
		{
			return 1;
		}
	}
	return 0;
}

int HashID::CompareTo(uint32 value) const
{
	if ((m_data[0]>0)||(m_data[1]>0)||(m_data[2]>0)||(m_data[3]>value))
	{
		return 1;
	}
	if (m_data[3]<value)
	{
		return -1;
	}
	return 0;
}

HashID& HashID::Add(const HashID& value)
{
	if (value==0)
	{
		return *this;
	}
	int64 sum=0;
	for (int i=3; i>=0; i--)
	{
		sum+=m_data[i];
		sum+=value.m_data[i];
		m_data[i]=(uint32)sum;
		sum>>=32;
	}
	return *this;
}

HashID& HashID::Add(uint32 value)
{
	if (value==0)
	{
		return *this;
	}
	HashID temp(value);
	Add(temp);
	return *this;
}

HashID& HashID::Subtract(const HashID& value)
{
	if (value==0)
	{
		return *this;
	}
	int64 sum=0;
	for (int i=3; i>=0; i--)
	{
		sum+=m_data[i];
		sum-=value.m_data[i];
		m_data[i]=(uint32)sum;
		sum=sum>>32;
	}
	return *this;
}

HashID& HashID::Subtract(uint32 value)
{
	if (value==0)
	{
		return *this;
	}
	HashID temp(value);
	Subtract(temp);
	return *this;
}

HashID& HashID::ShiftLeft(uint32 bits)
{
	if ((bits==0)||(CompareTo(0)==0))
	{
		return *this;
	}
	if (bits>127)
	{
		SetValue((uint32)0);
		return *this;
	}
	uint32 result[]=
	{
		0, 0, 0, 0
	};
	int indexShift=(int)bits/32;
	int64 shifted=0;
	for (int i=3; i>=indexShift; i--)
	{
		shifted+=((int64)m_data[i])<<(bits%32);
		result[i-indexShift]=(uint32)shifted;
		shifted=shifted>>32;
	}
	for (int i=0; i<4; ++i)
	{
		m_data[i]=result[i];
	}
	return *this;
}

uint32 HashID::GetBigestNoneZero()
{
	int n=0;
	uint32 i=0;
	for (;i<4;)
	{
		if (m_data[i]==0)
			n+=32;
		else
			break;
		++i;
	}
	if (n>127)
		return 127;
	const uint8* p=(const uint8*)(m_data+i);
#ifdef _BIG_ENDIAN_
	uint32 j=3;
	for (;j>=0;)
	{
		if(p[j]==0)
			n+=8;
		else
			break;
		--j;
	}
#else
	uint32 j=0;
	for (;j<4;)
	{
		if(p[j]==0)
			n+=8;
		else
			break;
		++j;
	}
#endif	
	for (uint32 k=7;k>=0;--k)
	{
		if (p[j]&(1<<k))
			break;
		++n;
	}
	return n;
}

HashID Hash(const void* what,uint32 len)
{
	char buf[32];
	CMD4::hash((const uint8*)what,len,buf);
	return HashID((const uint8*)buf);
}

HashID HashID::MaskBits(uint32 m) {
	int bl, i, nm;
	unsigned int bitmask;

	bl = m / 32;
	nm = m % 32;

	for(i = 0; i < 4; i++) {
		if(i == bl) {
			bitmask = 1 << nm;
			bitmask--;
			m_data[i] &= bitmask;
		} else if(i < bl) {
			m_data[i] = 0;
		}
		else 
			break;
	}
	return *this;
}


bool HashID::InIntervalOO(const HashID& a, const HashID& b) {
	if(a < b)
		return *this > a && *this < b;
	else
		return !(*this >= b && *this <= a);
}

bool HashID::InIntervalOC(const HashID& a, const HashID& b) {
	if(a < b)
		return *this > a && *this <= b;
	else
		return !(*this > b && *this <= a);
}