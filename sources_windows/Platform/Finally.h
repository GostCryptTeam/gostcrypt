/*
 Copyright (c) 2008-2009 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#ifndef GST_HEADER_Platform_Finally
#define GST_HEADER_Platform_Finally

#include "PlatformBase.h"

// Execute code when leaving scope
#define finally_do(code) \
struct GST_JOIN(Finally,__LINE__) \
{ \
	GST_JOIN(~Finally,__LINE__) () { try { code } catch (...) { } } \
} \
GST_UNUSED_VAR \
GST_JOIN(finally,__LINE__)

// Execute code with argument 'finally_arg' when leaving scope 
#define finally_do_arg(argType, arg, code) \
struct GST_JOIN(Finally,__LINE__) \
{ \
	GST_JOIN(Finally,__LINE__) (argType a) : finally_arg (a) { } \
	GST_JOIN(~Finally,__LINE__) () { try { code } catch (...) { } } \
	argType finally_arg; \
} \
GST_UNUSED_VAR \
GST_JOIN(finally,__LINE__) (arg)

#define finally_do_arg2(argType, arg, argType2, arg2, code) \
struct GST_JOIN(Finally,__LINE__) \
{ \
	GST_JOIN(Finally,__LINE__) (argType a, argType2 a2) : finally_arg (a), finally_arg2 (a2) { } \
	GST_JOIN(~Finally,__LINE__) () { try { code } catch (...) { } } \
	argType finally_arg; \
	argType2 finally_arg2; \
} \
GST_UNUSED_VAR \
GST_JOIN(finally,__LINE__) (arg, arg2)


#endif // GST_HEADER_Platform_Finally
