#ifndef FUSESERVICE_H
#define FUSESERVICE_H

#include <QMutex>
#include <QString>
#include "Core/CoreRequest.h"
#include "Core/CoreResponse.h" //TODO move definition of VolumeInformation to Volume subproject
#include "Core/Service.h"
#include "FuseService/FuseException.h"
#include "Volume/Volume.h"
#include "Volume/VolumeInformation.h"


namespace GostCrypt
{
namespace FuseDriver
{
/**
 * @brief Return the filepath of the volume image file relative to a FUSE mount
 *
 * @return const char Filepath of the volume image relative to a FUSE mount
 */
const char* getVolumeImagePath();
/**
 * @brief Return the filepath of the control file relative to a FUSE mount
 *
 * @return const char Filepath of the control file relative to a FUSE mount
 */
const char* getControlPath();

/**
 * @brief Store the path of the virtual device in the control file inside the given FUSE mountpoint. The path of the virtual device will be necessary to unmount the volume later.
 *
 * @param fuseMountPoint Path of the FUSE mountpoint (Usually /tmp/.gostcrypt_aux_mntN)
 * @param virtualDevice Path of the virtual device (Usually /dev/loopN)
 */
void sendAuxDeviceInfo(QFileInfo fuseMountPoint, QFileInfo virtualDevice);

/**
 * @brief Class corresponding to FuseService process in charge of creating the FUSE mount using the FUSE library and maintaining the fuse filesystem
 *
 */
class FuseService : public GostCrypt::Core::Service
{
    Q_OBJECT
 public:
    FuseService();
    /**
     * @brief Return wether or not the current user can access the files within the FUSE mount
     *
     * @return Boolean true if the current user can access the files within the FUSE mount
     */
    static bool checkAccessRights();
    /**
     * @brief Return the error code corresponding to the last thrown Exception
     *
     * @return Error code corresponding to the last thrown Exception as define in errno-base.h
     */
    static int handleExceptions();
    /**
     * @brief Close the volume and shutdown the encryption thread in order to destroy the FUSE mount.
     *
     */
    static void dismount();
    /**
     * @brief Return the size of the volume in byte
     *
     * @return Size of the volume in byte
     */
    static quint64 getVolumeSize();
    /**
     * @brief Return the sector size of the volume in byte
     *
     * @return Size of the volume in byte
     */
    static quint64 getVolumeSectorSize();
    /**
     * @brief Read sectors in the volume and store their data in the given buffer
     *
     * @param buffer Buffer where to store the read data
     * @param byteOffset Offset of the sectors to read (in bytes)
     */
    static void readVolumeSectors(BufferPtr& buffer, quint64 byteOffset);
    /**
     * @brief Write sectors in the volume with the data contained in the given buffer
     *
     * @param buffer Buffer containing the data to write
     * @param byteOffset Offset of the sectors where to write the data (in bytes)
     */
    static void writeVolumeSectors(const BufferPtr& buffer, quint64 byteOffset);
    /**
     * @brief Return whether or not the path of the virtual device (also called loop device) is known, that is to say that it has already been written in the control file.
     *
     * @return True if the path of the virtual device is known
     */
    static bool auxDeviceInfoReceived();
    /**
     * @brief Update volumeInfo with the received information
     *
     * @param buffer Buffer containing the bytes received
     */
    static void receiveAuxDeviceInfo(QByteArray& buffer);
    /**
     * @brief Return the serialized VolumeInfo object as a QByteArray. This object contains all the information about the volume mount.
     *
     * @return Pointer to a QByteArray containing the data of the serialized VolumeInfo object
     */
    static QSharedPointer<QByteArray> getVolumeInfo();
    /**
     * @brief Return the ID of the user for which the volume had been mounted
     *
     * @return ID of the user for which the volume has been mounted
     */
    static uid_t getUserId() { return FuseService::userId; }
    /**
     * @brief Return the ID of the group for which the volume had been mounted
     *
     * @return ID of the group for which the volume has been mounted
     */
    static gid_t getGroupId() { return FuseService::groupId; }
 private:
    static QFileInfo fuseMountPoint; /**< Path of the directory where the fuse file system should be mounted */
    static uid_t userId; /**< ID of the user for which the volume had been mounted */
    static QSharedPointer<Volume::Volume> mountedVolume; /**< Pointer to the mounted volume object */
    static QSharedPointer<Volume::VolumeInformation>
    volumeInfo; /**< Pointer to the Volume Information object containg all information about the mounted volume stored in the control file */
    static gid_t groupId; /**< ID of the group for which the volume had been mounted */
    static QMutex volumeInfoMutex; /**< Mutex to handle access to the volumeInfo object */
    /**
     * @brief Connect signals from the base Service class to the FuseService slots
     *
     */
    virtual void connectRequestHandlingSignals();
    /**
     * @brief Initialize all necessary class to make them serializable in order to transmit them between processes
     *
     */
    virtual void initSerializables();
    /**
     * @brief Send response to the parent process when the Fuse mount is ready
     *
     * @param response QVariant containing the response of the FUSE service with some information about the FUSE mount
     */
    void sendResponseWhenReady(QVariant response);
    /**
     * @brief Launch the FUSE libary to create the FUSE mount
     *
     */
    void launchFuse();
 private slots:
    /**
     * @brief Handle mount request by opening the given volume and launching FUSE in order to create the FUSE mount
     *
     * @param r Mount request containing all necessary information to mount the volume
     */
    void mountRequestHandler(QVariant r);
};
}
}

#endif // FUSESERVICE_H
