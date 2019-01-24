/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_Password
#define GST_HEADER_Encryption_Password

#include <QtGlobal>
#include "Core/Buffer.h"

namespace GostCrypt
{
namespace Volume
{

class VolumePassword
{
 public:
    VolumePassword();
    VolumePassword(const char* password, size_t size);
    ~VolumePassword() {}

    bool operator== (const VolumePassword& other) const;
    bool operator!= (const VolumePassword& other) const { return !(*this == other); }
    VolumePassword& operator= (const VolumePassword& password) { Set(password); return *this; }

    operator const BufferPtr() const { return BufferPtr(PasswordBuffer); }

    quint8* DataPtr() { return PasswordBuffer.get(); }
    const quint8* DataPtr() const { return PasswordBuffer.get(); }
    size_t Size() const { return PasswordSize; }
    void Set(const quint8* password, size_t size);
    void Set(const BufferPtr& password);
    void Set(const VolumePassword& password);
    bool IsPortable() const;

    static const size_t MaxSize = 64;
    static const size_t WarningSizeThreshold = 12;

 protected:
    void AllocateBuffer();

    SecureBuffer PasswordBuffer;
    size_t PasswordSize;
};

}
}
#endif // GST_HEADER_Encryption_Password
