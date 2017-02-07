/**
 *
 *	\file BootEncryptedIo.cpp
 *	\brief Manage to read and write and encrypted sectors
 *	\version 1.3.1
 *	\date 13/12/2016
 *
 */

/**
 *	
 *	Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.
 *	
 *	Governed by the TrueCrypt License 3.0 the full text of which is contained in
 *	the file License.txt included in TrueCrypt binary and source code distribution
 *	packages.
 *	
 */

#include "Crypto.h"
#include "Platform.h"
#include "BootConfig.h"
#include "BootDebug.h"
#include "BootDefs.h"
#include "BootDiskIo.h"
#include "BootEncryptedIo.h"


/**
 *
 *	\fn BiosResult ReadEncryptedSectors (uint16 destSegment, uint16 destOffset, byte drive, uint64 sector, uint16 sectorCount)
 *	\brief Read encrypted sectors
 *	\param[in] uint16 sectorCount
 *	\param[in] uint64 sector
 *	\param[in] byte drive
 *	\param[in] uint16 destOffset
 *	\param[in] uint16 destSegment
 *	\return BiosResult result
 *			BiosResultInvalidFunction
 *
 */
BiosResult ReadEncryptedSectors (uint16 destSegment, uint16 destOffset, byte drive, uint64 sector, uint16 sectorCount)
{
	BiosResult result;
	bool decrypt = true;

	if (BootCryptoInfo->hiddenVolume)
	{
		if (ReadWritePartiallyCoversEncryptedArea (sector, sectorCount))
			return BiosResultInvalidFunction;

		if (sector >= EncryptedVirtualPartition.StartSector && sector <= EncryptedVirtualPartition.EndSector)
		{
			// Remap the request to the hidden volume
			sector -= EncryptedVirtualPartition.StartSector;
			sector += HiddenVolumeStartSector;
		}
		else
			decrypt = false;
	}

	result = ReadSectors (destSegment, destOffset, drive, sector, sectorCount);

	if (result != BiosResultSuccess || !decrypt)
		return result;

	if (BootCryptoInfo->hiddenVolume)
	{
		// Convert sector number to data unit number of the hidden volume
		sector -= HiddenVolumeStartSector;
		sector += HiddenVolumeStartUnitNo;
	}

	if (drive == EncryptedVirtualPartition.Drive)
	{
		while (sectorCount-- > 0)
		{
			if (BootCryptoInfo->hiddenVolume
				|| (sector >= EncryptedVirtualPartition.StartSector && sector <= EncryptedVirtualPartition.EndSector))
			{
				AcquireSectorBuffer();
				CopyMemory (destSegment, destOffset, SectorBuffer, GST_LB_SIZE);

				DecryptDataUnits (SectorBuffer, &sector, 1, BootCryptoInfo);

				CopyMemory (SectorBuffer, destSegment, destOffset, GST_LB_SIZE);
				ReleaseSectorBuffer();
			}

			++sector;
			destOffset += GST_LB_SIZE;
		}
	}

	return result;
}


/**
 *
 *	\fn BiosResult WriteEncryptedSectors (uint16 sourceSegment, uint16 sourceOffset, byte drive, uint64 sector, uint16 sectorCount)
 *	\brief Write on encrypted sectors
 *	\param[in] uint16 sectorCount
 *	\param[in] uint64 sector
 *	\param[in] byte drive
 *	\param[in] uint16 sourceOffset
 *	\param[in] uint16 sourceSegment
 *	\return BiosResult result
 *
 */
BiosResult WriteEncryptedSectors (uint16 sourceSegment, uint16 sourceOffset, byte drive, uint64 sector, uint16 sectorCount)
{
	BiosResult result;
	AcquireSectorBuffer();
	uint64 dataUnitNo;
	uint64 writeOffset;

	dataUnitNo = sector;
	writeOffset.HighPart = 0;
	writeOffset.LowPart = 0;

	if (BootCryptoInfo->hiddenVolume)
	{
		if (ReadWritePartiallyCoversEncryptedArea (sector, sectorCount))
			return BiosResultInvalidFunction;

		// Remap the request to the hidden volume
		writeOffset = HiddenVolumeStartSector;
		writeOffset -= EncryptedVirtualPartition.StartSector;
		dataUnitNo -= EncryptedVirtualPartition.StartSector;
		dataUnitNo += HiddenVolumeStartUnitNo;
	}

	while (sectorCount-- > 0)
	{
		CopyMemory (sourceSegment, sourceOffset, SectorBuffer, GST_LB_SIZE);

		if (drive == EncryptedVirtualPartition.Drive && sector >= EncryptedVirtualPartition.StartSector && sector <= EncryptedVirtualPartition.EndSector)
		{
			EncryptDataUnits (SectorBuffer, &dataUnitNo, 1, BootCryptoInfo);
		}

		result = WriteSectors (SectorBuffer, drive, sector + writeOffset, 1);

		if (result != BiosResultSuccess)
			break;

		++sector;
		++dataUnitNo;
		sourceOffset += GST_LB_SIZE;
	}

	ReleaseSectorBuffer();
	return result;
}


/**
 *
 *	\fn static bool ReadWritePartiallyCoversEncryptedArea (const uint64 &sector, uint16 sectorCount)
 *	\brief Read, write and partially cover encrypted area
 *	\param[in] uint16 sectorCount
 *	\param[in] const uint64 & sector
 *	\return static bool
 *
 */
static bool ReadWritePartiallyCoversEncryptedArea (const uint64 &sector, uint16 sectorCount)
{
	uint64 readWriteEnd = sector + --sectorCount;

	return ((sector < EncryptedVirtualPartition.StartSector && readWriteEnd >= EncryptedVirtualPartition.StartSector)
		|| (sector >= EncryptedVirtualPartition.StartSector && readWriteEnd > EncryptedVirtualPartition.EndSector));
}
