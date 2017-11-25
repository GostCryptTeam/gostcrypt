/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "EncryptionAlgorithm.h"
#include "EncryptionAlgorithmGOST.h"
#include "EncryptionAlgorithmGrasshopper.h"
#include "EncryptionModeXTS.h"

namespace GostCrypt
{
namespace Volume {

	EncryptionAlgorithm::EncryptionAlgorithm () : Deprecated (false)
	{
	}

	EncryptionAlgorithm::~EncryptionAlgorithm ()
	{
	}

	void EncryptionAlgorithm::Decrypt (byte *data, uint64 length) const
	{
		if_debug (ValidateState ());
		Mode->Decrypt (data, length);
	}

	void EncryptionAlgorithm::Decrypt (const BufferPtr &data) const
	{
		Decrypt (data, data.Size());
	}

	void EncryptionAlgorithm::DecryptSectors (byte *data, uint64 sectorIndex, uint64 sectorCount, size_t sectorSize) const
	{
		if_debug (ValidateState());
		Mode->DecryptSectors (data, sectorIndex, sectorCount, sectorSize);
	}

	void EncryptionAlgorithm::Encrypt (byte *data, uint64 length) const
	{
		if_debug (ValidateState());
		Mode->Encrypt (data, length);
	}

	void EncryptionAlgorithm::Encrypt (const BufferPtr &data) const
	{
		Encrypt (data, data.Size());
	}

	void EncryptionAlgorithm::EncryptSectors (byte *data, uint64 sectorIndex, uint64 sectorCount, size_t sectorSize) const
	{
		if_debug (ValidateState ());
		Mode->EncryptSectors (data, sectorIndex, sectorCount, sectorSize);
	}

	EncryptionAlgorithmList EncryptionAlgorithm::GetAvailableAlgorithms ()
	{
		EncryptionAlgorithmList l;

        l.push_back (QSharedPointer <EncryptionAlgorithm> (new EncryptionAlgorithmGOST()));
        l.push_back (QSharedPointer <EncryptionAlgorithm> (new EncryptionAlgorithmGrasshopper()));

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
		if (Ciphers.size() < 1)
            throw;// NotInitialized (SRC_POS);

		size_t keySize = 0;

        for  (const QSharedPointer<CipherAlgorithm> c : Ciphers)
            keySize += c->GetKeySize();

		return keySize;
	}

	size_t EncryptionAlgorithm::GetMaxBlockSize () const
	{
		size_t blockSize = 0;

        for (const QSharedPointer<CipherAlgorithm> c : Ciphers) {
            if (c->GetBlockSize() > blockSize)
                blockSize = c->GetBlockSize();
        }

		return blockSize;
	}

	size_t EncryptionAlgorithm::GetMinBlockSize () const
	{
		size_t blockSize = 0;

        for (const QSharedPointer<CipherAlgorithm> c : Ciphers) {
            if (blockSize == 0 || c->GetBlockSize() < blockSize) {
                blockSize = c->GetBlockSize();
             }
         }

		return blockSize;
	}

    QSharedPointer <EncryptionMode> EncryptionAlgorithm::GetMode () const
	{
        if (Mode.isNull())
            throw;// NotInitialized (SRC_POS);

		return Mode;
	}

	wstring EncryptionAlgorithm::GetName () const
	{
		if (Ciphers.size() < 1)
            throw;// NotInitialized (SRC_POS);

		wstring name;

        for (const QSharedPointer<CipherAlgorithm> c : Ciphers)
		{
			if (name.empty())
                name = c->GetName();
			else
                name += wstring (L"-") + c->GetName();
		}

		return name;
	}

        wstring EncryptionAlgorithm::GetDescription () const
        {
                if (Ciphers.size() < 1)
                        throw;// NotInitialized (SRC_POS);

                wstring desc;

                for (const QSharedPointer<CipherAlgorithm> c : Ciphers)
                {
                        if (desc.empty())
                                desc = c->GetDescription();
                        else
                                desc += wstring (L"\n\n") + c->GetDescription();
                }

                return desc;
        }

	bool EncryptionAlgorithm::IsModeSupported (const EncryptionMode &mode) const
	{
		bool supported = false;

        for (const QSharedPointer<EncryptionMode> em : SupportedModes)
		{
            if (typeid (mode) == typeid (*em))
			{
				supported = true;
				break;
			}
		}

		return supported;
	}


    bool EncryptionAlgorithm::IsModeSupported (const QSharedPointer <EncryptionMode> mode) const
	{
		return IsModeSupported (*mode);
	}

    void EncryptionAlgorithm::SetMode (QSharedPointer <EncryptionMode> mode)
	{
		if (!IsModeSupported (*mode))
            throw;// ParameterIncorrect (SRC_POS);

		mode->SetCiphers (Ciphers);
		Mode = mode;
	}

	void EncryptionAlgorithm::SetKey (const ConstBufferPtr &key)
	{
		if (Ciphers.size() < 1)
            throw;// NotInitialized (SRC_POS);

		if (GetKeySize() != key.Size())
            throw;// ParameterIncorrect (SRC_POS);

		size_t keyOffset = 0;
        for (QSharedPointer<CipherAlgorithm> c : Ciphers)
		{
            c->SetKey (key.GetRange (keyOffset, c->GetKeySize()));
            keyOffset += c->GetKeySize();
		}
	}

	void EncryptionAlgorithm::ValidateState () const
	{
        if (Ciphers.size() < 1 || Mode.isNull())
            throw;// NotInitialized (SRC_POS);
	}
}
}
