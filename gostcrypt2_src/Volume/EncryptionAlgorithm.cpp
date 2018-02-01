/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#include "VolumeException.h"
#include "EncryptionAlgorithm.h"
#include "EncryptionAlgorithmGOST.h"
#include "EncryptionAlgorithmGrasshopper.h"
#include "EncryptionModeXTS.h"
#include <typeinfo>

namespace GostCrypt
{
namespace Volume {

	EncryptionAlgorithm::EncryptionAlgorithm () : Deprecated (false)
	{
	}

	EncryptionAlgorithm::~EncryptionAlgorithm ()
	{
	}

	void EncryptionAlgorithm::Decrypt (quint8 *data, quint64 length) const
	{
		//if_debug (ValidateState ());
		Mode->Decrypt (data, length);
	}

    void EncryptionAlgorithm::Decrypt (BufferPtr &data) const
	{
		Decrypt (data, data.Size());
	}

	void EncryptionAlgorithm::DecryptSectors (quint8 *data, quint64 sectorIndex, quint64 sectorCount, size_t sectorSize) const
	{
		//if_debug (ValidateState());
		Mode->DecryptSectors (data, sectorIndex, sectorCount, sectorSize);
	}

	void EncryptionAlgorithm::Encrypt (quint8 *data, quint64 length) const
	{
		//if_debug (ValidateState());
		Mode->Encrypt (data, length);
	}

    void EncryptionAlgorithm::Encrypt (BufferPtr &data) const
	{
		Encrypt (data, data.Size());
	}

	void EncryptionAlgorithm::EncryptSectors (quint8 *data, quint64 sectorIndex, quint64 sectorCount, size_t sectorSize) const
	{
		//if_debug (ValidateState ());
		Mode->EncryptSectors (data, sectorIndex, sectorCount, sectorSize);
	}

	EncryptionAlgorithmList EncryptionAlgorithm::GetAvailableAlgorithms ()
	{
		EncryptionAlgorithmList l;

        l.push_back (QSharedPointer <EncryptionAlgorithm> (new EncryptionAlgorithmGOST(QSharedPointer <EncryptionMode>(new EncryptionModeXTS()))));
        l.push_back (QSharedPointer <EncryptionAlgorithm> (new EncryptionAlgorithmGrasshopper(QSharedPointer <EncryptionMode>(new EncryptionModeXTS()))));

		return l;
	}

	size_t EncryptionAlgorithm::GetLargestKeySize (const EncryptionAlgorithmList &algorithms)
	{
		size_t largestKeySize = 0;

        for (const QSharedPointer<EncryptionAlgorithm> ea : algorithms)
		{
            if (ea->GetKeySize() > largestKeySize)
                largestKeySize = ea->GetKeySize();
		}

		return largestKeySize;
	}

	size_t EncryptionAlgorithm::GetKeySize () const
	{
        return this->Mode->GetKeySize();
	}

    QSharedPointer <EncryptionMode> EncryptionAlgorithm::GetMode () const
	{
        if (Mode.isNull())
            throw EncryptionAlgorithmNotInitializedException();

		return Mode;
	}

    QString EncryptionAlgorithm::GetName () const
	{
        if (this->Mode->GetCiphers().size() < 1)
            throw EncryptionAlgorithmNotInitializedException();

        QString name;

        for (const QSharedPointer<CipherAlgorithm> c : this->Mode->GetCiphers())
		{
            if (name.isEmpty())
                name = c->GetName();
			else
                name += "-" + c->GetName();
		}

		return name;
	}

        QString EncryptionAlgorithm::GetDescription () const
        {
                if (this->Mode->GetCiphers().size() < 1)
                        throw EncryptionAlgorithmNotInitializedException();

                QString desc;

                for (const QSharedPointer<CipherAlgorithm> c : this->Mode->GetCiphers())
                {
                        if (desc.isEmpty())
                                desc = c->GetDescription();
                        else
                                desc += "\n\n" + c->GetDescription();
                }

                return desc;
        }

    void EncryptionAlgorithm::SetKey (const BufferPtr &key)
	{
        if (this->Mode->GetCiphers().size() < 1)
            throw EncryptionAlgorithmNotInitializedException();

		if (GetKeySize() != key.Size())
            throw IncorrectParameterException("Key size mismatch");

		size_t keyOffset = 0;
        for (QSharedPointer<CipherAlgorithm> c : this->Mode->GetCiphers())
		{
            c->SetKey (key.GetRange (keyOffset, c->GetKeySize()));
            keyOffset += c->GetKeySize();
		}
	}
}
}
