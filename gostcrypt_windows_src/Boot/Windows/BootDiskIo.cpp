/**
 *
 *	\file BootDiskIo.cpp
 *	\brief Manage Sectors (read/write), MBR (read/write) and partition
 *	\version 1.3.1
 *	\date 13/12/2016
 *
 */

/**
 *	
 *	Copyright (c) 2008-2011 TrueCrypt Developers Association. All rights reserved.
 *	
 *	Governed by the TrueCrypt License 3.0 the full text of which is contained in
 *	the file License.txt included in TrueCrypt binary and source code distribution
 *	packages.
 *	
 */

#include "Bios.h"
#include "BootConsoleIo.h"
#include "BootConfig.h"
#include "BootDebug.h"
#include "BootDefs.h"
#include "BootDiskIo.h"
#include "BootStrings.h"


byte SectorBuffer[GST_LB_SIZE];

#ifdef GST_BOOT_DEBUG_ENABLED
static bool SectorBufferInUse = false;

/**
 *
 *	\fn void AcquireSectorBuffer ()
 *	\brief Mark sector buffer as being used
 *	\return void
 *
 */
void AcquireSectorBuffer ()
{
	if (SectorBufferInUse)
		GST_THROW_FATAL_EXCEPTION;

	SectorBufferInUse = true;
}


/**
 *
 *	\fn void ReleaseSectorBuffer ()
 *	\brief Mark sector buffer as free
 *	\return void
 *
 */
void ReleaseSectorBuffer ()
{
	SectorBufferInUse = false;
}

#endif


/**
 *
 *	\fn bool IsLbaSupported (byte drive)
 *	\brief Check if Lba is supported
 *	\param[in] byte drive
 *	\return bool CachedStatus if CachedDrive == drive
 *			true if result == 0xaa55
 *			false else
 *
 */
bool IsLbaSupported (byte drive)
{
	static byte CachedDrive = GST_INVALID_BIOS_DRIVE;
	static bool CachedStatus;
	uint16 result = 0;

	if (CachedDrive == drive)
		goto ret;

	__asm
	{
		mov bx, 0x55aa
		mov dl, drive
		mov ah, 0x41
		int 0x13
		jc err
		mov result, bx
	err:
	}

	CachedDrive = drive;
	CachedStatus = (result == 0xaa55);
ret:
	return CachedStatus;
}


/**
 *
 *	\fn void PrintDiskError (BiosResult error, bool write, byte drive, const uint64 *sector, const ChsAddress *chs)
 *	\brief Print dick errors
 *	\param[in] const ChsAddress * chs
 *	\param[in] const uint64 * sector
 *	\param[in] byte drive
 *	\param[in] bool write
 *	\param[in] BiosResult error
 *	\return void
 *
 */
void PrintDiskError (BiosResult error, bool write, byte drive, const uint64 *sector, const ChsAddress *chs)
{
	PrintEndl();
	Print (write ? "Write" : "Read"); Print (" error:");
	Print (error);
	Print (" Drive:");
	Print (drive ^ 0x80);

	if (sector)
	{
		Print (" Sector:");
		Print (*sector);
	}

	if (chs)
	{
		Print (" CHS:");
		Print (*chs);
	}

	PrintEndl();
	Beep();
}


/**
 *
 *	\fn void Print (const ChsAddress &chs)
 *	\brief Print a ChsAddress
 *	\param[in] const ChsAddress & chs
 *	\return void
 *
 */
void Print (const ChsAddress &chs)
{
	Print (chs.Cylinder);
	PrintChar ('/');
	Print (chs.Head);
	PrintChar ('/');
	Print (chs.Sector);
}


/**
 *
 *	\fn void PrintSectorCountInMB (const uint64 &sectorCount)
 *	\brief Print sectorcount in MB
 *	\param[in] const uint64 & sectorCount
 *	\return void
 *
 */
void PrintSectorCountInMB (const uint64 &sectorCount)
{
	Print (sectorCount >> (GST_LB_SIZE_BIT_SHIFT_DIVISOR + 2)); Print (" MB ");
}


/**
 *
 *	\fn BiosResult ReadWriteSectors (bool write, uint16 bufferSegment, uint16 bufferOffset, byte drive, const ChsAddress &chs, byte sectorCount, bool silent)
 *	\brief Read and write sectors
 *	\param[in] bool silent
 *	\param[in] byte sectorCount
 *	\param[in] const ChsAddress & chs
 *	\param[in] byte drive
 *	\param[in] uint16 bufferOffset
 *	\param[in] uint16 bufferSegment
 *	\param[in] bool write
 *	\return BiosResult result
 *
 */
BiosResult ReadWriteSectors (bool write, uint16 bufferSegment, uint16 bufferOffset, byte drive, const ChsAddress &chs, byte sectorCount, bool silent)
{
	CheckStack();

	byte cylinderLow = (byte) chs.Cylinder;
	byte sector = chs.Sector;
	sector |= byte (chs.Cylinder >> 2) & 0xc0;
	byte function = write ? 0x03 : 0x02;

	BiosResult result;
	byte tryCount = GST_MAX_BIOS_DISK_IO_RETRIES;

	do
	{
		result = BiosResultSuccess;

		__asm
		{
			push es
			mov ax, bufferSegment
			mov	es, ax
			mov	bx, bufferOffset
			mov dl, drive
			mov ch, cylinderLow
			mov si, chs
			mov dh, [si].Head
			mov cl, sector
			mov	al, sectorCount
			mov	ah, function
			int	0x13
			jnc ok				// If CF=0, ignore AH to prevent issues caused by potential bugs in BIOSes
			mov	result, ah
		ok:
			pop es
		}

		if (result == BiosResultEccCorrected)
			result = BiosResultSuccess;

	// Some BIOSes report I/O errors prematurely in some cases
	} while (result != BiosResultSuccess && --tryCount != 0);

	if (!silent && result != BiosResultSuccess)
		PrintDiskError (result, write, drive, nullptr, &chs);

	return result;
}


/**
 *
 *	\fn BiosResult ReadWriteSectors (bool write, byte *buffer, byte drive, const ChsAddress &chs, byte sectorCount, bool silent)
 *	\brief Read and write sectors
 *	\param[in] bool silent
 *	\param[in] byte sectorCount
 *	\param[in] const ChsAddress & chs
 *	\param[in] byte drive
 *	\param[in] byte * buffer
 *	\param[in] bool write
 *	\return BiosResult ReadWriteSectors (write, codeSeg, (uint16) buffer, drive, chs, sectorCount, silent)
 *
 */
BiosResult ReadWriteSectors (bool write, byte *buffer, byte drive, const ChsAddress &chs, byte sectorCount, bool silent)
{
	uint16 codeSeg;
	__asm mov codeSeg, cs
	return ReadWriteSectors (write, codeSeg, (uint16) buffer, drive, chs, sectorCount, silent);
}


/**
 *
 *	\fn BiosResult ReadSectors (byte *buffer, byte drive, const ChsAddress &chs, byte sectorCount, bool silent)
 *	\brief Read sectors
 *	\param[in] bool silent
 *	\param[in] byte sectorCount
 *	\param[in] const ChsAddress & chs
 *	\param[in] byte drive
 *	\param[in] byte * buffer
 *	\return BiosResult ReadWriteSectors (false, buffer, drive, chs, sectorCount, silent)
 *
 */
BiosResult ReadSectors (byte *buffer, byte drive, const ChsAddress &chs, byte sectorCount, bool silent)
{
	return ReadWriteSectors (false, buffer, drive, chs, sectorCount, silent);
}


/**
 *
 *	\fn BiosResult WriteSectors (byte *buffer, byte drive, const ChsAddress &chs, byte sectorCount, bool silent)
 *	\brief Write sectors
 *	\param[in] bool silent
 *	\param[in] byte sectorCount
 *	\param[in] const ChsAddress & chs
 *	\param[in] byte drive
 *	\param[in] byte * buffer
 *	\return BiosResult ReadWriteSectors (true, buffer, drive, chs, sectorCount, silent)
 *
 */
BiosResult WriteSectors (byte *buffer, byte drive, const ChsAddress &chs, byte sectorCount, bool silent)
{
	return ReadWriteSectors (true, buffer, drive, chs, sectorCount, silent);
}


/**
 *
 *	\fn static BiosResult ReadWriteSectors (bool write, BiosLbaPacket &dapPacket, byte drive, const uint64 &sector, uint16 sectorCount, bool silent)
 *	\brief Read and write sectors
 *	\param[in] bool silent
 *	\param[in] uint16 sectorCount
 *	\param[in] const uint64 & sector
 *	\param[in] byte drive
 *	\param[in] BiosLbaPacket & dapPacket
 *	\param[in] bool write
 *	\return BiosResult ReadWriteSectors (write, (uint16) (dapPacket.Buffer >> 16), (uint16) dapPacket.Buffer, drive, chs, sectorCount, silent) if LBA is not supported
 *			result else
 *
 */
static BiosResult ReadWriteSectors (bool write, BiosLbaPacket &dapPacket, byte drive, const uint64 &sector, uint16 sectorCount, bool silent)
{
	CheckStack();

	if (!IsLbaSupported (drive))
	{
		DriveGeometry geometry;

		BiosResult result = GetDriveGeometry (drive, geometry, silent);
		if (result != BiosResultSuccess)
			return result;

		ChsAddress chs;
		LbaToChs (geometry, sector, chs);
		return ReadWriteSectors (write, (uint16) (dapPacket.Buffer >> 16), (uint16) dapPacket.Buffer, drive, chs, sectorCount, silent);
	}

	dapPacket.Size = sizeof (dapPacket);
	dapPacket.Reserved = 0;
	dapPacket.SectorCount = sectorCount;
	dapPacket.Sector = sector;

	byte function = write ? 0x43 : 0x42;
	
	BiosResult result;
	byte tryCount = GST_MAX_BIOS_DISK_IO_RETRIES;

	do
	{
		result = BiosResultSuccess;

		__asm
		{
			mov	bx, 0x55aa
			mov	dl, drive
			mov si, [dapPacket]
			mov	ah, function
			xor al, al
			int	0x13
			jnc ok				// If CF=0, ignore AH to prevent issues caused by potential bugs in BIOSes
			mov	result, ah
		ok:
		}

		if (result == BiosResultEccCorrected)
			result = BiosResultSuccess;

	// Some BIOSes report I/O errors prematurely in some cases
	} while (result != BiosResultSuccess && --tryCount != 0);

	if (!silent && result != BiosResultSuccess)
		PrintDiskError (result, write, drive, &sector);

	return result;
}


/**
 *
 *	\fn static BiosResult ReadWriteSectors (bool write, byte *buffer, byte drive, const uint64 &sector, uint16 sectorCount, bool silent)
 *	\brief Read and write sectors
 *	\param[in] bool silent
 *	\param[in] uint16 sectorCount
 *	\param[in] const uint64 & sector
 *	\param[in] byte drive
 *	\param[in] byte * buffer
 *	\param[in] bool write
 *	\return static BiosResult ReadWriteSectors (write, dapPacket, drive, sector, sectorCount, silent)
 *
 */
static BiosResult ReadWriteSectors (bool write, byte *buffer, byte drive, const uint64 &sector, uint16 sectorCount, bool silent)
{
	BiosLbaPacket dapPacket;
	dapPacket.Buffer = (uint32) buffer;
	return ReadWriteSectors (write, dapPacket, drive, sector, sectorCount, silent);
}


/**
 *
 *	\fn BiosResult ReadWriteSectors (bool write, uint16 bufferSegment, uint16 bufferOffset, byte drive, const uint64 &sector, uint16 sectorCount, bool silent)
 *	\brief Read and write sectors
 *	\param[in] bool silent
 *	\param[in] uint16 sectorCount
 *	\param[in] const uint64 & sector
 *	\param[in] byte drive
 *	\param[in] uint16 bufferOffset
 *	\param[in] uint16 bufferSegment
 *	\param[in] bool write
 *	\return BiosResult ReadWriteSectors (write, dapPacket, drive, sector, sectorCount, silent)
 *
 */
BiosResult ReadWriteSectors (bool write, uint16 bufferSegment, uint16 bufferOffset, byte drive, const uint64 &sector, uint16 sectorCount, bool silent)
{
	BiosLbaPacket dapPacket;
	dapPacket.Buffer = ((uint32) bufferSegment << 16) | bufferOffset;
	return ReadWriteSectors (write, dapPacket, drive, sector, sectorCount, silent);
}

/**
 *
 *	\fn BiosResult ReadSectors (uint16 bufferSegment, uint16 bufferOffset, byte drive, const uint64 &sector, uint16 sectorCount, bool silent)
 *	\brief Read sectors
 *	\param[in] bool silent
 *	\param[in] uint16 sectorCount
 *	\param[in] const uint64 & sector
 *	\param[in] byte drive
 *	\param[in] uint16 bufferOffset
 *	\param[in] uint16 bufferSegment
 *	\return BiosResult ReadWriteSectors (false, bufferSegment, bufferOffset, drive, sector, sectorCount, silent)
 *
 */
BiosResult ReadSectors (uint16 bufferSegment, uint16 bufferOffset, byte drive, const uint64 &sector, uint16 sectorCount, bool silent)
{
	return ReadWriteSectors (false, bufferSegment, bufferOffset, drive, sector, sectorCount, silent);
}


/**
 *
 *	\fn BiosResult ReadSectors (byte *buffer, byte drive, const uint64 &sector, uint16 sectorCount, bool silent)
 *	\brief Read sectors
 *	\param[in] bool silent
 *	\param[in] uint16 sectorCount
 *	\param[in] const uint64 & sector
 *	\param[in] byte drive
 *	\param[in] byte * buffer
 *	\return BiosResult result
 *
 */
BiosResult ReadSectors (byte *buffer, byte drive, const uint64 &sector, uint16 sectorCount, bool silent)
{
	BiosResult result;
	uint16 codeSeg;
	__asm mov codeSeg, cs
	
	result = ReadSectors (BootStarted ? codeSeg : GST_BOOT_LOADER_ALT_SEGMENT, (uint16) buffer, drive, sector, sectorCount, silent);

	// Alternative segment is used to prevent memory corruption caused by buggy BIOSes
	if (!BootStarted)
		CopyMemory (GST_BOOT_LOADER_ALT_SEGMENT, (uint16) buffer, buffer, sectorCount * GST_LB_SIZE);

	return result;
}


/**
 *
 *	\fn BiosResult WriteSectors (byte *buffer, byte drive, const uint64 &sector, uint16 sectorCount, bool silent)
 *	\brief Write sectors
 *	\param[in] bool silent
 *	\param[in] uint16 sectorCount
 *	\param[in] const uint64 & sector
 *	\param[in] byte drive
 *	\param[in] byte * buffer
 *	\return BiosResult ReadWriteSectors (true, buffer, drive, sector, sectorCount, silent)
 *
 */
BiosResult WriteSectors (byte *buffer, byte drive, const uint64 &sector, uint16 sectorCount, bool silent)
{
	return ReadWriteSectors (true, buffer, drive, sector, sectorCount, silent);
}


/**
 *
 *	\fn BiosResult GetDriveGeometry (byte drive, DriveGeometry &geometry, bool silent)
 *	\brief Get geometry of the drive (cylinders, heads and sectors)
 *	\param[in] bool silent
 *	\param[in] DriveGeometry & geometry
 *	\param[in] byte drive
 *	\return BiosResult result
 *
 */
BiosResult GetDriveGeometry (byte drive, DriveGeometry &geometry, bool silent)
{
	CheckStack();

	byte maxCylinderLow, maxHead, maxSector;
	BiosResult result;
	__asm
	{
		push es
		mov dl, drive
		mov ah, 0x08
		int	0x13

		mov	result, ah
		mov maxCylinderLow, ch
		mov maxSector, cl
		mov maxHead, dh
		pop es
	}

	if (result == BiosResultSuccess)
	{
		geometry.Cylinders = (maxCylinderLow | (uint16 (maxSector & 0xc0) << 2)) + 1;
		geometry.Heads = maxHead + 1;
		geometry.Sectors = maxSector & ~0xc0;
	}
	else if (!silent)
	{
		Print ("Drive ");
		Print (drive ^ 0x80);
		Print (" not found: ");
		PrintErrorNoEndl ("");
		Print (result);
		PrintEndl();
	}

	return result;
}


/**
 *
 *	\fn void ChsToLba (const DriveGeometry &geometry, const ChsAddress &chs, uint64 &lba)
 *	\brief Transform Chs to Lba
 *	\param[in] uint64 & lba
 *	\param[in] const ChsAddress & chs
 *	\param[in] const DriveGeometry & lba
 *	\return void
 *
 */
void ChsToLba (const DriveGeometry &geometry, const ChsAddress &chs, uint64 &lba)
{
	lba.HighPart = 0;
	lba.LowPart = (uint32 (chs.Cylinder) * geometry.Heads + chs.Head) * geometry.Sectors + chs.Sector - 1;
}


/**
 *
 *	\fn void LbaToChs (const DriveGeometry &geometry, const uint64 &lba, ChsAddress &chs)
 *	\brief Transform Lba to Chs
 *	\param[in] ChsAddress & chs
 *	\param[in] const uint64 & lba
 *	\param[in] const DriveGeometry & geomtry
 *	\return void
 *
 */
void LbaToChs (const DriveGeometry &geometry, const uint64 &lba, ChsAddress &chs)
{
	chs.Sector = (byte) ((lba.LowPart % geometry.Sectors) + 1);
	uint32 ch = lba.LowPart / geometry.Sectors;
	chs.Head = (byte) (ch % geometry.Heads);
	chs.Cylinder = (uint16) (ch / geometry.Heads);
}


/**
 *
 *	\fn void PartitionEntryMBRToPartition (const PartitionEntryMBR &partEntry, Partition &partition)
 *	\brief Tranform PartitionEntryMBR to Partition
 *	\param[in] Partition & partition
 *	\param[in] const PartitionEntryMBR & partEntry
 *	\return void
 *
 */
void PartitionEntryMBRToPartition (const PartitionEntryMBR &partEntry, Partition &partition)
{
	partition.Active = partEntry.BootIndicator == 0x80;
	partition.EndSector.HighPart = 0;
	partition.EndSector.LowPart = partEntry.StartLBA + partEntry.SectorCountLBA - 1;
	partition.SectorCount.HighPart = 0;
	partition.SectorCount.LowPart = partEntry.SectorCountLBA;
	partition.StartSector.HighPart = 0;
	partition.StartSector.LowPart = partEntry.StartLBA;
	partition.Type = partEntry.Type;
}


/**
 *
 *	\fn BiosResult ReadWriteMBR (bool write, byte drive, bool silent)
 *	\brief Read and write MBR
 *	\param[in] bool silent
 *	\param[in] byte drive
 *	\param[in] bool write
 *	\return BiosResult WriteSectors (SectorBuffer, drive, mbrSector, 1, silent) if write == true
 *			ReadSectors (SectorBuffer, drive, mbrSector, 1, silent);
 *
 */
BiosResult ReadWriteMBR (bool write, byte drive, bool silent)
{
	uint64 mbrSector;
	mbrSector.HighPart = 0;
	mbrSector.LowPart = 0;

	if (write)
		return WriteSectors (SectorBuffer, drive, mbrSector, 1, silent);

	return ReadSectors (SectorBuffer, drive, mbrSector, 1, silent);		// Uses alternative segment
}


/**
 *
 *	\fn BiosResult GetDrivePartitions (byte drive, Partition *partitionArray, size_t partitionArrayCapacity, size_t &partitionCount, bool activeOnly, Partition *findPartitionFollowingThis, bool silent)
 *	\brief Get drive partitions
 *	\param[in] bool silent
 *	\param[in] Partition * findPartitionFollowingThis
 *	\param[in] bool activeOnly
 *	\param[in] size_t & partitionCount
 *	\param[in] size_t partitionArrayCapacity
 *	\param[in] Partition * partitionArray
 *	\param[in] byte drive
 *	\return BiosResult result
 *
 */
BiosResult GetDrivePartitions (byte drive, Partition *partitionArray, size_t partitionArrayCapacity, size_t &partitionCount, bool activeOnly, Partition *findPartitionFollowingThis, bool silent)
{
	Partition *followingPartition;
	Partition tmpPartition;

	if (findPartitionFollowingThis)
	{
		assert (partitionArrayCapacity == 1);
		partitionArrayCapacity = 0xff;
		followingPartition = partitionArray;
		partitionArray = &tmpPartition;

		followingPartition->Drive = GST_INVALID_BIOS_DRIVE;
		followingPartition->StartSector.LowPart = 0xFFFFffffUL;
	}

	AcquireSectorBuffer();
	BiosResult result = ReadWriteMBR (false, drive, silent);
	ReleaseSectorBuffer();

	partitionCount = 0;

	MBR *mbr = (MBR *) SectorBuffer;
	if (result != BiosResultSuccess || mbr->Signature != 0xaa55)
		return result;

	PartitionEntryMBR mbrPartitions[4];
	memcpy (mbrPartitions, mbr->Partitions, sizeof (mbrPartitions));
	size_t partitionArrayPos = 0, partitionNumber;
	
	for (partitionNumber = 0;
		partitionNumber < array_capacity (mbrPartitions) && partitionArrayPos < partitionArrayCapacity;
		++partitionNumber)
	{
		const PartitionEntryMBR &partEntry = mbrPartitions[partitionNumber];
		
		if (partEntry.SectorCountLBA > 0)
		{
			Partition &partition = partitionArray[partitionArrayPos];
			PartitionEntryMBRToPartition (partEntry, partition);

			if (activeOnly && !partition.Active)
				continue;

			partition.Drive = drive;
			partition.Number = partitionArrayPos;

			if (partEntry.Type == 0x5 || partEntry.Type == 0xf) // Extended partition
			{
				if (IsLbaSupported (drive))
				{
					// Find all extended partitions
					uint64 firstExtStartLBA = partition.StartSector;
					uint64 extStartLBA = partition.StartSector;
					MBR *extMbr = (MBR *) SectorBuffer;

					while (partitionArrayPos < partitionArrayCapacity &&
						(result = ReadSectors ((byte *) extMbr, drive, extStartLBA, 1, silent)) == BiosResultSuccess
						&& extMbr->Signature == 0xaa55)
					{
						if (extMbr->Partitions[0].SectorCountLBA > 0)
						{
							Partition &logPart = partitionArray[partitionArrayPos];
							PartitionEntryMBRToPartition (extMbr->Partitions[0], logPart);
							logPart.Drive = drive;

							logPart.Number = partitionArrayPos;
							logPart.Primary = false;

							logPart.StartSector.LowPart += extStartLBA.LowPart;
							logPart.EndSector.LowPart += extStartLBA.LowPart;

							if (findPartitionFollowingThis)
							{
								if (logPart.StartSector.LowPart > findPartitionFollowingThis->EndSector.LowPart
									&& logPart.StartSector.LowPart < followingPartition->StartSector.LowPart)
								{
									*followingPartition = logPart;
								}
							}
							else
								++partitionArrayPos;
						}

						// Secondary extended
						if (extMbr->Partitions[1].Type != 0x5 && extMbr->Partitions[1].Type == 0xf
							|| extMbr->Partitions[1].SectorCountLBA == 0)
							break;

						extStartLBA.LowPart = extMbr->Partitions[1].StartLBA + firstExtStartLBA.LowPart;
					}
				}
			}
			else
			{
				partition.Primary = true;

				if (findPartitionFollowingThis)
				{
					if (partition.StartSector.LowPart > findPartitionFollowingThis->EndSector.LowPart
						&& partition.StartSector.LowPart < followingPartition->StartSector.LowPart)
					{
						*followingPartition = partition;
					}
				}
				else
					++partitionArrayPos;
			}
		}
	}

	partitionCount = partitionArrayPos;
	return result;
}


/**
 *
 *	\fn bool GetActivePartition (byte drive)
 *	\brief Get the active partition
 *	\param[in] byte drive
 *	\return bool false if (GetDrivePartitions (drive, &ActivePartition, 1, partCount, true) != BiosResultSuccess || partCount < 1)
 *			true else
 *
 */
bool GetActivePartition (byte drive)
{
	size_t partCount;

	if (GetDrivePartitions (drive, &ActivePartition, 1, partCount, true) != BiosResultSuccess || partCount < 1)
	{
		ActivePartition.Drive = GST_INVALID_BIOS_DRIVE;
		PrintError (GST_BOOT_STR_NO_BOOT_PARTITION);
		return false;
	}
	
	return true;
}
