#include "md4.h"
#include <memory.h>
#include <stdio.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

inline void halfByteToHex(unsigned char hb, char*& hexBuffer)
{
	if (hb<10)
	{
		*hexBuffer++='0'+hb;
	}
	else
	{
		*hexBuffer++='A'+(hb-10);
	}
}

inline void bytesToHex(unsigned char* bytes, size_t len, char*& hexBuffer)
{
	for (size_t i=0; i<len; i++)
	{
		halfByteToHex((bytes[i]&0xF0)>>4,hexBuffer);
		halfByteToHex((bytes[i]&0x0F),hexBuffer);
	}
}

void CMD4::hash(const uint8* data, unsigned int inputLen, char* digest)
{
	CMD4 md4;
	md4.update(data,inputLen);
	md4.getHash(digest);
}

void CMD4::hashToString(const uint8* data, unsigned int inputLen, string& digest)
{
	char buf[16];
	char tmp[33];
	char* p=tmp;
	hash(data,inputLen,buf);
	bytesToHex((unsigned char*)buf,16,p);
	tmp[32]='\0';
	digest=tmp;
}

/////////////////////////////////////////////////////
// Need an instance to call repeatedly with blocks of data
/////////////////////////////////////////////////////

CMD4::CMD4(void)
{
	init(&context);
}

void CMD4::update(const uint8* data, unsigned int length)
{
	update(&context,data,length);
}

void CMD4::getHash(char* digest)
{
	final(&context,digest);
}

/////////////////////////////////////////////////////
// Private internal stuff
/////////////////////////////////////////////////////

void CMD4::init(MD4_CTX* context)
{
	context->count[0]=0;
	context->count[1]=0;
	context->state[0]=0x67452301;
	context->state[1]=0xefcdab89;
	context->state[2]=0x98badcfe;
	context->state[3]=0x10325476;
	for (int i=0; i<64; i++)
	{
		context->buffer[i]=0;
	}
}

void CMD4::update(MD4_CTX* context, const uint8* input, unsigned int inputLen)
{
	unsigned int i;
	unsigned int index;
	unsigned int partLen;

	/* Compute number of bytes mod 64 */
	index=((context->count[0]>>3)&0x3F);

	/* Update number of bits */
	if ((context->count[0]+=(inputLen<<3))<(inputLen<<3))
	{
		context->count[1]++;
	}
	context->count[1]+=(inputLen>>29);

	partLen=64-index;

	/* Transform as many times as possible. */
	if (inputLen>=partLen)
	{
		memcpy(&context->buffer[index],input,partLen);
		transform(context->state,context->buffer);
		for (i=partLen; i+63<inputLen; i+=64)
		{
			transform(context->state,&input[i]);
		}
		index=0;
	}
	else
	{
		i=0;
	}

	/* Buffer remaining input */
	memcpy(&context->buffer[index],&input[i],inputLen-i);
}

void CMD4::transform(unsigned int* state, const uint8* block)
{
	unsigned int a=state[0];
	unsigned int b=state[1];
	unsigned int c=state[2];
	unsigned int d=state[3];
	unsigned int x[16];

	decode(x,block,64);

	// Round 1
	MD4_FF(a,b,c,d,x[0],MD4_S11); /* 1 */
	MD4_FF(d,a,b,c,x[1],MD4_S12); /* 2 */
	MD4_FF(c,d,a,b,x[2],MD4_S13); /* 3 */
	MD4_FF(b,c,d,a,x[3],MD4_S14); /* 4 */
	MD4_FF(a,b,c,d,x[4],MD4_S11); /* 5 */
	MD4_FF(d,a,b,c,x[5],MD4_S12); /* 6 */
	MD4_FF(c,d,a,b,x[6],MD4_S13); /* 7 */
	MD4_FF(b,c,d,a,x[7],MD4_S14); /* 8 */
	MD4_FF(a,b,c,d,x[8],MD4_S11); /* 9 */
	MD4_FF(d,a,b,c,x[9],MD4_S12); /* 10 */
	MD4_FF(c,d,a,b,x[10],MD4_S13); /* 11 */
	MD4_FF(b,c,d,a,x[11],MD4_S14); /* 12 */
	MD4_FF(a,b,c,d,x[12],MD4_S11); /* 13 */
	MD4_FF(d,a,b,c,x[13],MD4_S12); /* 14 */
	MD4_FF(c,d,a,b,x[14],MD4_S13); /* 15 */
	MD4_FF(b,c,d,a,x[15],MD4_S14); /* 16 */

	// Round 2
	MD4_GG(a,b,c,d,x[0],MD4_S21); /* 17 */
	MD4_GG(d,a,b,c,x[4],MD4_S22); /* 18 */
	MD4_GG(c,d,a,b,x[8],MD4_S23); /* 19 */
	MD4_GG(b,c,d,a,x[12],MD4_S24); /* 20 */
	MD4_GG(a,b,c,d,x[1],MD4_S21); /* 21 */
	MD4_GG(d,a,b,c,x[5],MD4_S22); /* 22 */
	MD4_GG(c,d,a,b,x[9],MD4_S23); /* 23 */
	MD4_GG(b,c,d,a,x[13],MD4_S24); /* 24 */
	MD4_GG(a,b,c,d,x[2],MD4_S21); /* 25 */
	MD4_GG(d,a,b,c,x[6],MD4_S22); /* 26 */
	MD4_GG(c,d,a,b,x[10],MD4_S23); /* 27 */
	MD4_GG(b,c,d,a,x[14],MD4_S24); /* 28 */
	MD4_GG(a,b,c,d,x[3],MD4_S21); /* 29 */
	MD4_GG(d,a,b,c,x[7],MD4_S22); /* 30 */
	MD4_GG(c,d,a,b,x[11],MD4_S23); /* 31 */
	MD4_GG(b,c,d,a,x[15],MD4_S24); /* 32 */

	// Round 3
	MD4_HH(a,b,c,d,x[0],MD4_S31); /* 33 */
	MD4_HH(d,a,b,c,x[8],MD4_S32); /* 34 */
	MD4_HH(c,d,a,b,x[4],MD4_S33); /* 35 */
	MD4_HH(b,c,d,a,x[12],MD4_S34); /* 36 */
	MD4_HH(a,b,c,d,x[2],MD4_S31); /* 37 */
	MD4_HH(d,a,b,c,x[10],MD4_S32); /* 38 */
	MD4_HH(c,d,a,b,x[6],MD4_S33); /* 39 */
	MD4_HH(b,c,d,a,x[14],MD4_S34); /* 40 */
	MD4_HH(a,b,c,d,x[1],MD4_S31); /* 41 */
	MD4_HH(d,a,b,c,x[9],MD4_S32); /* 42 */
	MD4_HH(c,d,a,b,x[5],MD4_S33); /* 43 */
	MD4_HH(b,c,d,a,x[13],MD4_S34); /* 44 */
	MD4_HH(a,b,c,d,x[3],MD4_S31); /* 45 */
	MD4_HH(d,a,b,c,x[11],MD4_S32); /* 46 */
	MD4_HH(c,d,a,b,x[7],MD4_S33); /* 47 */
	MD4_HH(b,c,d,a,x[15],MD4_S34); /* 48 */

	state[0]+=a;
	state[1]+=b;
	state[2]+=c;
	state[3]+=d;

	// Zeroize sensitive information.
	memset(x,0,16);
}

void CMD4::encode(uint8* output, const unsigned int* input, unsigned int len)
{
	unsigned int i;
	unsigned int j;
	for (i=0, j=0; j<len; i++, j+=4)
	{
		output[j]=(uint8)(input[i]&0xff);
		output[j+1]=(uint8)((input[i]>>8)&0xff);
		output[j+2]=(uint8)((input[i]>>16)&0xff);
		output[j+3]=(uint8)((input[i]>>24)&0xff);
	}
}

void CMD4::decode(unsigned int* output, const uint8* input, unsigned int len)
{
	unsigned int i;
	unsigned int j;
	for (i=0, j=0; j<len; i++, j+=4)
	{
		output[i]=(((int)input[j])&0xFF)
				 |((((int)input[j+1])&0xFF)<<8)
				 |((((int)input[j+2])&0xFF)<<16)
				 |((((int)input[j+3])&0xFF)<<24);
	}
}

void CMD4::final(MD4_CTX* context, char* digest)
{
	uint8 bits[8];
	unsigned int index;
	unsigned int padLen;

	// Save number of bits
	encode(bits,context->count,8);
	// Pad out to 56 mod 64.
	index=(unsigned int)((context->count[0]>>3)&0x3f);
	padLen=(index<56)?(56-index):(120-index);
	update(context,MD4_PADDING,padLen);
	// Append length (before padding)
	update(context,bits,8);
	// Store state in digest
	//直接转化，不管字节顺序了
	/*uint8 b[16];
	encode(b, context->state, 16);
	setValueBE(digest,(const char*)b);*/
	encode((uint8*)digest,context->state,16);
	// Zeroize sensitive information.
	init(context);
}
void CMD4::setValueBE(char* to, const char* from)
{
	for (int i=0; i<4; ++i)
	{
		from+=4;
		const char* p=from-1;
		for (int j=0; j<4; ++j)
		{
			*to++= *p--;
		}
	}
}