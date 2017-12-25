/********************************************************************
  文件:   d:\Simulator\FEPeerSim\FEPeerSim\Utility.h
  日期: 18:1:2007   13:59
  作者: 武广柱
  说明: 类型定义

  Copyright (C) 2007 - All Rights Reserved
*********************************************************************/
#ifndef _UTILITY_H_
#define _UTILITY_H_
#include "Valuedef.h"
#include "random.h"
#include <limits>
#include <cstdlib>


typedef signed char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef unsigned int uint;
typedef long long int64;
typedef unsigned long long uint64;


typedef size_t id_t;
typedef id_t pkt_id_t;
typedef id_t pkt_type_t;
typedef id_t addr_t;
typedef addr_t IPAddress;
typedef id_t agent_id_t;
typedef id_t node_id_t;
typedef id_t app_id_t;

typedef double time_s_t;
typedef double time_ms_t;
typedef double band_bps_t;
typedef double band_Bps_t;
typedef double rate_bps_t;
typedef double rate_Bps_t;
typedef double size_b_f_t;
typedef double size_B_f_t;
typedef size_t size_b_i_t;
typedef size_t size_B_i_t;

const size_t INVALID_ID = (size_t) (~0);
const size_t INVALID_ADDR = (size_t) (~0);

#define PORT_NONE -1//不存在的port
#define ADDR_NONE -1//不存在的address

#define SEC2MS(sec)   ((sec)*1000.0)
#define MIN2MS(min)   SEC2MS((min)*60.0)
#define HR2MS(hr)     MIN2MS((hr)*60.0)
#define DAY2MS(day)   HR2MS((day)*24.0)
#define SEC(sec)      (sec)
#define MIN2S(min)    ((min)*60.0)
#define HR2S(hr)      MIN2S((hr)*60.0)
#define DAY2S(day)    HR2S((day)*24.0)

#define _1K_         1024   
#define _1M_         (_1K_*_1K_)  
#define _1G_         (_1K_*_1M_) 
#define _1Kb_         (_1K_/8)   
#define _1Mb_         (_1M_/8) 
#define _1Gb_         (_1G_/8)
#define _1KB_         _1K_   
#define _1MB_         _1M_ 
#define _1GB_         _1G_

#define _K_           *_1K_   
#define _M_           *_1M_ 
#define _G_           *_1G_
#define _Kb_          *_1Kb_   
#define _Mb_          *_1Mb_ 
#define _Gb_          *_1Gb_
#define _KB_          *_1KB_   
#define _MB_          *_1MB_ 
#define _GB_          *_1GB_


class NonCopyble
{
public:
	NonCopyble()
	{
	}
	NonCopyble(const NonCopyble&);
	NonCopyble& operator=(const NonCopyble&);
};



#if defined(_WIN32) 
#	if (_MSC_VER >= 1400)
#		define _HAS_INTERNAL_SAFE_FUNCS
#	endif
#endif

#ifndef _HAS_INTERNAL_SAFE_FUNCS
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <ctime>

inline int strcpy_s(
					char *strDestination,
					size_t numberOfElements,
					const char *strSource 
					)
{
	strcpy(strDestination, strSource);
	return 0;
}

inline int strcat_s(
					char *strDestination,
					size_t numberOfElements,
					const char *strSource 
					)
{
	strcat(strDestination, strSource);
	return 0;
}

inline int localtime_s(
struct tm* _tm,
	const time_t *time 
	)
{
	memcpy(_tm, localtime(time), sizeof(tm));
	return 0;
}

inline int sprintf_s(
					 char *buffer,
					 size_t sizeOfBuffer,
					 const char *format, ... 
					 )
{
	va_list args;
	va_start(args, format);
	int ret = vsprintf(buffer, format, args);
	va_end(args);
	return ret;
}

inline int fopen_s( 
				   FILE** pFile,
				   const char *filename,
				   const char *mode 
				   )
{
	*pFile = fopen(filename, mode);
	return (*pFile != NULL) ? 0 : -1;
}

#endif



#endif//_UTILITY_H_