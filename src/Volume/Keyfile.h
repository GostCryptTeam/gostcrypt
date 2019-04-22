/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_Keyfile
#define GST_HEADER_Encryption_Keyfile

#include "VolumePassword.h"
#include "Core/Buffer.h"
#include <QSharedPointer>
#include <QFileInfo>
#include <QList>

// TODO this constant may not belong here
#define FILE_OPTIMAL_READ_SIZE 256*1024

namespace GostCrypt
{
namespace Volume
{

class Keyfile;
typedef QList < QSharedPointer <Keyfile> > KeyfileList;

/**
 * @brief The Keyfile class represents a keyfile.
 * It justs uses an input file for initialiszation and its only use is to be applied to a password.
 */
class Keyfile
{
 public:

    /**
     * @brief Default constructor that creates the keyfile from a file
     * @param path is the path we want the keyfile to have
     */
    explicit Keyfile(const QFileInfo& path) : Path(path) {}
    virtual ~Keyfile() { }

    /**
     * @brief ApplyListToPassword applies a list of keyfiles to a password
     * @param keyfiles the list of files to apply to this password (can be nullptr if no keyfiles were provided)
     * @param password is the user password provided by the user if any (can be nullptr)
     * @return A combination of the provided password and the list of keyfiles
     */
    static QSharedPointer <VolumePassword> ApplyListToPassword(QSharedPointer <KeyfileList> keyfiles,
            QSharedPointer <VolumePassword> password);

    /**
     * @brief MaxProcessedLength is the maximum processed length of the keyfile
     */
    static const size_t MaxProcessedLength = 1024 * 1024;

 protected:

    /**
     * @brief Apply is the non-static function to applie a keyfile to a buffer (password).
     * For every byte of the file, the crc32 is computed then added byte to byte at the password.
     * This method is not really documented so is to be used with care.
     * @param pool the password to apply the keyfile to
     */
    void Apply(BufferPtr& pool) const;

    /**
     * @brief Path is the path of the keyfile.
     */
    QFileInfo Path;

 private:
    Keyfile(const Keyfile&);
    Keyfile& operator= (const Keyfile&);
};
}
}

#endif // GST_HEADER_Encryption_Keyfile
