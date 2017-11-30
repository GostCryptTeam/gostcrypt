/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "EncryptionMode.h"
#include "EncryptionModeXTS.h"
#include "EncryptionThreadPool.h"

namespace GostCrypt
{
namespace Volume {

	EncryptionMode::EncryptionMode () : KeySet (false), SectorOffset (0)
	{
	}

	EncryptionMode::~EncryptionMode ()
	{
	}

	void EncryptionMode::DecryptSectors (quint8 *data, quint64 sectorIndex, quint64 sectorCount, size_t sectorSize) const
	{
		EncryptionThreadPool::DoWork (EncryptionThreadPool::WorkType::DecryptDataUnits, this, data, sectorIndex, sectorCount, sectorSize);
	}

	void EncryptionMode::EncryptSectors (quint8 *data, quint64 sectorIndex, quint64 sectorCount, size_t sectorSize) const
	{
		EncryptionThreadPool::DoWork (EncryptionThreadPool::WorkType::EncryptDataUnits, this, data, sectorIndex, sectorCount, sectorSize);
	}

	void EncryptionMode::ValidateState () const
	{
		if (!KeySet || Ciphers.size() < 1)
            throw;// NotInitialized (SRC_POS);
	}

	void EncryptionMode::ValidateParameters (quint8 *data, quint64 sectorCount, size_t sectorSize) const
	{
        (void)data;
        if (sectorCount == 0 || sectorSize == 0 || (sectorSize % EncryptionDataUnitSize) != 0)
            throw;// ParameterIncorrect (SRC_POS);
	}
}
}
