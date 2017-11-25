/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include <pthread.h>
#include <unistd.h>
#include "Platform/Thread.h"

namespace GostCrypt
{
	void Thread::Join () const
	{
		int status = pthread_join (SystemHandle, nullptr);
		if (status != 0)
            throw; //TODO (SRC_POS, status);
	}

	void Thread::Start (ThreadProcPtr threadProc, void *parameter)
	{
		pthread_attr_t attr;
		size_t stackSize = 0;
		int status;

		status = pthread_attr_init (&attr);
		if (status != 0)
            throw; // (SRC_POS, status);

		status = pthread_attr_getstacksize (&attr, &stackSize);
		if (status != 0)
            throw; // (SRC_POS, status);

		if (stackSize < MinThreadStackSize)
		{
			status = pthread_attr_setstacksize (&attr, MinThreadStackSize);
			if (status != 0)
                throw; // (SRC_POS, status);
		}

		status = pthread_create (&SystemHandle, nullptr, threadProc, parameter);
		if (status != 0)
            throw; // (SRC_POS, status);
	}

    void Thread::Sleep (quint32 milliSeconds)
	{
		::usleep (milliSeconds * 1000);
	}
}
