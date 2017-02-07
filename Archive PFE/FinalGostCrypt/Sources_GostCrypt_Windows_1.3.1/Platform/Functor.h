/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#ifndef GST_HEADER_Platform_Functor
#define GST_HEADER_Platform_Functor

#include "PlatformBase.h"

namespace GostCrypt
{
	struct Functor
	{
		virtual ~Functor () { }
		virtual void operator() () = 0;
	};

	struct GetStringFunctor
	{
		virtual ~GetStringFunctor () { }
		virtual void operator() (string &str) = 0;
	};
}

#endif // GST_HEADER_Platform_Functor
