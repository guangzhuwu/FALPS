/************************************************************************/
/* All rights reserved. 												*/
/* Author:Guangzhu Wu   												*/
/************************************************************************/
#ifndef _HANDLER_H_
#define _HANDLER_H_

#include "Event.h"
class Event;
class Handler : public NonCopyble
{
public:
	virtual ~Handler()
	{
	}
	virtual void handle(Event* event) = 0;
};

#endif//_HANDLER_H_