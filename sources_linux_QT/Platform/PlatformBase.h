/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Platform_PlatformBase
#define GST_HEADER_Platform_PlatformBase

#include <cstddef>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#ifndef _MSC_VER
#include <inttypes.h>
#endif

using namespace std;

#ifdef nullptr
#undef nullptr
#endif

#if !(defined(_MSC_VER) && _MSC_VER >= 1600)
#define nullptr 0
#endif

namespace GostCrypt
{
#ifdef _MSC_VER
#	ifndef GST_INT_TYPES_DEFINED
	typedef __int8 int8;
	typedef __int16 int16;
	typedef __int32 int32;
	typedef __int64 int64;
	typedef unsigned __int8 byte;
	typedef unsigned __int16 uint16;
	typedef unsigned __int32 uint32;
	typedef unsigned __int64 uint64;
#	endif
#else
	typedef int8_t int8;
	typedef int16_t int16;
	typedef int32_t int32;
	typedef int64_t int64;
	typedef uint8_t byte;
	typedef uint16_t uint16;
	typedef uint32_t uint32;
	typedef uint64_t uint64;
#endif
}

#if defined(_DEBUG) && !defined(DEBUG)
#	define DEBUG
#endif

#ifndef GST_TO_STRING
#	define GST_TO_STRING2(n) #n
#	define GST_TO_STRING(n) GST_TO_STRING2(n)
#endif

#define GST_JOIN_ARGS(a,b) a##b
#define GST_JOIN(a,b) GST_JOIN_ARGS(a,b)

#ifdef __GNUC__
	template <class T> string GetFunctionName (T pos)
	{
		string s (pos);
		size_t p = s.find ('(');
		if (p == string::npos)
			return s;
		s = s.substr (0, p);
		p = s.find_last_of (" ");
		if (p == string::npos)
			return s;
		return s.substr (p + 1);
	}
#	define SRC_POS (GetFunctionName (__PRETTY_FUNCTION__) += ":" GST_TO_STRING(__LINE__))
#	define GST_UNUSED_VAR __attribute__ ((unused))
#else
#	define SRC_POS (__FUNCTION__ ":" GST_TO_STRING(__LINE__))
#	define GST_UNUSED_VAR
#endif

#ifdef trace_point
#	undef trace_point
#endif

#ifdef trace_msg
#	undef trace_msg
#endif

#ifdef DEBUG
#	define if_debug(...) __VA_ARGS__

#	include <iostream>
#	define trace_point cerr << (SRC_POS) << endl
#	define trace_msg(stream_args) cerr << (SRC_POS) << ": " << stream_args << endl
#	define trace_msgw(stream_args) cerr << (SRC_POS); wcerr << L": " << stream_args << endl

#	include "Platform/SystemLog.h"
#	define trace_log_point SystemLog::WriteError (SRC_POS)
#	define trace_log_msg(stream_args) do { stringstream s; s << (SRC_POS) << ": " << stream_args; SystemLog::WriteError (s.str()); } while (0)

#else
#	define if_debug(...)
#	define trace_point
#	define trace_msg(...)
#	define trace_msgw(...)
#	define trace_log_point
#	define trace_log_msg(...)
#endif

#define trace_val(VAL) trace_msg (#VAL << '=' << (VAL));

#define array_capacity(arr) (sizeof (arr) / sizeof ((arr)[0]))

#endif // GST_HEADER_Platform_PlatformBase
