/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "Platform/Finally.h"
#include "Platform/ForEach.h"

#include "Platform/SerializerFactory.h"
#include "Platform/StringConverter.h"
#include "Platform/SystemException.h"

#ifdef GST_UNIX
#	include <dlfcn.h>
#endif

#include "SecurityToken.h"

#ifndef burn
#	define burn Memory::Erase
#endif

using namespace std;

namespace GostCrypt
{
	SecurityTokenKeyfile::SecurityTokenKeyfile (const SecurityTokenKeyfilePath &path)
	{
		wstring pathStr = path;
		unsigned long slotId;

		if (swscanf (pathStr.c_str(), GST_SECURITY_TOKEN_KEYFILE_URL_PREFIX GST_SECURITY_TOKEN_KEYFILE_URL_SLOT L"/%lu", &slotId) != 1)
			throw InvalidSecurityTokenKeyfilePath();

		SlotId = slotId;

		size_t keyIdPos = pathStr.find (L"/" GST_SECURITY_TOKEN_KEYFILE_URL_FILE L"/");
		if (keyIdPos == string::npos)
			throw InvalidSecurityTokenKeyfilePath();

		Id = pathStr.substr (keyIdPos + wstring (L"/" GST_SECURITY_TOKEN_KEYFILE_URL_FILE L"/").size());

		vector <SecurityTokenKeyfile> keyfiles = SecurityToken::GetAvailableKeyfiles (&SlotId, Id);

		if (keyfiles.empty())
			throw SecurityTokenKeyfileNotFound();

		*this = keyfiles.front();
	}

	SecurityTokenKeyfile::operator SecurityTokenKeyfilePath () const
	{
		wstringstream path;
		path << GST_SECURITY_TOKEN_KEYFILE_URL_PREFIX GST_SECURITY_TOKEN_KEYFILE_URL_SLOT L"/" << SlotId << L"/" GST_SECURITY_TOKEN_KEYFILE_URL_FILE L"/" << Id;
		return path.str();
	}

	void SecurityToken::CheckLibraryStatus ()
	{
		if (!Initialized)
			throw SecurityTokenLibraryNotInitialized();
	}

	void SecurityToken::CloseLibrary ()
	{
		if (Initialized)
		{
			CloseAllSessions();
			Pkcs11Functions->C_Finalize (NULL_PTR);

			dlclose (Pkcs11LibraryHandle);

			Initialized = false;
		}
	}

	void SecurityToken::CloseAllSessions () throw ()
	{
		if (!Initialized)
			return;

		typedef pair <CK_SLOT_ID, Pkcs11Session> SessionMapPair;

		foreach (SessionMapPair p, Sessions)
		{
			try
			{
				CloseSession (p.first);
			}
			catch (...) { }
		}
	}

	void SecurityToken::CloseSession (CK_SLOT_ID slotId)
	{
		if (Sessions.find (slotId) == Sessions.end())
			throw ParameterIncorrect (SRC_POS);

		Pkcs11Functions->C_CloseSession (Sessions[slotId].Handle);
		Sessions.erase (Sessions.find (slotId));
	}

	void SecurityToken::CreateKeyfile (CK_SLOT_ID slotId, vector <byte> &keyfileData, const string &name)
	{
		if (name.empty())
			throw ParameterIncorrect (SRC_POS);

		LoginUserIfRequired (slotId);

		foreach (const SecurityTokenKeyfile &keyfile, GetAvailableKeyfiles (&slotId))
		{
			if (keyfile.IdUtf8 == name)
				throw SecurityTokenKeyfileAlreadyExists();
		}

		CK_OBJECT_CLASS dataClass = CKO_DATA;
		CK_BBOOL trueVal = CK_TRUE;

		CK_ATTRIBUTE keyfileTemplate[] =
		{
			{ CKA_CLASS, &dataClass, sizeof (dataClass) },
			{ CKA_TOKEN, &trueVal, sizeof (trueVal) },
			{ CKA_PRIVATE, &trueVal, sizeof (trueVal) },
			{ CKA_LABEL, (CK_UTF8CHAR *) name.c_str(), name.size() },
			{ CKA_VALUE, &keyfileData.front(), keyfileData.size() }
		};

		CK_OBJECT_HANDLE keyfileHandle;

		CK_RV status = Pkcs11Functions->C_CreateObject (Sessions[slotId].Handle, keyfileTemplate, array_capacity (keyfileTemplate), &keyfileHandle);

		switch (status)
		{
		case CKR_DATA_LEN_RANGE:
			status = CKR_DEVICE_MEMORY;
			break;

		case CKR_SESSION_READ_ONLY:
			status = CKR_TOKEN_WRITE_PROTECTED;
			break;
		}

		if (status != CKR_OK)
			throw Pkcs11Exception (status);

		// Some tokens report success even if the new object was truncated to fit in the available memory
		vector <byte> objectData;

		GetObjectAttribute (slotId, keyfileHandle, CKA_VALUE, objectData);
		finally_do_arg (vector <byte> *, &objectData, { if (!finally_arg->empty()) burn (&finally_arg->front(), finally_arg->size()); });

		if (objectData.size() != keyfileData.size())
		{
			Pkcs11Functions->C_DestroyObject (Sessions[slotId].Handle, keyfileHandle);
			throw Pkcs11Exception (CKR_DEVICE_MEMORY);
		}
	}

	void SecurityToken::DeleteKeyfile (const SecurityTokenKeyfile &keyfile)
	{
		LoginUserIfRequired (keyfile.SlotId);
		
		CK_RV status = Pkcs11Functions->C_DestroyObject (Sessions[keyfile.SlotId].Handle, keyfile.Handle);
		if (status != CKR_OK)
			throw Pkcs11Exception (status);
	}

	vector <SecurityTokenKeyfile> SecurityToken::GetAvailableKeyfiles (CK_SLOT_ID *slotIdFilter, const wstring keyfileIdFilter)
	{
		bool unrecognizedTokenPresent = false;
		vector <SecurityTokenKeyfile> keyfiles;

		foreach (const CK_SLOT_ID &slotId, GetTokenSlots())
		{
			SecurityTokenInfo token;

			if (slotIdFilter && *slotIdFilter != slotId)
				continue;

			try
			{
				LoginUserIfRequired (slotId);
				token = GetTokenInfo (slotId);
			}
			catch (UserAbort &)
			{
				continue;
			}
			catch (Pkcs11Exception &e)
			{
				if (e.GetErrorCode() == CKR_TOKEN_NOT_RECOGNIZED)
				{
					unrecognizedTokenPresent = true;
					continue;
				}

				throw;
			}

			foreach (const CK_OBJECT_HANDLE &dataHandle, GetObjects (slotId, CKO_DATA))
			{
				SecurityTokenKeyfile keyfile;
				keyfile.Handle = dataHandle;
				keyfile.SlotId = slotId;
				keyfile.Token = token;

				vector <byte> privateAttrib;
				GetObjectAttribute (slotId, dataHandle, CKA_PRIVATE, privateAttrib);

				if (privateAttrib.size() == sizeof (CK_BBOOL) && *(CK_BBOOL *) &privateAttrib.front() != CK_TRUE)
					continue;

				vector <byte> label;
				GetObjectAttribute (slotId, dataHandle, CKA_LABEL, label);
				label.push_back (0);

				keyfile.IdUtf8 = (char *) &label.front();

				keyfile.Id = StringConverter::ToWide ((const char *) &label.front());
				
				if (keyfile.Id.empty() || (!keyfileIdFilter.empty() && keyfileIdFilter != keyfile.Id))
					continue;

				keyfiles.push_back (keyfile);

				if (!keyfileIdFilter.empty())
					break;
			}
		}

		if (keyfiles.empty() && unrecognizedTokenPresent)
			throw Pkcs11Exception (CKR_TOKEN_NOT_RECOGNIZED);

		return keyfiles;
	}

	list <SecurityTokenInfo> SecurityToken::GetAvailableTokens ()
	{
		bool unrecognizedTokenPresent = false;
		list <SecurityTokenInfo> tokens;

		foreach (const CK_SLOT_ID &slotId, GetTokenSlots())
		{
			try
			{
				tokens.push_back (GetTokenInfo (slotId));
			}
			catch (Pkcs11Exception &e)
			{
				if (e.GetErrorCode() == CKR_TOKEN_NOT_RECOGNIZED)
				{
					unrecognizedTokenPresent = true;
					continue;
				}

				throw;
			}
		}

		if (tokens.empty() && unrecognizedTokenPresent)
			throw Pkcs11Exception (CKR_TOKEN_NOT_RECOGNIZED);

		return tokens;
	}

	SecurityTokenInfo SecurityToken::GetTokenInfo (CK_SLOT_ID slotId)
	{
		CK_TOKEN_INFO info;
		CK_RV status = Pkcs11Functions->C_GetTokenInfo (slotId, &info);
		if (status != CKR_OK)
			throw Pkcs11Exception (status);

		SecurityTokenInfo token;
		token.SlotId = slotId;
		token.Flags = info.flags;

		char label[sizeof (info.label) + 1];
		memset (label, 0, sizeof (label));
		memcpy (label, info.label, sizeof (info.label));

		token.LabelUtf8 = label;

		size_t lastSpace = token.LabelUtf8.find_last_not_of (' ');
		if (lastSpace == string::npos)
			token.LabelUtf8.clear();
		else
			token.LabelUtf8 = token.LabelUtf8.substr (0, lastSpace + 1);

		token.Label = StringConverter::ToWide (token.LabelUtf8);

		return token;
	}

	void SecurityToken::GetKeyfileData (const SecurityTokenKeyfile &keyfile, vector <byte> &keyfileData)
	{
		LoginUserIfRequired (keyfile.SlotId);
		GetObjectAttribute (keyfile.SlotId, keyfile.Handle, CKA_VALUE, keyfileData);
	}

	vector <CK_OBJECT_HANDLE> SecurityToken::GetObjects (CK_SLOT_ID slotId, CK_ATTRIBUTE_TYPE objectClass)
	{
		if (Sessions.find (slotId) == Sessions.end())
			throw ParameterIncorrect (SRC_POS);

		CK_ATTRIBUTE findTemplate;
		findTemplate.type = CKA_CLASS;
		findTemplate.pValue = &objectClass;
		findTemplate.ulValueLen = sizeof (objectClass);

		CK_RV status = Pkcs11Functions->C_FindObjectsInit (Sessions[slotId].Handle, &findTemplate, 1);
		if (status != CKR_OK)
			throw Pkcs11Exception (status);

		finally_do_arg (CK_SLOT_ID, slotId, { Pkcs11Functions->C_FindObjectsFinal (Sessions[finally_arg].Handle); });

		CK_ULONG objectCount;	
		vector <CK_OBJECT_HANDLE> objects;

		while (true)
		{
			CK_OBJECT_HANDLE object;
			CK_RV status = Pkcs11Functions->C_FindObjects (Sessions[slotId].Handle, &object, 1, &objectCount);
			if (status != CKR_OK)
				throw Pkcs11Exception (status);

			if (objectCount != 1)
				break;

			objects.push_back (object);
		}

		return objects;
	}

	void SecurityToken::GetObjectAttribute (CK_SLOT_ID slotId, CK_OBJECT_HANDLE tokenObject, CK_ATTRIBUTE_TYPE attributeType, vector <byte> &attributeValue)
	{
		attributeValue.clear();

		if (Sessions.find (slotId) == Sessions.end())
			throw ParameterIncorrect (SRC_POS);

		CK_ATTRIBUTE attribute;
		attribute.type = attributeType;
		attribute.pValue = NULL_PTR;

		CK_RV status = Pkcs11Functions->C_GetAttributeValue (Sessions[slotId].Handle, tokenObject, &attribute, 1);
		if (status != CKR_OK)
			throw Pkcs11Exception (status);

		if (attribute.ulValueLen == 0)
			return;

		attributeValue = vector <byte> (attribute.ulValueLen);
		attribute.pValue = &attributeValue.front();

		status = Pkcs11Functions->C_GetAttributeValue (Sessions[slotId].Handle, tokenObject, &attribute, 1);
		if (status != CKR_OK)
			throw Pkcs11Exception (status);
	}

	list <CK_SLOT_ID> SecurityToken::GetTokenSlots ()
	{
		CheckLibraryStatus();

        list <CK_SLOT_ID> gslots;
		CK_ULONG slotCount;

		CK_RV status = Pkcs11Functions->C_GetSlotList (TRUE, NULL_PTR, &slotCount);
		if (status != CKR_OK)
			throw Pkcs11Exception (status);

		if (slotCount > 0)
		{
			vector <CK_SLOT_ID> slotArray (slotCount);
			status = Pkcs11Functions->C_GetSlotList (TRUE, &slotArray.front(), &slotCount);
			if (status != CKR_OK)
				throw Pkcs11Exception (status);

			for (size_t i = 0; i < slotCount; i++)
			{
				CK_SLOT_INFO slotInfo;
				status = Pkcs11Functions->C_GetSlotInfo (slotArray[i], &slotInfo);

				if (status != CKR_OK || !(slotInfo.flags & CKF_TOKEN_PRESENT))
					continue;

                gslots.push_back (slotArray[i]);
			}
		}

        return gslots;
	}

	bool SecurityToken::IsKeyfilePathValid (const wstring &securityTokenKeyfilePath)
	{
		return securityTokenKeyfilePath.find (GST_SECURITY_TOKEN_KEYFILE_URL_PREFIX) == 0;
	}

	void SecurityToken::Login (CK_SLOT_ID slotId, const string &pin)
	{
		if (Sessions.find (slotId) == Sessions.end())
			OpenSession (slotId);
		else if (Sessions[slotId].UserLoggedIn)
			return;

		CK_RV status = Pkcs11Functions->C_Login (Sessions[slotId].Handle, CKU_USER, (CK_CHAR_PTR) pin.c_str(), pin.size());

		if (status != CKR_OK)
			throw Pkcs11Exception (status);

		Sessions[slotId].UserLoggedIn = true;
	}
	
	void SecurityToken::LoginUserIfRequired (CK_SLOT_ID slotId)
	{
		CheckLibraryStatus();
		CK_RV status;

		if (Sessions.find (slotId) == Sessions.end())
		{
			OpenSession (slotId);
		}
		else
		{
			CK_SESSION_INFO sessionInfo;
			status = Pkcs11Functions->C_GetSessionInfo (Sessions[slotId].Handle, &sessionInfo);
			
			if (status == CKR_OK)
			{
				Sessions[slotId].UserLoggedIn = (sessionInfo.state == CKS_RO_USER_FUNCTIONS || sessionInfo.state == CKS_RW_USER_FUNCTIONS);
			}
			else
			{
				try
				{
					CloseSession (slotId);
				}
				catch (...) { }
				OpenSession (slotId);
			}
		}

		SecurityTokenInfo tokenInfo = GetTokenInfo (slotId);

		while (!Sessions[slotId].UserLoggedIn && (tokenInfo.Flags & CKF_LOGIN_REQUIRED))
		{
			try
			{
				if (tokenInfo.Flags & CKF_PROTECTED_AUTHENTICATION_PATH)
				{
					status = Pkcs11Functions->C_Login (Sessions[slotId].Handle, CKU_USER, NULL_PTR, 0);
					if (status != CKR_OK)
						throw Pkcs11Exception (status);
				}
				else
				{
					string pin = tokenInfo.LabelUtf8;
					if (tokenInfo.Label.empty())
					{
						stringstream s;
						s << "#" << slotId;
						pin = s.str();
					}

					finally_do_arg (string*, &pin, { burn ((void *) finally_arg->c_str(), finally_arg->size()); });

					(*PinCallback) (pin);
					Login (slotId, pin);
				}

				Sessions[slotId].UserLoggedIn = true;
			}
			catch (Pkcs11Exception &e)
			{
				CK_RV error = e.GetErrorCode();

				if (error == CKR_USER_ALREADY_LOGGED_IN)
				{
					break;
				}
				else if (error == CKR_PIN_INCORRECT && !(tokenInfo.Flags & CKF_PROTECTED_AUTHENTICATION_PATH))
				{
					(*WarningCallback) (Pkcs11Exception (CKR_PIN_INCORRECT));
					continue;
				}

				throw;
			}
		}
	}

    void SecurityToken::InitLibrary (const string &pkcs11LibraryPath, QSharedPointer <GetPinFunctor> pinCallback, QSharedPointer <SendExceptionFunctor> warningCallback)
	{
		if (Initialized)
			CloseLibrary();

		Pkcs11LibraryHandle = dlopen (pkcs11LibraryPath.c_str(), RTLD_NOW | RTLD_LOCAL);

		throw_sys_if (!Pkcs11LibraryHandle);

		typedef CK_RV (*C_GetFunctionList_t) (CK_FUNCTION_LIST_PTR_PTR ppFunctionList);

		C_GetFunctionList_t C_GetFunctionList = (C_GetFunctionList_t) dlsym (Pkcs11LibraryHandle, "C_GetFunctionList");

		if (!C_GetFunctionList)
			throw SecurityTokenLibraryNotInitialized();

		CK_RV status = C_GetFunctionList (&Pkcs11Functions);
		if (status != CKR_OK)
			throw Pkcs11Exception (status);

		status = Pkcs11Functions->C_Initialize (NULL_PTR);
		if (status != CKR_OK)
			throw Pkcs11Exception (status);

		PinCallback = pinCallback;
		WarningCallback = warningCallback;

		Initialized = true;
	}

	void SecurityToken::OpenSession (CK_SLOT_ID slotId)
	{
		if (Sessions.find (slotId) != Sessions.end())
			return;

		CK_SESSION_HANDLE session;

		CK_FLAGS flags = CKF_SERIAL_SESSION;

		if (!(GetTokenInfo (slotId).Flags & CKF_WRITE_PROTECTED))
			 flags |= CKF_RW_SESSION;

		CK_RV status = Pkcs11Functions->C_OpenSession (slotId, flags, NULL_PTR, NULL_PTR, &session);
		if (status != CKR_OK)
			throw Pkcs11Exception (status);

		Sessions[slotId].Handle = session;
	}

	Pkcs11Exception::operator string () const
	{
		if (ErrorCode == CKR_OK)
			return string();

		static const struct
		{
			CK_RV ErrorCode;
			const char *ErrorString;
		} ErrorStrings[] =
		{
#			define GST_TOKEN_ERR(CODE) { CODE, #CODE },

			GST_TOKEN_ERR (CKR_CANCEL)
			GST_TOKEN_ERR (CKR_HOST_MEMORY)
			GST_TOKEN_ERR (CKR_SLOT_ID_INVALID)
			GST_TOKEN_ERR (CKR_GENERAL_ERROR)
			GST_TOKEN_ERR (CKR_FUNCTION_FAILED)
			GST_TOKEN_ERR (CKR_ARGUMENTS_BAD)
			GST_TOKEN_ERR (CKR_NO_EVENT)
			GST_TOKEN_ERR (CKR_NEED_TO_CREATE_THREADS)
			GST_TOKEN_ERR (CKR_CANT_LOCK)
			GST_TOKEN_ERR (CKR_ATTRIBUTE_READ_ONLY)
			GST_TOKEN_ERR (CKR_ATTRIBUTE_SENSITIVE)
			GST_TOKEN_ERR (CKR_ATTRIBUTE_TYPE_INVALID)
			GST_TOKEN_ERR (CKR_ATTRIBUTE_VALUE_INVALID)
			GST_TOKEN_ERR (CKR_DATA_INVALID)
			GST_TOKEN_ERR (CKR_DATA_LEN_RANGE)
			GST_TOKEN_ERR (CKR_DEVICE_ERROR)
			GST_TOKEN_ERR (CKR_DEVICE_MEMORY)
			GST_TOKEN_ERR (CKR_DEVICE_REMOVED)
			GST_TOKEN_ERR (CKR_ENCRYPTED_DATA_INVALID)
			GST_TOKEN_ERR (CKR_ENCRYPTED_DATA_LEN_RANGE)
			GST_TOKEN_ERR (CKR_FUNCTION_CANCELED)
			GST_TOKEN_ERR (CKR_FUNCTION_NOT_PARALLEL)
			GST_TOKEN_ERR (CKR_FUNCTION_NOT_SUPPORTED)
			GST_TOKEN_ERR (CKR_KEY_HANDLE_INVALID)
			GST_TOKEN_ERR (CKR_KEY_SIZE_RANGE)
			GST_TOKEN_ERR (CKR_KEY_TYPE_INCONSISTENT)
			GST_TOKEN_ERR (CKR_KEY_NOT_NEEDED)
			GST_TOKEN_ERR (CKR_KEY_CHANGED)
			GST_TOKEN_ERR (CKR_KEY_NEEDED)
			GST_TOKEN_ERR (CKR_KEY_INDIGESTIBLE)
			GST_TOKEN_ERR (CKR_KEY_FUNCTION_NOT_PERMITTED)
			GST_TOKEN_ERR (CKR_KEY_NOT_WRAPPABLE)
			GST_TOKEN_ERR (CKR_KEY_UNEXTRACTABLE)
			GST_TOKEN_ERR (CKR_MECHANISM_INVALID)
			GST_TOKEN_ERR (CKR_MECHANISM_PARAM_INVALID)
			GST_TOKEN_ERR (CKR_OBJECT_HANDLE_INVALID)
			GST_TOKEN_ERR (CKR_OPERATION_ACTIVE)
			GST_TOKEN_ERR (CKR_OPERATION_NOT_INITIALIZED)
			GST_TOKEN_ERR (CKR_PIN_INCORRECT)
			GST_TOKEN_ERR (CKR_PIN_INVALID)
			GST_TOKEN_ERR (CKR_PIN_LEN_RANGE)
			GST_TOKEN_ERR (CKR_PIN_EXPIRED)
			GST_TOKEN_ERR (CKR_PIN_LOCKED)
			GST_TOKEN_ERR (CKR_SESSION_CLOSED)
			GST_TOKEN_ERR (CKR_SESSION_COUNT)
			GST_TOKEN_ERR (CKR_SESSION_HANDLE_INVALID)
			GST_TOKEN_ERR (CKR_SESSION_PARALLEL_NOT_SUPPORTED)
			GST_TOKEN_ERR (CKR_SESSION_READ_ONLY)
			GST_TOKEN_ERR (CKR_SESSION_EXISTS)
			GST_TOKEN_ERR (CKR_SESSION_READ_ONLY_EXISTS)
			GST_TOKEN_ERR (CKR_SESSION_READ_WRITE_SO_EXISTS)
			GST_TOKEN_ERR (CKR_SIGNATURE_INVALID)
			GST_TOKEN_ERR (CKR_SIGNATURE_LEN_RANGE)
			GST_TOKEN_ERR (CKR_TEMPLATE_INCOMPLETE)
			GST_TOKEN_ERR (CKR_TEMPLATE_INCONSISTENT)
			GST_TOKEN_ERR (CKR_TOKEN_NOT_PRESENT)
			GST_TOKEN_ERR (CKR_TOKEN_NOT_RECOGNIZED)
			GST_TOKEN_ERR (CKR_TOKEN_WRITE_PROTECTED)
			GST_TOKEN_ERR (CKR_UNWRAPPING_KEY_HANDLE_INVALID)
			GST_TOKEN_ERR (CKR_UNWRAPPING_KEY_SIZE_RANGE)
			GST_TOKEN_ERR (CKR_UNWRAPPING_KEY_TYPE_INCONSISTENT)
			GST_TOKEN_ERR (CKR_USER_ALREADY_LOGGED_IN)
			GST_TOKEN_ERR (CKR_USER_NOT_LOGGED_IN)
			GST_TOKEN_ERR (CKR_USER_PIN_NOT_INITIALIZED)
			GST_TOKEN_ERR (CKR_USER_TYPE_INVALID)
			GST_TOKEN_ERR (CKR_USER_ANOTHER_ALREADY_LOGGED_IN)
			GST_TOKEN_ERR (CKR_USER_TOO_MANY_TYPES)
			GST_TOKEN_ERR (CKR_WRAPPED_KEY_INVALID)
			GST_TOKEN_ERR (CKR_WRAPPED_KEY_LEN_RANGE)
			GST_TOKEN_ERR (CKR_WRAPPING_KEY_HANDLE_INVALID)
			GST_TOKEN_ERR (CKR_WRAPPING_KEY_SIZE_RANGE)
			GST_TOKEN_ERR (CKR_WRAPPING_KEY_TYPE_INCONSISTENT)
			GST_TOKEN_ERR (CKR_RANDOM_SEED_NOT_SUPPORTED)
			GST_TOKEN_ERR (CKR_RANDOM_NO_RNG)
			GST_TOKEN_ERR (CKR_DOMAIN_PARAMS_INVALID)
			GST_TOKEN_ERR (CKR_BUFFER_TOO_SMALL)
			GST_TOKEN_ERR (CKR_SAVED_STATE_INVALID)
			GST_TOKEN_ERR (CKR_INFORMATION_SENSITIVE)
			GST_TOKEN_ERR (CKR_STATE_UNSAVEABLE)
			GST_TOKEN_ERR (CKR_CRYPTOKI_NOT_INITIALIZED)
			GST_TOKEN_ERR (CKR_CRYPTOKI_ALREADY_INITIALIZED)
			GST_TOKEN_ERR (CKR_MUTEX_BAD)
			GST_TOKEN_ERR (CKR_MUTEX_NOT_LOCKED)
			GST_TOKEN_ERR (CKR_NEW_PIN_MODE)
			GST_TOKEN_ERR (CKR_NEXT_OTP)
			GST_TOKEN_ERR (CKR_FUNCTION_REJECTED)

#undef		GST_TOKEN_ERR
		};


		for (size_t i = 0; i < array_capacity (ErrorStrings); ++i)
		{
			if (ErrorStrings[i].ErrorCode == ErrorCode)
				return ErrorStrings[i].ErrorString;
		}

		stringstream s;
		s << "0x" << hex << ErrorCode;
		return s.str();

	}

#ifdef GST_HEADER_Common_Exception
	void Pkcs11Exception::Show (HWND parent) const
	{
		string errorString = string (*this);

		if (!errorString.empty())
		{
			wstringstream subjectErrorCode;
			if (SubjectErrorCodeValid)
				subjectErrorCode << L": " << SubjectErrorCode;

			if (!GetDictionaryValue (errorString.c_str()))
			{
				if (errorString.find ("CKR_") == 0)
				{
					errorString = errorString.substr (4);
					for (size_t i = 0; i < errorString.size(); ++i)
					{
						if (errorString[i] == '_')
							errorString[i] = ' ';
					}
				}
				wchar_t err[8192];
				wsprintfW (err, L"%s:\n\n%hs%s", GetString ("SECURITY_TOKEN_ERROR"), errorString.c_str(), subjectErrorCode.str().c_str());
				ErrorDirect (err);
			}
			else
			{
				wstring err = GetString (errorString.c_str());

				if (SubjectErrorCodeValid)
					err += L"\n\nError code" + subjectErrorCode.str();

				ErrorDirect (err.c_str());
			}
		}
	}
#endif // GST_HEADER_Common_Exception

    QSharedPointer <GetPinFunctor> SecurityToken::PinCallback;
    QSharedPointer <SendExceptionFunctor> SecurityToken::WarningCallback;

	bool SecurityToken::Initialized;
	CK_FUNCTION_LIST_PTR SecurityToken::Pkcs11Functions;
	map <CK_SLOT_ID, Pkcs11Session> SecurityToken::Sessions;

	void *SecurityToken::Pkcs11LibraryHandle;

#ifdef GST_HEADER_Platform_Exception

    void Pkcs11Exception::Deserialize (QSharedPointer <Stream> stream)
	{
		Exception::Deserialize (stream);
		Serializer sr (stream);
		uint64 code;
		sr.Deserialize ("ErrorCode", code);
		sr.Deserialize ("SubjectErrorCodeValid", SubjectErrorCodeValid);
		sr.Deserialize ("SubjectErrorCode", SubjectErrorCode);
		ErrorCode = (CK_RV) code;
	}

    void Pkcs11Exception::Serialize (QSharedPointer <Stream> stream) const
	{
		Exception::Serialize (stream);
		Serializer sr (stream);
		sr.Serialize ("ErrorCode", (uint64) ErrorCode);
		sr.Serialize ("SubjectErrorCodeValid", SubjectErrorCodeValid);
		sr.Serialize ("SubjectErrorCode", SubjectErrorCode);
	}

#	define GST_EXCEPTION(TYPE) GST_SERIALIZER_FACTORY_ADD(TYPE)
#	undef GST_EXCEPTION_NODECL
#	define GST_EXCEPTION_NODECL(TYPE) GST_SERIALIZER_FACTORY_ADD(TYPE)

	GST_SERIALIZER_FACTORY_ADD_EXCEPTION_SET (SecurityTokenException);

#endif
}
