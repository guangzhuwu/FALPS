#include "Debug.h"
#include "Utility.h"
#include <stdio.h>
#include <stdarg.h>

void OutputBreak()
{
#ifdef WIN32
	__asm{int 3};
#else
	//__asm__("int $3");
#endif
}

void OutputConsole(const char *format,...)
{
	va_list args;
	va_start(args,format);
	vfprintf(CONSOLE_HANDLE,format,args);
	va_end(args);	
}

void OutputPrint(const char *format,...)
{
	va_list args;
	va_start(args,format);
	vfprintf(PRINT_HANDLE,format,args);
	va_end(args);	
}

void PrintNowTime()
{
	printf("[TIME:%f]",NOW());
}
