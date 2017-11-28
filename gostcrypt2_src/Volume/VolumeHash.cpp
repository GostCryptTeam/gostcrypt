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

		l.push_back (QSharedPointer <VolumeHash> (new Stribog ()));
		l.push_back (QSharedPointer <VolumeHash> (new GostHash ()));
		l.push_back (QSharedPointer <VolumeHash> (new Whirlpool ()));

		return l;
	}

	void VolumeHash::ValidateDataParameters (const ConstBufferPtr &data) const
	{
		if (data.Size() < 1)
            throw;// ParameterIncorrect (SRC_POS);
	}

	void VolumeHash::ValidateDigestParameters (const BufferPtr &buffer) const
	{
		if (buffer.Size() != GetDigestSize ())
            throw;// ParameterIncorrect (SRC_POS);
	}

    void VolumeHash::ValidateKeyDerivationParameters (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount) const
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
