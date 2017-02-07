/**
 *
 *	\file BootConfig.cpp
 *	\brief Manage the boot configurations
 *	\version 1.3.1
 *	\date 13/12/2016
 *
 */

/**
 *
 *	Copyright (c) 2008-2012 TrueCrypt Developers Association. All rights reserved.
 *	
 *	Governed by the TrueCrypt License 3.0 the full text of which is contained in
 *	the file License.txt included in TrueCrypt binary and source code distribution
 *	packages.
 *
 */

#include "BootConfig.h"

byte BootSectorFlags;

byte BootLoaderDrive;
byte BootDrive;
bool BootDriveGeometryValid = false;
bool PreventNormalSystemBoot = false;
bool PreventBootMenu = false;
char CustomUserMessage[GST_BOOT_SECTOR_USER_MESSAGE_MAX_LENGTH + 1];
uint32 OuterVolumeBackupHeaderCrc;

bool BootStarted = false;

DriveGeometry BootDriveGeometry;

CRYPTO_INFO *BootCryptoInfo;
Partition EncryptedVirtualPartition;

Partition ActivePartition;
Partition PartitionFollowingActive;
bool ExtraBootPartitionPresent = false;
uint64 HiddenVolumeStartUnitNo;
uint64 HiddenVolumeStartSector;

#ifndef GST_WINDOWS_BOOT_RESCUE_DISK_MODE

/**
 *
 *	\fn void ReadBootSectorUserConfiguration ()
 *	\brief Read the boot sector of user configuration
 *	\return void
 *
 */
void ReadBootSectorUserConfiguration ()
{
	byte userConfig;

	AcquireSectorBuffer();

	if (ReadWriteMBR (false, BootLoaderDrive, true) != BiosResultSuccess)
		goto ret;

	userConfig = SectorBuffer[GST_BOOT_SECTOR_USER_CONFIG_OFFSET];

	PreventBootMenu = (userConfig & GST_BOOT_USER_CFG_FLAG_DISABLE_ESC);

	memcpy (CustomUserMessage, SectorBuffer + GST_BOOT_SECTOR_USER_MESSAGE_OFFSET, GST_BOOT_SECTOR_USER_MESSAGE_MAX_LENGTH);
	CustomUserMessage[GST_BOOT_SECTOR_USER_MESSAGE_MAX_LENGTH] = 0;

	if (userConfig & GST_BOOT_USER_CFG_FLAG_SILENT_MODE)
	{
		if (CustomUserMessage[0])
		{
			InitVideoMode();
			Print (CustomUserMessage);
		}

		DisableScreenOutput();
	}

	OuterVolumeBackupHeaderCrc = *(uint32 *) (SectorBuffer + GST_BOOT_SECTOR_OUTER_VOLUME_BAK_HEADER_CRC_OFFSET);

ret:
	ReleaseSectorBuffer();
}


/**
 *
 *	\fn BiosResult UpdateBootSectorConfiguration (byte drive)
 *	\brief Update boot sector configuration
 *	\param[in] byte drive
 *	\return BiosResult result
 *
 */
BiosResult UpdateBootSectorConfiguration (byte drive)
{
	AcquireSectorBuffer();

	BiosResult result = ReadWriteMBR (false, drive);
	if (result != BiosResultSuccess)
		goto ret;

	SectorBuffer[GST_BOOT_SECTOR_CONFIG_OFFSET] = BootSectorFlags;
	result = ReadWriteMBR (true, drive);

ret:
	ReleaseSectorBuffer();
	return result;
}

#endif // !GST_WINDOWS_BOOT_RESCUE_DISK_MODE
