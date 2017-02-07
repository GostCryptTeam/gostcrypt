/**
 *
 *	\file BootEncryption.cpp
 *	\brief Class Elevator with its function :
 *				AddReference
 *				CallDriver
 *				ReadWriteFile
 *				RegisterFilterDriver
 *				Release
 *				SetDriverServiceStartType
 *	\version 1.3.1
 *	\date 25/01/2017
 *
 */

/*
 *
 *	Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.
 *
 *	Governed by the TrueCrypt License 3.0 the full text of which is contained in
 *	the file License.txt included in TrueCrypt binary and source code distribution
 *	packages.
 *
 */

#include "Gstdefs.h"
#include "Platform/Finally.h"
#include "Platform/ForEach.h"
#include <Setupapi.h>
#include <devguid.h>
#include <io.h>
#include <shlobj.h>
#include <atlbase.h>
#include "BootEncryption.h"
#include "Boot/Windows/BootCommon.h"
#include "Common/Resource.h"
#include "Crc.h"
#include "Crypto.h"
#include "Dlgcode.h"
#include "Endian.h"
#include "Language.h"
#include "Random.h"
#include "Registry.h"
#include "Volumes.h"

#ifdef VOLFORMAT
#include "Format/FormatCom.h"
#elif defined (GSTMOUNT)
#include "Mount/MainCom.h"
#endif

namespace GostCrypt
{
#if !defined (SETUP)

	class Elevator
	{
	public:
		
		/**
		 *
		 *	\fn static void AddReference ()
		 *	\brief Add a reference
		 *	\return static void
		 *
		 */
		static void AddReference ()
		{
			++ReferenceCount;
		}


		/**
		 *
		 *	\fn static void CallDriver (DWORD ioctl, void *input, DWORD inputSize, void *output, DWORD outputSize)
		 *	\brief Call the driver
		 *	\param[in] DWORD outputSize
		 *	\param[in] void * output
		 *	\param[in] DWORD inputSize
		 *	\param[in] void * input
		 *	\param[in] DWORD ioctl
		 *	\return static void
		 *
		 */
		static void CallDriver (DWORD ioctl, void *input, DWORD inputSize, void *output, DWORD outputSize)
		{
			Elevate();

			CComBSTR inputBstr;
			if (input && inputBstr.AppendBytes ((const char *) input, inputSize) != S_OK)
				throw ParameterIncorrect (SRC_POS);

			CComBSTR outputBstr;
			if (output && outputBstr.AppendBytes ((const char *) output, outputSize) != S_OK)
				throw ParameterIncorrect (SRC_POS);

			DWORD result = ElevatedComInstance->CallDriver (ioctl, inputBstr, &outputBstr);

			if (output)
				memcpy (output, *(void **) &outputBstr, outputSize);

			if (result != ERROR_SUCCESS)
			{
				SetLastError (result);
				throw SystemException();
			}
		}

		/**
		 *
		 *	\fn static void CopyFile (const string &sourceFile, const string &destinationFile)
		 *	\brief Copy a file
		 *	\param[in] const string & destinationFile
		 *	\param[in] const string & sourceFile
		 *	\return static void
		 *
		 */
		static void CopyFile (const string &sourceFile, const string &destinationFile)
		{
			Elevate();

			DWORD result = ElevatedComInstance->CopyFile (CComBSTR (sourceFile.c_str()), CComBSTR (destinationFile.c_str()));

			if (result != ERROR_SUCCESS)
			{
				SetLastError (result);
				throw SystemException();
			}
		}

		/**
		 *
		 *	\fn static void DeleteFile (const string &file)
		 *	\brief Delete a file
		 *	\param[in] const string * file
		 *	\return static void
		 *
		 */
		static void DeleteFile (const string &file)
		{
			Elevate();

			DWORD result = ElevatedComInstance->DeleteFile (CComBSTR (file.c_str()));

			if (result != ERROR_SUCCESS)
			{
				SetLastError (result);
				throw SystemException();
			}
		}

		/**
		 *
		 *	\fn static void ReadWriteFile (BOOL write, BOOL device, const string &filePath, byte *buffer, uint64 offset, uint32 size, DWORD *sizeDone)
		 *	\brief Process writing and reading on file
		 *	\param[in] DWORD * sizeDone
		 *	\param[in] uint32 size
		 *	\param[in] uint64 offset
		 *	\param[in] byte * buffer
		 *	\param[in] const string
		 *	\param[in] BOOL device
		 *	\param[in] BOOL write
		 *	\return static void
		 *
		 */
		static void ReadWriteFile (BOOL write, BOOL device, const string &filePath, byte *buffer, uint64 offset, uint32 size, DWORD *sizeDone)
		{
			Elevate();

			CComBSTR bufferBstr;
			if (bufferBstr.AppendBytes ((const char *) buffer, size) != S_OK)
				throw ParameterIncorrect (SRC_POS);
			DWORD result = ElevatedComInstance->ReadWriteFile (write, device, CComBSTR (filePath.c_str()), &bufferBstr, offset, size, sizeDone);

			if (result != ERROR_SUCCESS)
			{
				SetLastError (result);
				throw SystemException();
			}

			if (!write)
				memcpy (buffer, (BYTE *) bufferBstr.m_str, size);
		}

		/**
		 *
		 *	\fn static BOOL IsPagingFileActive (BOOL checkNonWindowsPartitionsOnly)
		 *	\brief Check if paging file is active
		 *	\param[in] BOOL checkNonWindowsPartitionsOnly)
		 *	\return BOOL ElevatedComInstance->IsPagingFileActive (checkNonWindowsPartitionsOnly)
		 *
		 */
		static BOOL IsPagingFileActive (BOOL checkNonWindowsPartitionsOnly)
		{
			Elevate();

			return ElevatedComInstance->IsPagingFileActive (checkNonWindowsPartitionsOnly);
		}

		/**
		 *
		 *	\fn static void WriteLocalMachineRegistryDwordValue (char *keyPath, char *valueName, DWORD value)
		 *	\brief Write the DWORD value in the local machine registry
		 *	\param[in] DWORD value
		 *	\param[in] char * valueName
		 *	\param[in] char * keyPath
		 *	\return stati void
		 *
		 */
		static void WriteLocalMachineRegistryDwordValue (char *keyPath, char *valueName, DWORD value)
		{
			Elevate();

			DWORD result = ElevatedComInstance->WriteLocalMachineRegistryDwordValue (CComBSTR (keyPath), CComBSTR (valueName), value);
		
			if (result != ERROR_SUCCESS)
			{
				SetLastError (result);
				throw SystemException();
			}
		}

		/**
		 *
		 *	\fn static void RegisterFilterDriver (bool registerDriver, BootEncryption::FilterType filterType)
		 *	\brief Register the filter driver
		 *	\param[in] BootEncryption::FilterType filterType
		 *	\param[in] bool registerDriver
		 *	\return static void
		 *
		 */
		static void RegisterFilterDriver (bool registerDriver, BootEncryption::FilterType filterType)
		{
			Elevate();

			DWORD result = ElevatedComInstance->RegisterFilterDriver (registerDriver ? TRUE : FALSE, filterType);
			if (result != ERROR_SUCCESS)
			{
				SetLastError (result);
				throw SystemException();
			}
		}

		/**
		 *
		 *	\fn static void RegisterSystemFavoritesService (BOOL registerService)
		 *	\brief Register the system favorites service
		 *	\param[in] BOOL registerService
		 *	\return static void
		 *
		 */
		static void RegisterSystemFavoritesService (BOOL registerService)
		{
			Elevate();

			DWORD result = ElevatedComInstance->RegisterSystemFavoritesService (registerService);
			if (result != ERROR_SUCCESS)
			{
				SetLastError (result);
				throw SystemException();
			}
		}

		/**
		 *
		 *	\fn static void Release ()
		 *	\brief Release the instance
		 *	\return static void
		 *
		 */
		static void Release ()
		{
			if (--ReferenceCount == 0 && ElevatedComInstance)
			{
				ElevatedComInstance->Release();
				ElevatedComInstance = nullptr;
				CoUninitialize ();
			}
		}

		/**
		 *
		 *	\fn static void SetDriverServiceStartType (DWORD startType)
		 *	\brief the driver services start type
		 *	\param[in] DWORD startType
		 *	\return static void
		 *
		 */
		static void SetDriverServiceStartType (DWORD startType)
		{
			Elevate();

			DWORD result = ElevatedComInstance->SetDriverServiceStartType (startType);
			if (result != ERROR_SUCCESS)
			{
				SetLastError (result);
				throw SystemException();
			}
		}

	protected:

		/**
		 *
		 *	\fn static void Elevate ()
		 *	\brief Elevate the instance
		 *	\return static void
		 *
		 */
		static void Elevate ()
		{
			if (IsAdmin())
			{
				SetLastError (ERROR_ACCESS_DENIED);
				throw SystemException();
			}

			if (!ElevatedComInstance || ElevatedComInstanceThreadId != GetCurrentThreadId())
			{
				CoInitialize (NULL);
				ElevatedComInstance = GetElevatedInstance (GetActiveWindow() ? GetActiveWindow() : MainDlg);
				ElevatedComInstanceThreadId = GetCurrentThreadId();
			}
		}
		
#if defined (GSTMOUNT)
		static IGostCryptMainCom *ElevatedComInstance;
#elif defined (VOLFORMAT)
		static IGostCryptFormatCom *ElevatedComInstance;
#endif
		static DWORD ElevatedComInstanceThreadId;
		static int ReferenceCount;
	};

#if defined (GSTMOUNT)
	IGostCryptMainCom *Elevator::ElevatedComInstance;
#elif defined (VOLFORMAT)
	IGostCryptFormatCom *Elevator::ElevatedComInstance;
#endif
	DWORD Elevator::ElevatedComInstanceThreadId;
	int Elevator::ReferenceCount = 0;

#else // SETUP
	
	class Elevator
	{
	public:
		static void AddReference () { }
		static void CallDriver (DWORD ioctl, void *input, DWORD inputSize, void *output, DWORD outputSize) { throw ParameterIncorrect (SRC_POS); }
		static void ReadWriteFile (BOOL write, BOOL device, const string &filePath, byte *buffer, uint64 offset, uint32 size, DWORD *sizeDone) { throw ParameterIncorrect (SRC_POS); }
		static void RegisterFilterDriver (bool registerDriver, BootEncryption::FilterType filterType) { throw ParameterIncorrect (SRC_POS); }
		static void Release () { }
		static void SetDriverServiceStartType (DWORD startType) { throw ParameterIncorrect (SRC_POS); }
	};

#endif // SETUP

	/**
	 *
	 *	\fn File::File (string path, bool readOnly, bool create) : Elevated (false), FileOpen (false)
	 *	\brief File constructor
	 *	\param[in] bool create
	 *	\param[in] bool readOnly
	 *	\param[in] path path
	 *
	 */
	File::File (string path, bool readOnly, bool create) : Elevated (false), FileOpen (false)
	{
		Handle = CreateFile (path.c_str(),
			readOnly ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, create ? CREATE_ALWAYS : OPEN_EXISTING,
			FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_WRITE_THROUGH, NULL);

		try
		{
			throw_sys_if (Handle == INVALID_HANDLE_VALUE);
		}
		catch (SystemException &)
		{
			if (GetLastError() == ERROR_ACCESS_DENIED && IsUacSupported())
				Elevated = true;
			else
				throw;
		}

		FileOpen = true;
		FilePointerPosition = 0;
		IsDevice = false;
		Path = path;
	}

	/**
	 *
	 *	\fn void File::Close ()
	 *	\brief Close a file
	 *	\return
	 *
	 */
	void File::Close ()
	{
		if (FileOpen)
		{
			if (!Elevated)
				CloseHandle (Handle);

			FileOpen = false;
		}
	}

	/**
	 *
	 *	\fn DWORD File::Read (byte *buffer, DWORD size)
	 *	\brief Read a file
	 *	\param[in] DWORD size
	 *	\param[in] byte * buffer
	 *	\return DWORD bytesRead if (Elevated)
	 *			bytesRead else
	 *
	 */
	DWORD File::Read (byte *buffer, DWORD size)
	{
		DWORD bytesRead;

		if (Elevated)
		{
			DWORD bytesRead;

			Elevator::ReadWriteFile (false, IsDevice, Path, buffer, FilePointerPosition, size, &bytesRead);
			FilePointerPosition += bytesRead;
			return bytesRead;
		}

		throw_sys_if (!ReadFile (Handle, buffer, size, &bytesRead, NULL));
		return bytesRead;
	}

	/**
	 *
	 *	\fn void File::SeekAt (int64 position)
	 *	\brief Seek at on file
	 *	\param[in] int64 position
	 *	\return void
	 *
	 */
	void File::SeekAt (int64 position)
	{
		FilePointerPosition = position;

		if (!Elevated)
		{
			LARGE_INTEGER pos;
			pos.QuadPart = position;
			throw_sys_if (!SetFilePointerEx (Handle, pos, NULL, FILE_BEGIN));
		}
	}

	/**
	 *
	 *	\fn void File::Write (byte *buffer, DWORD size)
	 *	\brief Write on a file
	 *	\param[in] DWORD size
	 *	\param[in] byte * buffer
	 *	\return void
	 *
	 */
	void File::Write (byte *buffer, DWORD size)
	{
		DWORD bytesWritten;

		try
		{
			if (Elevated)
			{
				Elevator::ReadWriteFile (true, IsDevice, Path, buffer, FilePointerPosition, size, &bytesWritten);
				FilePointerPosition += bytesWritten;
				throw_sys_if (bytesWritten != size);
			}
			else
			{
				throw_sys_if (!WriteFile (Handle, buffer, size, &bytesWritten, NULL) || bytesWritten != size);
			}
		}
		catch (SystemException &e)
		{
			if (!IsDevice || e.ErrorCode != ERROR_WRITE_PROTECT)
				throw;

			BootEncryption bootEnc (NULL);

			while (size >= GST_SECTOR_SIZE_BIOS)
			{
				bootEnc.WriteBootDriveSector (FilePointerPosition, buffer);

				FilePointerPosition += GST_SECTOR_SIZE_BIOS;
				buffer += GST_SECTOR_SIZE_BIOS;
				size -= GST_SECTOR_SIZE_BIOS;
			}
		}
	}

	/**
	 *
	 *	\fn void Show (HWND parent, const string &str)
	 *	\brief Show a file
	 *	\param[in] const string & str
	 *	\param[in] HWND parent
	 *	\return void
	 *
	 */
	void Show (HWND parent, const string &str)
	{
		MessageBox (parent, str.c_str(), NULL, 0);
	}


	/**
	 *
	 *	\fn Device::Device (string path, bool readOnly)
	 *	\brief Device constructor
	 *	\param[in] bool readOnly
	 *	\param[in] string path
	 *
	 */
	Device::Device (string path, bool readOnly)
	{
		 FileOpen = false;
		 Elevated = false;

		Handle = CreateFile ((string ("\\\\.\\") + path).c_str(),
			readOnly ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
			FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_WRITE_THROUGH, NULL);

		try
		{
			throw_sys_if (Handle == INVALID_HANDLE_VALUE);
		}
		catch (SystemException &)
		{
			if (GetLastError() == ERROR_ACCESS_DENIED && IsUacSupported())
				Elevated = true;
			else
				throw;
		}

		FileOpen = true;
		FilePointerPosition = 0;
		IsDevice = true;
		Path = path;
	}


	/**
	 *
	 *	\fn BootEncryption::BootEncryption (HWND parent): DriveConfigValid (false), ParentWindow (parent), RealSystemDriveSizeValid (false), RescueIsoImage (nullptr), RescueVolumeHeaderValid (false), SelectedEncryptionAlgorithmId (0), VolumeHeaderValid (false)
	 *	\brief BootEncryption constructor
	 *	\param[in] HWND parent): DriveConfigValid (false)
	 *	\param[in] ParentWindow (parent)
	 *	\param[in] RealSystemDriveSizeValid (false)
	 *	\param[in] RescueIsoImage (nullptr)
	 *	\param[in] RescueVolumeHeaderValid (false)
	 *	\param[in] SelectedEncryptionAlgorithmId (0)
	 *	\param[in] VolumeHeaderValid (false)
	 *
	 */
	BootEncryption::BootEncryption (HWND parent)
		: DriveConfigValid (false),
		ParentWindow (parent),
		RealSystemDriveSizeValid (false),
		RescueIsoImage (nullptr),
		RescueVolumeHeaderValid (false),
		SelectedEncryptionAlgorithmId (0),
		VolumeHeaderValid (false)
	{
		Elevator::AddReference();
	}

	/**
	 *
	 *	\fn BootEncryption::~BootEncryption ()
	 *	\brief BootEncryption destructor
	 *
	 */
	BootEncryption::~BootEncryption ()
	{
		if (RescueIsoImage)
			delete[] RescueIsoImage;

		Elevator::Release();
	}


	/**
	 *
	 *	\fn void BootEncryption::CallDriver (DWORD ioctl, void *input, DWORD inputSize, void *output, DWORD outputSize)
	 *	\brief Call the driver
	 *	\param[in] DWORD outputSize)
	 *	\param[in] void * output
	 *	\param[in] DWORD inputSize
	 *	\param[in] void * input
	 *	\param[in] DWORD ioctl
	 *	\return void
	 *
	 */
	void BootEncryption::CallDriver (DWORD ioctl, void *input, DWORD inputSize, void *output, DWORD outputSize)
	{
		try
		{
			DWORD bytesReturned;
			throw_sys_if (!DeviceIoControl (hDriver, ioctl, input, inputSize, output, outputSize, &bytesReturned, NULL));
		}
		catch (SystemException &)
		{
			if (GetLastError() == ERROR_ACCESS_DENIED && IsUacSupported())
				Elevator::CallDriver (ioctl, input, inputSize, output, outputSize);
			else
				throw;
		}
	}


	/**
	 *
	 *	\fn Partition BootEncryption::GetPartitionForHiddenOS ()
	 *	\brief Finds the first partition physically located behind the active one and returns its properties
	 *	\return Partition candidatePartition
	 *
	 */
	Partition BootEncryption::GetPartitionForHiddenOS ()
	{
		Partition candidatePartition;

		memset (&candidatePartition, 0, sizeof(candidatePartition));

		// The user may have modified/added/deleted partitions since the time the partition table was last scanned
		InvalidateCachedSysDriveProperties();

		SystemDriveConfiguration config = GetSystemDriveConfiguration ();
		bool activePartitionFound = false;
		bool candidateForHiddenOSFound = false;

		if (config.SystemPartition.IsGPT)
			throw ParameterIncorrect (SRC_POS);	// It is assumed that CheckRequirements() had been called

		// Find the first active partition on the system drive 
		foreach (const Partition &partition, config.Partitions)
		{
			if (partition.Info.BootIndicator)
			{
				if (partition.Info.PartitionNumber != config.SystemPartition.Number)
				{
					// If there is an extra boot partition, the system partition must be located right behind it
					if (IsOSAtLeast (WIN_7) && config.ExtraBootPartitionPresent)
					{
						int64 minOffsetFound = config.DrivePartition.Info.PartitionLength.QuadPart;
						Partition bootPartition = partition;
						Partition partitionBehindBoot;

						foreach (const Partition &partition, config.Partitions)
						{
							if (partition.Info.StartingOffset.QuadPart > bootPartition.Info.StartingOffset.QuadPart
								&& partition.Info.StartingOffset.QuadPart < minOffsetFound)
							{
								minOffsetFound = partition.Info.StartingOffset.QuadPart;
								partitionBehindBoot = partition;
							}
						}

						if (minOffsetFound != config.DrivePartition.Info.PartitionLength.QuadPart
							&& partitionBehindBoot.Number == config.SystemPartition.Number)
						{
							activePartitionFound = true;
							break;
						}
					}

					throw ErrorException (wstring (GetString ("SYSTEM_PARTITION_NOT_ACTIVE"))
						+ GetRemarksOnHiddenOS());
				}

				activePartitionFound = true;
				break;
			}
		}

		/* WARNING: Note that the partition number at the end of a device path (\Device\HarddiskY\PartitionX) must
		NOT be used to find the first partition physically located behind the active one. The reason is that the 
		user may have deleted and created partitions during this session and e.g. the second partition could have 
		a higer number than the third one. */

		
		// Find the first partition physically located behind the active partition
		if (activePartitionFound)
		{
			int64 minOffsetFound = config.DrivePartition.Info.PartitionLength.QuadPart;

			foreach (const Partition &partition, config.Partitions)
			{
				if (partition.Info.StartingOffset.QuadPart > config.SystemPartition.Info.StartingOffset.QuadPart
					&& partition.Info.StartingOffset.QuadPart < minOffsetFound)
				{
					minOffsetFound = partition.Info.StartingOffset.QuadPart;

					candidatePartition = partition;

					candidateForHiddenOSFound = true;
				}
			}

			if (!candidateForHiddenOSFound)
			{
				throw ErrorException (wstring (GetString ("NO_PARTITION_FOLLOWS_BOOT_PARTITION"))
					+ GetRemarksOnHiddenOS());
			}

			if (config.SystemPartition.Info.PartitionLength.QuadPart > GST_MAX_FAT_SECTOR_COUNT * GST_SECTOR_SIZE_BIOS)
			{
				if ((double) candidatePartition.Info.PartitionLength.QuadPart / config.SystemPartition.Info.PartitionLength.QuadPart < MIN_HIDDENOS_DECOY_PARTITION_SIZE_RATIO_NTFS)
				{
					throw ErrorException (wstring (GetString ("PARTITION_TOO_SMALL_FOR_HIDDEN_OS_NTFS"))
						+ GetRemarksOnHiddenOS());
				}
			}
			else if ((double) candidatePartition.Info.PartitionLength.QuadPart / config.SystemPartition.Info.PartitionLength.QuadPart < MIN_HIDDENOS_DECOY_PARTITION_SIZE_RATIO_FAT)
			{
				throw ErrorException (wstring (GetString ("PARTITION_TOO_SMALL_FOR_HIDDEN_OS"))
					+ GetRemarksOnHiddenOS());
			}
		}
		else
		{
			// No active partition on the system drive
			throw ErrorException ("SYSTEM_PARTITION_NOT_ACTIVE");
		}

		HiddenOSCandidatePartition = candidatePartition;
		return candidatePartition;
	}


	/**
	 *
	 *	\fn DWORD BootEncryption::GetDriverServiceStartType ()
	 *	\brief Get the driver service start type
	 *	\return DWORD startType
	 *
	 */
	DWORD BootEncryption::GetDriverServiceStartType ()
	{
		DWORD startType;
		throw_sys_if (!ReadLocalMachineRegistryDword ("SYSTEM\\CurrentControlSet\\Services\\gostcrypt", "Start", &startType));
		return startType;
	}


	/**
	 *
	 *	\fn wstring BootEncryption::GetRemarksOnHiddenOS ()
	 *	\brief Get remarks on hidden os
	 *	\return wstring (wstring (L"\n\n") + GetString ("TWO_SYSTEMS_IN_ONE_PARTITION_REMARK") + L"\n\n" + GetString ("FOR_MORE_INFO_ON_PARTITIONS"))
	 *
	 */
	wstring BootEncryption::GetRemarksOnHiddenOS ()
	{
		return (wstring (L"\n\n")
				+ GetString ("TWO_SYSTEMS_IN_ONE_PARTITION_REMARK")
				+ L"\n\n"
				+ GetString ("FOR_MORE_INFO_ON_PARTITIONS"));
	}


/**
 *
 *	\fn void BootEncryption::SetDriverServiceStartType (DWORD startType)
 *	\brief Set the driver service start type
 *	\param[in] DWORD startType
 *	\return void
 *
 */
	void BootEncryption::SetDriverServiceStartType (DWORD startType)
	{
		if (!IsAdmin() && IsUacSupported())
		{
			Elevator::SetDriverServiceStartType (startType);
			return;
		}

		BOOL startOnBoot = (startType == SERVICE_BOOT_START);

		SC_HANDLE serviceManager = OpenSCManager (NULL, NULL, SC_MANAGER_ALL_ACCESS);
		throw_sys_if (!serviceManager);

		finally_do_arg (SC_HANDLE, serviceManager, { CloseServiceHandle (finally_arg); });

		SC_HANDLE service = OpenService (serviceManager, "gostcrypt", SERVICE_CHANGE_CONFIG);
		throw_sys_if (!service);

		finally_do_arg (SC_HANDLE, service, { CloseServiceHandle (finally_arg); });

		// Windows versions preceding Vista can be installed on FAT filesystem which does not
		// support long filenames during boot. Convert the driver path to short form if required.
		string driverPath;
		if (startOnBoot && !IsOSAtLeast (WIN_VISTA))
		{
			char pathBuf[MAX_PATH];
			char filesystem[128];

			string path (GetWindowsDirectory());
			path += "\\drivers\\gostcrypt.sys";

			if (GetVolumePathName (path.c_str(), pathBuf, sizeof (pathBuf))
				&& GetVolumeInformation (pathBuf, NULL, 0, NULL, NULL, NULL, filesystem, sizeof(filesystem))
				&& memcmp (filesystem, "FAT", 3) == 0)
			{
				throw_sys_if (GetShortPathName (path.c_str(), pathBuf, sizeof (pathBuf)) == 0);

				// Convert absolute path to relative to the Windows directory
				driverPath = pathBuf;
				driverPath = driverPath.substr (driverPath.rfind ("\\", driverPath.rfind ("\\", driverPath.rfind ("\\") - 1) - 1) + 1);
			}
		}

		throw_sys_if (!ChangeServiceConfig (service, SERVICE_NO_CHANGE, SERVICE_NO_CHANGE,
			startOnBoot ? SERVICE_ERROR_SEVERE : SERVICE_ERROR_NORMAL,
			driverPath.empty() ? NULL : driverPath.c_str(),
			startOnBoot ? "Filter" : NULL,
			NULL, NULL, NULL, NULL, NULL));

		// ChangeServiceConfig() rejects SERVICE_BOOT_START with ERROR_INVALID_PARAMETER
		throw_sys_if (!WriteLocalMachineRegistryDword ("SYSTEM\\CurrentControlSet\\Services\\gostcrypt", "Start", startType));
	}


	/**
	 *
	 *	\fn void BootEncryption::ProbeRealSystemDriveSize ()
	 *	\brief Get the real system drive size
	 *	\return void
	 *
	 */
	void BootEncryption::ProbeRealSystemDriveSize ()
	{
		if (RealSystemDriveSizeValid)
			return;

		GetSystemDriveConfiguration();

		ProbeRealDriveSizeRequest request;
		_snwprintf (request.DeviceName, array_capacity (request.DeviceName), L"%hs", DriveConfig.DrivePartition.DevicePath.c_str());
		
		CallDriver (GST_IOCTL_PROBE_REAL_DRIVE_SIZE, &request, sizeof (request), &request, sizeof (request));
		DriveConfig.DrivePartition.Info.PartitionLength = request.RealDriveSize;

		RealSystemDriveSizeValid = true;

		if (request.TimeOut)
			throw TimeOut (SRC_POS);
	}


	/**
	 *
	 *	\fn void BootEncryption::InvalidateCachedSysDriveProperties ()
	 *	\brief Set invalid drive properties ( invalid size, ...)
	 *	\return void
	 *
	 */
	void BootEncryption::InvalidateCachedSysDriveProperties ()
	{
		DriveConfigValid = false;
		RealSystemDriveSizeValid = false;
	}


/**
 *
 *	\fn PartitionList BootEncryption::GetDrivePartitions (int driveNumber)
 *	\brief Get the drive partitions
 *	\param[in] int driveNumber
 *	\return PartitionList partList
 *
 */
	PartitionList BootEncryption::GetDrivePartitions (int driveNumber)
	{
		PartitionList partList;

		for (int partNumber = 0; partNumber < 64; ++partNumber)
		{
			stringstream partPath;
			partPath << "\\Device\\Harddisk" << driveNumber << "\\Partition" << partNumber;

			DISK_PARTITION_INFO_STRUCT diskPartInfo;
			_snwprintf (diskPartInfo.deviceName, array_capacity (diskPartInfo.deviceName), L"%hs", partPath.str().c_str());

			try
			{
				CallDriver (GST_IOCTL_GET_DRIVE_PARTITION_INFO, &diskPartInfo, sizeof (diskPartInfo), &diskPartInfo, sizeof (diskPartInfo));
			}
			catch (...)
			{
				continue;
			}

			Partition part;
			part.DevicePath = partPath.str();
			part.Number = partNumber;
			part.Info = diskPartInfo.partInfo;
			part.IsGPT = diskPartInfo.IsGPT;

			// Mount point
			wstringstream ws;
			ws << partPath.str().c_str();
			int driveNumber = GetDiskDeviceDriveLetter ((wchar_t *) ws.str().c_str());

			if (driveNumber >= 0)
			{
				part.MountPoint += (char) (driveNumber + 'A');
				part.MountPoint += ":";
			}

			// Volume ID
			wchar_t volumePath[GST_MAX_PATH];
			if (ResolveSymbolicLink ((wchar_t *) ws.str().c_str(), volumePath))
			{
				wchar_t volumeName[GST_MAX_PATH];
				HANDLE fh = FindFirstVolumeW (volumeName, array_capacity (volumeName));
				if (fh != INVALID_HANDLE_VALUE)
				{
					do
					{
						wstring volumeNameStr = volumeName;
						wchar_t devicePath[GST_MAX_PATH];

						if (QueryDosDeviceW (volumeNameStr.substr (4, volumeNameStr.size() - 1 - 4).c_str(), devicePath, array_capacity (devicePath)) != 0
							&& wcscmp (volumePath, devicePath) == 0)
						{
							part.VolumeNameId = volumeName;
							break;
						}

					} while (FindNextVolumeW (fh, volumeName, array_capacity (volumeName)));

					FindVolumeClose (fh);
				}
			}

			partList.push_back (part);
		}

		return partList;
	}
	

/**
 *
 *	\fn DISK_GEOMETRY BootEncryption::GetDriveGeometry (int driveNumber)
 *	\brief Get the drive geometry
 *	\param[in] int driveNumber
 *	\return DISK_GEOMETRY geometry
 *
 */
	DISK_GEOMETRY BootEncryption::GetDriveGeometry (int driveNumber)
	{
		stringstream devName;
		devName << "\\Device\\Harddisk" << driveNumber << "\\Partition0";

		DISK_GEOMETRY geometry;
		throw_sys_if (!::GetDriveGeometry ((char *) devName.str().c_str(), &geometry));
		return geometry;
	}

	
	/**
	 *
	 *	\fn string BootEncryption::GetWindowsDirectory ()
	 *	\brief Get windows directory
	 *	\return string string (buf)
	 *
	 */
	string BootEncryption::GetWindowsDirectory ()
	{
		char buf[MAX_PATH];
		throw_sys_if (GetSystemDirectory (buf, sizeof (buf)) == 0);
		
		return string (buf);
	}


	/**
	 *
	 *	\fn string BootEncryption::GetTempPath ()
	 *	\brief Get the temp path
	 *	\return string string (tempPath)
	 *
	 */
	string BootEncryption::GetTempPath ()
	{
		char tempPath[MAX_PATH];
		DWORD tempLen = ::GetTempPath (sizeof (tempPath), tempPath);
		if (tempLen == 0 || tempLen > sizeof (tempPath))
			throw ParameterIncorrect (SRC_POS);

		return string (tempPath);
	}


	/**
	 *
	 *	\fn uint16 BootEncryption::GetInstalledBootLoaderVersion ()
	 *	\brief Get the version of the installed boot loader
	 *	\return uint16 version
	 *
	 */
	uint16 BootEncryption::GetInstalledBootLoaderVersion ()
	{
		uint16 version;
		CallDriver (GST_IOCTL_GET_BOOT_LOADER_VERSION, NULL, 0, &version, sizeof (version));
		return version;
	}


/**
 *
 *	\fn bool BootEncryption::IsBootLoaderOnDrive (char *devicePath)
 *	\brief Check if bootloader is on drive*
 *			Note that this does not require admin rights (it just requires the driver to be running)
 *	\param[in] char * devicePath
 *	\return booL FALSE catch (...)
 *			(DeviceIoControl (hDriver, GST_IOCTL_OPEN_TEST, &openTestStruct, sizeof (OPEN_TEST_STRUCT), &openTestStruct, sizeof (OPEN_TEST_STRUCT), &dwResult, NULL) && openTestStruct.GSTBootLoaderDetected) else
 *
 */
	bool BootEncryption::IsBootLoaderOnDrive (char *devicePath)
	{
		try 
		{
			OPEN_TEST_STRUCT openTestStruct;
			memset (&openTestStruct, 0, sizeof (openTestStruct));
			DWORD dwResult;

			strcpy ((char *) &openTestStruct.wszFileName[0], devicePath);
			ToUNICODE ((char *) &openTestStruct.wszFileName[0]);

			openTestStruct.bDetectGSTBootLoader = TRUE;

			return (DeviceIoControl (hDriver, GST_IOCTL_OPEN_TEST,
				   &openTestStruct, sizeof (OPEN_TEST_STRUCT),
				   &openTestStruct, sizeof (OPEN_TEST_STRUCT),
				   &dwResult, NULL) && openTestStruct.GSTBootLoaderDetected);
		}
		catch (...)
		{
			return false;
		}
	}


	/**
	 *
	 *	\fn BootEncryptionStatus BootEncryption::GetStatus ()
	 *	\brief Get status
	 *	\return BootEncryptionStatus status
	 *
	 */
	BootEncryptionStatus BootEncryption::GetStatus ()
	{
		/* IMPORTANT: Do NOT add any potentially time-consuming operations to this function. */

		BootEncryptionStatus status;
		CallDriver (GST_IOCTL_GET_BOOT_ENCRYPTION_STATUS, NULL, 0, &status, sizeof (status));
		return status;
	}


	/**
	 *
	 *	\fn void BootEncryption::GetVolumeProperties (VOLUME_PROPERTIES_STRUCT *properties)
	 *	\brief Get volume properties
	 *	\param[in] VOLUME_PROPERTIES_STRUCT * properties
	 *	\return void
	 *
	 */
	void BootEncryption::GetVolumeProperties (VOLUME_PROPERTIES_STRUCT *properties)
	{
		if (properties == NULL)
			throw ParameterIncorrect (SRC_POS);

		CallDriver (GST_IOCTL_GET_BOOT_DRIVE_VOLUME_PROPERTIES, NULL, 0, properties, sizeof (*properties));
	}


	/**
	 *
	 *	\fn bool BootEncryption::IsHiddenSystemRunning ()
	 *	\brief Checks if the hidden system is running
	 *	\return bool hiddenSystemStatus != 0
	 *
	 */
	bool BootEncryption::IsHiddenSystemRunning ()
	{
		int hiddenSystemStatus;
		
		CallDriver (GST_IOCTL_IS_HIDDEN_SYSTEM_RUNNING, nullptr, 0, &hiddenSystemStatus, sizeof (hiddenSystemStatus));
		return hiddenSystemStatus != 0;
	}


	/**
	 *
	 *	\fn bool BootEncryption::SystemDriveContainsPartitionType (byte type)
	 *	\brief Checks if the system drive contains partition type
	 *	\param[in] byte type
	 *	\return bool true if (mbr->Partitions[i].Type == type)
	 *			false else
	 *
	 */
	bool BootEncryption::SystemDriveContainsPartitionType (byte type)
	{
		Device device (GetSystemDriveConfiguration().DevicePath, true);

		byte mbrBuf[GST_SECTOR_SIZE_BIOS];
		device.SeekAt (0);
		device.Read (mbrBuf, sizeof (mbrBuf));

		MBR *mbr = reinterpret_cast <MBR *> (mbrBuf);
		if (mbr->Signature != 0xaa55)
			throw ParameterIncorrect (SRC_POS);

		for (size_t i = 0; i < array_capacity (mbr->Partitions); ++i)
		{
			if (mbr->Partitions[i].Type == type)
				return true;
		}

		return false;
	}


	/**
	 *
	 *	\fn bool BootEncryption::SystemDriveContainsExtendedPartition ()
	 *	\brief Checks if system drive contains extended partition
	 *	\return bool SystemDriveContainsPartitionType (PARTITION_EXTENDED) || SystemDriveContainsPartitionType (PARTITION_XINT13_EXTENDED)
	 *
	 */
	bool BootEncryption::SystemDriveContainsExtendedPartition ()
	{
		return SystemDriveContainsPartitionType (PARTITION_EXTENDED) || SystemDriveContainsPartitionType (PARTITION_XINT13_EXTENDED);
	}


	/**
	 *
	 *	\fn bool BootEncryption::SystemDriveContainsNonStandardPartitions ()
	 *	\brief Checks if the system drive contains non standard partitions
	 *	\return bool true if (SystemDriveContainsPartitionType ((byte) partitionType))
	 *			false else
	 *
	 */
	bool BootEncryption::SystemDriveContainsNonStandardPartitions ()
	{
		for (int partitionType = 1; partitionType <= 0xff; ++partitionType)
		{
			switch (partitionType)
			{
			case PARTITION_FAT_12:
			case PARTITION_FAT_16:
			case PARTITION_EXTENDED:
			case PARTITION_HUGE:
			case PARTITION_IFS:
			case PARTITION_FAT32:
			case PARTITION_FAT32_XINT13:
			case PARTITION_XINT13:
			case PARTITION_XINT13_EXTENDED:
				continue;
			}

			if (SystemDriveContainsPartitionType ((byte) partitionType))
				return true;
		}

		return false;
	}


/**
 *
 *	\fn bool BootEncryption::SystemDriveIsDynamic ()
 *	\brief Checks if the system drive is dynamic
 *	\return bool request.DriveIsDynamic ? true : false
 *
 */
	bool BootEncryption::SystemDriveIsDynamic ()
	{
		GetSystemDriveConfigurationRequest request;
		_snwprintf (request.DevicePath, array_capacity (request.DevicePath), L"%hs", GetSystemDriveConfiguration().DeviceKernelPath.c_str());

		CallDriver (GST_IOCTL_GET_SYSTEM_DRIVE_CONFIG, &request, sizeof (request), &request, sizeof (request));
		return request.DriveIsDynamic ? true : false;
	}


	/**
	 *
	 *	\fn SystemDriveConfiguration BootEncryption::GetSystemDriveConfiguration ()
	 *	\brief Get the system drive configuration
	 *	\return SystemDriveConfiguration DriveConfig if (DriveConfigValid)
	 *			DriveConfig else
	 *
	 */
	SystemDriveConfiguration BootEncryption::GetSystemDriveConfiguration ()
	{
		if (DriveConfigValid)
			return DriveConfig;

		SystemDriveConfiguration config;

		string winDir = GetWindowsDirectory();

		// Scan all drives
		for (int driveNumber = 0; driveNumber < 32; ++driveNumber)
		{
			bool windowsFound = false;
			bool activePartitionFound = false;
			config.ExtraBootPartitionPresent = false;
			config.SystemLoaderPresent = false;

			PartitionList partitions = GetDrivePartitions (driveNumber);
			foreach (const Partition &part, partitions)
			{
				if (!part.MountPoint.empty()
					&& (_access ((part.MountPoint + "\\bootmgr").c_str(), 0) == 0 || _access ((part.MountPoint + "\\ntldr").c_str(), 0) == 0))
				{
					config.SystemLoaderPresent = true;
				}
				else if (!part.VolumeNameId.empty()
					&& (_waccess ((part.VolumeNameId + L"\\bootmgr").c_str(), 0) == 0 || _waccess ((part.VolumeNameId + L"\\ntldr").c_str(), 0) == 0))
				{
					config.SystemLoaderPresent = true;
				}

				if (!windowsFound && !part.MountPoint.empty() && ToUpperCase (winDir).find (ToUpperCase (part.MountPoint)) == 0)
				{
					config.SystemPartition = part;
					windowsFound = true;
				}

				if (!activePartitionFound && part.Info.BootIndicator)
				{
					activePartitionFound = true;

					if (part.Info.PartitionLength.QuadPart > 0 && part.Info.PartitionLength.QuadPart <= GST_MAX_EXTRA_BOOT_PARTITION_SIZE)
						config.ExtraBootPartitionPresent = true;
				}
			}

			if (windowsFound)
			{
				config.DriveNumber = driveNumber;

				stringstream ss;
				ss << "PhysicalDrive" << driveNumber;
				config.DevicePath = ss.str();

				stringstream kernelPath;
				kernelPath << "\\Device\\Harddisk" << driveNumber << "\\Partition0";
				config.DeviceKernelPath = kernelPath.str();

				config.DrivePartition = partitions.front();
				partitions.pop_front();
				config.Partitions = partitions;

				config.InitialUnallocatedSpace = 0x7fffFFFFffffFFFFull;
				config.TotalUnallocatedSpace = config.DrivePartition.Info.PartitionLength.QuadPart;

				foreach (const Partition &part, config.Partitions)
				{
					if (part.Info.StartingOffset.QuadPart < config.InitialUnallocatedSpace)
						config.InitialUnallocatedSpace = part.Info.StartingOffset.QuadPart;

					config.TotalUnallocatedSpace -= part.Info.PartitionLength.QuadPart;
				}

				DriveConfig = config;
				DriveConfigValid = true;
				return DriveConfig;
			}
		}

		throw ParameterIncorrect (SRC_POS);
	}


	/**
	 *
	 *	\fn bool BootEncryption::SystemPartitionCoversWholeDrive ()
	 *	\brief Checks if the system partition covers the whole drive
	 *	\return bool true if (IsOSAtLeast (WIN_7) && config.Partitions.size() == 2 && config.ExtraBootPartitionPresent && config.DrivePartition.Info.PartitionLength.QuadPart - config.SystemPartition.Info.PartitionLength.QuadPart < 164 * BYTES_PER_MB)
	 *			config.Partitions.size() == 1 && config.DrivePartition.Info.PartitionLength.QuadPart - config.SystemPartition.Info.PartitionLength.QuadPart < 64 * BYTES_PER_MB else
	 *
	 */
	bool BootEncryption::SystemPartitionCoversWholeDrive ()
	{
		SystemDriveConfiguration config = GetSystemDriveConfiguration();

		if (IsOSAtLeast (WIN_7)
			&& config.Partitions.size() == 2
			&& config.ExtraBootPartitionPresent
			&& config.DrivePartition.Info.PartitionLength.QuadPart - config.SystemPartition.Info.PartitionLength.QuadPart < 164 * BYTES_PER_MB)
		{
			return true;
		}

		return config.Partitions.size() == 1
			&& config.DrivePartition.Info.PartitionLength.QuadPart - config.SystemPartition.Info.PartitionLength.QuadPart < 64 * BYTES_PER_MB;
	}


	/**
	 *
	 *	\fn uint32 BootEncryption::GetChecksum (byte *data, size_t size)
	 *	\brief Get the checksum
	 *	\param[in] size_t size
	 *	\param[in] byte * data
	 *	\return uint32 sum
	 *
	 */
	uint32 BootEncryption::GetChecksum (byte *data, size_t size)
	{
		uint32 sum = 0;

		while (size-- > 0)
		{
			sum += *data++;
			sum = _rotl (sum, 1);
		}

		return sum;
	}


	/**
	 *
	 *	\fn void BootEncryption::CreateBootLoaderInMemory (byte *buffer, size_t bufferSize, bool rescueDisk, bool hiddenOSCreation)
	 *	\brief Create the bootloader in memory
	 *	\param[in] bool hiddenOSCreation
	 *	\param[in] bool rescueDisk
	 *	\param[in] size_t bufferSize
	 *	\param[in] byte * buffer
	 *	\return void
	 *
	 */
	void BootEncryption::CreateBootLoaderInMemory (byte *buffer, size_t bufferSize, bool rescueDisk, bool hiddenOSCreation)
	{
		if (bufferSize < GST_BOOT_LOADER_AREA_SIZE - GST_BOOT_ENCRYPTION_VOLUME_HEADER_SIZE)
			throw ParameterIncorrect (SRC_POS);

		ZeroMemory (buffer, bufferSize);

		int ea = 0;
		if (GetStatus().DriveMounted)
		{
			try
			{
				GetBootEncryptionAlgorithmNameRequest request;
				CallDriver (GST_IOCTL_GET_BOOT_ENCRYPTION_ALGORITHM_NAME, NULL, 0, &request, sizeof (request));
			}
			catch (...)
			{
				try
				{
					VOLUME_PROPERTIES_STRUCT properties;
					GetVolumeProperties (&properties);
					ea = properties.ea;
				}
				catch (...) { }
			}
		}
		else
		{
			if (SelectedEncryptionAlgorithmId == 0)
				throw ParameterIncorrect (SRC_POS);

			ea = SelectedEncryptionAlgorithmId;
		}

		int bootSectorId = rescueDisk ? IDR_RESCUE_BOOT_SECTOR : IDR_BOOT_SECTOR;
		int bootLoaderId = rescueDisk ? IDR_RESCUE_LOADER : IDR_BOOT_LOADER;

		// Boot sector
		DWORD size;
		byte *bootSecResourceImg = MapResource ("BIN", bootSectorId, &size);
		if (!bootSecResourceImg || size != GST_SECTOR_SIZE_BIOS)
			throw ParameterIncorrect (SRC_POS);

		memcpy (buffer, bootSecResourceImg, size);

		*(uint16 *) (buffer + GST_BOOT_SECTOR_VERSION_OFFSET) = BE16 (VERSION_NUM);

		if (IsOSAtLeast (WIN_VISTA))
			buffer[GST_BOOT_SECTOR_CONFIG_OFFSET] |= GST_BOOT_CFG_FLAG_WINDOWS_VISTA_OR_LATER;

		if (rescueDisk && (ReadDriverConfigurationFlags() & GST_DRIVER_CONFIG_DISABLE_HARDWARE_ENCRYPTION))
			buffer[GST_BOOT_SECTOR_CONFIG_OFFSET] |= GST_BOOT_CFG_FLAG_RESCUE_DISABLE_HW_ENCRYPTION;

		// Checksum of the backup header of the outer volume for the hidden system
		if (hiddenOSCreation)
		{
			Device device (GetSystemDriveConfiguration().DevicePath);
			byte headerSector[GST_SECTOR_SIZE_BIOS];

			device.SeekAt (HiddenOSCandidatePartition.Info.StartingOffset.QuadPart + HiddenOSCandidatePartition.Info.PartitionLength.QuadPart - GST_VOLUME_HEADER_GROUP_SIZE + GST_VOLUME_HEADER_EFFECTIVE_SIZE);
			device.Read (headerSector, sizeof (headerSector));

			*(uint32 *) (buffer + GST_BOOT_SECTOR_OUTER_VOLUME_BAK_HEADER_CRC_OFFSET) = GetCrc32 (headerSector, sizeof (headerSector));
		}

		// Decompressor
		byte *decompressor = MapResource ("BIN", IDR_BOOT_LOADER_DECOMPRESSOR, &size);
		if (!decompressor || size > GST_BOOT_LOADER_DECOMPRESSOR_SECTOR_COUNT * GST_SECTOR_SIZE_BIOS)
			throw ParameterIncorrect (SRC_POS);

		memcpy (buffer + GST_SECTOR_SIZE_BIOS, decompressor, size);

		// Compressed boot loader
		byte *bootLoader = MapResource ("BIN", bootLoaderId, &size);
		if (!bootLoader)
			throw ParameterIncorrect (SRC_POS);

		if (size > GST_MAX_BOOT_LOADER_SECTOR_COUNT * GST_SECTOR_SIZE_BIOS)
			throw ParameterIncorrect (SRC_POS);

		memcpy (buffer + GST_SECTOR_SIZE_BIOS + GST_BOOT_LOADER_DECOMPRESSOR_SECTOR_COUNT * GST_SECTOR_SIZE_BIOS, bootLoader, size);

		// Boot loader and decompressor checksum
		*(uint16 *) (buffer + GST_BOOT_SECTOR_LOADER_LENGTH_OFFSET) = static_cast <uint16> (size);
		*(uint32 *) (buffer + GST_BOOT_SECTOR_LOADER_CHECKSUM_OFFSET) = GetChecksum (buffer + GST_SECTOR_SIZE_BIOS,
			GST_BOOT_LOADER_DECOMPRESSOR_SECTOR_COUNT * GST_SECTOR_SIZE_BIOS + size);

		// Backup of decompressor and boot loader
		if (size + GST_BOOT_LOADER_DECOMPRESSOR_SECTOR_COUNT * GST_SECTOR_SIZE_BIOS <= GST_BOOT_LOADER_BACKUP_SECTOR_COUNT * GST_SECTOR_SIZE_BIOS)
		{
			memcpy (buffer + GST_SECTOR_SIZE_BIOS + GST_BOOT_LOADER_BACKUP_SECTOR_COUNT * GST_SECTOR_SIZE_BIOS,
				buffer + GST_SECTOR_SIZE_BIOS, GST_BOOT_LOADER_BACKUP_SECTOR_COUNT * GST_SECTOR_SIZE_BIOS);

			buffer[GST_BOOT_SECTOR_CONFIG_OFFSET] |= GST_BOOT_CFG_FLAG_BACKUP_LOADER_AVAILABLE;
		}
		else if (!rescueDisk && bootLoaderId != IDR_BOOT_LOADER)
		{
			throw ParameterIncorrect (SRC_POS);
		}
	}


	/**
	 *
	 *	\fn void BootEncryption::ReadBootSectorConfig (byte *config, size_t bufLength, byte *userConfig, string *customUserMessage, uint16 *bootLoaderVersion)
	 *	\brief Read the boot sector configurations
	 *	\param[in] uint16 * bootLoaderVersion
	 *	\param[in] string * customUserMessage
	 *	\param[in] byte * userConfig
	 *	\param[in] size_t bufLength
	 *	\param[in] byte * config
	 *	\return void
	 *
	 */
	void BootEncryption::ReadBootSectorConfig (byte *config, size_t bufLength, byte *userConfig, string *customUserMessage, uint16 *bootLoaderVersion)
	{
		if (config && bufLength < GST_BOOT_CFG_FLAG_AREA_SIZE)
			throw ParameterIncorrect (SRC_POS);

		GetSystemDriveConfigurationRequest request;
		_snwprintf (request.DevicePath, array_capacity (request.DevicePath), L"%hs", GetSystemDriveConfiguration().DeviceKernelPath.c_str());

		try
		{
			CallDriver (GST_IOCTL_GET_SYSTEM_DRIVE_CONFIG, &request, sizeof (request), &request, sizeof (request));
			if (config)
				*config = request.Configuration;

			if (userConfig)
				*userConfig = request.UserConfiguration;
			
			if (customUserMessage)
			{
				request.CustomUserMessage[GST_BOOT_SECTOR_USER_MESSAGE_MAX_LENGTH] = 0;
				*customUserMessage = request.CustomUserMessage;
			}

			if (bootLoaderVersion)
				*bootLoaderVersion = request.BootLoaderVersion;
		}
		catch (...)
		{
			if (config)
				*config = 0;

			if (userConfig)
				*userConfig = 0;
			
			if (customUserMessage)
				customUserMessage->clear();

			if (bootLoaderVersion)
				*bootLoaderVersion = 0;
		}
	}


	/**
	 *
	 *	\fn void BootEncryption::WriteBootSectorConfig (const byte newConfig[])
	 *	\brief Write on boot sector configuration
	 *	\param[in] const byte newConfig[]
	 *	\return void
	 *
	 */
	void BootEncryption::WriteBootSectorConfig (const byte newConfig[])
	{
		Device device (GetSystemDriveConfiguration().DevicePath);
		byte mbr[GST_SECTOR_SIZE_BIOS];

		device.SeekAt (0);
		device.Read (mbr, sizeof (mbr));

		memcpy (mbr + GST_BOOT_SECTOR_CONFIG_OFFSET, newConfig, GST_BOOT_CFG_FLAG_AREA_SIZE);

		device.SeekAt (0);
		device.Write (mbr, sizeof (mbr));

		byte mbrVerificationBuf[GST_SECTOR_SIZE_BIOS];
		device.SeekAt (0);
		device.Read (mbrVerificationBuf, sizeof (mbr));

		if (memcmp (mbr, mbrVerificationBuf, sizeof (mbr)) != 0)
			throw ErrorException ("ERROR_MBR_PROTECTED");
	}


	/**
	 *
	 *	\fn void BootEncryption::WriteBootSectorUserConfig (byte userConfig, const string &customUserMessage)
	 *	\brief Write on boot sector user configuration
	 *	\param[in] const string & customUserMessage
	 *	\param[in] byte userConfig
	 *	\return void
	 *
	 */
	void BootEncryption::WriteBootSectorUserConfig (byte userConfig, const string &customUserMessage)
	{
		Device device (GetSystemDriveConfiguration().DevicePath);
		byte mbr[GST_SECTOR_SIZE_BIOS];

		device.SeekAt (0);
		device.Read (mbr, sizeof (mbr));

		if (!BufferContainsString (mbr, sizeof (mbr), GST_APP_NAME)
			|| BE16 (*(uint16 *) (mbr + GST_BOOT_SECTOR_VERSION_OFFSET)) != VERSION_NUM)
		{
			return;
		}

		mbr[GST_BOOT_SECTOR_USER_CONFIG_OFFSET] = userConfig;

		memset (mbr + GST_BOOT_SECTOR_USER_MESSAGE_OFFSET, 0, GST_BOOT_SECTOR_USER_MESSAGE_MAX_LENGTH);

		if (!customUserMessage.empty())
		{
			if (customUserMessage.size() > GST_BOOT_SECTOR_USER_MESSAGE_MAX_LENGTH)
				throw ParameterIncorrect (SRC_POS);

			memcpy (mbr + GST_BOOT_SECTOR_USER_MESSAGE_OFFSET, customUserMessage.c_str(), customUserMessage.size());
		}

		device.SeekAt (0);
		device.Write (mbr, sizeof (mbr));

		byte mbrVerificationBuf[GST_SECTOR_SIZE_BIOS];
		device.SeekAt (0);
		device.Read (mbrVerificationBuf, sizeof (mbr));

		if (memcmp (mbr, mbrVerificationBuf, sizeof (mbr)) != 0)
			throw ErrorException ("ERROR_MBR_PROTECTED");
	}


	/**
	 *
	 *	\fn unsigned int BootEncryption::GetHiddenOSCreationPhase ()
	 *	\brief Get the hidden OS creation phase
	 *	\return unsigned int (configFlags[0] & GST_BOOT_CFG_MASK_HIDDEN_OS_CREATION_PHASE)
	 *
	 */
	unsigned int BootEncryption::GetHiddenOSCreationPhase ()
	{
		byte configFlags [GST_BOOT_CFG_FLAG_AREA_SIZE];

		ReadBootSectorConfig (configFlags, sizeof(configFlags));

		return (configFlags[0] & GST_BOOT_CFG_MASK_HIDDEN_OS_CREATION_PHASE);
	}


	/**
	 *
	 *	\fn void BootEncryption::SetHiddenOSCreationPhase (unsigned int newPhase)
	 *	\brief Set hidden OS creation page
	 *	\param[in] unsigned int newPhase
	 *	\return void
	 *
	 */
	void BootEncryption::SetHiddenOSCreationPhase (unsigned int newPhase)
	{
#if GST_BOOT_CFG_FLAG_AREA_SIZE != 1
#	error GST_BOOT_CFG_FLAG_AREA_SIZE != 1; revise GetHiddenOSCreationPhase() and SetHiddenOSCreationPhase()
#endif
		byte configFlags [GST_BOOT_CFG_FLAG_AREA_SIZE];

		ReadBootSectorConfig (configFlags, sizeof(configFlags));

		configFlags[0] &= (byte) ~GST_BOOT_CFG_MASK_HIDDEN_OS_CREATION_PHASE;

		configFlags[0] |= newPhase;

		WriteBootSectorConfig (configFlags);
	}


#ifndef SETUP

	/**
	 *
	 *	\fn void BootEncryption::StartDecoyOSWipe (WipeAlgorithmId wipeAlgorithm)
	 *	\brief Start the decoy OS wipe
	 *	\param[in] WipeAlgorithmId wipeAlgorithm
	 *	\return void
	 *
	 */
	void BootEncryption::StartDecoyOSWipe (WipeAlgorithmId wipeAlgorithm)
	{
		if (!IsHiddenOSRunning())
			throw ParameterIncorrect (SRC_POS);

		WipeDecoySystemRequest request;
		ZeroMemory (&request, sizeof (request));

		request.WipeAlgorithm = wipeAlgorithm;
		
		if (Randinit() != ERR_SUCCESS)
			throw ParameterIncorrect (SRC_POS);

		UserEnrichRandomPool (ParentWindow);

		if (!RandgetBytes (request.WipeKey, sizeof (request.WipeKey), TRUE))
			throw ParameterIncorrect (SRC_POS);

		CallDriver (GST_IOCTL_START_DECOY_SYSTEM_WIPE, &request, sizeof (request), NULL, 0);

		burn (&request, sizeof (request));
	}


	/**
	 *
	 *	\fn void BootEncryption::AbortDecoyOSWipe ()
	 *	\brief Abort the decoy OS wipe
	 *	\return void
	 *
	 */
	void BootEncryption::AbortDecoyOSWipe ()
	{
		CallDriver (GST_IOCTL_ABORT_DECOY_SYSTEM_WIPE);
	}

	
	/**
	 *
	 *	\fn DecoySystemWipeStatus BootEncryption::GetDecoyOSWipeStatus ()
	 *	\brief Get the decoy os wipe status
	 *	\return DecoySystemWipeStatus status
	 *
	 */
	DecoySystemWipeStatus BootEncryption::GetDecoyOSWipeStatus ()
	{
		DecoySystemWipeStatus status;
		CallDriver (GST_IOCTL_GET_DECOY_SYSTEM_WIPE_STATUS, NULL, 0, &status, sizeof (status));
		return status;
	}


	/**
	 *
	 *	\fn void BootEncryption::CheckDecoyOSWipeResult ()
	 *	\brief Checks the OS wipe result
	 *	\return void
	 *
	 */
	void BootEncryption::CheckDecoyOSWipeResult ()
	{
		CallDriver (GST_IOCTL_GET_DECOY_SYSTEM_WIPE_RESULT);
	}


/**
 *
 *	\fn void BootEncryption::WipeHiddenOSCreationConfig ()
 *	\brief Wipe the fidden OS creation on configuration
 *	\return void
 *
 */
	void BootEncryption::WipeHiddenOSCreationConfig ()
	{
		if (IsHiddenOSRunning() || Randinit() != ERR_SUCCESS)
			throw ParameterIncorrect (SRC_POS);

		Device device (GetSystemDriveConfiguration().DevicePath);
		byte mbr[GST_SECTOR_SIZE_BIOS];

		device.SeekAt (0);
		device.Read (mbr, sizeof (mbr));
		
		finally_do_arg (BootEncryption *, this,
		{
			try
			{
				finally_arg->SetHiddenOSCreationPhase (GST_HIDDEN_OS_CREATION_PHASE_NONE);
			} catch (...) { }
		});

#if PRAND_DISK_WIPE_PASSES > RNG_POOL_SIZE
#	error PRAND_DISK_WIPE_PASSES > RNG_POOL_SIZE
#endif

		byte randData[PRAND_DISK_WIPE_PASSES];
		if (!RandgetBytes (randData, sizeof (randData), FALSE))
			throw ParameterIncorrect (SRC_POS);

		for (int wipePass = 0; wipePass < PRAND_DISK_WIPE_PASSES; wipePass++)
		{
			for (int i = 0; i < GST_BOOT_SECTOR_OUTER_VOLUME_BAK_HEADER_CRC_SIZE; ++i)
			{
				mbr[GST_BOOT_SECTOR_OUTER_VOLUME_BAK_HEADER_CRC_OFFSET + i] = randData[wipePass];
			}

			mbr[GST_BOOT_SECTOR_CONFIG_OFFSET] &= (byte) ~GST_BOOT_CFG_MASK_HIDDEN_OS_CREATION_PHASE;
			mbr[GST_BOOT_SECTOR_CONFIG_OFFSET] |= randData[wipePass] & GST_BOOT_CFG_MASK_HIDDEN_OS_CREATION_PHASE;

			if (wipePass == PRAND_DISK_WIPE_PASSES - 1)
				memset (mbr + GST_BOOT_SECTOR_OUTER_VOLUME_BAK_HEADER_CRC_OFFSET, 0, GST_BOOT_SECTOR_OUTER_VOLUME_BAK_HEADER_CRC_SIZE);

			device.SeekAt (0);
			device.Write (mbr, sizeof (mbr));
		}

		for (int wipePass = 0; wipePass < PRAND_DISK_WIPE_PASSES/4 + 1; wipePass++)
		{
			SetHiddenOSCreationPhase (GST_HIDDEN_OS_CREATION_PHASE_NONE);
			SetHiddenOSCreationPhase (GST_HIDDEN_OS_CREATION_PHASE_CLONING);
			SetHiddenOSCreationPhase (GST_HIDDEN_OS_CREATION_PHASE_WIPING);
			SetHiddenOSCreationPhase (GST_HIDDEN_OS_CREATION_PHASE_WIPED);
		}
		SetHiddenOSCreationPhase (GST_HIDDEN_OS_CREATION_PHASE_NONE);
	}

#endif // !SETUP


	/**
	 *
	 *	\fn void BootEncryption::InstallBootLoader (bool preserveUserConfig, bool hiddenOSCreation)
	 *	\brief Install the bootloader
	 *	\param[in] bool hiddenOSCreation
	 *	\param[in] bool preserveUserConfig
	 *	\return void
	 *
	 */
	void BootEncryption::InstallBootLoader (bool preserveUserConfig, bool hiddenOSCreation)
	{
		byte bootLoaderBuf[GST_BOOT_LOADER_AREA_SIZE - GST_BOOT_ENCRYPTION_VOLUME_HEADER_SIZE];
		CreateBootLoaderInMemory (bootLoaderBuf, sizeof (bootLoaderBuf), false, hiddenOSCreation);

		// Write MBR
		Device device (GetSystemDriveConfiguration().DevicePath);
		byte mbr[GST_SECTOR_SIZE_BIOS];

		device.SeekAt (0);
		device.Read (mbr, sizeof (mbr));

		if (preserveUserConfig && BufferContainsString (mbr, sizeof (mbr), GST_APP_NAME))
		{
			uint16 version = BE16 (*(uint16 *) (mbr + GST_BOOT_SECTOR_VERSION_OFFSET));
			if (version != 0)
			{
				bootLoaderBuf[GST_BOOT_SECTOR_USER_CONFIG_OFFSET] = mbr[GST_BOOT_SECTOR_USER_CONFIG_OFFSET];
				memcpy (bootLoaderBuf + GST_BOOT_SECTOR_USER_MESSAGE_OFFSET, mbr + GST_BOOT_SECTOR_USER_MESSAGE_OFFSET, GST_BOOT_SECTOR_USER_MESSAGE_MAX_LENGTH);
			}
		}

		memcpy (mbr, bootLoaderBuf, GST_MAX_MBR_BOOT_CODE_SIZE);

		device.SeekAt (0);
		device.Write (mbr, sizeof (mbr));

		byte mbrVerificationBuf[GST_SECTOR_SIZE_BIOS];
		device.SeekAt (0);
		device.Read (mbrVerificationBuf, sizeof (mbr));

		if (memcmp (mbr, mbrVerificationBuf, sizeof (mbr)) != 0)
			throw ErrorException ("ERROR_MBR_PROTECTED");

		// Write boot loader
		device.SeekAt (GST_SECTOR_SIZE_BIOS);
		device.Write (bootLoaderBuf + GST_SECTOR_SIZE_BIOS, sizeof (bootLoaderBuf) - GST_SECTOR_SIZE_BIOS);
	}


	/**
	 *
	 *	\fn string BootEncryption::GetSystemLoaderBackupPath ()
	 *	\brief Get system loader backup path
	 *	\return string path + '\\' + GST_SYS_BOOT_LOADER_BACKUP_NAME
	 *
	 */
	string BootEncryption::GetSystemLoaderBackupPath ()
	{
		char pathBuf[MAX_PATH];

		throw_sys_if (!SUCCEEDED (SHGetFolderPath (NULL, CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, pathBuf)));
		
		string path = string (pathBuf) + "\\" GST_APP_NAME;
		CreateDirectory (path.c_str(), NULL);

		return path + '\\' + GST_SYS_BOOT_LOADER_BACKUP_NAME;
	}


	/**
	 *
	 *	\fn void BootEncryption::RenameDeprecatedSystemLoaderBackup ()
	 *	\brief Rename the deprecated system loader bacup
	 *	\return void
	 *
	 */
	void BootEncryption::RenameDeprecatedSystemLoaderBackup ()
	{
		char pathBuf[MAX_PATH];

		if (SUCCEEDED (SHGetFolderPath (NULL, CSIDL_COMMON_APPDATA, NULL, 0, pathBuf)))
		{
			string path = string (pathBuf) + "\\" GST_APP_NAME + '\\' + GST_SYS_BOOT_LOADER_BACKUP_NAME_LEGACY;

			if (FileExists (path.c_str()) && !FileExists (GetSystemLoaderBackupPath().c_str()))
				throw_sys_if (rename (path.c_str(), GetSystemLoaderBackupPath().c_str()) != 0);
		}
	}


#ifndef SETUP
	/**
	 *
	 *	\fn void BootEncryption::CreateRescueIsoImage (bool initialSetup, const string &isoImagePath)
	 *	\brief Create the rescue iso image
	 *	\param[in] const string & isoImagePath
	 *	\param[in] bool initialSetup
	 *	\return void
	 *
	 */
	void BootEncryption::CreateRescueIsoImage (bool initialSetup, const string &isoImagePath)
	{
		BootEncryptionStatus encStatus = GetStatus();
		if (encStatus.SetupInProgress)
			throw ParameterIncorrect (SRC_POS);

		Buffer imageBuf (RescueIsoImageSize);
		
		byte *image = imageBuf.Ptr();
		memset (image, 0, RescueIsoImageSize);

		// Primary volume descriptor
		strcpy ((char *)image + 0x8000, "\001CD001\001");
		strcpy ((char *)image + 0x7fff + 41, "GostCrypt Rescue Disk           ");
		*(uint32 *) (image + 0x7fff + 81) = RescueIsoImageSize / 2048;
		*(uint32 *) (image + 0x7fff + 85) = BE32 (RescueIsoImageSize / 2048);
		image[0x7fff + 121] = 1;
		image[0x7fff + 124] = 1;
		image[0x7fff + 125] = 1;
		image[0x7fff + 128] = 1;
		image[0x7fff + 130] = 8;
		image[0x7fff + 131] = 8;

		image[0x7fff + 133] = 10;
		image[0x7fff + 140] = 10;
		image[0x7fff + 141] = 0x14;
		image[0x7fff + 157] = 0x22;
		image[0x7fff + 159] = 0x18;

		// Boot record volume descriptor
		strcpy ((char *)image + 0x8801, "CD001\001EL TORITO SPECIFICATION");
		image[0x8800 + 0x47] = 0x19;

		// Volume descriptor set terminator
		strcpy ((char *)image + 0x9000, "\377CD001\001");

		// Path table
		image[0xA000 + 0] = 1;
		image[0xA000 + 2] = 0x18;
		image[0xA000 + 6] = 1;

		// Root directory
		image[0xc000 + 0] = 0x22;
		image[0xc000 + 2] = 0x18;
		image[0xc000 + 9] = 0x18;
		image[0xc000 + 11] = 0x08;
		image[0xc000 + 16] = 0x08;
		image[0xc000 + 25] = 0x02;
		image[0xc000 + 28] = 0x01;
		image[0xc000 + 31] = 0x01;
		image[0xc000 + 32] = 0x01;
		image[0xc000 + 34] = 0x22;
		image[0xc000 + 36] = 0x18;
		image[0xc000 + 43] = 0x18;
		image[0xc000 + 45] = 0x08;
		image[0xc000 + 50] = 0x08;
		image[0xc000 + 59] = 0x02;
		image[0xc000 + 62] = 0x01;
		*(uint32 *) (image + 0xc000 + 65) = 0x010101;

		// Validation entry
		image[0xc800] = 1;
		int offset = 0xc800 + 0x1c;
		image[offset++] = 0xaa;
		image[offset++] = 0x55;
		image[offset++] = 0x55;
		image[offset] = 0xaa;

		// Initial entry
		offset = 0xc820;
		image[offset++] = 0x88;
		image[offset++] = 2;
		image[0xc820 + 6] = 1;
		image[0xc820 + 8] = GST_CD_BOOT_LOADER_SECTOR;

		// GostCrypt Boot Loader
		CreateBootLoaderInMemory (image + GST_CD_BOOTSECTOR_OFFSET, GST_BOOT_LOADER_AREA_SIZE, true);

		// Volume header
		if (initialSetup)
		{
			if (!RescueVolumeHeaderValid)
				throw ParameterIncorrect (SRC_POS);

			memcpy (image + GST_CD_BOOTSECTOR_OFFSET + GST_BOOT_VOLUME_HEADER_SECTOR_OFFSET, RescueVolumeHeader, GST_BOOT_ENCRYPTION_VOLUME_HEADER_SIZE);
		}
		else
		{
			Device bootDevice (GetSystemDriveConfiguration().DevicePath, true);
			bootDevice.SeekAt (GST_BOOT_VOLUME_HEADER_SECTOR_OFFSET);
			bootDevice.Read (image + GST_CD_BOOTSECTOR_OFFSET + GST_BOOT_VOLUME_HEADER_SECTOR_OFFSET, GST_BOOT_ENCRYPTION_VOLUME_HEADER_SIZE);
		}

		// Original system loader
		try
		{
			File sysBakFile (GetSystemLoaderBackupPath(), true);
			sysBakFile.Read (image + GST_CD_BOOTSECTOR_OFFSET + GST_ORIG_BOOT_LOADER_BACKUP_SECTOR_OFFSET, GST_BOOT_LOADER_AREA_SIZE);
			
			image[GST_CD_BOOTSECTOR_OFFSET + GST_BOOT_SECTOR_CONFIG_OFFSET] |= GST_BOOT_CFG_FLAG_RESCUE_DISK_ORIG_SYS_LOADER;
		}
		catch (Exception &e)
		{
			e.Show (ParentWindow);
			Warning ("SYS_LOADER_UNAVAILABLE_FOR_RESCUE_DISK");
		}
		
		// Boot loader backup
		CreateBootLoaderInMemory (image + GST_CD_BOOTSECTOR_OFFSET + GST_BOOT_LOADER_BACKUP_RESCUE_DISK_SECTOR_OFFSET, GST_BOOT_LOADER_AREA_SIZE, false);

		RescueIsoImage = new byte[RescueIsoImageSize];
		if (!RescueIsoImage)
			throw bad_alloc();
		memcpy (RescueIsoImage, image, RescueIsoImageSize);

		if (!isoImagePath.empty())
		{
			File isoFile (isoImagePath, false, true);
			isoFile.Write (image, RescueIsoImageSize);
		}
	}
#endif


/**
 *
 *	\fn bool BootEncryption::IsCDDrivePresent ()
 *	\brief Checks if the CD drive is present
 *	\return bool true if (GetDriveType (path.c_str()) == DRIVE_CDROM)
 *			false else
 *
 */
	bool BootEncryption::IsCDDrivePresent ()
	{
		for (char drive = 'Z'; drive >= 'C'; --drive)
		{
			string path = "X:\\";
			path[0] = drive;

			if (GetDriveType (path.c_str()) == DRIVE_CDROM)
				return true;
		}

		return false;
	}


	/**
	 *
	 *	\fn bool BootEncryption::VerifyRescueDisk ()
	 *	\brief Verify the rescue Disk
	 *	\return bool true if (memcmp (buffer.Ptr(), RescueIsoImage, buffer.Size()) == 0)
	 *			false else
	 *
	 */
	bool BootEncryption::VerifyRescueDisk ()
	{
		if (!RescueIsoImage)
			throw ParameterIncorrect (SRC_POS);

		for (char drive = 'Z'; drive >= 'C'; --drive)
		{
			try
			{
				string path = "X:";
				path[0] = drive;

				Device driveDevice (path, true);
				size_t verifiedSectorCount = (GST_CD_BOOTSECTOR_OFFSET + GST_ORIG_BOOT_LOADER_BACKUP_SECTOR_OFFSET + GST_BOOT_LOADER_AREA_SIZE) / 2048;
				Buffer buffer ((verifiedSectorCount + 1) * 2048);

				DWORD bytesRead = driveDevice.Read (buffer.Ptr(), buffer.Size());
				if (bytesRead != buffer.Size())
					continue;

				if (memcmp (buffer.Ptr(), RescueIsoImage, buffer.Size()) == 0)
					return true;
			}
			catch (...) { }
		}

		return false;
	}


#ifndef SETUP

	/**
	 *
	 *	\fn void BootEncryption::CreateVolumeHeader (uint64 volumeSize, uint64 encryptedAreaStart, Password *password, int ea, int mode, int pkcs5)
	 *	\brief Create the volume header
	 *	\param[in] int pkcs5
	 *	\param[in] int mode
	 *	\param[in] int ea
	 *	\param[in] Password * password
	 *	\param[in] uint64 encryptedAreaStart
	 *	\param[in] uint64 volumeSize
	 *	\return void
	 *
	 */
	void BootEncryption::CreateVolumeHeader (uint64 volumeSize, uint64 encryptedAreaStart, Password *password, int ea, int mode, int pkcs5)
	{
		PCRYPTO_INFO cryptoInfo = NULL;

		if (!IsRandomNumberGeneratorStarted())
			throw ParameterIncorrect (SRC_POS);

		throw_sys_if (CreateVolumeHeaderInMemory (TRUE, (char *) VolumeHeader, ea, mode, password, pkcs5, NULL, &cryptoInfo,
			volumeSize, 0, encryptedAreaStart, 0, GST_SYSENC_KEYSCOPE_MIN_REQ_PROG_VERSION, GST_HEADER_FLAG_ENCRYPTED_SYSTEM, GST_SECTOR_SIZE_BIOS, FALSE) != 0);

		finally_do_arg (PCRYPTO_INFO*, &cryptoInfo, { crypto_close (*finally_arg); });

		// Initial rescue disk assumes encryption of the drive has been completed (EncryptedAreaLength == volumeSize)
		memcpy (RescueVolumeHeader, VolumeHeader, sizeof (RescueVolumeHeader));
		ReadVolumeHeader (TRUE, (char *) RescueVolumeHeader, password, NULL, cryptoInfo);

		DecryptBuffer (RescueVolumeHeader + HEADER_ENCRYPTED_DATA_OFFSET, HEADER_ENCRYPTED_DATA_SIZE, cryptoInfo);

		if (GetHeaderField32 (RescueVolumeHeader, GST_HEADER_OFFSET_MAGIC) != 0x54525545)
			throw ParameterIncorrect (SRC_POS);

		byte *fieldPos = RescueVolumeHeader + GST_HEADER_OFFSET_ENCRYPTED_AREA_LENGTH;
		mputInt64 (fieldPos, volumeSize);

		// CRC of the header fields
		uint32 crc = GetCrc32 (RescueVolumeHeader + GST_HEADER_OFFSET_MAGIC, GST_HEADER_OFFSET_HEADER_CRC - GST_HEADER_OFFSET_MAGIC);
		fieldPos = RescueVolumeHeader + GST_HEADER_OFFSET_HEADER_CRC;
		mputLong (fieldPos, crc);

		EncryptBuffer (RescueVolumeHeader + HEADER_ENCRYPTED_DATA_OFFSET, HEADER_ENCRYPTED_DATA_SIZE, cryptoInfo);

		VolumeHeaderValid = true;
		RescueVolumeHeaderValid = true;
	}


	/**
	 *
	 *	\fn void BootEncryption::InstallVolumeHeader ()
	 *	\brief Install the volume header
	 *	\return void
	 *
	 */
	void BootEncryption::InstallVolumeHeader ()
	{
		if (!VolumeHeaderValid)
			throw ParameterIncorrect (SRC_POS);

		Device device (GetSystemDriveConfiguration().DevicePath);

		device.SeekAt (GST_BOOT_VOLUME_HEADER_SECTOR_OFFSET);
		device.Write ((byte *) VolumeHeader, sizeof (VolumeHeader));
	}


	/**
	 *
	 *	\fn void BootEncryption::AbortSetup ()
	 *	\brief For synchronous operations use AbortSetupWait()
	 *	\return void
	 *
	 */
	void BootEncryption::AbortSetup ()
	{
		CallDriver (GST_IOCTL_ABORT_BOOT_ENCRYPTION_SETUP);
	}


	/**
	 *
	 *	\fn void BootEncryption::AbortSetupWait ()
	 *	\brief For asynchronous operations use AbortSetup()
	 *	\return void
	 *
	 */
	void BootEncryption::AbortSetupWait ()
	{
		CallDriver (GST_IOCTL_ABORT_BOOT_ENCRYPTION_SETUP);

		BootEncryptionStatus encStatus = GetStatus();

		while (encStatus.SetupInProgress)
		{
			Sleep (GST_ABORT_TRANSFORM_WAIT_INTERVAL);
			encStatus = GetStatus();
		}
	}


	/**
	 *
	 *	\fn void BootEncryption::BackupSystemLoader ()
	 *	\brief Backup system loader
	 *	\return void
	 *
	 */
	void BootEncryption::BackupSystemLoader ()
	{
		Device device (GetSystemDriveConfiguration().DevicePath, true);
	
		byte bootLoaderBuf[GST_BOOT_LOADER_AREA_SECTOR_COUNT * GST_SECTOR_SIZE_BIOS];

		device.SeekAt (0);
		device.Read (bootLoaderBuf, sizeof (bootLoaderBuf));

		// Prevent GostCrypt loader from being backed up
		for (size_t i = 0; i < sizeof (bootLoaderBuf) - strlen (GST_APP_NAME); ++i)
		{
			if (memcmp (bootLoaderBuf + i, GST_APP_NAME, strlen (GST_APP_NAME)) == 0)
			{
				if (AskWarnNoYes ("GST_BOOT_LOADER_ALREADY_INSTALLED") == IDNO)
					throw UserAbort (SRC_POS);
				return;
			}
		}

		File backupFile (GetSystemLoaderBackupPath(), false, true);
		backupFile.Write (bootLoaderBuf, sizeof (bootLoaderBuf));
	}


	/**
	 *
	 *	\fn void BootEncryption::RestoreSystemLoader ()
	 *	\brief Restore the system loader
	 *	\return void
	 *
	 */
	void BootEncryption::RestoreSystemLoader ()
	{
		byte bootLoaderBuf[GST_BOOT_LOADER_AREA_SECTOR_COUNT * GST_SECTOR_SIZE_BIOS];

		File backupFile (GetSystemLoaderBackupPath(), true);
		
		if (backupFile.Read (bootLoaderBuf, sizeof (bootLoaderBuf)) != sizeof (bootLoaderBuf))
			throw ParameterIncorrect (SRC_POS);

		Device device (GetSystemDriveConfiguration().DevicePath);

		// Preserve current partition table
		byte mbr[GST_SECTOR_SIZE_BIOS];
		device.SeekAt (0);
		device.Read (mbr, sizeof (mbr));
		memcpy (bootLoaderBuf + GST_MAX_MBR_BOOT_CODE_SIZE, mbr + GST_MAX_MBR_BOOT_CODE_SIZE, sizeof (mbr) - GST_MAX_MBR_BOOT_CODE_SIZE);

		device.SeekAt (0);
		device.Write (bootLoaderBuf, sizeof (bootLoaderBuf));
	}

#endif // SETUP

	/**
	 *
	 *	\fn void BootEncryption::RegisterFilter (bool registerFilter, FilterType filterType, const GUID *deviceClassGuid)
	 *	\brief Register the filter
	 *	\param[in] const GUID
	 *	\param[in] FilterType filterType
	 *	\param[in] bool registerFilter
	 *	\return void
	 *
	 */
	void BootEncryption::RegisterFilter (bool registerFilter, FilterType filterType, const GUID *deviceClassGuid)
	{
		string filter;
		string filterReg;
		HKEY regKey;

		switch (filterType)
		{
		case DriveFilter:
		case VolumeFilter:
			filter = "gostcrypt";
			filterReg = "UpperFilters";
			regKey = SetupDiOpenClassRegKey (deviceClassGuid, KEY_READ | KEY_WRITE);
			throw_sys_if (regKey == INVALID_HANDLE_VALUE);

			break;

		case DumpFilter:
			if (!IsOSAtLeast (WIN_VISTA))
				return;

			filter = "gostcrypt.sys";
			filterReg = "DumpFilters";
			SetLastError (RegOpenKeyEx (HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\CrashControl", 0, KEY_READ | KEY_WRITE, &regKey));
			throw_sys_if (GetLastError() != ERROR_SUCCESS);

			break;

		default:
			throw ParameterIncorrect (SRC_POS);
		}

		finally_do_arg (HKEY, regKey, { RegCloseKey (finally_arg); });

		if (registerFilter && filterType != DumpFilter)
		{
			// Register class filter below all other filters in the stack

			size_t strSize = filter.size() + 1;
			byte regKeyBuf[65536];
			DWORD size = sizeof (regKeyBuf) - strSize;

			// SetupInstallFromInfSection() does not support prepending of values so we have to modify the registry directly
			strncpy ((char *) regKeyBuf, filter.c_str(), sizeof (regKeyBuf));

			if (RegQueryValueEx (regKey, filterReg.c_str(), NULL, NULL, regKeyBuf + strSize, &size) != ERROR_SUCCESS)
				size = 1;

			SetLastError (RegSetValueEx (regKey, filterReg.c_str(), 0, REG_MULTI_SZ, regKeyBuf, strSize + size));
			throw_sys_if (GetLastError() != ERROR_SUCCESS);
		}
		else
		{
			string infFileName = GetTempPath() + "\\gostcrypt_driver_setup.inf";

			File infFile (infFileName, false, true);
			finally_do_arg (string, infFileName, { DeleteFile (finally_arg.c_str()); });

			string infTxt = "[gostcrypt]\r\n"
							+ string (registerFilter ? "Add" : "Del") + "Reg=gostcrypt_reg\r\n\r\n"
							"[gostcrypt_reg]\r\n"
							"HKR,,\"" + filterReg + "\",0x0001" + string (registerFilter ? "0008" : "8002") + ",\"" + filter + "\"\r\n";

			infFile.Write ((byte *) infTxt.c_str(), infTxt.size());
			infFile.Close();

			HINF hInf = SetupOpenInfFile (infFileName.c_str(), NULL, INF_STYLE_OLDNT | INF_STYLE_WIN4, NULL);
			throw_sys_if (hInf == INVALID_HANDLE_VALUE);
			finally_do_arg (HINF, hInf, { SetupCloseInfFile (finally_arg); });

			throw_sys_if (!SetupInstallFromInfSection (ParentWindow, hInf, "gostcrypt", SPINST_REGISTRY, regKey, NULL, 0, NULL, NULL, NULL, NULL));
		}
	}

	/**
	 *
	 *	\fn void BootEncryption::RegisterFilterDriver (bool registerDriver, FilterType filterType)
	 *	\brief Register the filter driver
	 *	\param[in] FilterType filterType
	 *	\param[in] bool registerDriver
	 *	\return void
	 *
	 */
	void BootEncryption::RegisterFilterDriver (bool registerDriver, FilterType filterType)
	{
		if (!IsAdmin() && IsUacSupported())
		{
			Elevator::RegisterFilterDriver (registerDriver, filterType);
			return;
		}

		switch (filterType)
		{
		case DriveFilter:
			RegisterFilter (registerDriver, filterType, &GUID_DEVCLASS_DISKDRIVE);
			break;

		case VolumeFilter:
			RegisterFilter (registerDriver, filterType, &GUID_DEVCLASS_VOLUME);
			RegisterFilter (registerDriver, filterType, &GUID_DEVCLASS_FLOPPYDISK);
			break;

		case DumpFilter:
			RegisterFilter (registerDriver, filterType);
			break;

		default:
			throw ParameterIncorrect (SRC_POS);
		}
	}

#ifndef SETUP

	/**
	 *
	 *	\fn void BootEncryption::RegisterSystemFavoritesService (BOOL registerService)
	 *	\brief Register the system favorites service
	 *	\param[in] BOOL registerService
	 *	\return void
	 *
	 */
	void BootEncryption::RegisterSystemFavoritesService (BOOL registerService)
	{
		if (!IsAdmin() && IsUacSupported())
		{
			Elevator::RegisterSystemFavoritesService (registerService);
			return;
		}

		SC_HANDLE scm = OpenSCManager (NULL, NULL, SC_MANAGER_ALL_ACCESS);
		throw_sys_if (!scm);

		string servicePath = GetServiceConfigPath (GST_APP_NAME ".exe");

		if (registerService)
		{
			try
			{
				RegisterSystemFavoritesService (FALSE);
			}
			catch (...) { }

			char appPath[GST_MAX_PATH];
			throw_sys_if (!GetModuleFileName (NULL, appPath, sizeof (appPath)));

			throw_sys_if (!CopyFile (appPath, servicePath.c_str(), FALSE));

			SC_HANDLE service = CreateService (scm,
				GST_SYSTEM_FAVORITES_SERVICE_NAME,
				GST_APP_NAME " System Favorites",
				SERVICE_ALL_ACCESS,
				SERVICE_WIN32_OWN_PROCESS,
				SERVICE_AUTO_START,
				SERVICE_ERROR_NORMAL,
				(string ("\"") + servicePath + "\" " GST_SYSTEM_FAVORITES_SERVICE_CMDLINE_OPTION).c_str(),
				GST_SYSTEM_FAVORITES_SERVICE_LOAD_ORDER_GROUP,
				NULL,
				NULL,
				NULL,
				NULL);

			throw_sys_if (!service);

			SERVICE_DESCRIPTION description;
			description.lpDescription = "Mounts GostCrypt system favorite volumes.";
			ChangeServiceConfig2 (service, SERVICE_CONFIG_DESCRIPTION, &description);

			CloseServiceHandle (service);

			try
			{
				WriteLocalMachineRegistryString ("SYSTEM\\CurrentControlSet\\Control\\SafeBoot\\Minimal\\" GST_SYSTEM_FAVORITES_SERVICE_NAME, NULL, "Service", FALSE);
				WriteLocalMachineRegistryString ("SYSTEM\\CurrentControlSet\\Control\\SafeBoot\\Network\\" GST_SYSTEM_FAVORITES_SERVICE_NAME, NULL, "Service", FALSE);

				SetDriverConfigurationFlag (GST_DRIVER_CONFIG_CACHE_BOOT_PASSWORD_FOR_SYS_FAVORITES, true);
			}
			catch (...)
			{
				try
				{
					RegisterSystemFavoritesService (false);
				}
				catch (...) { }

				throw;
			}
		}
		else
		{
			SetDriverConfigurationFlag (GST_DRIVER_CONFIG_CACHE_BOOT_PASSWORD_FOR_SYS_FAVORITES, false);

			DeleteLocalMachineRegistryKey ("SYSTEM\\CurrentControlSet\\Control\\SafeBoot\\Minimal", GST_SYSTEM_FAVORITES_SERVICE_NAME);
			DeleteLocalMachineRegistryKey ("SYSTEM\\CurrentControlSet\\Control\\SafeBoot\\Network", GST_SYSTEM_FAVORITES_SERVICE_NAME);

			SC_HANDLE service = OpenService (scm, GST_SYSTEM_FAVORITES_SERVICE_NAME, SERVICE_ALL_ACCESS);
			throw_sys_if (!service);

			throw_sys_if (!DeleteService (service));
			CloseServiceHandle (service);

			DeleteFile (servicePath.c_str());
		}
	}

	/**
	 *
	 *	\fn void BootEncryption::CheckRequirements ()
	 *	\brief Checks the requirements
	 *	\return void
	 *
	 */
	void BootEncryption::CheckRequirements ()
	{
		if (nCurrentOS == WIN_2000)
			throw ErrorException ("SYS_ENCRYPTION_UNSUPPORTED_ON_CURRENT_OS");
 
		if (CurrentOSMajor == 6 && CurrentOSMinor == 0 && CurrentOSServicePack < 1)
			throw ErrorException ("SYS_ENCRYPTION_UNSUPPORTED_ON_VISTA_SP0");

		if (IsNonInstallMode())
			throw ErrorException ("FEATURE_REQUIRES_INSTALLATION");

		SystemDriveConfiguration config = GetSystemDriveConfiguration ();

		if (config.SystemPartition.IsGPT)
			throw ErrorException ("GPT_BOOT_DRIVE_UNSUPPORTED");

		if (SystemDriveIsDynamic())
			throw ErrorException ("SYSENC_UNSUPPORTED_FOR_DYNAMIC_DISK");

		if (config.InitialUnallocatedSpace < GST_BOOT_LOADER_AREA_SIZE)
			throw ErrorException ("NO_SPACE_FOR_BOOT_LOADER");

		DISK_GEOMETRY geometry = GetDriveGeometry (config.DriveNumber);

		if (geometry.BytesPerSector != GST_SECTOR_SIZE_BIOS)
			throw ErrorException ("SYSENC_UNSUPPORTED_SECTOR_SIZE_BIOS");

		bool activePartitionFound = false;
		if (!config.SystemPartition.IsGPT)
		{
			// Determine whether there is an Active partition on the system drive
			foreach (const Partition &partition, config.Partitions)
			{
				if (partition.Info.BootIndicator)
				{
					activePartitionFound = true;
					break;
				}
			}
		}

		if (!config.SystemLoaderPresent || !activePartitionFound)
		{
			static bool confirmed = false;

			if (!confirmed && AskWarnNoYes ("WINDOWS_NOT_ON_BOOT_DRIVE_ERROR") == IDNO)
				throw UserAbort (SRC_POS);

			confirmed = true;
		}
	}


	/**
	 *
	 *	\fn void BootEncryption::CheckRequirementsHiddenOS ()
	 *	\brief Checks the hidden OS requirements
	 *	\return void
	 *
	 */
	void BootEncryption::CheckRequirementsHiddenOS ()
	{
		// It is assumed that CheckRequirements() had been called (so we don't check e.g. whether it's GPT).

		// The user may have modified/added/deleted partitions since the partition table was last scanned.
		InvalidateCachedSysDriveProperties ();

		GetPartitionForHiddenOS ();
	}


	/**
	 *
	 *	\fn void BootEncryption::InitialSecurityChecksForHiddenOS ()
	 *	\brief The initial security checks for hidden OS
	 *	\return
	 *
	 */
	void BootEncryption::InitialSecurityChecksForHiddenOS ()
	{
		char windowsDrive = (char) toupper (GetWindowsDirectory()[0]);

		// Paging files
		bool pagingFilesOk = !IsPagingFileActive (TRUE);

		char pagingFileRegData[65536];
		DWORD pagingFileRegDataSize = sizeof (pagingFileRegData);

		if (ReadLocalMachineRegistryMultiString ("System\\CurrentControlSet\\Control\\Session Manager\\Memory Management", "PagingFiles", pagingFileRegData, &pagingFileRegDataSize)
			&& pagingFileRegDataSize > 4)
		{
			for (size_t i = 1; i < pagingFileRegDataSize - 2; ++i)
			{
				if (memcmp (pagingFileRegData + i, ":\\", 2) == 0 && toupper (pagingFileRegData[i - 1]) != windowsDrive)
				{
					pagingFilesOk = false;
					break;
				}
			}
		}

		if (!pagingFilesOk)
		{
			if (AskWarnYesNoString ((wchar_t *) (wstring (GetString ("PAGING_FILE_NOT_ON_SYS_PARTITION")) 
				+ GetString ("LEAKS_OUTSIDE_SYSPART_UNIVERSAL_EXPLANATION")
				+ L"\n\n\n"
				+ GetString ("RESTRICT_PAGING_FILES_TO_SYS_PARTITION")
				).c_str()) == IDYES)
			{
				RestrictPagingFilesToSystemPartition();
				RestartComputer();
				AbortProcessSilent();
			}

			throw ErrorException (wstring (GetString ("PAGING_FILE_NOT_ON_SYS_PARTITION")) 
				+ GetString ("LEAKS_OUTSIDE_SYSPART_UNIVERSAL_EXPLANATION"));
		}

		// User profile
		char *configPath = GetConfigPath ("dummy");
		if (configPath && toupper (configPath[0]) != windowsDrive)
		{
			throw ErrorException (wstring (GetString ("USER_PROFILE_NOT_ON_SYS_PARTITION")) 
				+ GetString ("LEAKS_OUTSIDE_SYSPART_UNIVERSAL_EXPLANATION"));
		}

		// Temporary files
		if (toupper (GetTempPath()[0]) != windowsDrive)
		{
			throw ErrorException (wstring (GetString ("TEMP_NOT_ON_SYS_PARTITION")) 
				+ GetString ("LEAKS_OUTSIDE_SYSPART_UNIVERSAL_EXPLANATION"));
		}
	}


	/**
	 *
	 *	\fn void BootEncryption::Deinstall (bool displayWaitDialog)
	 *	\brief Uninstall
	 *			This operation may take a long time when an antivirus is installed and its real-time protection enabled.
				Therefore, if calling it without the wizard displayed, it should be called with displayWaitDialog set to true.
	 *	\param[in] bool displayWaitDialog)
	 *	\return void
	 *
	 */
	void BootEncryption::Deinstall (bool displayWaitDialog)
	{
		BootEncryptionStatus encStatus = GetStatus();

		if (encStatus.DriveEncrypted || encStatus.DriveMounted)
			throw ParameterIncorrect (SRC_POS);

		SystemDriveConfiguration config = GetSystemDriveConfiguration ();

		if (encStatus.VolumeHeaderPresent)
		{
			// Verify CRC of header salt
			Device device (config.DevicePath, true);
			byte header[GST_BOOT_ENCRYPTION_VOLUME_HEADER_SIZE];

			device.SeekAt (GST_BOOT_VOLUME_HEADER_SECTOR_OFFSET);
			device.Read (header, sizeof (header));

			if (encStatus.VolumeHeaderSaltCrc32 != GetCrc32 ((byte *) header, PKCS5_SALT_SIZE))
				throw ParameterIncorrect (SRC_POS);
		}

		try
		{
			RegisterFilterDriver (false, DriveFilter);
			RegisterFilterDriver (false, VolumeFilter);
			RegisterFilterDriver (false, DumpFilter);
			SetDriverServiceStartType (SERVICE_SYSTEM_START);
		}
		catch (...)
		{
			try
			{
				RegisterBootDriver (IsHiddenSystemRunning());
			}
			catch (...) { }

			throw;
		}

		SetHiddenOSCreationPhase (GST_HIDDEN_OS_CREATION_PHASE_NONE);	// In case RestoreSystemLoader() fails

		try
		{
			RegisterSystemFavoritesService (false);
		}
		catch (...) { }

		try
		{
			if (displayWaitDialog)
				DisplayStaticModelessWaitDlg (ParentWindow);

			finally_do_arg (bool, displayWaitDialog, { if (finally_arg) CloseStaticModelessWaitDlg(); });

			RestoreSystemLoader ();
		}
		catch (Exception &e)
		{
			e.Show (ParentWindow);
			throw ErrorException ("SYS_LOADER_RESTORE_FAILED");
		}
	}


	/**
	 *
	 *	\fn int BootEncryption::ChangePassword (Password *oldPassword, Password *newPassword, int pkcs5)
	 *	\brief Change the password
	 *	\param[in] int pkcs5
	 *	\param[in] Password * newPassword
	 *	\param[in] Password * oldPassword
	 *	\return int status if (status != 0)
	 *			result else
	 *
	 */
	int BootEncryption::ChangePassword (Password *oldPassword, Password *newPassword, int pkcs5)
	{
		BootEncryptionStatus encStatus = GetStatus();

		if (encStatus.SetupInProgress)
			throw ParameterIncorrect (SRC_POS);

		SystemDriveConfiguration config = GetSystemDriveConfiguration ();

		char header[GST_BOOT_ENCRYPTION_VOLUME_HEADER_SIZE];
		Device device (config.DevicePath);

		// Only one algorithm is currently supported
		if (pkcs5 != 0)
			throw ParameterIncorrect (SRC_POS);

		int64 headerOffset = GST_BOOT_VOLUME_HEADER_SECTOR_OFFSET;
		int64 backupHeaderOffset = -1;

		if (encStatus.HiddenSystem)
		{
			headerOffset = encStatus.HiddenSystemPartitionStart + GST_HIDDEN_VOLUME_HEADER_OFFSET;

			// Find hidden system partition
			foreach (const Partition &partition, config.Partitions)
			{
				if (partition.Info.StartingOffset.QuadPart == encStatus.HiddenSystemPartitionStart)
				{
					backupHeaderOffset = partition.Info.StartingOffset.QuadPart + partition.Info.PartitionLength.QuadPart - GST_VOLUME_HEADER_SIZE;
					break;
				}
			}

			if (backupHeaderOffset == -1)
				throw ParameterIncorrect (SRC_POS);
		}

		device.SeekAt (headerOffset);
		device.Read ((byte *) header, sizeof (header));

		PCRYPTO_INFO cryptoInfo = NULL;
		
		int status = ReadVolumeHeader (!encStatus.HiddenSystem, header, oldPassword, &cryptoInfo, NULL);
		finally_do_arg (PCRYPTO_INFO, cryptoInfo, { if (finally_arg) crypto_close (finally_arg); });

		if (status != 0)
		{
			handleError (ParentWindow, status);
			return status;
		}

		// Change the PKCS-5 PRF if requested by user
		if (pkcs5 != 0)
		{
			cryptoInfo->pkcs5 = pkcs5;
			RandSetHashFunction (pkcs5);
		}

		throw_sys_if (Randinit () != 0);
		finally_do ({ RandStop (FALSE); });

		NormalCursor();
		UserEnrichRandomPool (ParentWindow);
		WaitCursor();

		/**
		 *
		 *	The header will be re-encrypted PRAND_DISK_WIPE_PASSES times to prevent adversaries from using 
		 *	techniques such as magnetic force microscopy or magnetic force scanning tunnelling microscopy
		 *	to recover the overwritten header. According to Peter Gutmann, data should be overwritten 22
		 *	times (ideally, 35 times) using non-random patterns and pseudorandom data. However, as users might
		 *	impatiently interupt the process (etc.) we will not use the Gutmann's patterns but will write the
		 *	valid re-encrypted header, i.e. pseudorandom data, and there will be many more passes than Guttman
		 *	recommends. During each pass we will write a valid working header. Each pass will use the same master
		 *	key, and also the same header key, secondary key (XTS), etc., derived from the new password. The only
		 *	item that will be different for each pass will be the salt. This is sufficient to cause each "version"
		 *	of the header to differ substantially and in a random manner from the versions written during the
		 *	other passes. 
		*
		*/

		bool headerUpdated = false;
		int result = ERR_SUCCESS;

		try
		{
			BOOL backupHeader = FALSE;
			while (TRUE)
			{
				for (int wipePass = 0; wipePass < PRAND_DISK_WIPE_PASSES; wipePass++)
				{
					PCRYPTO_INFO tmpCryptoInfo = NULL;

					status = CreateVolumeHeaderInMemory (!encStatus.HiddenSystem,
						header,
						cryptoInfo->ea,
						cryptoInfo->mode,
						newPassword,
						cryptoInfo->pkcs5,
						(char *) cryptoInfo->master_keydata,
						&tmpCryptoInfo,
						cryptoInfo->VolumeSize.Value,
						cryptoInfo->hiddenVolumeSize,
						cryptoInfo->EncryptedAreaStart.Value,
						cryptoInfo->EncryptedAreaLength.Value,
						cryptoInfo->RequiredProgramVersion,
						cryptoInfo->HeaderFlags | GST_HEADER_FLAG_ENCRYPTED_SYSTEM,
						cryptoInfo->SectorSize,
						wipePass < PRAND_DISK_WIPE_PASSES - 1);

					if (tmpCryptoInfo)
						crypto_close (tmpCryptoInfo);

					if (status != 0)
					{
						handleError (ParentWindow, status);
						return status;
					}

					device.SeekAt (headerOffset);
					device.Write ((byte *) header, sizeof (header));
					headerUpdated = true;
				}

				if (!encStatus.HiddenSystem || backupHeader)
					break;

				backupHeader = TRUE;
				headerOffset = backupHeaderOffset;
			}
		}
		catch (Exception &e)
		{
			e.Show (ParentWindow);
			result = ERR_OS_ERROR;
		}

		if (headerUpdated)
		{
			ReopenBootVolumeHeaderRequest reopenRequest;
			reopenRequest.VolumePassword = *newPassword;
			finally_do_arg (ReopenBootVolumeHeaderRequest*, &reopenRequest, { burn (finally_arg, sizeof (*finally_arg)); });

			CallDriver (GST_IOCTL_REOPEN_BOOT_VOLUME_HEADER, &reopenRequest, sizeof (reopenRequest));
		}

		return result;
	}


	/**
	 *
	 *	\fn void BootEncryption::CheckEncryptionSetupResult ()
	 *	\brief Checks the encryption setup result
	 *	\return void
	 *
	 */
	void BootEncryption::CheckEncryptionSetupResult ()
	{
		CallDriver (GST_IOCTL_GET_BOOT_ENCRYPTION_SETUP_RESULT);
	}


	/**
	 *
	 *	\fn void BootEncryption::Install (bool hiddenSystem)
	 *	\brief Install
	 *	\param[in] bool hiddenSystem
	 *	\return void
	 *
	 */
	void BootEncryption::Install (bool hiddenSystem)
	{
		BootEncryptionStatus encStatus = GetStatus();
		if (encStatus.DriveMounted)
			throw ParameterIncorrect (SRC_POS);

		try
		{
			InstallBootLoader (false, hiddenSystem);

			if (!hiddenSystem)
				InstallVolumeHeader ();

			RegisterBootDriver (hiddenSystem);
		}
		catch (Exception &)
		{
			try
			{
				RestoreSystemLoader ();
			}
			catch (Exception &e)
			{
				e.Show (ParentWindow);
			}

			throw;
		}
	}


	/**
	 *
	 *	\fn void BootEncryption::PrepareHiddenOSCreation (int ea, int mode, int pkcs5)
	 *	\brief Prepare the hidden OS creation
	 *	\param[in] int pkcs5
	 *	\param[in] int mode
	 *	\param[in] int ea
	 *	\return void
	 *
	 */
	void BootEncryption::PrepareHiddenOSCreation (int ea, int mode, int pkcs5)
	{
		BootEncryptionStatus encStatus = GetStatus();
		if (encStatus.DriveMounted)
			throw ParameterIncorrect (SRC_POS);

		CheckRequirements();
		BackupSystemLoader();

		SelectedEncryptionAlgorithmId = ea;
	}


	/**
	 *
	 *	\fn void BootEncryption::PrepareInstallation (bool systemPartitionOnly, Password &password, int ea, int mode, int pkcs5, const string &rescueIsoImagePath)
	 *	\brief Prepare the installation
	 *	\param[in] const string
	 *	\param[in] int pkcs5
	 *	\param[in] int mode
	 *	\param[in] int ea
	 *	\param[in] Password & password
	 *	\param[in] bool systemPartitionOnly
	 *	\return void
	 *
	 */
	void BootEncryption::PrepareInstallation (bool systemPartitionOnly, Password &password, int ea, int mode, int pkcs5, const string &rescueIsoImagePath)
	{
		BootEncryptionStatus encStatus = GetStatus();
		if (encStatus.DriveMounted)
			throw ParameterIncorrect (SRC_POS);

		CheckRequirements ();

		SystemDriveConfiguration config = GetSystemDriveConfiguration();

		// Some chipset drivers may prevent access to the last sector of the drive
		if (!systemPartitionOnly)
		{
			DISK_GEOMETRY geometry = GetDriveGeometry (config.DriveNumber);
			Buffer sector (geometry.BytesPerSector);

			Device device (config.DevicePath);

			try
			{
				device.SeekAt (config.DrivePartition.Info.PartitionLength.QuadPart - geometry.BytesPerSector);
				device.Read (sector.Ptr(), sector.Size());
			}
			catch (SystemException &e)
			{
				if (e.ErrorCode != ERROR_CRC)
				{
					e.Show (ParentWindow);
					Error ("WHOLE_DRIVE_ENCRYPTION_PREVENTED_BY_DRIVERS");
					throw UserAbort (SRC_POS);
				}
			}
		}

		BackupSystemLoader ();

		uint64 volumeSize;
		uint64 encryptedAreaStart;

		if (systemPartitionOnly)
		{
			volumeSize = config.SystemPartition.Info.PartitionLength.QuadPart;
			encryptedAreaStart = config.SystemPartition.Info.StartingOffset.QuadPart;
		}
		else
		{
			volumeSize = config.DrivePartition.Info.PartitionLength.QuadPart - GST_BOOT_LOADER_AREA_SIZE;
			encryptedAreaStart = config.DrivePartition.Info.StartingOffset.QuadPart + GST_BOOT_LOADER_AREA_SIZE;
		}

		SelectedEncryptionAlgorithmId = ea;
		CreateVolumeHeader (volumeSize, encryptedAreaStart, &password, ea, mode, pkcs5);
		
		if (!rescueIsoImagePath.empty())
			CreateRescueIsoImage (true, rescueIsoImagePath);
	}

	/**
	 *
	 *	\fn bool BootEncryption::IsPagingFileActive (BOOL checkNonWindowsPartitionsOnly)
	 *	\brief Checks if the paging file is active
	 *	\param[in] BOOL checkNonWindowsPartitionsOnly
	 *	\return bool ::IsPagingFileActive (checkNonWindowsPartitionsOnly) ? true : false
	 *
	 */
	bool BootEncryption::IsPagingFileActive (BOOL checkNonWindowsPartitionsOnly)
	{
		if (!IsAdmin() && IsUacSupported())
			return Elevator::IsPagingFileActive (checkNonWindowsPartitionsOnly) ? true : false;

		return ::IsPagingFileActive (checkNonWindowsPartitionsOnly) ? true : false;
	}

	/**
	 *
	 *	\fn void BootEncryption::RestrictPagingFilesToSystemPartition ()
	 *	\brief Restrict the paging file to system partitions
	 *	\return void
	 *
	 */
	void BootEncryption::RestrictPagingFilesToSystemPartition ()
	{
		char pagingFiles[128];
		strncpy (pagingFiles, "X:\\pagefile.sys 0 0", sizeof (pagingFiles));
		pagingFiles[0] = GetWindowsDirectory()[0];

		throw_sys_if (!WriteLocalMachineRegistryMultiString ("System\\CurrentControlSet\\Control\\Session Manager\\Memory Management", "PagingFiles", pagingFiles, strlen (pagingFiles) + 2));
	}

	/**
	 *
	 *	\fn void BootEncryption::WriteLocalMachineRegistryDwordValue (char *keyPath, char *valueName, DWORD value)
	 *	\brief Write the DWORD value on local machine registry
	 *	\param[in] DWORD value
	 *	\param[in] char * valueName
	 *	\param[in] char * keyPath
	 *	\return void
	 *
	 */
	void BootEncryption::WriteLocalMachineRegistryDwordValue (char *keyPath, char *valueName, DWORD value)
	{
		if (!IsAdmin() && IsUacSupported())
		{
			Elevator::WriteLocalMachineRegistryDwordValue (keyPath, valueName, value);
			return;
		}

		throw_sys_if (!WriteLocalMachineRegistryDword (keyPath, valueName, value));
	}

/**
 *
 *	\fn void BootEncryption::SetDriverConfigurationFlag (uint32 flag, bool state)
 *	\brief Set the driver configuration flag
 *	\param[in] bool state
 *	\param[in] uint32 flag
 *	\return void
 *
 */
	void BootEncryption::SetDriverConfigurationFlag (uint32 flag, bool state)
	{
		DWORD configMap = ReadDriverConfigurationFlags();

		if (state)
			configMap |= flag;
		else
			configMap &= ~flag;

		WriteLocalMachineRegistryDwordValue ("SYSTEM\\CurrentControlSet\\Services\\gostcrypt", GST_DRIVER_CONFIG_REG_VALUE_NAME, configMap);
	}

	/**
	 *
	 *	\fn void BootEncryption::StartDecryption (BOOL discardUnreadableEncryptedSectors)
	 *	\brief Start the decryption
	 *	\param[in] BOOL discardUnreadableEncryptedSectors
	 *	\return void
	 *
	 */
	void BootEncryption::StartDecryption (BOOL discardUnreadableEncryptedSectors)
	{
		BootEncryptionStatus encStatus = GetStatus();

		if (!encStatus.DeviceFilterActive || !encStatus.DriveMounted || encStatus.SetupInProgress)
			throw ParameterIncorrect (SRC_POS);

		BootEncryptionSetupRequest request;
		ZeroMemory (&request, sizeof (request));
		
		request.SetupMode = SetupDecryption;
		request.DiscardUnreadableEncryptedSectors = discardUnreadableEncryptedSectors;

		CallDriver (GST_IOCTL_BOOT_ENCRYPTION_SETUP, &request, sizeof (request), NULL, 0);
	}

	/**
	 *
	 *	\fn void BootEncryption::StartEncryption (WipeAlgorithmId wipeAlgorithm, bool zeroUnreadableSectors)
	 *	\brief Start the encryption
	 *	\param[in] bool zeroUnreadableSectors
	 *	\param[in] WipeAlgorithmId wipeAlgorithm
	 *	\return void
	 *
	 */
	void BootEncryption::StartEncryption (WipeAlgorithmId wipeAlgorithm, bool zeroUnreadableSectors)
	{
		BootEncryptionStatus encStatus = GetStatus();

		if (!encStatus.DeviceFilterActive || !encStatus.DriveMounted || encStatus.SetupInProgress)
			throw ParameterIncorrect (SRC_POS);

		BootEncryptionSetupRequest request;
		ZeroMemory (&request, sizeof (request));
		
		request.SetupMode = SetupEncryption;
		request.WipeAlgorithm = wipeAlgorithm;
		request.ZeroUnreadableSectors = zeroUnreadableSectors;

		CallDriver (GST_IOCTL_BOOT_ENCRYPTION_SETUP, &request, sizeof (request), NULL, 0);
	}

	/**
	 *
	 *	\fn void BootEncryption::CopyFileAdmin (const string &sourceFile, const string &destinationFile)
	 *	\brief Copy admin file
	 *	\param[in] const string & destinationFile
	 *	\param[in] const string & sourceFile
	 *	\return void
	 *
	 */
	void BootEncryption::CopyFileAdmin (const string &sourceFile, const string &destinationFile)
	{
		if (!IsAdmin())
		{
			if (!IsUacSupported())
			{
				SetLastError (ERROR_ACCESS_DENIED);
				throw SystemException();
			}
			else
				Elevator::CopyFile (sourceFile, destinationFile);
		}
		else
			throw_sys_if (!::CopyFile (sourceFile.c_str(), destinationFile.c_str(), FALSE));
	}

	/**
	 *
	 *	\fn void BootEncryption::DeleteFileAdmin (const string &file)
	 *	\brief Delete the admin file
	 *	\param[in] const string & file
	 *	\return void
	 *
	 */
	void BootEncryption::DeleteFileAdmin (const string &file)
	{
		if (!IsAdmin() && IsUacSupported())
			Elevator::DeleteFile (file);
		else
			throw_sys_if (!::DeleteFile (file.c_str()));
	}

#endif // !SETUP

	/**
	 *
	 *	\fn uint32 BootEncryption::ReadDriverConfigurationFlags ()
	 *	\brief Read the driver configuration flags
	 *	\return uint32 configMap
	 *
	 */
	uint32 BootEncryption::ReadDriverConfigurationFlags ()
	{
		DWORD configMap;

		if (!ReadLocalMachineRegistryDword ("SYSTEM\\CurrentControlSet\\Services\\gostcrypt", GST_DRIVER_CONFIG_REG_VALUE_NAME, &configMap))
			configMap = 0;

		return configMap;
	}

	/**
	 *
	 *	\fn void BootEncryption::WriteBootDriveSector (uint64 offset, byte *data)
	 *	\brief Write on boot drive sector
	 *	\param[in] byte * data
	 *	\param[in] uint64 offset
	 *	\return void
	 *
	 */
	void BootEncryption::WriteBootDriveSector (uint64 offset, byte *data)
	{
		WriteBootDriveSectorRequest request;
		request.Offset.QuadPart = offset;
		memcpy (request.Data, data, sizeof (request.Data));

		CallDriver (GST_IOCTL_WRITE_BOOT_DRIVE_SECTOR, &request, sizeof (request), NULL, 0);
	}

	/**
	 *
	 *	\fn void BootEncryption::RegisterBootDriver (bool hiddenSystem)
	 *	\brief Register the boot drive
	 *	\param[in] bool hiddenSystem
	 *	\return
	 *
	 */
	void BootEncryption::RegisterBootDriver (bool hiddenSystem)
	{
		SetDriverServiceStartType (SERVICE_BOOT_START);

		try
		{
			RegisterFilterDriver (false, DriveFilter);
			RegisterFilterDriver (false, VolumeFilter);
			RegisterFilterDriver (false, DumpFilter);
		}
		catch (...) { }

		try
		{
			RegisterFilterDriver (true, DriveFilter);

			if (hiddenSystem)
				RegisterFilterDriver (true, VolumeFilter);

			RegisterFilterDriver (true, DumpFilter);
		}
		catch (...)
		{
			try { RegisterFilterDriver (false, DriveFilter); } catch (...) { }
			try { RegisterFilterDriver (false, VolumeFilter); } catch (...) { }
			try { RegisterFilterDriver (false, DumpFilter); } catch (...) { }
			try { SetDriverServiceStartType (SERVICE_SYSTEM_START); } catch (...) { }

			throw;
		}
	}

	/**
	 *
	 *	\fn bool BootEncryption::RestartComputer (void)
	 *	\brief Restor the computer
	 *	\return bool (::RestartComputer() != FALSE)
	 *
	 */
	bool BootEncryption::RestartComputer (void)
	{
		return (::RestartComputer() != FALSE);
	}
}
