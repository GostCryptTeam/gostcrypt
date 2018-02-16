/*
 Copyright (c) 2008-2009 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "Core/GostCryptException.h"
#include "Crc32.h"
#include "Keyfile.h"
#include <QFile>

namespace GostCrypt
{
namespace Volume {

    void Keyfile::Apply (BufferPtr &pool) const
	{
        QFile file(Path.absoluteFilePath());
		Crc32 crc32;
		size_t poolPos = 0;
        quint64 totalLength = 0;
        qint64 readLength;
        SecureBuffer keyfileBuf (FILE_OPTIMAL_READ_SIZE);

        file.open(QIODevice::ReadOnly);

        while ((readLength = file.read(reinterpret_cast<char*> (keyfileBuf.get()), keyfileBuf.size())))
		{
            if(readLength == -1)
                throw FailedReadFileException(Path);
            for (qint64 i = 0; i < readLength; i++)
			{
                quint32 crc = crc32.Process (keyfileBuf[i]);

                pool[poolPos++] += (quint8) (crc >> 24);
                pool[poolPos++] += (quint8) (crc >> 16);
                pool[poolPos++] += (quint8) (crc >> 8);
                pool[poolPos++] += (quint8) crc;

				if (poolPos >= pool.size())
					poolPos = 0;

				if (++totalLength >= MaxProcessedLength)
                    return;
			}
		}

        file.close();
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
        keyfilePool.erase();
        keyfilePool.copyFrom (BufferPtr (password->DataPtr(), password->Size()));

        // Apply all keyfiles
        for (const QSharedPointer<Keyfile> k : *keyfiles)
        {
            k->Apply (keyfilePool);
        }

        newPassword->Set (keyfilePool);

		return newPassword;
	}
}
}
