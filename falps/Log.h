#ifndef _LOGBASE_H_
#define _LOGBASE_H_

#include "Singleton.h"

enum enum_LogLevel{
	LOG_FATAL = 1,  //致命错误
	LOG_CRITICAL,   //严重错误
	LOG_ERROR,      //一般错误
	LOG_WARNING,    //警告
	LOG_NOTICE,     //注意
	LOG_INFORMATION,//消息
	LOG_DEBUG,      //调试
	LOG_TRACE       //跟踪
};

//API_PORT
class Logger
{
	friend class Singleton<Logger>;

private:
	Logger(){level_=LOG_TRACE;}
public:
	void setLevel(enum_LogLevel level){level_=level;}
	enum_LogLevel getLevel() const{return level_;}

	void log(enum_LogLevel level,char* fname,long line,const char* msg);
	void pathToName(char* path,char* name);

private:
	enum_LogLevel level_;
	static const  char* LogHead[];
};

#ifdef USE_LOG

#define LOG(x) x

#define __DO_LOG(level,msg)\
{\
	Singleton<Logger>::instance().log(level,__FILE__,__LINE__,msg);\
}


#define LogFatal(msg)\
	if(LOG_FATAL<=Singleton<Logger>::instance().getLevel())\
	__DO_LOG(LOG_FATAL,msg)

#define LogCritical(msg) \
	if(LOG_CRITICAL<=Singleton<Logger>::instance().getLevel())\
	__DO_LOG(LOG_CRITICAL,msg)

#define LogError(msg) \
	if(LOG_ERROR<=Singleton<Logger>::instance().getLevel())\
	__DO_LOG(LOG_ERROR,msg)

#define logWarning(msg) \
	if(LOG_WARNING<Singleton<Logger>::instance().getLevel())\
	__DO_LOG(LOG_WARNING,msg)

#define LogInfo(msg) \
	if(LOG_INFORMATION<=Singleton<Logger>::instance().getLevel())\
	__DO_LOG(LOG_INFORMATION,msg)

#define LogDebug(msg) \
	if(LOG_DEBUG<=Singleton<Logger>::instance().getLevel())\
	__DO_LOG(LOG_DEBUG,msg)

#define LogTrace(msg) \
	if(LOG_TRACE<=Singleton<Logger>::instance().getLevel())\
	__DO_LOG(LOG_TRACE,msg)

#else 
#define LOG(x) 
#define LogFatal(msg)
#define LogCritical(msg)
#define LogError(msg)
#define logWarning(msg)
#define LogInfo(msg)
#define LogDebug(msg)
#define LogTrace(msg)

#endif//USE_LOG

#endif//_LOGBASE_H_
