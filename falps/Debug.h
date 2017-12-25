#ifndef _DEBUG_OUTPUT_H_
#define _DEBUG_OUTPUT_H_

#include "scheduler.h"

#define CONSOLE_HANDLE stdout
#define PRINT_HANDLE   stderr 

#ifdef _DEBUG
#define PRINT_MESSAGE
#define PRINT_WARNING
#define PRINT_CRITICAL
#define PRINT_ASSERT
#define PRINT_ERROR
#define PRINT_FAILURE
#define PRINT_TIME
#endif

#define PRINT_FLUSH
#define PRINT_SOURCE
#define PRINT_TRACE
#define PRINT_TRAP


void OutputConsole(const char *format,...);
void OutputPrint(const char *format,...);
void PrintNowTime();
void OutputBreak();
inline void  OutputVoid(const char *,...)
{
}

#ifdef PRINT_TIME
#define OutputTime() OutputPrint("[TIME:%f] ",NOW())
#else
#define OutputTime()
#endif

#ifdef PRINT_MESSAGE
#define	OutputMessage OutputPrint
#else
#define OutputMessage OutputVoid
#endif

#ifdef PRINT_SOURCE
#define OutputSource() (PrintNowTime(),OutputPrint("[file:%s,line:%d] ",__FILE__,__LINE__))
#else
#define OutputSource()
#endif

#ifdef PRINT_WARNING
#define OutputWarning OutputPrint("[WARNING] "),OutputSource(),OutputPrint
#else
#define OutputWarning OutputVoid
#endif

#ifdef PRINT_CRITICAL
#define OutputCritical OutputPrint("[CRITICAL] "),OutputSource(),OutputPrint	
#else
#define OutputCritical OutputVoid
#endif

#ifdef PRINT_ERROR
#define OutputError	OutputPrint("[ERROR] "),OutputSource(),OutputPrint
#else
#define OutputError OutputVoid
#endif

#ifdef PRINT_FAILURE
#define OutputFailure OutputPrint("[FAILURE] "),OutputSource(),OutputPrint	
#else
#define OutputFailure OutputVoid
#endif

#ifdef PRINT_TRACE
#define OutputTrace() (OutputPrint("[TRACE] "),OutputSource(),OutputPrint("\n"))
#else
#define OutputTrace() 
#endif

#ifdef PRINT_TRAP
#define OutputTrap() (OutputTrace(),OutputBreak())
#else
#define OutputTrap()
#endif

#ifdef PRINT_ASSERT
#define OutputAssert(x) (void)((x)||(OutputTrace(),OutputBreak(),0))
#else
#define OutputAssert(x)
#endif

#ifdef PRINT_FLUSH
#define OutputFlush() fflush(stderr)
#else
#define OutputFlush()
#endif

#endif//_DEBUG_OUTPUT_H_




