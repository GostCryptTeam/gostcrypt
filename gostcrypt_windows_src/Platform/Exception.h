/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#ifndef GST_HEADER_Platform_Exception
#define GST_HEADER_Platform_Exception

#include <exception>
#include "PlatformBase.h"
#include "Serializable.h"

namespace GostCrypt
{
#define GST_SERIALIZABLE_EXCEPTION(TYPE) GST_SERIALIZABLE (TYPE); \
	virtual Exception *CloneNew () { return new TYPE (*this); } \
	virtual void Throw () const { throw *this; }

	struct Exception : public exception, public Serializable
	{
	public:
		Exception () { }
		Exception (const string &message) : Message (message) { }
		Exception (const string &message, const wstring &subject) : Message (message), Subject (subject) { }
		virtual ~Exception () throw () { }

		GST_SERIALIZABLE_EXCEPTION (Exception);

		virtual const char *what () const throw () { return Message.c_str(); }
		virtual const wstring &GetSubject() const { return Subject; }

	protected:
		string Message;
		wstring Subject;
	};

	struct ExecutedProcessFailed : public Exception
	{
		ExecutedProcessFailed () { }
		ExecutedProcessFailed (const string &message, const string &command, int exitCode, const string &errorOutput)
			: Exception (message), Command (command), ExitCode (exitCode), ErrorOutput (errorOutput) { }
		virtual ~ExecutedProcessFailed () throw () { }

		GST_SERIALIZABLE_EXCEPTION (ExecutedProcessFailed);

		string GetCommand () const { return Command; }
		int64 GetExitCode () const { return ExitCode; }
		string GetErrorOutput () const { return ErrorOutput; }

	protected:
		string Command;
		int64 ExitCode;
		string ErrorOutput;
	};

#define GST_EXCEPTION_DECL(NAME,BASE) \
	struct NAME  : public BASE \
	{ \
		NAME () { } \
		NAME (const string &message) : BASE (message) { } \
		NAME (const string &message, const wstring &subject) : BASE (message, subject) { } \
		virtual Exception *CloneNew () { return new NAME (*this); } \
		static Serializable *GetNewSerializable () { return new NAME (); } \
		virtual void Throw () const { throw *this; } \
	}

#define GST_EXCEPTION_NODECL(dummy) //
#define GST_EXCEPTION(NAME) GST_EXCEPTION_DECL(NAME,Exception)

#ifdef GST_EXCEPTION_SET
#undef GST_EXCEPTION_SET
#endif
#define GST_EXCEPTION_SET \
	GST_EXCEPTION_NODECL (Exception); \
	GST_EXCEPTION_NODECL (ExecutedProcessFailed); \
	GST_EXCEPTION (AlreadyInitialized); \
	GST_EXCEPTION (AssertionFailed); \
	GST_EXCEPTION (ExternalException); \
	GST_EXCEPTION (InsufficientData); \
	GST_EXCEPTION (NotApplicable); \
	GST_EXCEPTION (NotImplemented); \
	GST_EXCEPTION (NotInitialized); \
	GST_EXCEPTION (ParameterIncorrect); \
	GST_EXCEPTION (ParameterTooLarge); \
	GST_EXCEPTION (PartitionDeviceRequired); \
	GST_EXCEPTION (StringConversionFailed); \
	GST_EXCEPTION (TestFailed); \
	GST_EXCEPTION (TimeOut); \
	GST_EXCEPTION (UnknownException); \
	GST_EXCEPTION (UserAbort)

	GST_EXCEPTION_SET;

#undef GST_EXCEPTION
}

#ifdef assert
#	undef assert
#endif

#ifdef DEBUG
#	define assert(condition) do { if (!(condition)) throw AssertionFailed (SRC_POS); } while (false)
#else
#	define assert(condition) ((void) 0)
#endif

#endif // GST_HEADER_Platform_Exception
