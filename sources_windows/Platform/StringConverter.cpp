/*
 Copyright (c) 2008-2009 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

/**
*
*	\file StringConverter.cpp
*	\brief
*	\version
*	\date
*
*/

#ifdef __GNUC__
#	include <cxxabi.h>
#endif
#include <locale>
#include <typeinfo>
#include "Buffer.h"
#include "Exception.h"
#include "ForEach.h"
#include "StringConverter.h"
#include "SystemException.h"

namespace GostCrypt
{
/**
 *
 *	\fn void StringConverter::Erase (string &str)
 *	\brief
 *	\param[in] string &str)
 *	\return
 *
 */
	void StringConverter::Erase (string &str)
	{
		for (size_t i = 0; i < str.size(); ++i)
		{
			str[i] = ' ';
		}
	}

/**
 *
 *	\fn void StringConverter::Erase (wstring &str)
 *	\brief
 *	\param[in] wstring &str)
 *	\return
 *
 */
	void StringConverter::Erase (wstring &str)
	{
		for (size_t i = 0; i < str.size(); ++i)
		{
			str[i] = ' ';
		}
	}

/**
 *
 *	\fn wstring StringConverter::FromNumber (double number)
 *	\brief
 *	\param[in] double number)
 *	\return
 *
 */
	wstring StringConverter::FromNumber (double number)
	{
		wstringstream s;
		s << number;
		return s.str();
	}

/**
 *
 *	\fn wstring StringConverter::FromNumber (int32 number)
 *	\brief
 *	\param[in] int32 number)
 *	\return
 *
 */
	wstring StringConverter::FromNumber (int32 number)
	{
		wstringstream s;
		s << number;
		return s.str();
	}

/**
 *
 *	\fn wstring StringConverter::FromNumber (uint32 number)
 *	\brief
 *	\param[in] uint32 number)
 *	\return
 *
 */
	wstring StringConverter::FromNumber (uint32 number)
	{
		wstringstream s;
		s << number;
		return s.str();
	}

/**
 *
 *	\fn wstring StringConverter::FromNumber (int64 number)
 *	\brief
 *	\param[in] int64 number)
 *	\return
 *
 */
	wstring StringConverter::FromNumber (int64 number)
	{
		wstringstream s;
		s << number;
		return s.str();
	}

/**
 *
 *	\fn wstring StringConverter::FromNumber (uint64 number)
 *	\brief
 *	\param[in] uint64 number)
 *	\return
 *
 */
	wstring StringConverter::FromNumber (uint64 number)
	{
		wstringstream s;
		s << number;
		return s.str();
	}

/**
 *
 *	\fn string StringConverter::GetTrailingNumber (const string &str)
 *	\brief
 *	\param[in] const string
 *	\return
 *
 */
	string StringConverter::GetTrailingNumber (const string &str)
	{
		size_t start = str.find_last_not_of ("0123456789");
		if (start == string::npos)
			return str;

		string s = str.substr (start + 1);
		if (s.empty ())
			throw ParameterIncorrect (SRC_POS);

		return s;
	}

/**
 *
 *	\fn string StringConverter::GetTypeName (const type_info &typeInfo)
 *	\brief
 *	\param[in] const type_info
 *	\return
 *
 */
	string StringConverter::GetTypeName (const type_info &typeInfo)
	{
		try
		{
#ifdef _MSC_VER
			// type_info::name() leaks memory as of MS VC++ 8.0
			string rawName (typeInfo.raw_name());

			size_t cut1 = (rawName.find (".?A") != string::npos) ? 4 : string::npos;
			size_t cut2 = rawName.find ("@");
			size_t cut3 = rawName.find ("@@");

			if (cut1 == string::npos || cut2 == string::npos || cut3 == string::npos)
				return typeInfo.name();

			return rawName.substr (cut2 + 1, cut3 - cut2 - 1) + "::" + rawName.substr (cut1, cut2 - cut1);

#elif defined (__GNUC__)
			int status;
			char *name = abi::__cxa_demangle (typeInfo.name(), nullptr, nullptr, &status);

			if (name)
			{
				string s (name);
				free (name);
				return s;
			}
#endif
		}
		catch (...) { }

		return typeInfo.name();
	}

/**
 *
 *	\fn wstring StringConverter::QuoteSpaces (const wstring &str)
 *	\brief
 *	\param[in] const wstring
 *	\return
 *
 */
	wstring StringConverter::QuoteSpaces (const wstring &str)
	{
		if (str.find (L' ') == string::npos)
			return str;

		wstring escaped (L"'");
		foreach (wchar_t c, str)
		{
			if (c == L'\'')
				escaped += L'\'';
			escaped += c;
		}
		return escaped + L'\'';
	}

/**
 *
 *	\fn vector <string> StringConverter::Split (const string &str, const string &separators, bool returnEmptyFields)
 *	\brief
 *	\param[in] bool returnEmptyFields)
 *	\param[in] const string
 *	\param[in] const string
 *	\return
 *
 */
	vector <string> StringConverter::Split (const string &str, const string &separators, bool returnEmptyFields)
	{
		vector <string> elements;

		if (!returnEmptyFields)
		{
			size_t p = 0;
			while ((p = str.find_first_not_of (separators, p)) != string::npos)
			{
				size_t end = str.find_first_of (separators, p);
				if (end == string::npos)
				{
					elements.push_back (str.substr (p));
					break;
				}

				elements.push_back (str.substr (p, end - p));
				p = end;
			}
		}
		else
		{
			string element;
			elements.push_back (element);
			foreach (char c, str)
			{
				if (separators.find (c) != string::npos)
				{
					element.erase();
					elements.push_back (element);
				}
				else
				{
					elements.back() += c;
				}
			}
		}

		return elements;
	}
	
/**
 *
 *	\fn string StringConverter::StripTrailingNumber (const string &str)
 *	\brief
 *	\param[in] const string
 *	\return
 *
 */
	string StringConverter::StripTrailingNumber (const string &str)
	{
		size_t start = str.find_last_not_of ("0123456789");
		if (start == string::npos)
			return "";

		return str.substr (0, start + 1);
	}

/**
 *
 *	\fn wstring StringConverter::ToExceptionString (const exception &ex)
 *	\brief
 *	\param[in] const exception
 *	\return
 *
 */
	wstring StringConverter::ToExceptionString (const exception &ex)
	{
		const SystemException *sysEx = dynamic_cast <const SystemException *> (&ex);
		if (sysEx)
			return ToWide (sysEx->what()) + L": " + sysEx->SystemText() + L": " + sysEx->GetSubject();

		if (ex.what() && !string (ex.what()).empty())
			return ToWide (GetTypeName (typeid (ex)) + ": " + ex.what());
		
		return ToWide (GetTypeName (typeid (ex)));
	}

/**
 *
 *	\fn string StringConverter::ToLower (const string &str)
 *	\brief
 *	\param[in] const string
 *	\return
 *
 */
	string StringConverter::ToLower (const string &str)
	{
		string s;
		foreach (char c, str)
			s += tolower (c, locale());
		return s;
	}

/**
 *
 *	\fn string StringConverter::ToSingle (const wstring &wstr, bool noThrow)
 *	\brief
 *	\param[in] bool noThrow)
 *	\param[in] const wstring
 *	\return
 *
 */
	string StringConverter::ToSingle (const wstring &wstr, bool noThrow)
	{
		string str;
		ToSingle (wstr, str, noThrow);
		return str;
	}

/**
 *
 *	\fn void StringConverter::ToSingle (const wstring &wstr, string &str, bool noThrow)
 *	\brief
 *	\param[in] bool noThrow)
 *	\param[in] string &str
 *	\param[in] const wstring
 *	\return
 *
 */
	void StringConverter::ToSingle (const wstring &wstr, string &str, bool noThrow)
	{
		try
		{
			mbstate_t mbState;
			Memory::Zero (&mbState, sizeof (mbState));
			const wchar_t *src = wstr.c_str();

			size_t size = wcsrtombs (nullptr, &src, 0, &mbState);
			if (size == (size_t) -1)
				throw StringConversionFailed (SRC_POS, wstr);

			vector <char> buf (size + 1);
			Memory::Zero (&mbState, sizeof (mbState));

			if ((size = wcsrtombs (&buf[0], &src, buf.size(), &mbState)) == (size_t) -1)
				throw StringConversionFailed (SRC_POS, wstr);

			str.clear();
			str.insert (0, &buf.front(), size);
			Memory::Erase (&buf.front(), buf.size());
		}
		catch (...)
		{
			if (!noThrow)
				throw;
		}
	}

/**
 *
 *	\fn uint32 StringConverter::ToUInt32 (const string &str)
 *	\brief
 *	\param[in] const string
 *	\return
 *
 */
	uint32 StringConverter::ToUInt32 (const string &str)
	{
		uint32 n;
		stringstream ss (str);

		ss >> n;
		if (ss.fail() || n == 0xffffFFFFU)
			throw ParameterIncorrect (SRC_POS);

		return n;
	}

/**
 *
 *	\fn uint32 StringConverter::ToUInt32 (const wstring &str)
 *	\brief
 *	\param[in] const wstring
 *	\return
 *
 */
	uint32 StringConverter::ToUInt32 (const wstring &str)
	{
		uint32 n;
		wstringstream ss (str);

		ss >> n;
		if (ss.fail() || n == 0xffffFFFFU)
			throw ParameterIncorrect (SRC_POS);

		return n;
	}

/**
 *
 *	\fn uint64 StringConverter::ToUInt64 (const string &str)
 *	\brief
 *	\param[in] const string
 *	\return
 *
 */
	uint64 StringConverter::ToUInt64 (const string &str)
	{
		uint64 n;
		stringstream ss (str);

		ss >> n;
		if (ss.fail() || n == 0xffffFFFFffffFFFFULL)
			throw ParameterIncorrect (SRC_POS);

		return n;
	}

/**
 *
 *	\fn uint64 StringConverter::ToUInt64 (const wstring &str)
 *	\brief
 *	\param[in] const wstring
 *	\return
 *
 */
	uint64 StringConverter::ToUInt64 (const wstring &str)
	{
		uint64 n;
		wstringstream ss (str);

		ss >> n;
		if (ss.fail() || n == 0xffffFFFFffffFFFFULL)
			throw ParameterIncorrect (SRC_POS);

		return n;
	}
	
/**
 *
 *	\fn string StringConverter::ToUpper (const string &str)
 *	\brief
 *	\param[in] const string
 *	\return
 *
 */
	string StringConverter::ToUpper (const string &str)
	{
		string s;
		foreach (char c, str)
			s += toupper (c, locale());
		return s;
	}

/**
 *
 *	\fn wstring StringConverter::ToWide (const string &str, bool noThrow)
 *	\brief
 *	\param[in] bool noThrow)
 *	\param[in] const string
 *	\return
 *
 */
	wstring StringConverter::ToWide (const string &str, bool noThrow)
	{
		try
		{
			mbstate_t mbState;
			Memory::Zero (&mbState, sizeof (mbState));
			const char *src = str.c_str();

			size_t size = mbsrtowcs (nullptr, &src, 0, &mbState);
			if (size == (size_t) -1)
				throw StringConversionFailed (SRC_POS);

			vector <wchar_t> buf (size + 1);
			Memory::Zero (&mbState, sizeof (mbState));

			if ((size = mbsrtowcs (&buf[0], &src, buf.size(), &mbState)) == (size_t) -1)
				throw StringConversionFailed (SRC_POS);

			wstring s;
			s.insert (s.begin(), buf.begin(), buf.begin() + size);
			return s;
		}
		catch (...)
		{
			if (noThrow)
				return L"";
			throw;
		}
	}

/**
 *
 *	\fn void StringConverter::ToWideBuffer (const wstring &str, wchar_t *buffer, size_t bufferSize)
 *	\brief
 *	\param[in] size_t bufferSize)
 *	\param[in] wchar_t* buffer
 *	\param[in] const wstring
 *	\return
 *
 */
	void StringConverter::ToWideBuffer (const wstring &str, wchar_t *buffer, size_t bufferSize)
	{
		if (str.length() < 1)
		{
			buffer[0] = 0;
			return;
		}

		BufferPtr (
			(byte *) buffer,
			bufferSize).CopyFrom (
				ConstBufferPtr ((byte *) (wstring (str).c_str()),
				(str.length() + 1) * sizeof (wchar_t)
			)
		);
	}

/**
 *
 *	\fn string StringConverter::Trim (const string &str)
 *	\brief
 *	\param[in] const string
 *	\return
 *
 */
	string StringConverter::Trim (const string &str)
	{
		size_t start = 0;
		size_t end = str.size();
		if (end < 1)
			return str;

		foreach (char c, str)
		{
			if (c > ' ')
				break;
			++start;
		}

		foreach_reverse (char c, str)
		{
			if (c > ' ')
				break;
			--end;
		}

		return str.substr (start, end - start);
	}
}
