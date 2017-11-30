/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "VolumeHash.h"
#include "VolumeHashGostHash.h"
#include "VolumeHashStribog.h"
#include "VolumeHashWhirlpool.h"

namespace GostCrypt
{
namespace Volume {

	VolumeHashList VolumeHash::GetAvailableAlgorithms ()
	{
		VolumeHashList l;

        l.push_back (QSharedPointer <VolumeHash> (new VolumeHashStribog ()));
        l.push_back (QSharedPointer <VolumeHash> (new VolumeHashGostHash ()));
        l.push_back (QSharedPointer <VolumeHash> (new VolumeHashWhirlpool ()));

		return l;
	}

    void VolumeHash::ValidateKeyDerivationParameters (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount)
    {
        if (key.Size() < 1 || password.Size() < 1 || salt.Size() < 1 || iterationCount < 1)
            throw;// (SRC_POS);
    }

    void VolumeHash::DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt) const
    {
        DeriveKey (key, password, salt, GetIterationCount());
    }

}
}
