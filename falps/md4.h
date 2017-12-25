#ifndef _MD4_H_
#define _MD4_H_
#include <string>
using namespace std;

#ifndef uint8
typedef unsigned char uint8;
#endif

#ifndef NOMD4MACROS

/** Constants for MD4Transform routine. */
#define MD4_S11  3
#define MD4_S12  7
#define MD4_S13 11
#define MD4_S14 19
#define MD4_S21  3
#define MD4_S22  5
#define MD4_S23  9
#define MD4_S24 13
#define MD4_S31  3
#define MD4_S32  9
#define MD4_S33 11
#define MD4_S34 15

/** Buffer padding. */
static const uint8 MD4_PADDING[64]=
{
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/** ROTATE_LEFT rotates x left n bits. */
#define MD4_ROTATE_LEFT(x, n) ((x << n) | (x >> (32-n)))

/** F, G and H are basic MD4 functions. */
#define MD4_F(x, y, z)  ( (x & y) | ((~x) & z) )
#define MD4_G(x, y, z)  ( (x & y) | (x & z) | (y & z) )
#define MD4_H(x, y, z)  ( x ^ y ^ z )

/** FF, GG and HH combine the basic MD4 functions. */
#define MD4_FF(a, b, c, d, x, s)  a += MD4_F(b, c, d) + x;  	  a = MD4_ROTATE_LEFT(a, s); 
#define MD4_GG(a, b, c, d, x, s)  a += MD4_G(b, c, d) + x + 0x5a827999; a = MD4_ROTATE_LEFT(a, s); 
#define MD4_HH(a, b, c, d, x, s)  a += MD4_H(b, c, d) + x + 0x6ed9eba1; a = MD4_ROTATE_LEFT(a, s); 

#endif //!NOMD4MACROS

/** MD4 context. */
struct MD4_CTX
{
	/** state (ABCD) */
	unsigned int state[4];
	/** number of bits, modulo 2^64 (lsb first) */
	unsigned int count[2];
	/** input buffer */
	uint8 buffer[64];
};


/**
* This class implements the MD4 algorithm with convenient static methods.
*/
class CMD4
{
public:
	/////////////////////////////////////////////////////
	// Easy to use single-use static methods
	/////////////////////////////////////////////////////

	/**
	* This will calculate the MD4 hash of the given data.
	*
	* @param data     [in]  The data to hash.
	* @param inputLen [in]  The length of the data to hash.
	* @param digest   [out] The MD4 hash of the data
	*/
	static void hash(const uint8* data, unsigned int inputLen, char* digest);

	/**
	* This will calculate the MD4 hash of the given data.
	*
	* @param data     [in]  The data to hash.
	* @param inputLen [in]  The length of the data to hash.
	* @param digest   [out] The MD4 hash of the data in hex format.
	*/
	static void hashToString(const uint8* data, unsigned int inputLen,
							 string& digest);

	/////////////////////////////////////////////////////
	// Need an instance to call repeatedly with blocks of data
	/////////////////////////////////////////////////////

	/**
	* Create a new MD4 object ready to begin updating.
	*/
	CMD4(void);

	/**
	* Process another block of data.
	*
	* @param data   [in] The data to process.
	* @param length [in] The amount of data to process.
	*/
	void update(const uint8* data, unsigned int length);

	/**
	* This will return the MD4 hash of all previously processed data.      <p>
	*
	* Note that the MD4 class will be re-initialised after this call,
	* the next call to update will begin a new MD4 hash process.
	*
	* @param digest [out] The MD4 hash of the data
	*/
	void getHash(char* digest);


private:
	/////////////////////////////////////////////////////
	// Private internal stuff
	/////////////////////////////////////////////////////

	/** The conext for this object instance. */
	MD4_CTX context;

	/** MD4 initialization. Begins an MD4 operation, writing a new context. */
	static void init(MD4_CTX* context);

	/** MD4 block update operation. Continues an MD4 message-digest operation,
	*  processing another message block, and updating the context. */
	static void update(MD4_CTX* context, const uint8* input,
					   unsigned int inputLen);

	/** MD4 basic transformation. Transforms state based on block. */
	static void transform(unsigned int* state, const uint8* block);

	/** Encodes input (unsigned int) into output (uint8). Assumes len is a multiple of 4. */
	static void encode(uint8* output, const unsigned int* input,
					   unsigned int len);

	/** Decodes input (uint8) into output (unsigned int). Assumes len is a multiple of 4. */
	static void decode(unsigned int* output, const uint8* input,
					   unsigned int len);

	/** MD4 finalization. Ends an MD4 message-digest operation, writing the message digest and zeroizing the context. */
	static void final(MD4_CTX* context, char* digest);
	static void setValueBE(char* to, const char* from);
};

#endif