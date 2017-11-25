/*
 Copyright (c) 2008-2009 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "Crc32.h"
#include "Keyfile.h"

namespace GostCrypt
{
namespace Volume {

	void Keyfile::Apply (const BufferPtr &pool) const
	{
		File file;

		Crc32 crc32;
		size_t poolPos = 0;
        quint64 totalLength = 0;
        quint64 readLength;

		SecureBuffer keyfileBuf (File::GetOptimalReadSize());
		file.Open (Path, File::OpenRead, File::ShareRead);

		while ((readLength = file.Read (keyfileBuf)) > 0)
		{
			for (size_t i = 0; i < readLength; i++)
			{
                quint32 crc = crc32.Process (keyfileBuf[i]);

                pool[poolPos++] += (quint8) (crc >> 24);
                pool[poolPos++] += (quint8) (crc >> 16);
                pool[poolPos++] += (quint8) (crc >> 8);
                pool[poolPos++] += (quint8) crc;

				if (poolPos >= pool.Size())
					poolPos = 0;

				if (++totalLength >= MaxProcessedLength)
                    return;
			}
		}
	}

    QSharedPointer <VolumePassword> Keyfile::ApplyListToPassword (QSharedPointer <KeyfileList> keyfiles, QSharedPointer <VolumePassword> password)
	{
		if (!password)
			password.reset (new VolumePassword);

		if (!keyfiles || keyfiles->size() < 1)
			return password;

        QSharedPointer<VolumePassword> newPassword (new VolumePassword());

        SecureBuffer keyfilePool (VolumePassword::MaxSize);

        // Pad password with zeros if shorter than max length
        keyfilePool.Zero();
        keyfilePool.CopyFrom (ConstBufferPtr (password->DataPtr(), password->Size()));

        // Apply all keyfiles
        for (const QSharedPointer<Keyfile> k : *keyfiles)
        {
            k->Apply (keyfilePool);
        }

        newPassword->Set (keyfilePool);

		return newPassword;
	}

	bool Keyfile::HiddenFileWasPresentInKeyfilePath = false;
}
}
